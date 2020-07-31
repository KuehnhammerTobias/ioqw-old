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

/*
=======================================================================================================================================
Use_Target_Give
=======================================================================================================================================
*/
void Use_Target_Give(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	gentity_t *t;
	trace_t trace;

	if (!activator->client) {
		return;
	}

	if (!ent->target) {
		return;
	}

	memset(&trace, 0, sizeof(trace));

	t = NULL;

	while ((t = G_Find(t, FOFS(targetname), ent->target)) != NULL) {
		if (!t->item) {
			continue;
		}

		Touch_Item(t, activator, &trace);
		// make sure it isn't going to respawn or show any events
		t->nextthink = 0;

		trap_UnlinkEntity(t);
	}
}

/*QUAKED target_give (1 0 0) (-8 -8 -8) (8 8 8)
Gives the activator all the items pointed to.
*/
void SP_target_give(gentity_t *ent) {
	ent->use = Use_Target_Give;
}

/*
=======================================================================================================================================
Use_Target_Remove_Powerups
=======================================================================================================================================
*/
void Use_Target_Remove_Powerups(gentity_t *ent, gentity_t *other, gentity_t *activator) {

	if (!activator->client) {
		return;
	}

	if (activator->client->ps.powerups[PW_REDFLAG]) {
		Team_ReturnFlag(TEAM_RED);
	} else if (activator->client->ps.powerups[PW_BLUEFLAG]) {
		Team_ReturnFlag(TEAM_BLUE);
	} else if (activator->client->ps.powerups[PW_NEUTRALFLAG]) {
		Team_ReturnFlag(TEAM_FREE);
	}

	memset(activator->client->ps.powerups, 0, sizeof(activator->client->ps.powerups));
}

/*QUAKED target_remove_powerups (1 0 0) (-8 -8 -8) (8 8 8)
takes away all the activators powerups.
Used to drop powerups into death puts.
*/
void SP_target_remove_powerups(gentity_t *ent) {
	ent->use = Use_Target_Remove_Powerups;
}

/*
=======================================================================================================================================
Think_Target_Delay
=======================================================================================================================================
*/
void Think_Target_Delay(gentity_t *ent) {
	G_UseTargets(ent, ent->activator);
}

/*
=======================================================================================================================================
Use_Target_Delay
=======================================================================================================================================
*/
void Use_Target_Delay(gentity_t *ent, gentity_t *other, gentity_t *activator) {

	ent->nextthink = level.time + (ent->wait + ent->random * crandom()) * 1000;
	ent->think = Think_Target_Delay;
	ent->activator = activator;
}

/*QUAKED target_delay (1 0 0) (-8 -8 -8) (8 8 8)
"wait" seconds to pause before firing targets.
"random" delay variance, total delay = delay +/- random seconds
*/
void SP_target_delay(gentity_t *ent) {

	// check delay for backwards compatibility
	if (!G_SpawnFloat("delay", "0", &ent->wait)) {
		G_SpawnFloat("wait", "1", &ent->wait);
	}

	if (!ent->wait) {
		ent->wait = 1;
	}

	ent->use = Use_Target_Delay;
}

/*
=======================================================================================================================================
Use_Target_Score
=======================================================================================================================================
*/
void Use_Target_Score(gentity_t *ent, gentity_t *other, gentity_t *activator) {
	AddScore(activator, ent->r.currentOrigin, ent->count);
}

/*QUAKED target_score (1 0 0) (-8 -8 -8) (8 8 8)
"count" number of points to add, default 1

The activator is given this many points.
*/
void SP_target_score(gentity_t *ent) {

	if (!ent->count) {
		ent->count = 1;
	}

	ent->use = Use_Target_Score;
}

