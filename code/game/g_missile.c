/*
=======================================================================================================================================
Copyright (C) 1999-2010 id Software LLC, a ZeniMax Media company.

This file is part of Spearmint Source Code.

Spearmint Source Code is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 3 of the License, or (at your option) any later version.

Spearmint Source Code is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with Spearmint Source Code.
If not, see <http://www.gnu.org/licenses/>.

In addition, Spearmint Source Code is also subject to certain additional terms. You should have received a copy of these additional
terms immediately following the terms and conditions of the GNU General Public License. If not, please request a copy in writing from
id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o
ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.
=======================================================================================================================================
*/

#include "g_local.h"

#define MISSILE_PRESTEP_TIME 50

/*
=======================================================================================================================================
G_BounceMissile
=======================================================================================================================================
*/
qboolean G_BounceMissile(gentity_t *ent, trace_t *trace) {
	vec3_t velocity;
	int hitTime, contents;
	float dot;

	// reflect the velocity on the trace plane
	hitTime = level.previousTime + (level.time - level.previousTime) * trace->fraction;

	BG_EvaluateTrajectoryDelta(&ent->s.pos, hitTime, velocity);

	dot = DotProduct(velocity, trace->plane.normal);

	VectorMA(velocity, -2 * dot, trace->plane.normal, ent->s.pos.trDelta);
// Tobias FIXME: do some simplifications and use a 'jumptable' here (like 'case: isSoftMaterial' etc.)...
	if ((trace->surfaceFlags & SURF_MATERIAL_MASK) == MAT_SAND_GR_COL_01 || (trace->surfaceFlags & SURF_MATERIAL_MASK) == MAT_SAND_GR_COL_02 || (trace->surfaceFlags & SURF_MATERIAL_MASK) == MAT_SAND_GR_COL_03 || (trace->surfaceFlags & SURF_MATERIAL_MASK) == MAT_SAND_GR_COL_04) {
		ent->s.pos.trDelta[2] *= 0.65f;
	}
// Tobias END
	contents = trap_PointContents(ent->r.currentOrigin, -1);

	if (contents & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA)) {
		ent->s.pos.trDelta[2] *= 0.25f;
	}

	if (ent->s.eFlags & EF_BOUNCE_HALF) {
		// if it hit a client then barely bounce off of them since they are "soft"
		if (trace->entityNum >= 0 && trace->entityNum < MAX_CLIENTS) {
			VectorScale(ent->s.pos.trDelta, 0.02f, ent->s.pos.trDelta);
		} else {
			VectorScale(ent->s.pos.trDelta, 0.65, ent->s.pos.trDelta);
			// check for stop
			if (trace->plane.normal[2] > 0.2 && VectorLength(ent->s.pos.trDelta) < 40) {
				G_SetOrigin(ent, trace->endpos);
				ent->s.time = level.time / 4;
				return qfalse;
			}
		}
	}

	VectorAdd(ent->r.currentOrigin, trace->plane.normal, ent->r.currentOrigin);
	VectorCopy(ent->r.currentOrigin, ent->s.pos.trBase);

	ent->s.pos.trTime = level.time;
	return qtrue;
}

/*
=======================================================================================================================================
G_ExplodeMissile

Explode a missile without an impact.
=======================================================================================================================================
*/
void G_ExplodeMissile(gentity_t *ent) {
	vec3_t dir;
	vec3_t origin;

	BG_EvaluateTrajectory(&ent->s.pos, level.time, origin);
	SnapVector(origin);
	G_SetOrigin(ent, origin);
	// we don't have a valid direction, so just point straight up
	dir[0] = dir[1] = 0;
	dir[2] = 1;
	ent->s.eType = ET_GENERAL;

	G_AddEvent(ent, EV_MISSILE_MISS, DirToByte(dir));

	ent->freeAfterEvent = qtrue;
	// splash damage
	if (ent->splashDamage) {
		if (G_RadiusDamage(ent->r.currentOrigin, ent->parent, ent->splashDamage, ent->splashRadius, ent, ent->splashMethodOfDeath)) {
			g_entities[ent->r.ownerNum].client->accuracy_hits++;
		}
	}

	trap_LinkEntity(ent);
}

