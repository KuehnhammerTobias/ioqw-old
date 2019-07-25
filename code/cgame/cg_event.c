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

/**************************************************************************************************************************************
 Handle entity events at snapshot or playerstate transitions.
**************************************************************************************************************************************/

#include "cg_local.h"
#include "../../ui/menudef.h" // for the voice chats

/*
=======================================================================================================================================
CG_PlaceString

Also called by scoreboard drawing.
=======================================================================================================================================
*/
const char *CG_PlaceString(int rank) {
	static char str[64];
	char *s, *t;

	if (rank & RANK_TIED_FLAG) {
		rank &= ~RANK_TIED_FLAG;
		t = "Tied for ";
	} else {
		t = "";
	}

	if (rank == 1) {
		s = S_COLOR_BLUE "1st" S_COLOR_WHITE; // draw in blue
	} else if (rank == 2) {
		s = S_COLOR_RED "2nd" S_COLOR_WHITE; // draw in red
	} else if (rank == 3) {
		s = S_COLOR_YELLOW "3rd" S_COLOR_WHITE; // draw in yellow
	} else if (rank == 11) {
		s = "11th";
	} else if (rank == 12) {
		s = "12th";
	} else if (rank == 13) {
		s = "13th";
	} else if (rank % 10 == 1) {
		s = va("%ist", rank);
	} else if (rank % 10 == 2) {
		s = va("%ind", rank);
	} else if (rank % 10 == 3) {
		s = va("%ird", rank);
	} else {
		s = va("%ith", rank);
	}

	Com_sprintf(str, sizeof(str), "%s%s", t, s);
	return str;
}

/*
=======================================================================================================================================
CG_Obituary
=======================================================================================================================================
*/
static void CG_Obituary(entityState_t *ent) {
	int mod;
	int target, attacker;
	char *message;
	char *message2;
	char targetName[32];
	char attackerName[32];
	gender_t targetgender, attackergender;
	clientInfo_t *ci, *ca;

	target = ent->otherEntityNum;
	attacker = ent->otherEntityNum2;
	mod = ent->eventParm;

	if (target < 0 || target >= MAX_CLIENTS) {
		CG_Error("CG_Obituary: target out of range");
	}

	ci = &cgs.clientinfo[target];

	if (!ci) {
		return;
	}

	if (attacker < 0 || attacker >= MAX_CLIENTS) {
		ca = NULL;
		attacker = ENTITYNUM_WORLD;
	} else {
		ca = &cgs.clientinfo[attacker];
	}

	Q_strncpyz(targetName, ci->name, sizeof(targetName) - 2);
	strcat(targetName, S_COLOR_WHITE);

	targetgender = ci->gender;
	attackergender = ca->gender;
	message2 = "";
	// check for single client messages
	switch (mod) {
		case MOD_WATER:
			message = "sank like a rock";
			break;
		case MOD_SLIME:
			message = "melted";
			break;
		case MOD_LAVA:
			message = "does a back flip into the lava";
			break;
		case MOD_FALLING:
			message = "cratered";
			break;
		case MOD_TRIGGER_HURT:
			message = "was in the wrong place";
			break;
		case MOD_CRUSH:
			message = "was squished";
			break;
		case MOD_TARGET_LASER:
			message = "saw the light";
			break;
		case MOD_SUICIDE:
			message = "suicides";
			break;
		default:
			message = NULL;
			break;
	}

	if (attacker == target) {
		switch (mod) {
			case MOD_PROXIMITY_MINE:
				if (targetgender == GENDER_FEMALE) {
					message = "found her prox mine";
				} else if (targetgender == GENDER_NEUTER) {
					message = "found its prox mine";
				} else {
					message = "found his prox mine";
				}

				break;
			case MOD_GRENADE_SPLASH:
				if (targetgender == GENDER_FEMALE) {
					message = "tripped on her own grenade";
				} else if (targetgender == GENDER_NEUTER) {
					message = "tripped on its own grenade";
				} else {
					message = "tripped on his own grenade";
				}

				break;
			case MOD_NAPALM_SPLASH:
				if (targetgender == GENDER_FEMALE) {
					message = "set herself alight";
				} else if (targetgender == GENDER_NEUTER) {
					message = "set itself alight";
				} else {
					message = "set himself alight";
				}

				break;
			case MOD_ROCKET_SPLASH:
				if (targetgender == GENDER_FEMALE) {
					message = "blew herself up";
				} else if (targetgender == GENDER_NEUTER) {
					message = "blew itself up";
				} else {
					message = "blew himself up";
				}

				break;
			case MOD_PLASMA_SPLASH:
				if (targetgender == GENDER_FEMALE) {
					message = "melted herself";
				} else if (targetgender == GENDER_NEUTER) {
					message = "melted itself";
				} else {
					message = "melted himself";
				}

				break;
			case MOD_BFG_SPLASH:
				message = "should have used a smaller gun";
				break;
			case MOD_KAMIKAZE:
				message = "goes out with a bang";
				break;
			default:
				if (targetgender == GENDER_FEMALE) {
					message = "killed herself";
				} else if (targetgender == GENDER_NEUTER) {
					message = "killed itself";
				} else {
					message = "killed himself";
				}

				break;
		}
	}

	if (message) {
		CG_Printf("%s %s.\n", targetName, message);
		return;
	}
	// check for kill messages from the current clientNum
	if (attacker == cg.snap->ps.clientNum) {
		char *s;

		if (cgs.gametype < GT_TEAM) {
			s = va("You fragged %s\n%s place with %i", targetName, CG_PlaceString(cg.snap->ps.persistant[PERS_RANK] + 1), cg.snap->ps.persistant[PERS_SCORE]);
		} else {
			s = va("You fragged %s", targetName);
		}

		if (!(cg_singlePlayer.integer && cg_cameraOrbit.integer)) {
			CG_CenterPrint(s, SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH);
		}
		// print the text message as well
	}
	// check for double client messages
	if (!ca) {
		attacker = ENTITYNUM_WORLD;
		strcpy(attackerName, "Noname");
	} else {
		Q_strncpyz(attackerName, ca->name, sizeof(attackerName) - 2);
		strcat(attackerName, S_COLOR_WHITE);
		// check for kill messages about the current clientNum
		if (target == cg.snap->ps.clientNum) {
			Q_strncpyz(cg.killerName, attackerName, sizeof(cg.killerName));
		}
	}

	if (attacker != ENTITYNUM_WORLD) {
		switch (mod) {
			case MOD_GAUNTLET:
				if (targetgender == GENDER_FEMALE && attackergender == GENDER_FEMALE) {
					message = "was fisted by";
				} else {
					message = "was pummeled by";
				}

				break;
			case MOD_MACHINEGUN:
				message = "was machinegunned by";
				break;
			case MOD_CHAINGUN:
				message = "got lead poisoning from";
				message2 = "'s chain gun";
				break;
			case MOD_SHOTGUN:
				message = "was gunned down by";
				break;
			case MOD_NAIL:
				message = "was nailed by";
				break;
			case MOD_PROXIMITY_MINE:
				message = "was too close to";
				message2 = "'s prox mine";
				break;
			case MOD_GRENADE_SPLASH:
				message = "was shredded by";
				message2 = "'s shrapnel";
				break;
			case MOD_NAPALM:
			case MOD_NAPALM_SPLASH:
				message = "roasted to a nice golden brown by";
				message2 = "'s napalm launcher";
				break;
			case MOD_ROCKET:
				message = "ate";
				message2 = "'s rocket";
				break;
			case MOD_ROCKET_SPLASH:
				message = "almost dodged";
				message2 = "'s rocket";
				break;
			case MOD_BEAMGUN:
				message = "was lacerated by";
				break;
			case MOD_RAILGUN:
				message = "was railed by";
				break;
			case MOD_PLASMA:
			case MOD_PLASMA_SPLASH:
				message = "was melted by";
				message2 = "'s plasma gun";
				break;
			case MOD_BFG:
				message = "was vaporized by";
				message2 = "'s BFG";
				break;
			case MOD_BFG_SPLASH:
				message = "was blasted by";
				message2 = "'s BFG";
				break;
			case MOD_KAMIKAZE:
				message = "falls to";
				message2 = "'s Kamikaze blast";
				break;
			case MOD_TELEFRAG:
				message = "tried to invade";
				message2 = "'s personal space";
				break;
			case MOD_GOOMBA:
				if (attackergender == GENDER_FEMALE) {
					message = "was crushed by";
					message2 = "'s butt";
				} else {
					message = "experienced death from above from";
					message2 = "";
				}

				break;
			default:
				message = "was killed by";
				break;
		}

		if (message) {
			CG_Printf("%s %s %s%s.\n", targetName, message, attackerName, message2);
			return;
		}
	}
	// we don't know what it was
	CG_Printf("%s died.\n", targetName);
}