/*
=======================================================================================================================================
Use_Target_Print
=======================================================================================================================================
*/
void Use_Target_Print(gentity_t *ent, gentity_t *other, gentity_t *activator) {

	if (activator->client && (ent->spawnflags & 4)) {
		trap_SendServerCommand(activator - g_entities, va("cp \"%s\"", ent->message));
		return;
	}

	if (ent->spawnflags & 3) {
		if (ent->spawnflags & 1) {
			G_TeamCommand(TEAM_RED, va("cp \"%s\"", ent->message));
		}

		if (ent->spawnflags & 2) {
			G_TeamCommand(TEAM_BLUE, va("cp \"%s\"", ent->message));
		}

		return;
	}

	trap_SendServerCommand(-1, va("cp \"%s\"", ent->message));
}

/*QUAKED target_print (1 0 0) (-8 -8 -8) (8 8 8) REDTEAM BLUETEAM PRIVATE
"message"	text to print
If "private", only the activator gets the message. If no checks, all clients get the message.
*/
void SP_target_print(gentity_t *ent) {
	ent->use = Use_Target_Print;
}

/*
=======================================================================================================================================
Use_Target_Speaker
=======================================================================================================================================
*/
void Use_Target_Speaker(gentity_t *ent, gentity_t *other, gentity_t *activator) {

	if (ent->spawnflags & 3) { // looping sound toggles
		if (ent->s.loopSound) {
			ent->s.loopSound = 0; // turn it off
		} else {
			ent->s.loopSound = ent->noise_index; // start it
		}
	} else { // normal sound
		if (ent->spawnflags & 8) {
			G_AddEvent(activator, EV_GENERAL_SOUND, ent->noise_index);
		} else if (ent->spawnflags & 4) {
			G_AddEvent(ent, EV_GLOBAL_SOUND, ent->noise_index);
		} else {
			G_AddEvent(ent, EV_GENERAL_SOUND, ent->noise_index);
		}
	}
}

/*
=======================================================================================================================================
Target_Speaker_Multiple_Think
=======================================================================================================================================
*/
void Target_Speaker_Multiple_Think(gentity_t *ent) {
	gentity_t *vis_dummy = NULL;

	if (!ent->target) {
		G_Error("target_speaker missing target at pos %s", VectorToString(ent->s.origin));
	}

	vis_dummy = G_PickTarget(ent->target);

	if (vis_dummy) {
		ent->r.visDummyNum = vis_dummy->s.number;
	} else {
		G_Error("target_speaker cant find vis_dummy_multiple %s", VectorToString(ent->s.origin));
	}
}

/*QUAKED target_speaker (1 0 0) (-8 -8 -8) (8 8 8) LOOPED_ON LOOPED_OFF GLOBAL ACTIVATOR VIS_MULTIPLE NO_PVS
"noise"		wav file to play

A global sound will play full volume throughout the level.
Activator sounds will play on the player that activated the target.
Global and activator sounds can't be combined with looping.
Normal sounds play each time the target is used.
Looped sounds will be toggled by use functions.
Multiple identical looping sounds will just increase volume without any speed cost.
NO_PVS - this sound will not turn off when not in the player's PVS.
"wait" : Seconds between auto triggerings, 0 = don't auto trigger.
"random" wait variance, default is 0.
"radius" radius control, 128 is default.
*/
void SP_target_speaker(gentity_t *ent) {
	char buffer[MAX_QPATH];
	char *s;

	G_SpawnFloat("wait", "0", &ent->wait);
	G_SpawnFloat("random", "0", &ent->random);

	if (!G_SpawnString("noise", "NOSOUND", &s)) {
		G_Error("target_speaker without a noise key at %s", VectorToString(ent->s.origin));
	}
	// force all client relative sounds to be "activator" speakers that play on the entity that activates it
	if (s[0] == '*') {
		ent->spawnflags |= 8;
	}

	if (!strstr(s, ".wav")) {
		Com_sprintf(buffer, sizeof(buffer), "%s.wav", s);
	} else {
		Q_strncpyz(buffer, s, sizeof(buffer));
	}

	ent->noise_index = G_SoundIndex(buffer);
	// a repeating speaker can be done completely client side
	ent->s.eType = ET_SPEAKER;
	ent->s.eventParm = ent->noise_index;
	ent->s.frame = ent->wait * 10;
	ent->s.clientNum = ent->random * 10;
	// check for prestarted looping sound
	if (ent->spawnflags & 1) {
		ent->s.loopSound = ent->noise_index;
	}

	ent->use = Use_Target_Speaker;
	// GLOBAL
	if (ent->spawnflags & 4) {
		ent->r.svFlags |= SVF_BROADCAST;
	}

	if (ent->spawnflags & 16) {
		ent->think = Target_Speaker_Multiple_Think;
		ent->nextthink = level.time + 50;
	}
	// radius control
	G_SpawnInt("radius", "0", &ent->s.soundRange);

	if (!ent->s.soundRange) {
		ent->s.soundRange = SOUND_RANGE_DEFAULT;
	}

	VectorCopy(ent->s.origin, ent->s.pos.trBase);
	// must link the entity so we get areas and clusters so the server can determine who to send updates to
	trap_LinkEntity(ent);
}