/*
=======================================================================================================================================
ProximityMine_Explode
=======================================================================================================================================
*/
static void ProximityMine_Explode(gentity_t *mine) {

	G_ExplodeMissile(mine);
	// if the prox mine has a trigger free it
	if (mine->activator) {
		G_FreeEntity(mine->activator);
		mine->activator = NULL;
	}
}

/*
=======================================================================================================================================
ProximityMine_Die
=======================================================================================================================================
*/
static void ProximityMine_Die(gentity_t *ent, gentity_t *inflictor, gentity_t *attacker, int damage, int mod) {

	ent->think = ProximityMine_Explode;
	ent->nextthink = level.time + 1;
}

/*
=======================================================================================================================================
ProximityMine_Trigger
=======================================================================================================================================
*/
void ProximityMine_Trigger(gentity_t *trigger, gentity_t *other, trace_t *trace) {
	vec3_t v;
	gentity_t *mine;

	if (!other->client) {
		return;
	}
	// trigger is a cube, do a distance test now to act as if it's a sphere
	VectorSubtract(trigger->s.pos.trBase, other->s.pos.trBase, v);

	if (VectorLength(v) > trigger->parent->splashRadius) {
		return;
	}

	if (g_gametype.integer > GT_TOURNAMENT) {
		// don't trigger same team mines
		if (trigger->parent->s.team == other->client->sess.sessionTeam) {
			return;
		}
	}
	// ok, now check for ability to damage so we don't get triggered through walls, closed doors, etc.
	if (!CanDamage(other, trigger->s.pos.trBase)) {
		return;
	}
	// trigger the mine!
	mine = trigger->parent;
	mine->s.loopSound = 0;

	G_AddEvent(mine, EV_PROXIMITY_MINE_TRIGGER, 0);

	mine->nextthink = level.time + 500;

	G_FreeEntity(trigger);
}

/*
=======================================================================================================================================
ProximityMine_Activate
=======================================================================================================================================
*/
static void ProximityMine_Activate(gentity_t *ent) {
	gentity_t *trigger;
	float r;

	ent->think = ProximityMine_Explode;
	ent->nextthink = level.time + g_proxMineTimeout.integer;
	ent->takedamage = qtrue;
	ent->health = 1;
	ent->die = ProximityMine_Die;
	ent->s.loopSound = G_SoundIndex("sound/weapons/proxmine/wstbtick.wav");
	// build the proximity trigger
	trigger = G_Spawn();
	trigger->classname = "proxmine_trigger";
	r = ent->splashRadius;

	VectorSet(trigger->r.mins, -r, -r, -r);
	VectorSet(trigger->r.maxs, r, r, r);
	G_SetOrigin(trigger, ent->s.pos.trBase);

	trigger->parent = ent;
	trigger->r.contents = CONTENTS_TRIGGER;
	trigger->touch = ProximityMine_Trigger;

	trap_LinkEntity(trigger);
	// set pointer to trigger so the entity can be freed when the mine explodes
	ent->activator = trigger;
}

/*
=======================================================================================================================================
ProximityMine_ExplodeOnPlayer
=======================================================================================================================================
*/
static void ProximityMine_ExplodeOnPlayer(gentity_t *mine) {
	gentity_t *player;

	player = mine->enemy;
	player->client->ps.eFlags &= ~EF_TICKING;

	G_SetOrigin(mine, player->s.pos.trBase);
	// make sure the explosion gets to the client
	mine->r.svFlags &= ~SVF_NOCLIENT;
	mine->splashMethodOfDeath = MOD_PROXIMITY_MINE;

	G_ExplodeMissile(mine);
}