/*
=======================================================================================================================================
CG_UseItem
=======================================================================================================================================
*/
static void CG_UseItem(centity_t *cent) {
	clientInfo_t *ci;
	int itemNum, clientNum;
	gitem_t *item;
	entityState_t *es;

	es = &cent->currentState;
	itemNum = (es->event & ~EV_EVENT_BITS) - EV_USE_ITEM0;

	if (itemNum < 0 || itemNum > HI_NUM_HOLDABLE) {
		itemNum = 0;
	}
	// print a message if the local player
	if (es->number == cg.snap->ps.clientNum) {
		if (!itemNum) {
			CG_CenterPrint("No item to use", SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH);
		} else {
			item = BG_FindItemForHoldable(itemNum);
			CG_CenterPrint(va("Use %s", item->pickup_name), SCREEN_HEIGHT * 0.30, BIGCHAR_WIDTH);
		}
	}

	switch (itemNum) {
		default:
		case HI_NONE:
			trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.useNothingSound, 32);
			break;
		case HI_MEDKIT:
			clientNum = cent->currentState.clientNum;

			if (clientNum >= 0 && clientNum < MAX_CLIENTS) {
				ci = &cgs.clientinfo[clientNum];
				ci->medkitUsageTime = cg.time;
			}

			trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.medkitSound, 32);
			break;
		case HI_KAMIKAZE:
			break;
	}
}

/*
=======================================================================================================================================
CG_ItemPickup

A new item was picked up this frame.
=======================================================================================================================================
*/
static void CG_ItemPickup(int itemNum) {
#ifndef BASEGAME
	cg.itemPickup = itemNum;
	cg.itemPickupTime = cg.time;
	cg.itemPickupBlendTime = cg.time;
#endif
	// see if it should be the grabbed weapon
	if (bg_itemlist[itemNum].giType == IT_WEAPON) {
		// select it immediately
		if (cg_autoswitch.integer && bg_itemlist[itemNum].giTag != WP_MACHINEGUN) {
			cg.weaponSelectTime = cg.time;
			cg.weaponSelect = bg_itemlist[itemNum].giTag;
		}
	}
}

/*
=======================================================================================================================================
CG_WaterLevel

Returns waterlevel for entity origin.
=======================================================================================================================================
*/
int CG_WaterLevel(centity_t *cent) {
	vec3_t point;
	int contents, sample1, sample2, anim, waterlevel;
	int viewheight;

	anim = cent->currentState.legsAnim & ~ANIM_TOGGLEBIT;

	if (anim == LEGS_WALKCR || anim == LEGS_IDLECR) {
		viewheight = CROUCH_VIEWHEIGHT;
	} else {
		viewheight = DEFAULT_VIEWHEIGHT;
	}
	// get waterlevel, accounting for ducking
	waterlevel = 0;

	point[0] = cent->lerpOrigin[0];
	point[1] = cent->lerpOrigin[1];
	point[2] = cent->lerpOrigin[2] + MINS_Z + 1;
	contents = CG_PointContents(point, -1);

	if (contents & MASK_WATER) {
		sample2 = viewheight - MINS_Z;
		sample1 = sample2 / 2;
		waterlevel = 1;
		point[2] = cent->lerpOrigin[2] + MINS_Z + sample1;
		contents = CG_PointContents(point, -1);

		if (contents & MASK_WATER) {
			waterlevel = 2;
			point[2] = cent->lerpOrigin[2] + MINS_Z + sample2;
			contents = CG_PointContents(point, -1);

			if (contents & MASK_WATER) {
				waterlevel = 3;
			}
		}
	}

	return waterlevel;
}