/*
=======================================================================================================================================
Target_Laser_Think
=======================================================================================================================================
*/
void Target_Laser_Think(gentity_t *self) {
	vec3_t end;
	trace_t tr;
	vec3_t point;

	// if pointed at another entity, set movedir to point at it
	if (self->enemy) {
		VectorMA(self->enemy->s.origin, 0.5, self->enemy->r.mins, point);
		VectorMA(point, 0.5, self->enemy->r.maxs, point);
		VectorSubtract(point, self->s.origin, self->movedir);
		VectorNormalize(self->movedir);
	}
	// fire forward and see what we hit
	VectorMA(self->s.origin, 2048, self->movedir, end);
	trap_Trace(&tr, self->s.origin, NULL, NULL, end, self->s.number, CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE);

	if (tr.entityNum) {
		// hurt it if we can
		G_Damage(&g_entities[tr.entityNum], self, self->activator, self->movedir, tr.endpos, self->damage, DAMAGE_NO_KNOCKBACK, MOD_TARGET_LASER);
	}

	VectorCopy(tr.endpos, self->s.origin2);
	trap_LinkEntity(self);

	self->nextthink = level.time + FRAMETIME;
}

/*
=======================================================================================================================================
Target_Laser_On
=======================================================================================================================================
*/
void Target_Laser_On(gentity_t *self) {

	if (!self->activator) {
		self->activator = self;
	}

	Target_Laser_Think(self);
}

/*
=======================================================================================================================================
Target_Laser_Off
=======================================================================================================================================
*/
void Target_Laser_Off(gentity_t *self) {

	trap_UnlinkEntity(self);

	self->nextthink = 0;
}

/*
=======================================================================================================================================
Use_Target_Laser
=======================================================================================================================================
*/
void Use_Target_Laser(gentity_t *self, gentity_t *other, gentity_t *activator) {

	self->activator = activator;

	if (self->nextthink > 0) {
		Target_Laser_Off(self);
	} else {
		Target_Laser_On(self);
	}
}

/*
=======================================================================================================================================
Target_Laser_Start
=======================================================================================================================================
*/
void Target_Laser_Start(gentity_t *self) {
	gentity_t *ent;

	self->s.eType = ET_BEAM;

	if (self->target) {
		ent = G_Find(NULL, FOFS(targetname), self->target);

		if (!ent) {
			G_Printf("%s at %s: %s is a bad target\n", self->classname, VectorToString(self->s.origin), self->target);
		}

		self->enemy = ent;
	} else {
		G_SetMovedir(self->s.angles, self->movedir);
	}

	self->use = Use_Target_Laser;
	self->think = Target_Laser_Think;

	if (!self->damage) {
		self->damage = 1;
	}

	if (self->spawnflags & 1) {
		Target_Laser_On(self);
	} else {
		Target_Laser_Off(self);
	}
}

/*QUAKED target_laser (0 .5 .8) (-8 -8 -8) (8 8 8) START_ON
When triggered, fires a laser. You can either set a target or a direction.
*/
void SP_target_laser(gentity_t *self) {

	// let everything else get spawned before we start firing
	self->think = Target_Laser_Start;
	self->nextthink = level.time + FRAMETIME;
}