/*
=======================================================================================================================================
ProximityMine_Player
=======================================================================================================================================
*/
static void ProximityMine_Player(gentity_t *mine, gentity_t *player) {

	if (mine->s.eFlags & EF_NODRAW) {
		return;
	}

	G_AddEvent(mine, EV_PROXIMITY_MINE_STICK, 0);

	if (player->s.eFlags & EF_TICKING) {
		player->activator->splashDamage += mine->splashDamage;
		player->activator->splashRadius *= 1.50;
		mine->think = G_FreeEntity;
		mine->nextthink = level.time;
		return;
	}

	player->client->ps.eFlags |= EF_TICKING;
	player->activator = mine;

	mine->s.eFlags |= EF_NODRAW;
	mine->r.svFlags |= SVF_NOCLIENT;
	mine->s.pos.trType = TR_LINEAR;

	VectorClear(mine->s.pos.trDelta);

	mine->enemy = player;
	mine->think = ProximityMine_ExplodeOnPlayer;
	mine->nextthink = level.time + 10 * 1000;
}

/*
=======================================================================================================================================
G_MissileImpact
=======================================================================================================================================
*/
void G_MissileImpact(gentity_t *ent, trace_t *trace) {
	gentity_t *other;
	qboolean hitClient = qfalse;
	vec3_t velocity;

	other = &g_entities[trace->entityNum];
	// check for bounce
	if (ent->s.eFlags & (EF_BOUNCE|EF_BOUNCE_HALF)) {
		if (G_BounceMissile(ent, trace) && !trace->startsolid) { // no bounce, no bounce sound
			G_AddEvent(ent, EV_GRENADE_BOUNCE, 0);
		}

		return;
	}
	// impact damage
	if (other->takedamage) {
		// FIXME: wrong damage direction?
		if (ent->damage) {
			if (LogAccuracyHit(other, &g_entities[ent->r.ownerNum])) {
				g_entities[ent->r.ownerNum].client->accuracy_hits++;
				hitClient = qtrue;
			}

			BG_EvaluateTrajectoryDelta(&ent->s.pos, level.time, velocity);

			if (VectorLengthSquared(velocity) == 0) {
				velocity[2] = 1; // stepped on a grenade
			}

			G_Damage(other, ent, &g_entities[ent->r.ownerNum], velocity, ent->s.origin, ent->damage, 0, ent->methodOfDeath);
		}
	}

	if (ent->s.weapon == WP_PROXLAUNCHER) {
		if (ent->s.pos.trType != TR_GRAVITY) {
			return;
		}
		// if it's a player, stick it on to them (flag them and remove this entity)
		if (other->s.eType == ET_PLAYER && other->health > 0) {
			ProximityMine_Player(ent, other);
			return;
		}

		SnapVectorTowards(trace->endpos, ent->s.pos.trBase);
		G_SetOrigin(ent, trace->endpos);

		ent->s.pos.trType = TR_STATIONARY;

		VectorClear(ent->s.pos.trDelta);
		G_AddEvent(ent, EV_PROXIMITY_MINE_STICK, trace->surfaceFlags);

		ent->think = ProximityMine_Activate;
		ent->nextthink = level.time + 2000;

		VectorToAngles(trace->plane.normal, ent->s.angles);

		ent->s.angles[0] += 90;
		// link the prox mine to the other entity
		ent->enemy = other;
		ent->die = ProximityMine_Die;

		VectorCopy(trace->plane.normal, ent->movedir);
		VectorSet(ent->r.mins, -4, -4, -4);
		VectorSet(ent->r.maxs, 4, 4, 4);

		trap_LinkEntity(ent);
		return;
	}
	// is it cheaper in bandwidth to just remove this ent and create a new one, rather than changing the missile into the explosion?
	if (other->takedamage && other->client) {
		G_AddEvent(ent, EV_MISSILE_HIT, DirToByte(trace->plane.normal));
		ent->s.otherEntityNum = other->s.number;
	} else {
		G_AddEvent(ent, EV_MISSILE_MISS, DirToByte(trace->plane.normal));
	}

	ent->freeAfterEvent = qtrue;
	// change over to a normal entity right at the point of impact
	ent->s.eType = ET_GENERAL;

	SnapVectorTowards(trace->endpos, ent->s.pos.trBase); // save net bandwidth
	G_SetOrigin(ent, trace->endpos);
	// splash damage (doesn't apply to person directly hit)
	if (ent->splashDamage) {
		if (G_RadiusDamage(trace->endpos, ent->parent, ent->splashDamage, ent->splashRadius, other, ent->splashMethodOfDeath)) {
			if (!hitClient) {
				g_entities[ent->r.ownerNum].client->accuracy_hits++;
			}
		}
	}

	trap_LinkEntity(ent);
}