/*
=======================================================================================================================================
CG_PainEvent

Also called by playerstate transition.
=======================================================================================================================================
*/
void CG_PainEvent(centity_t *cent, int health) {
	char *snd;

	// don't do more than two pain sounds a second
	if (cg.time - cent->pe.painTime < 500) {
		return;
	}
	// default pain sounds
	if (health < 25) {
		snd = "*pd4.wav";
	} else if (health < 50) {
		snd = "*pd3.wav";
	} else if (health < 75) {
		snd = "*pd2.wav";
	} else {
		snd = "*pd1.wav";
	}
	// play a gurp sound instead of a normal pain sound
	if (CG_WaterLevel(cent) == 3) {
		if (rand()&1) {
			trap_S_StartSound(NULL, cent->currentState.number, CHAN_VOICE, CG_CustomSound(cent->currentState.number, "snd/c/gu1.wav"), 64);
		} else {
			trap_S_StartSound(NULL, cent->currentState.number, CHAN_VOICE, CG_CustomSound(cent->currentState.number, "snd/c/gu2.wav"), 64);
		}
	} else {
		trap_S_StartSound(NULL, cent->currentState.number, CHAN_VOICE, CG_CustomSound(cent->currentState.number, snd), 64);
	}
	// save pain time for programitic twitch animation
	cent->pe.painTime = cg.time;
	cent->pe.painDirection ^= 1;
}