/*
=======================================================================================================================================
Use_Target_Teleporter
=======================================================================================================================================
*/
void Use_Target_Teleporter(gentity_t *self, gentity_t *other, gentity_t *activator) {
	gentity_t *dest;

	if (!activator->client) {
		return;
	}

	dest = G_PickTarget(self->target);

	if (!dest) {
		G_Printf("Couldn't find teleporter destination\n");
		return;
	}

	TeleportPlayer(activator, dest->s.origin, dest->s.angles);
}

/*QUAKED target_teleporter (1 0 0) (-8 -8 -8) (8 8 8)
The activator will be teleported away.
*/
void SP_target_teleporter(gentity_t *self) {

	if (!self->targetname) {
		G_Printf("untargeted %s at %s\n", self->classname, VectorToString(self->s.origin));
	}

	self->use = Use_Target_Teleporter;
}

/*
=======================================================================================================================================
Use_Target_Relay
=======================================================================================================================================
*/
void Use_Target_Relay(gentity_t *self, gentity_t *other, gentity_t *activator) {

	if ((self->spawnflags & 1) && activator->client && activator->client->sess.sessionTeam != TEAM_RED) {
		return;
	}

	if ((self->spawnflags & 2) && activator->client && activator->client->sess.sessionTeam != TEAM_BLUE) {
		return;
	}

	if (self->spawnflags & 4) {
		gentity_t *ent;

		ent = G_PickTarget(self->target);

		if (ent && ent->use) {
			ent->use(ent, self, activator);
		}

		return;
	}

	G_UseTargets(self, activator);
}

/*QUAKED target_relay (.5 .5 .5) (-8 -8 -8) (8 8 8) RED_ONLY BLUE_ONLY RANDOM
This doesn't perform any actions except fire its targets.
The activator can be forced to be from a certain team.
if RANDOM is checked, only one of the targets will be fired, not all of them
*/
void SP_target_relay(gentity_t *self) {
	self->use = Use_Target_Relay;
}

/*
=======================================================================================================================================
Use_Target_Kill
=======================================================================================================================================
*/
void Use_Target_Kill(gentity_t *self, gentity_t *other, gentity_t *activator) {
	G_Damage(activator, NULL, NULL, NULL, NULL, 100000, DAMAGE_NO_PROTECTION, MOD_TELEFRAG);
}

/*QUAKED target_kill (.5 .5 .5) (-8 -8 -8) (8 8 8)
Kills the activator.
*/
void SP_target_kill(gentity_t *self) {
	self->use = Use_Target_Kill;
}

/*QUAKED target_position (0 0.5 0) (-4 -4 -4) (4 4 4)
Used as a positional target for in-game calculation, like jumppad targets.
*/
void SP_target_position(gentity_t *self) {
	G_SetOrigin(self, self->s.origin);
}

/*
=======================================================================================================================================
Target_Location_Linkup
=======================================================================================================================================
*/
static void Target_Location_Linkup(gentity_t *ent) {
	int i;
	int n;

	if (level.locationLinked) {
		return;
	}

	level.locationLinked = qtrue;
	level.locationHead = NULL;

	trap_SetConfigstring(CS_LOCATIONS, "unknown");

	for (i = 0, ent = g_entities, n = 1; i < level.num_entities; i++, ent++) {
		if (ent->classname && !Q_stricmp(ent->classname, "target_location")) {
			// lets overload some variables!
			ent->health = n; // use for location marking
			trap_SetConfigstring(CS_LOCATIONS + n, ent->message);
			n++;
			ent->nextTrain = level.locationHead;
			level.locationHead = ent;
		}
	}
	// all linked together now
}

/*QUAKED target_location (0 0.5 0) (-8 -8 -8) (8 8 8)
Set "message" to the name of this location.
Set "count" to 0-7 for color.
	0: white
	1: red
	2: green
	3: yellow
	4: blue
	5: cyan
	6: magenta
	7: white

Closest target_location in sight used for the location, if none
in site, closest in distance
*/
void SP_target_location(gentity_t *self) {

	self->think = Target_Location_Linkup;
	self->nextthink = level.time + 200; // let them all spawn first

	G_SetOrigin(self, self->s.origin);
}