/*
=======================================================================================================================================
G_RunMissile
=======================================================================================================================================
*/
void G_RunMissile(gentity_t *ent) {
	vec3_t origin;
	trace_t tr;
	int passent;

	// get current position
	BG_EvaluateTrajectory(&ent->s.pos, level.time, origin);
	// missiles that left the owner bbox will interact with anything, even the owner
	if (!ent->count) {
		// ignore interactions with the missile owner
		passent = ent->r.ownerNum;
	} else {
		passent = ENTITYNUM_NONE;
	}
	// trace a line from the previous position to the current position
	trap_Trace(&tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, origin, passent, ent->clipmask);

	if (tr.startsolid || tr.allsolid) {
		// make sure the tr.entityNum is set to the entity we're stuck in
		trap_Trace(&tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, passent, ent->clipmask);
		tr.fraction = 0;
		ent->count = 0;
	} else {
		VectorCopy(tr.endpos, ent->r.currentOrigin);
	}

	trap_LinkEntity(ent);

	if (tr.fraction != 1) {
		// never explode or bounce on sky
		if (tr.surfaceFlags & SURF_NOIMPACT) {
			G_FreeEntity(ent);
			return;
		}

		G_MissileImpact(ent, &tr);

		if (ent->s.eType != ET_MISSILE) {
			return; // exploded
		}
	}
	// if the missile wasn't yet outside the player body
	if (ent->s.weapon != WP_NAILGUN && !ent->count) {
		// check if the missile is outside the owner bbox
		trap_Trace(&tr, ent->r.currentOrigin, ent->r.mins, ent->r.maxs, ent->r.currentOrigin, ENTITYNUM_NONE, ent->clipmask);

		if (!tr.startsolid || tr.entityNum != ent->r.ownerNum) {
			ent->count = 1;
		}
	}
	// check think function after bouncing
	G_RunThink(ent);
}

#define NAILGUN_SPREAD 500
/*
=======================================================================================================================================
fire_nail
=======================================================================================================================================
*/
gentity_t *fire_nail(gentity_t *self, vec3_t start, vec3_t forward, vec3_t right, vec3_t up) {
	gentity_t *bolt;
	vec3_t dir;
	vec3_t end;
	float r, u, scale;

	bolt = G_Spawn();
	bolt->classname = "nail";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.weapon = WP_NAILGUN;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 20;
	bolt->methodOfDeath = MOD_NAIL;
	bolt->clipmask = MASK_SHOT;
	// count is used to check if the missile left the player bbox, if count == 1 then the missile left the player bbox and can attack to it
	bolt->count = 0;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;

	VectorCopy(start, bolt->s.pos.trBase);

	r = random() * M_PI * 2.0f;
	u = sin(r) * crandom() * NAILGUN_SPREAD * 16;
	r = cos(r) * crandom() * NAILGUN_SPREAD * 16;

	VectorMA(start, 131072, forward, end); // 8192 * 16
	VectorMA(end, r, right, end);
	VectorMA(end, u, up, end);
	VectorSubtract(end, start, dir);
	VectorNormalize(dir);

	scale = 555 + random() * 1800;

	VectorScale(dir, scale, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta);
	VectorCopy(start, bolt->r.currentOrigin);

	if (self->client) {
		bolt->s.team = self->client->sess.sessionTeam;
	} else {
		bolt->s.team = TEAM_FREE;
	}

	return bolt;
}