/*
=======================================================================================================================================
CG_EntityEvent

An entity has an event value. Also called by CG_CheckPlayerstateEvents.
=======================================================================================================================================
*/
#define DEBUGNAME(x) if (cg_debugEvents.integer) {CG_Printf(x"\n");}
void CG_EntityEvent(centity_t *cent, vec3_t position) {
	entityState_t *es;
	int event;
	vec3_t dir;
	const char *s;
	int clientNum;
	clientInfo_t *ci;

	es = &cent->currentState;
	event = es->event & ~EV_EVENT_BITS;

	if (cg_debugEvents.integer) {
		CG_Printf("ent:%3i  event:%3i ", es->number, event);
	}

	if (!event) {
		DEBUGNAME("ZEROEVENT");
		return;
	}

	clientNum = es->clientNum;

	if (clientNum < 0 || clientNum >= MAX_CLIENTS) {
		clientNum = 0;
	}

	ci = &cgs.clientinfo[clientNum];

	switch (event) {
		case EV_FIRE_WEAPON:
			DEBUGNAME("EV_FIRE_WEAPON");
			CG_FireWeapon(cent);
			break;
		// bullet impacts
		case EV_BULLET_HIT_FLESH:
			DEBUGNAME("EV_BULLET_HIT_FLESH");
			CG_Bullet(es->pos.trBase, es->otherEntityNum, dir, qtrue, es->eventParm);
			break;
		case EV_BULLET_HIT_WALL:
			DEBUGNAME("EV_BULLET_HIT_WALL");
			ByteToDir(es->eventParm, dir);
			CG_Bullet(es->pos.trBase, es->otherEntityNum, dir, qfalse, ENTITYNUM_WORLD);
			break;
		// missile impacts
		case EV_MISSILE_HIT:
			DEBUGNAME("EV_MISSILE_HIT");
			ByteToDir(es->eventParm, dir);
			CG_MissileHitPlayer(es->weapon, position, dir, es->otherEntityNum);
			break;
		case EV_MISSILE_MISS:
			DEBUGNAME("EV_MISSILE_MISS");
			ByteToDir(es->eventParm, dir);
			CG_MissileHitWall(es->weapon, 0, position, dir);
			break;
		case EV_RAILTRAIL:
			DEBUGNAME("EV_RAILTRAIL");
			cent->currentState.weapon = WP_RAILGUN;

			if (es->clientNum == cg.snap->ps.clientNum && !cg.renderingThirdPerson) {
				if (cg_drawGun.integer == 2) {
					VectorMA(es->origin2, 8, cg.refdef.viewaxis[1], es->origin2);
				} else if (cg_drawGun.integer == 3) {
					VectorMA(es->origin2, 4, cg.refdef.viewaxis[1], es->origin2);
				}
			}

			CG_RailTrail(ci, es->origin2, es->pos.trBase);
			// if the end was on a nomark surface, don't make an explosion
			if (es->eventParm != 255) {
				ByteToDir(es->eventParm, dir);
				CG_MissileHitWall(es->weapon, clientNum, position, dir);
			}

			break;
		case EV_SHOTGUN:
			DEBUGNAME("EV_SHOTGUN");
			CG_ShotgunFire(es);
			break;
		case EV_PROXIMITY_MINE_STICK:
			DEBUGNAME("EV_PROXIMITY_MINE_STICK");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.wstbimpdSound, 64);
			break;
		case EV_PROXIMITY_MINE_TRIGGER:
			DEBUGNAME("EV_PROXIMITY_MINE_TRIGGER");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.wstbactvSound, 64);
			break;
		case EV_GRENADE_BOUNCE:
			DEBUGNAME("EV_GRENADE_BOUNCE");

			if (rand() & 1) {
				trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.hgrenb1aSound, 64);
			} else {
				trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.hgrenb2aSound, 64);
			}

			break;
		case EV_KAMIKAZE:
			DEBUGNAME("EV_KAMIKAZE");
			CG_KamikazeEffect(cent->lerpOrigin);
			break;
		case EV_OBELISKEXPLODE:
			DEBUGNAME("EV_OBELISKEXPLODE");
			CG_ObeliskExplode(cent->lerpOrigin, es->eventParm);
			break;
		case EV_OBELISKPAIN:
			DEBUGNAME("EV_OBELISKPAIN");
			CG_ObeliskPain(cent->lerpOrigin);
			break;
		case EV_STOPLOOPINGSOUND:
			DEBUGNAME("EV_STOPLOOPINGSOUND");
			trap_S_StopLoopingSound(es->number);
			es->loopSound = 0;
			break;
		case EV_GENERAL_SOUND:
			DEBUGNAME("EV_GENERAL_SOUND");

			if (cgs.gameSounds[es->eventParm]) {
				trap_S_StartSound(NULL, es->number, CHAN_VOICE, cgs.gameSounds[es->eventParm], es->soundRange);
			} else {
				s = CG_ConfigString(CS_SOUNDS + es->eventParm);
				trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, s), es->soundRange);
			}

			break;
		case EV_GLOBAL_SOUND: // play as a local sound so it never diminishes
			DEBUGNAME("EV_GLOBAL_SOUND");

			if (cgs.gameSounds[es->eventParm]) {
				trap_S_StartLocalSound(cgs.gameSounds[es->eventParm], CHAN_AUTO);
			} else {
				s = CG_ConfigString(CS_SOUNDS + es->eventParm);
				trap_S_StartLocalSound(CG_CustomSound(es->number, s), CHAN_AUTO);
			}

			break;
		case EV_GLOBAL_TEAM_SOUND: // play as a local sound so it never diminishes
			{
				DEBUGNAME("EV_GLOBAL_TEAM_SOUND");
				switch (es->eventParm) {
					case GTS_RED_CAPTURE: // CTF: red team captured the blue flag, 1FCTF: red team captured the neutral flag
						if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
							trap_S_StartLocalSound(cgs.media.captureYourTeamSound, CHAN_LOCAL_SOUND);
						} else {
							trap_S_StartLocalSound(cgs.media.captureOpponentSound, CHAN_LOCAL_SOUND);
						}

						break;
					case GTS_BLUE_CAPTURE: // CTF: blue team captured the red flag, 1FCTF: blue team captured the neutral flag
						if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
							trap_S_StartLocalSound(cgs.media.captureYourTeamSound, CHAN_LOCAL_SOUND);
						} else {
							trap_S_StartLocalSound(cgs.media.captureOpponentSound, CHAN_LOCAL_SOUND);
						}

						break;
					case GTS_RED_RETURN: // CTF: blue flag returned, 1FCTF: never used
						if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
							trap_S_StartLocalSound(cgs.media.returnYourTeamSound, CHAN_LOCAL_SOUND);
						} else {
							trap_S_StartLocalSound(cgs.media.returnOpponentSound, CHAN_LOCAL_SOUND);
						}

						CG_AddBufferedAnnouncerSound(cgs.media.blueFlagReturnedSound);
						break;
					case GTS_BLUE_RETURN: // CTF red flag returned, 1FCTF: neutral flag returned
						if (cgs.gametype == GT_1FCTF) {
							trap_S_StartLocalSound(cgs.media.returnOpponentSound, CHAN_LOCAL_SOUND);
							CG_AddBufferedAnnouncerSound(cgs.media.neutralFlagReturnedSound);
						} else {
							if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
								trap_S_StartLocalSound(cgs.media.returnYourTeamSound, CHAN_LOCAL_SOUND);
							} else {
								trap_S_StartLocalSound(cgs.media.returnOpponentSound, CHAN_LOCAL_SOUND);
							}

							CG_AddBufferedAnnouncerSound(cgs.media.redFlagReturnedSound);
						}

						break;
					case GTS_RED_TAKEN: // CTF: red team took blue flag, 1FCTF: blue team took the neutral flag
						// if this player picked up the flag then a sound is played in CG_CheckLocalSounds
						if (cg.snap->ps.powerups[PW_BLUEFLAG] || cg.snap->ps.powerups[PW_NEUTRALFLAG]) {

						} else {
							if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
								if (cgs.gametype == GT_1FCTF) {
									trap_S_StartLocalSound(cgs.media.takenYourTeamSound, CHAN_LOCAL_SOUND);
									CG_AddBufferedAnnouncerSound(cgs.media.yourTeamTookTheFlagSound);
								} else {
									trap_S_StartLocalSound(cgs.media.takenOpponentSound, CHAN_LOCAL_SOUND);
									CG_AddBufferedAnnouncerSound(cgs.media.enemyTookYourFlagSound);
								}
							} else if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
								if (cgs.gametype == GT_1FCTF) {
									trap_S_StartLocalSound(cgs.media.takenOpponentSound, CHAN_LOCAL_SOUND);
									CG_AddBufferedAnnouncerSound(cgs.media.enemyTookTheFlagSound);
								} else {
									trap_S_StartLocalSound(cgs.media.takenYourTeamSound, CHAN_LOCAL_SOUND);
									CG_AddBufferedAnnouncerSound(cgs.media.yourTeamTookEnemyFlagSound);
								}
							}
						}

						break;
					case GTS_BLUE_TAKEN: // CTF: blue team took the red flag, 1FCTF red team took the neutral flag
						// if this player picked up the flag then a sound is played in CG_CheckLocalSounds
						if (cg.snap->ps.powerups[PW_REDFLAG] || cg.snap->ps.powerups[PW_NEUTRALFLAG]) {

						} else {
							if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
								if (cgs.gametype == GT_1FCTF) {
									trap_S_StartLocalSound(cgs.media.takenYourTeamSound, CHAN_LOCAL_SOUND);
									CG_AddBufferedAnnouncerSound(cgs.media.yourTeamTookTheFlagSound);
								} else {
									trap_S_StartLocalSound(cgs.media.takenOpponentSound, CHAN_LOCAL_SOUND);
									CG_AddBufferedAnnouncerSound(cgs.media.enemyTookYourFlagSound);
								}
							} else if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
								if (cgs.gametype == GT_1FCTF) {
									trap_S_StartLocalSound(cgs.media.takenOpponentSound, CHAN_LOCAL_SOUND);
									CG_AddBufferedAnnouncerSound(cgs.media.enemyTookTheFlagSound);
								} else {
									trap_S_StartLocalSound(cgs.media.takenYourTeamSound, CHAN_LOCAL_SOUND);
									CG_AddBufferedAnnouncerSound(cgs.media.yourTeamTookEnemyFlagSound);
								}
							}
						}

						break;
					case GTS_REDOBELISK_ATTACKED: // Overload: red obelisk is being attacked
						if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_RED) {
							CG_AddBufferedAnnouncerSound(cgs.media.yourBaseIsUnderAttackSound);
						}

						break;
					case GTS_BLUEOBELISK_ATTACKED: // Overload: blue obelisk is being attacked
						if (cg.snap->ps.persistant[PERS_TEAM] == TEAM_BLUE) {
							CG_AddBufferedAnnouncerSound(cgs.media.yourBaseIsUnderAttackSound);
						}

						break;
					case GTS_REDTEAM_SCORED:
						CG_AddBufferedAnnouncerSound(cgs.media.redScoredSound);
						break;
					case GTS_BLUETEAM_SCORED:
						CG_AddBufferedAnnouncerSound(cgs.media.blueScoredSound);
						break;
					case GTS_REDTEAM_TOOK_LEAD:
						CG_AddBufferedAnnouncerSound(cgs.media.redLeadsSound);
						break;
					case GTS_BLUETEAM_TOOK_LEAD:
						CG_AddBufferedAnnouncerSound(cgs.media.blueLeadsSound);
						break;
					case GTS_TEAMS_ARE_TIED:
						CG_AddBufferedAnnouncerSound(cgs.media.teamsTiedSound);
						break;
					case GTS_KAMIKAZE:
						trap_S_StartLocalSound(cgs.media.kamikazeFarSound, CHAN_AUTO);
						break;
					default:
						break;
				}

				break;
			}
		case EV_PAIN:
			// local player sounds are triggered in CG_CheckLocalSounds, so ignore events on the player
			DEBUGNAME("EV_PAIN");

			if (cent->currentState.number != cg.snap->ps.clientNum) {
				CG_PainEvent(cent, es->eventParm);
			}

			break;
		case EV_DEATH1:
		case EV_DEATH2:
		case EV_DEATH3:
			DEBUGNAME("EV_DEATHx");

			if (CG_WaterLevel(cent) == 3) {
				trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*dr1.wav"), 48);
			} else {
				trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, va("*dd%i.wav", event - EV_DEATH1 + 1)), 64);
			}

			break;
		case EV_OBITUARY:
			DEBUGNAME("EV_OBITUARY");
			CG_Obituary(es);
			break;
		case EV_GIB_PLAYER:
			DEBUGNAME("EV_GIB_PLAYER");
			// don't play gib sound when using the kamikaze because it interferes with the kamikaze sound, downside is that the gib sound
			// will also not be played when someone is gibbed while just carrying the kamikaze
			if (!(es->eFlags & EF_KAMIKAZE)) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.gibSound[rand()&11], 64);
			}

			CG_GibPlayer(cent->lerpOrigin);
			break;
		case EV_FALL_DIE:
			DEBUGNAME("EV_FALL_DIE");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, CG_CustomSound(es->number, "*df1.wav"), 64);

			cent->pe.painTime = cg.time; // don't play a pain sound right after this
			break;
		case EV_FALL_DMG_50:
			DEBUGNAME("EV_FALL_DMG_50");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, CG_CustomSound(es->number, "*ff5.wav"), 64);

			cent->pe.painTime = cg.time; // don't play a pain sound right after this

			if (clientNum == cg.predictedPlayerState.clientNum) {
				// smooth landing z changes
				cg.landChange = -24;
				cg.landTime = cg.time;
			}

			break;
		case EV_FALL_DMG_25:
			DEBUGNAME("EV_FALL_DMG_25");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, CG_CustomSound(es->number, "*ff4.wav"), 64);

			cent->pe.painTime = cg.time; // don't play a pain sound right after this

			if (clientNum == cg.predictedPlayerState.clientNum) {
				// smooth landing z changes
				cg.landChange = -24;
				cg.landTime = cg.time;
			}

			break;
		case EV_FALL_DMG_15:
			DEBUGNAME("EV_FALL_DMG_15");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, CG_CustomSound(es->number, "*ff3.wav"), 64);

			cent->pe.painTime = cg.time; // don't play a pain sound right after this

			if (clientNum == cg.predictedPlayerState.clientNum) {
				// smooth landing z changes
				cg.landChange = -16;
				cg.landTime = cg.time;
			}

			break;
		case EV_FALL_DMG_10:
			DEBUGNAME("EV_FALL_DMG_10");
			// use normal pain sound
			trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*ff2.wav"), 64);

			cent->pe.painTime = cg.time; // don't play a pain sound right after this

			if (clientNum == cg.predictedPlayerState.clientNum) {
				// smooth landing z changes
				cg.landChange = -16;
				cg.landTime = cg.time;
			}

			break;
		case EV_FALL_DMG_5:
			DEBUGNAME("EV_FALL_DMG_5");
			// use normal pain sound
			trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*ff1.wav"), 64);

			cent->pe.painTime = cg.time; // don't play a pain sound right after this

			if (clientNum == cg.predictedPlayerState.clientNum) {
				// smooth landing z changes
				cg.landChange = -16;
				cg.landTime = cg.time;
			}

			break;
		case EV_FALL_SHORT:
			DEBUGNAME("EV_FALL_SHORT");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.landSound, 64);

			if (clientNum == cg.predictedPlayerState.clientNum) {
				// smooth landing z changes
				cg.landChange = -8;
				cg.landTime = cg.time;
			}

			break;
		case EV_JUMP:
			DEBUGNAME("EV_JUMP");
			trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*jd1.wav"), 64);
			break;
		case EV_JUMP_PAD:
			DEBUGNAME("EV_JUMP_PAD");
			//CG_Printf("EV_JUMP_PAD w/effect #%i\n", es->eventParm);
			{
				vec3_t up = {0, 0, 1};

				CG_SmokePuff(cent->lerpOrigin, up, 32, 1, 1, 1, 0.33f, 1000, cg.time, 0, LEF_PUFF_DONT_SCALE, cgs.media.smokePuffShader);
			}
			// boing sound at origin, jump sound on player
			trap_S_StartSound(cent->lerpOrigin, -1, CHAN_VOICE, cgs.media.jumpPadSound, 64);
			trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*jd1.wav"), 64);
			break;
		case EV_PLAYER_TELEPORT_IN:
			DEBUGNAME("EV_PLAYER_TELEPORT_IN");
			trap_R_AddLightToScene(cent->lerpOrigin, 300, 1.0f, 1.0f, 1.0f, 0.9f, 0);
			CG_SpawnEffectDefault(position);
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.teleInSound[rand()&2], 64);
			break;
		case EV_PLAYER_TELEPORT_OUT:
			DEBUGNAME("EV_PLAYER_TELEPORT_OUT");
			CG_SpawnEffectDefault(position);
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.teleOutSound, 64);
			break;
		// weapon events
		case EV_NOAMMO:
			DEBUGNAME("EV_NOAMMO");
			//trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.noAmmoSound, 64);

			if (es->number == cg.snap->ps.clientNum) {
				CG_OutOfAmmoChange();
			}

			break;
		case EV_CHANGE_WEAPON:
			DEBUGNAME("EV_CHANGE_WEAPON");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.selectSound, 48);
			break;
		// movement generated events
		case EV_FOOTSTEP_HARD:
			DEBUGNAME("EV_FOOTSTEP_HARD");

			if (cg_footsteps.integer) {
				switch (ci->foottype) {
					default:
					case FOOTTYPE_DEFAULT:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_BOOT_01:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_BOOT_01_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_BOOT_02:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_BOOT_02_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_BOOT_03:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_BOOT_03_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_FLESH_01:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_FLESH_01_HARD][rand()&3], 48);
						break;
					case FOOTTYPE_FLESH_02:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_FLESH_02_HARD][rand()&3], 48);
						break;
					case FOOTTYPE_HEELS_01:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_HEELS_01_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_HEELS_02:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_HEELS_02_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_HEELS_03:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_HEELS_03_HARD][rand()&3], 48);
						break;
					case FOOTTYPE_SANDALS_01:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_SANDALS_01_HARD][rand()&3], 48);
						break;
					case FOOTTYPE_STEP_01:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_STEP_01_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_STEP_02:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_STEP_02_HARD][rand()&3], 48);
						break;
					case FOOTTYPE_STEP_03:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_STEP_03_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_STROGG_01:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_STROGG_01_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_SPEC_KLESK:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_SPEC_KLESK_HARD][rand()&3], 48);
						break;
					case FOOTTYPE_SPEC_SORLAG:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_SPEC_SORLAG_HARD][rand()&3], 48);
						break;
					case FOOTTYPE_T2_MEDIUM:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_T2_MEDIUM_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_T2_HEAVY:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_T2_HEAVY_HARD][rand()&7], 48);
						break;
					case FOOTTYPE_T2_SMALL:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_T2_SMALL_HARD][rand()&7], 48);
						break;
				}
			}

			break;
		case EV_FOOTSTEP_HARD_FROZEN:
			DEBUGNAME("EV_FOOTSTEP_HARD_FROZEN");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_HARD_FROZEN][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_HARD_SLUSH:
			DEBUGNAME("EV_FOOTSTEP_HARD_SLUSH");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_HARD_SLUSH][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_HARD_SNOW:
			DEBUGNAME("EV_FOOTSTEP_HARD_SNOW");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_HARD_SNOW][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_PUDDLE:
			DEBUGNAME("EV_FOOTSTEP_PUDDLE");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_PUDDLE][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_LEAVES:
			DEBUGNAME("EV_FOOTSTEP_LEAVES");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_LEAVES][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_BUSH:
			DEBUGNAME("EV_FOOTSTEP_BUSH");

			if (cg_footsteps.integer) {
				switch (ci->foottype) {
					default:
					case FOOTTYPE_DEFAULT:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_BUSH][rand()&7], 48);
						break;
					case FOOTTYPE_BOOT_01:
					case FOOTTYPE_BOOT_02:
					case FOOTTYPE_BOOT_03:
						trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_BOOT_01_BUSH][rand()&7], 48);
						break;
				}
			}

			break;
		case EV_FOOTSTEP_GRASS:
			DEBUGNAME("EV_FOOTSTEP_GRASS");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_GRASS][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_LONGGRASS:
			DEBUGNAME("EV_FOOTSTEP_LONGGRASS");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_LONGGRASS][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_LONGGRASS_MUD:
			DEBUGNAME("EV_FOOTSTEP_LONGGRASS_MUD");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_LONGGRASS_MUD][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_SAND:
			DEBUGNAME("EV_FOOTSTEP_SAND");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_SAND][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_GRAVEL:
			DEBUGNAME("EV_FOOTSTEP_GRAVEL");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_GRAVEL][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_RUBBLE:
			DEBUGNAME("EV_FOOTSTEP_RUBBLE");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_RUBBLE][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_RUBBLE_WET:
			DEBUGNAME("EV_FOOTSTEP_RUBBLE_WET");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_RUBBLE_WET][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_SOIL:
			DEBUGNAME("EV_FOOTSTEP_SOIL");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_SOIL][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_MUD:
			DEBUGNAME("EV_FOOTSTEP_MUD");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_MUD][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_SNOW_DEEP:
			DEBUGNAME("EV_FOOTSTEP_SNOW_DEEP");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_SNOW_DEEP][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_ICE:
			DEBUGNAME("EV_FOOTSTEP_ICE");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_ICE][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_METAL_HOLLOW:
			DEBUGNAME("EV_FOOTSTEP_METAL_HOLLOW");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_METAL_HOLLOW][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_METAL_HOLLOW_FROZEN:
			DEBUGNAME("EV_FOOTSTEP_METAL_HOLLOW_FROZEN");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_METAL_HOLLOW_FROZEN][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_METAL_HOLLOW_SNOW:
			DEBUGNAME("EV_FOOTSTEP_METAL_HOLLOW_SNOW");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_METAL_HOLLOW_SNOW][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_METAL_HOLLOW_SLUSH:
			DEBUGNAME("EV_FOOTSTEP_METAL_HOLLOW_SLUSH");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_METAL_HOLLOW_SLUSH][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_METAL_HOLLOW_SPLASH:
			DEBUGNAME("EV_FOOTSTEP_METAL_HOLLOW_SPLASH");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_METAL_HOLLOW_SPLASH][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_GRATE_01:
			DEBUGNAME("EV_FOOTSTEP_GRATE_01");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_DEFAULT_GRATE_01][rand()&7], 48);
			}

			break;
		case EV_FOOTSTEP_GRATE_02:
			DEBUGNAME("EV_FOOTSTEP_GRATE_02");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_GRATE_02][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_DUCT:
			DEBUGNAME("EV_FOOTSTEP_DUCT");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_DUCT][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_PLATE:
			DEBUGNAME("EV_FOOTSTEP_PLATE");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_PLATE][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_FENCE:
			DEBUGNAME("EV_FOOTSTEP_FENCE");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_FENCE][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_WOOD_HOLLOW:
			DEBUGNAME("EV_FOOTSTEP_WOOD_HOLLOW");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_WOOD_HOLLOW][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_WOOD_HOLLOW_FROZEN:
			DEBUGNAME("EV_FOOTSTEP_WOOD_HOLLOW_FROZEN");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_WOOD_HOLLOW_FROZEN][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_WOOD_HOLLOW_SNOW:
			DEBUGNAME("EV_FOOTSTEP_WOOD_HOLLOW_SNOW");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_WOOD_HOLLOW_SNOW][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_WOOD_HOLLOW_SLUSH:
			DEBUGNAME("EV_FOOTSTEP_WOOD_HOLLOW_SLUSH");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_WOOD_HOLLOW_SLUSH][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_WOOD_HOLLOW_SPLASH:
			DEBUGNAME("EV_FOOTSTEP_WOOD_HOLLOW_SPLASH");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_WOOD_HOLLOW_SPLASH][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_WOOD_SOLID:
			DEBUGNAME("EV_FOOTSTEP_WOOD_SOLID");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_WOOD_SOLID][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_WOOD_CREAKING:
			DEBUGNAME("EV_FOOTSTEP_WOOD_CREAKING");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_WOOD_CREAKING][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_ROOF:
			DEBUGNAME("EV_FOOTSTEP_ROOF");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_ROOF][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_SHINGLES:
			DEBUGNAME("EV_FOOTSTEP_SHINGLES");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_SHINGLES][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_SOFT:
			DEBUGNAME("EV_FOOTSTEP_SOFT");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_SOFT][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_GLASS_SHARDS:
			DEBUGNAME("EV_FOOTSTEP_GLASS_SHARDS");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_GLASS_SHARDS][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_TRASH_GLASS:
			DEBUGNAME("EV_FOOTSTEP_TRASH_GLASS");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_TRASH_GLASS][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_TRASH_DEBRIS:
			DEBUGNAME("EV_FOOTSTEP_TRASH_DEBRIS");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_TRASH_DEBRIS][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_TRASH_WIRE:
			DEBUGNAME("EV_FOOTSTEP_TRASH_WIRE");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_TRASH_WIRE][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_TRASH_PACKING:
			DEBUGNAME("EV_FOOTSTEP_TRASH_PACKING");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_TRASH_PACKING][rand()&3], 48);
			}

			break;
		case EV_FOOTSTEP_TRASH_PLASTIC:
			DEBUGNAME("EV_FOOTSTEP_TRASH_PLASTIC");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_DEFAULT_TRASH_PLASTIC][rand()&3], 48);
			}

			break;
		case EV_FOOTSPLASH:
			DEBUGNAME("EV_FOOTSPLASH");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps8[FOOTSTEP_SPLASH][rand()&7], 48);
			}

			break;
		case EV_FOOTWADE:
			DEBUGNAME("EV_FOOTWADE");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_WADE][rand()&3], 48);
			}

			break;
		case EV_SWIM:
			DEBUGNAME("EV_SWIM");

			if (cg_footsteps.integer) {
				trap_S_StartSound(NULL, es->number, CHAN_BODY, cgs.media.footsteps4[FOOTSTEP_SWIM][rand()&3], 48);
			}

			break;
		case EV_WATER_TOUCH:
			DEBUGNAME("EV_WATER_TOUCH");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.watrInSound, 48);
			break;
		case EV_WATER_LEAVE:
			DEBUGNAME("EV_WATER_LEAVE");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.watrOutSound, 48);
			break;
		case EV_WATER_UNDER:
			DEBUGNAME("EV_WATER_UNDER");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.watrUnSound, 48);
			break;
		case EV_WATER_CLEAR:
			DEBUGNAME("EV_WATER_CLEAR");
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, CG_CustomSound(es->number, "*gp1.wav"), 64);
			break;
		case EV_TAUNT:
			DEBUGNAME("EV_TAUNT");
			trap_S_StartSound(NULL, es->number, CHAN_VOICE, CG_CustomSound(es->number, "*ta1.wav"), 64);
			break;
		case EV_TAUNT_YES:
			DEBUGNAME("EV_TAUNT_YES");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_YES);
			break;
		case EV_TAUNT_NO:
			DEBUGNAME("EV_TAUNT_NO");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_NO);
			break;
		case EV_TAUNT_FOLLOWME:
			DEBUGNAME("EV_TAUNT_FOLLOWME");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_FOLLOWME);
			break;
		case EV_TAUNT_GETFLAG:
			DEBUGNAME("EV_TAUNT_GETFLAG");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_ONGETFLAG);
			break;
		case EV_TAUNT_GUARDBASE:
			DEBUGNAME("EV_TAUNT_GUARDBASE");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_ONDEFENSE);
			break;
		case EV_TAUNT_PATROL:
			DEBUGNAME("EV_TAUNT_PATROL");
			CG_VoiceChatLocal(SAY_TEAM, qfalse, es->number, COLOR_CYAN, VOICECHAT_ONPATROL);
			break;
		case EV_STEP_4:
		case EV_STEP_8:
		case EV_STEP_12:
		case EV_STEP_16: // smooth out step up transitions
			DEBUGNAME("EV_STEP");
			{
				float oldStep;
				int delta;
				int step;

				if (clientNum != cg.predictedPlayerState.clientNum) {
					break;
				}
				// if we are interpolating, we don't need to smooth steps
				if (cg.demoPlayback || (cg.snap->ps.pm_flags & PMF_FOLLOW) || cg_nopredict.integer || cg_synchronousClients.integer) {
					break;
				}
				// check for stepping up before a previous step is completed
				delta = cg.time - cg.stepTime;

				if (delta < STEP_TIME) {
					oldStep = cg.stepChange * (STEP_TIME - delta) / STEP_TIME;
				} else {
					oldStep = 0;
				}
				// add this amount
				step = 4 * (event - EV_STEP_4 + 1);
				cg.stepChange = oldStep + step;

				if (cg.stepChange > MAX_STEP_CHANGE) {
					cg.stepChange = MAX_STEP_CHANGE;
				}

				cg.stepTime = cg.time;
				break;
			}
		// other events
		case EV_ITEM_POP:
			DEBUGNAME("EV_ITEM_POP");
			CG_SpawnEffectSmall(position);
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.teleOutSound, 32);
			break;
		case EV_ITEM_RESPAWN:
			DEBUGNAME("EV_ITEM_RESPAWN");
			trap_R_AddLightToScene(cent->lerpOrigin, 100, 1.0f, 1.0f, 1.0f, 0.9f, 0);
			CG_SpawnEffectSmall(position);
			cent->miscTime = cg.time; // scale up from this
			trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.respawnSound[rand()&2], 32);
			break;
		case EV_ITEM_PICKUP:
			DEBUGNAME("EV_ITEM_PICKUP");
			{
				gitem_t *item;
				int index;

				index = es->eventParm; // player predicted

				if (index < 1 || index >= bg_numItems) {
					break;
				}

				item = &bg_itemlist[index];
				// powerups and team items will have a separate global sound, this one will be played at prediction time
				if (item->giType == IT_POWERUP || item->giType == IT_TEAM) {
					trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.pickupSound, 52);
				} else if (item->giType == IT_PERSISTANT_POWERUP) {
					switch (item->giTag) {
						case PW_AMMOREGEN:
							trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.ammoregenSound, 52);
							break;
						case PW_GUARD:
							trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.guardSound, 52);
							break;
						case PW_DOUBLER:
							trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.doublerSound, 52);
							break;
						case PW_SCOUT:
							trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.scoutSound, 52);
							break;
					}
				} else {
					trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.itemPickupSounds[index], 52);
				}
				// show icon and name on status bar
				if (es->number == cg.snap->ps.clientNum) {
					CG_ItemPickup(index);
				}
			}

			break;
		case EV_GLOBAL_ITEM_PICKUP:
			DEBUGNAME("EV_GLOBAL_ITEM_PICKUP");
			{
				int index;

				index = es->eventParm; // player predicted

				if (index < 1 || index >= bg_numItems) {
					break;
				}
				// powerup pickups aren't global if an announcer sound is already playing
				if (cg.soundPlaying) {
					trap_S_StartSound(NULL, es->number, CHAN_AUTO, cgs.media.pickupSound, 52);
				// otherwise they are global
				} else {
					CG_AddBufferedAnnouncerSound(cgs.media.itemPickupSounds[index]);
				}
				// show icon and name on status bar
				if (es->number == cg.snap->ps.clientNum) {
					CG_ItemPickup(index);
				}
			}

			break;
		// powerup events
		case EV_POWERUP_QUAD:
			DEBUGNAME("EV_POWERUP_QUAD");

			if (es->number == cg.snap->ps.clientNum) {
				cg.powerupActive = PW_QUAD;
				cg.powerupTime = cg.time;
			}

			trap_S_StartSound(NULL, es->number, CHAN_ITEM, cgs.media.quadSound, 48);
			break;
		case EV_POWERUP_REGEN:
			DEBUGNAME("EV_POWERUP_REGEN");

			if (es->number == cg.snap->ps.clientNum) {
				cg.powerupActive = PW_REGEN;
				cg.powerupTime = cg.time;
			}

			trap_S_StartSound(NULL, es->number, CHAN_ITEM, cgs.media.regenSound, 48);
			break;
		case EV_USE_ITEM0:
			DEBUGNAME("EV_USE_ITEM0");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM1:
			DEBUGNAME("EV_USE_ITEM1");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM2:
			DEBUGNAME("EV_USE_ITEM2");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM3:
			DEBUGNAME("EV_USE_ITEM3");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM4:
			DEBUGNAME("EV_USE_ITEM4");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM5:
			DEBUGNAME("EV_USE_ITEM5");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM6:
			DEBUGNAME("EV_USE_ITEM6");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM7:
			DEBUGNAME("EV_USE_ITEM7");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM8:
			DEBUGNAME("EV_USE_ITEM8");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM9:
			DEBUGNAME("EV_USE_ITEM9");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM10:
			DEBUGNAME("EV_USE_ITEM10");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM11:
			DEBUGNAME("EV_USE_ITEM11");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM12:
			DEBUGNAME("EV_USE_ITEM12");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM13:
			DEBUGNAME("EV_USE_ITEM13");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM14:
			DEBUGNAME("EV_USE_ITEM14");
			CG_UseItem(cent);
			break;
		case EV_USE_ITEM15:
			DEBUGNAME("EV_USE_ITEM15");
			CG_UseItem(cent);
			break;
		case EV_SCOREPLUM:
			DEBUGNAME("EV_SCOREPLUM");
			CG_ScorePlum(cent->currentState.otherEntityNum, cent->lerpOrigin, cent->currentState.time);
			break;
		case EV_LIGHTNINGBOLT:
			DEBUGNAME("EV_LIGHTNINGBOLT");
			CG_LightningBolt(es->origin2, es->pos.trBase);
			break;
		case EV_DEBUG_LINE:
			DEBUGNAME("EV_DEBUG_LINE");
			CG_Beam(cent);
			break;
		default:
			DEBUGNAME("UNKNOWN");
			CG_Error("Unknown event: %i", event);
			break;
	}
}

/*
=======================================================================================================================================
CG_CheckEvents
=======================================================================================================================================
*/
void CG_CheckEvents(centity_t *cent) {

	// check for event-only entities
	if (cent->currentState.eType > ET_EVENTS) {
		if (cent->previousEvent) {
			return; // already fired
		}
		// if this is a player event set the entity number of the client entity number
		if (cent->currentState.eFlags & EF_PLAYER_EVENT) {
			cent->currentState.number = cent->currentState.otherEntityNum;
		}

		cent->previousEvent = 1;
		cent->currentState.event = cent->currentState.eType - ET_EVENTS;
	} else {
		// check for events riding with another entity
		if (cent->currentState.event == cent->previousEvent) {
			return;
		}

		cent->previousEvent = cent->currentState.event;

		if ((cent->currentState.event & ~EV_EVENT_BITS) == 0) {
			return;
		}
	}
	// calculate the position at exactly the frame time
	BG_EvaluateTrajectory(&cent->currentState.pos, cg.snap->serverTime, cent->lerpOrigin);
	CG_SetEntitySoundPosition(cent);
	CG_EntityEvent(cent, cent->lerpOrigin);
}