/*
=======================================================================================================================================
fire_prox
=======================================================================================================================================
*/
gentity_t *fire_prox(gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t *bolt;

	VectorNormalize(dir);

	bolt = G_Spawn();
	bolt->classname = "prox mine";
	bolt->nextthink = level.time + 3000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.weapon = WP_PROXLAUNCHER;
	bolt->s.eFlags = 0;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 0;
	bolt->splashDamage = 100;
	bolt->splashRadius = 150;
	bolt->methodOfDeath = MOD_PROXIMITY_MINE;
	bolt->splashMethodOfDeath = MOD_PROXIMITY_MINE;
	bolt->clipmask = MASK_SHOT;
	// count is used to check if the missile left the player bbox, if count == 1 then the missile left the player bbox and can attack to it
	bolt->count = 0;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME; // move a bit on the very first frame

	VectorCopy(start, bolt->s.pos.trBase);
	VectorScale(dir, 700, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta); // save net bandwidth
	VectorCopy(start, bolt->r.currentOrigin);

	if (self->client) {
		bolt->s.team = self->client->sess.sessionTeam;
	} else {
		bolt->s.team = TEAM_FREE;
	}

	return bolt;
}

/*
=======================================================================================================================================
fire_grenade
=======================================================================================================================================
*/
gentity_t *fire_grenade(gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t *bolt;

	VectorNormalize(dir);

	bolt = G_Spawn();
	bolt->classname = "grenade";
	bolt->nextthink = level.time + 2500;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.weapon = WP_GRENADELAUNCHER;
	bolt->s.eFlags = EF_BOUNCE_HALF;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 0;
	bolt->splashDamage = 200;
	bolt->splashRadius = 200;
	bolt->methodOfDeath = MOD_GRENADE_SPLASH;
	bolt->splashMethodOfDeath = MOD_GRENADE_SPLASH;
	bolt->clipmask = MASK_SHOT;
	// count is used to check if the missile left the player bbox, if count == 1 then the missile left the player bbox and can attack to it
	bolt->count = 0;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME; // move a bit on the very first frame

	VectorCopy(start, bolt->s.pos.trBase);
	VectorScale(dir, 1100, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta); // save net bandwidth
	VectorCopy(start, bolt->r.currentOrigin);

	if (self->client) {
		bolt->s.team = self->client->sess.sessionTeam;
	} else {
		bolt->s.team = TEAM_FREE;
	}

	return bolt;
}

/*
=======================================================================================================================================
fire_napalm
=======================================================================================================================================
*/
gentity_t *fire_napalm(gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t *bolt;

	VectorNormalize(dir);

	bolt = G_Spawn();
	bolt->classname = "napalm";
	bolt->nextthink = level.time + 15000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.weapon = WP_NAPALMLAUNCHER;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 0;
	bolt->splashDamage = 10;
	bolt->splashRadius = 300;
	bolt->methodOfDeath = MOD_NAPALM;
	bolt->splashMethodOfDeath = MOD_NAPALM_SPLASH;
	bolt->clipmask = MASK_SHOT;
	// count is used to check if the missile left the player bbox, if count == 1 then the missile left the player bbox and can attack to it
	bolt->count = 0;
	bolt->s.pos.trType = TR_GRAVITY;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME; // move a bit on the very first frame

	VectorCopy(start, bolt->s.pos.trBase);
	VectorScale(dir, 5000, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta); // save net bandwidth
	VectorCopy(start, bolt->r.currentOrigin);

	if (self->client) {
		bolt->s.team = self->client->sess.sessionTeam;
	} else {
		bolt->s.team = TEAM_FREE;
	}

	return bolt;
}

/*
=======================================================================================================================================
fire_rocket
=======================================================================================================================================
*/
gentity_t *fire_rocket(gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t *bolt;

	VectorNormalize(dir);

	bolt = G_Spawn();
	bolt->classname = "rocket";
	bolt->nextthink = level.time + 15000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.weapon = WP_ROCKETLAUNCHER;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 100;
	bolt->splashDamage = 100;
	bolt->splashRadius = 120;
	bolt->methodOfDeath = MOD_ROCKET;
	bolt->splashMethodOfDeath = MOD_ROCKET_SPLASH;
	bolt->clipmask = MASK_SHOT;
	// count is used to check if the missile left the player bbox, if count == 1 then the missile left the player bbox and can attack to it
	bolt->count = 0;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME; // move a bit on the very first frame

	VectorCopy(start, bolt->s.pos.trBase);
	VectorScale(dir, 1100, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta); // save net bandwidth
	VectorCopy(start, bolt->r.currentOrigin);

	if (self->client) {
		bolt->s.team = self->client->sess.sessionTeam;
	} else {
		bolt->s.team = TEAM_FREE;
	}

	return bolt;
}

/*
=======================================================================================================================================
fire_plasma
=======================================================================================================================================
*/
gentity_t *fire_plasma(gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t *bolt;

	VectorNormalize(dir);

	bolt = G_Spawn();
	bolt->classname = "plasma";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.weapon = WP_PLASMAGUN;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 20;
	bolt->splashDamage = 15;
	bolt->splashRadius = 20;
	bolt->methodOfDeath = MOD_PLASMA;
	bolt->splashMethodOfDeath = MOD_PLASMA_SPLASH;
	bolt->clipmask = MASK_SHOT;
	// count is used to check if the missile left the player bbox, if count == 1 then the missile left the player bbox and can attack to it
	bolt->count = 0;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time;

	VectorCopy(start, bolt->s.pos.trBase);
	VectorScale(dir, 2000, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta); // save net bandwidth
	VectorCopy(start, bolt->r.currentOrigin);

	if (self->client) {
		bolt->s.team = self->client->sess.sessionTeam;
	} else {
		bolt->s.team = TEAM_FREE;
	}

	return bolt;
}

/*
=======================================================================================================================================
fire_bfg
=======================================================================================================================================
*/
gentity_t *fire_bfg(gentity_t *self, vec3_t start, vec3_t dir) {
	gentity_t *bolt;

	VectorNormalize(dir);

	bolt = G_Spawn();
	bolt->classname = "bfg";
	bolt->nextthink = level.time + 10000;
	bolt->think = G_ExplodeMissile;
	bolt->s.eType = ET_MISSILE;
	bolt->s.weapon = WP_BFG;
	bolt->r.ownerNum = self->s.number;
	bolt->parent = self;
	bolt->damage = 100;
	bolt->splashDamage = 100;
	bolt->splashRadius = 120;
	bolt->methodOfDeath = MOD_BFG;
	bolt->splashMethodOfDeath = MOD_BFG_SPLASH;
	bolt->clipmask = MASK_SHOT;
	// count is used to check if the missile left the player bbox, if count == 1 then the missile left the player bbox and can attack to it
	bolt->count = 0;
	bolt->s.pos.trType = TR_LINEAR;
	bolt->s.pos.trTime = level.time - MISSILE_PRESTEP_TIME; // move a bit on the very first frame

	VectorCopy(start, bolt->s.pos.trBase);
	VectorScale(dir, 2000, bolt->s.pos.trDelta);
	SnapVector(bolt->s.pos.trDelta); // save net bandwidth
	VectorCopy(start, bolt->r.currentOrigin);

	if (self->client) {
		bolt->s.team = self->client->sess.sessionTeam;
	} else {
		bolt->s.team = TEAM_FREE;
	}

	return bolt;
}
