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
#include "../botlib/botlib.h"
#include "../botlib/be_aas.h"
#include "../botlib/be_ea.h"
#include "../botlib/be_ai_char.h"
#include "../botlib/be_ai_chat.h"
#include "../botlib/be_ai_gen.h"
#include "../botlib/be_ai_goal.h"
#include "../botlib/be_ai_move.h"
#include "../botlib/be_ai_weap.h"
#include "ai_main.h"
#include "ai_dmq3.h"
#include "ai_chat.h"
#include "ai_cmd.h"
#include "ai_dmnet.h"
#include "ai_team.h"
#include "ai_vcmd.h"
// data file headers
#include "chars.h" // characteristics
#include "inv.h" // indexes into the inventory
#include "syn.h" // synonyms
#include "match.h" // string matching types and vars
#include "../../ui/menudef.h" // for the voice chats

int numnodeswitches;
char nodeswitch[MAX_NODESWITCHES + 1][144];

#define LOOKAHEAD_DISTANCE 300

/*
=======================================================================================================================================
BotResetNodeSwitches
=======================================================================================================================================
*/
void BotResetNodeSwitches(void) {
	numnodeswitches = 0;
}

/*
=======================================================================================================================================
BotDumpNodeSwitches
=======================================================================================================================================
*/
void BotDumpNodeSwitches(bot_state_t *bs) {
	int i;
	char netname[MAX_NETNAME];

	ClientName(bs->client, netname, sizeof(netname));
	BotAI_Print(PRT_MESSAGE, "%1.1f: %s switched more than %d AI nodes\n", FloatTime(), netname, MAX_NODESWITCHES);

	for (i = 0; i < numnodeswitches; i++) {
		BotAI_Print(PRT_MESSAGE, "%s", nodeswitch[i]);
	}

	BotAI_Print(PRT_FATAL, "");
}

/*
=======================================================================================================================================
BotRecordNodeSwitch
=======================================================================================================================================
*/
void BotRecordNodeSwitch(bot_state_t *bs, char *node, char *str, char *s) {
	char netname[MAX_NETNAME];

	ClientName(bs->client, netname, sizeof(netname));
	Com_sprintf(nodeswitch[numnodeswitches], 144, "%s at %2.1f entered %s: %s from %s\n", netname, FloatTime(), node, str, s);
#ifdef DEBUG
	if (0) {
		BotAI_Print(PRT_MESSAGE, "%s", nodeswitch[numnodeswitches]);
	}
#endif // DEBUG
	numnodeswitches++;
}

/*
=======================================================================================================================================
BotGetAirGoal
=======================================================================================================================================
*/
int BotGetAirGoal(bot_state_t *bs, bot_goal_t *goal) {
	bsp_trace_t bsptrace;
	vec3_t end, mins = {-15, -15, -2}, maxs = {15, 15, 2};
	int areanum;

	VectorCopy(bs->origin, end);
	// trace up until we hit solid
	end[2] += 1000;

	BotAI_Trace(&bsptrace, bs->origin, mins, maxs, end, bs->entitynum, CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BOTCLIP);
	// trace down until we hit water
	VectorCopy(bsptrace.endpos, end);
	BotAI_Trace(&bsptrace, end, mins, maxs, bs->origin, bs->entitynum, CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA);
	// if we found the water surface
	if (bsptrace.fraction > 0) {
		areanum = BotPointAreaNum(bsptrace.endpos);

		if (areanum) {
			VectorCopy(bsptrace.endpos, goal->origin);
			goal->origin[2] -= 2;
			goal->areanum = areanum;
			goal->mins[0] = -15;
			goal->mins[1] = -15;
			goal->mins[2] = -1;
			goal->maxs[0] = 15;
			goal->maxs[1] = 15;
			goal->maxs[2] = 1;
			goal->flags = GFL_AIR;
			goal->number = 0;
			goal->iteminfo = 0;
			goal->entitynum = 0;
			return qtrue;
		}
	}

	return qfalse;
}

/*
=======================================================================================================================================
BotGoForAir
=======================================================================================================================================
*/
int BotGoForAir(bot_state_t *bs, int tfl, bot_goal_t *ltg, float range) {
	bot_goal_t goal;

	// if the bot needs air
	if (bs->lastair_time < FloatTime() - 15) {
#ifdef DEBUG
		//BotAI_Print(PRT_MESSAGE, "going for air\n");
#endif // DEBUG
		// if we can find an air goal
		if (BotGetAirGoal(bs, &goal)) {
			trap_BotPushGoal(bs->gs, &goal);
			return qtrue;
		} else {
			// get a nearby goal outside the water
			while (trap_BotChooseNBGItem(bs->gs, bs->origin, bs->inventory, tfl, ltg, range)) {
				trap_BotGetTopGoal(bs->gs, &goal);
				// if the goal is not in water
				if (!(trap_AAS_PointContents(goal.origin) & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA))) {
					return qtrue;
				}

				trap_BotPopGoal(bs->gs);
			}

			trap_BotResetAvoidGoals(bs->gs);
		}
	}

	return qfalse;
}

/*
=======================================================================================================================================
BotNearbyGoal
=======================================================================================================================================
*/
int BotNearbyGoal(bot_state_t *bs, int tfl, bot_goal_t *ltg, float range) {
	int ret, dist;

	// check if the bot should go for air
	if (BotGoForAir(bs, tfl, ltg, range)) {
		return qtrue;
	}
	// if the bot is carrying a flag or cubes
	if (BotHasEmergencyGoal(bs)) {
		if (!BotFeelingBad(bs)) {
			dist = 900;
		} else {
			dist = 300;
		}
		// if the bot is just a few secs away from team goal area number
		if (trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, bs->teamgoal.areanum, TFL_DEFAULT) < dist) {
			// make the range really small
			range = 20;
		}
	}

	ret = trap_BotChooseNBGItem(bs->gs, bs->origin, bs->inventory, tfl, ltg, range);
	/*
	if (ret) {
		char buf[128];

		// get the goal at the top of the stack
		trap_BotGetTopGoal(bs->gs, &goal);
		trap_BotGoalName(goal.number, buf, sizeof(buf));
		BotAI_Print(PRT_MESSAGE, "%1.1f: new nearby goal %s\n", FloatTime(), buf);
	}
	*/
	return ret;
}

/*
=======================================================================================================================================
BotReachedGoal
=======================================================================================================================================
*/
int BotReachedGoal(bot_state_t *bs, bot_goal_t *goal) {

	if (goal->flags & GFL_ITEM) {
		// if the item is a dropped item it may no longer exist
		if (goal->flags & GFL_DROPPED) {
			if (!g_entities[goal->entitynum].inuse) {
				return qtrue;
			}

			if (Distance(goal->origin, g_entities[goal->entitynum].r.currentOrigin) > 50) {
				return qtrue;
			}
		}
		// if touching the goal
		if (trap_BotTouchingGoal(bs->origin, goal)) {
			if (!(goal->flags & GFL_DROPPED)) {
				trap_BotSetAvoidGoalTime(bs->gs, goal->number, -1);
			}

			return qtrue;
		}
		// if the goal isn't there
		if (trap_BotItemGoalInVisButNotVisible(bs->entitynum, bs->eye, bs->viewangles, goal)) {
			/*
			float avoidtime;
			int t;

			avoidtime = trap_BotAvoidGoalTime(bs->gs, goal->number);

			if (avoidtime > 0) {
				t = trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, goal->areanum, bs->tfl);

				if ((float)t * 0.009 < avoidtime) {
					return qtrue;
				}
			}
			*/
			return qtrue;
		}
		// if the bot is in the goal area and below or above the goal and not swimming
		if (bs->areanum == goal->areanum) {
			if (bs->origin[0] > goal->origin[0] + goal->mins[0] && bs->origin[0] < goal->origin[0] + goal->maxs[0]) {
				if (bs->origin[1] > goal->origin[1] + goal->mins[1] && bs->origin[1] < goal->origin[1] + goal->maxs[1]) {
					if (!trap_AAS_Swimming(bs->origin)) {
						return qtrue;
					}
				}
			}
		}
	} else if (goal->flags & GFL_AIR) {
		// if touching the goal
		if (trap_BotTouchingGoal(bs->origin, goal)) {
			return qtrue;
		}
		// if the bot got air
		if (bs->lastair_time > FloatTime() - 1) {
			return qtrue;
		}
	} else {
		// if touching the goal
		if (trap_BotTouchingGoal(bs->origin, goal)) {
			return qtrue;
		}
	}

	return qfalse;
}

/*
=======================================================================================================================================
BotGetItemLongTermGoal
=======================================================================================================================================
*/
int BotGetItemLongTermGoal(bot_state_t *bs, int tfl, bot_goal_t *goal) {

	// if the bot has no goal
	if (!trap_BotGetTopGoal(bs->gs, goal)) {
		//BotAI_Print(PRT_MESSAGE, "no ltg on stack\n");
		bs->ltg_time = 0;
	// if the bot touches the current goal
	} else if (BotReachedGoal(bs, goal)) {
		bs->ltg_time = 0;
	}
	// if it is time to find a new long term goal
	if (bs->ltg_time < FloatTime()) {
		// pop the current goal from the stack
		trap_BotPopGoal(bs->gs);
		//BotAI_Print(PRT_MESSAGE, "%s: choosing new ltg\n", ClientName(bs->client, netname, sizeof(netname)));
		// choose a new goal
		//BotAI_Print(PRT_MESSAGE, "%6.1f client %d: BotChooseLTGItem\n", FloatTime(), bs->client);

		if (trap_BotChooseLTGItem(bs->gs, bs->origin, bs->inventory, tfl)) {
			/*
			char buf[128];

			// get the goal at the top of the stack
			trap_BotGetTopGoal(bs->gs, goal);
			trap_BotGoalName(goal->number, buf, sizeof(buf));
			BotAI_Print(PRT_MESSAGE, "%1.1f: new long term goal %s\n", FloatTime(), buf);
			*/
			bs->ltg_time = FloatTime() + 20;
		} else { // the bot gets sorta stuck with all the avoid timings, shouldn't happen though
#ifdef DEBUG
			char netname[128];

			BotAI_Print(PRT_MESSAGE, "%s: no valid ltg (probably stuck)\n", ClientName(bs->client, netname, sizeof(netname)));
#endif
			//trap_BotDumpAvoidGoals(bs->gs);
			// reset the avoid goals and the avoid reach
			trap_BotResetAvoidGoals(bs->gs);
			trap_BotResetAvoidReach(bs->ms);
			// check if the bot is blocking team mates
			BotCheckBlockedTeammates(bs);
		}
		// get the goal at the top of the stack
		return trap_BotGetTopGoal(bs->gs, goal);
	}

	return qtrue;
}

/*
=======================================================================================================================================
BotGetLongTermGoal

We could also create a separate AI node for every long term goal type. However, this saves us a lot of code.
=======================================================================================================================================
*/
int BotGetLongTermGoal(bot_state_t *bs, int tfl, int retreat, bot_goal_t *goal) {
	vec3_t target, dir, start;
	char netname[MAX_NETNAME];
	char buf[MAX_MESSAGE_SIZE];
	int areanum, teammates;
	float croucher;
	aas_entityinfo_t entinfo;
	bot_waypoint_t *wp;
	bsp_trace_t bsptrace;

	if (bs->ltgtype == LTG_TEAMHELP && !retreat) {
		// check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "help_start", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
			trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			bs->teammessage_time = 0;
		}
		// if trying to help the team mate for more than a minute
		if (bs->teamgoal_time < FloatTime()) {
			bs->ltgtype = 0;
		}
		// if the team mate IS visible for quite some time
		if (bs->teammatevisible_time < FloatTime() - 10) {
			bs->ltgtype = 0;
		}
		// get the entity information
		BotEntityInfo(bs->teammate, &entinfo);
		// if the team mate is visible
		if (BotEntityVisible(&bs->cur_ps, 360, bs->teammate)) {
			// if close just stand still there
			VectorSubtract(entinfo.origin, bs->origin, dir);

			if (VectorLengthSquared(dir) < Square(100)) {
				trap_BotResetAvoidReach(bs->ms);
				// check if the bot is blocking team mates
				BotCheckBlockedTeammates(bs);
				return qfalse;
			}
		} else {
			// last time the bot was NOT visible
			bs->teammatevisible_time = FloatTime();
		}
		// if the entity information is valid
		if (entinfo.valid) {
			areanum = BotPointAreaNum(entinfo.origin);

			if (areanum && trap_AAS_AreaReachability(areanum)) {
				// update team goal
				bs->teamgoal.entitynum = bs->teammate;
				bs->teamgoal.areanum = areanum;

				VectorCopy(entinfo.origin, bs->teamgoal.origin);
				VectorSet(bs->teamgoal.mins, -8, -8, -8);
				VectorSet(bs->teamgoal.maxs, 8, 8, 8);
			}
		}
		// set the bot goal
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		return qtrue;
	}
	// if the bot accompanies someone
	if (bs->ltgtype == LTG_TEAMACCOMPANY && !retreat) {
		// check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "accompany_start", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
			trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			bs->teammessage_time = 0;
		}
		// if accompanying the companion for 3 minutes
		if (bs->teamgoal_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "accompany_stop", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
			bs->ltgtype = 0;
		}
		// get the entity information
		BotEntityInfo(bs->teammate, &entinfo);
		VectorSubtract(entinfo.origin, bs->origin, dir);

		teammates = BotCountAllTeamMates(bs, 256);

		if (VectorLengthSquared(dir) < Square(bs->formation_dist + (teammates * bs->formation_dist))) {
			// check if the bot wants to crouch, don't crouch if crouched less than 5 seconds ago
			if (bs->crouch_time < FloatTime() - 5) {
				croucher = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_CROUCHER, 0, 1);

				if (random() < bs->thinktime * croucher) {
					bs->crouch_time = FloatTime() + 5 + croucher * 15;
				}
			}
			// don't crouch when swimming
			if (trap_AAS_Swimming(bs->origin)) {
				bs->crouch_time = FloatTime() - 1;
			}
			// if the companion is visible
			if (BotEntityVisible(&bs->cur_ps, 360, bs->teammate)) {
				// update visible time
				bs->teammatevisible_time = FloatTime();
				// if not arrived yet or arived some time ago
				if (bs->arrive_time < FloatTime() - 2) {
					// if not arrived yet
					if (!bs->arrive_time) {
						trap_EA_Gesture(bs->client);
						BotAI_BotInitialChat(bs, "accompany_arrive", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
						trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
						bs->arrive_time = FloatTime();
					// if the bot wants to crouch
					} else if (bs->crouch_time > FloatTime()) {
						VectorCopy(bs->origin, start);
						// get the crouch view height
						start[2] += CROUCH_VIEWHEIGHT;
						// only try to crouch if the team mate remains visible
						BotAI_Trace(&bsptrace, start, NULL, NULL, entinfo.origin, bs->client, MASK_SHOT);
						// if the team mate remains visible from the predicted position
						if (bsptrace.fraction >= 1.0 || bsptrace.entityNum == bs->teammate) {
							trap_EA_Crouch(bs->client);
						}
					// else do some model taunts
					} else if (random() < bs->thinktime * 0.05) {
						// do a gesture :)
						trap_EA_Gesture(bs->client);
					}
				}
				// if just arrived look at the companion
				if (bs->arrive_time > FloatTime() - 2) {
					VectorSubtract(entinfo.origin, bs->origin, dir);
					VectorToAngles(dir, bs->ideal_viewangles);
					bs->ideal_viewangles[2] *= 0.5;
				}
			}
			// look strategically around for enemies
			if (random() < bs->thinktime * 0.8) {
				BotRoamGoal(bs, target);
				VectorSubtract(target, bs->origin, dir);
				VectorToAngles(dir, bs->ideal_viewangles);
				bs->ideal_viewangles[2] *= 0.5;
			}
			// check if the bot wants to go for air
			if (BotGoForAir(bs, bs->tfl, &bs->teamgoal, 400)) {
				trap_BotResetLastAvoidReach(bs->ms);
				// get the goal at the top of the stack
				//trap_BotGetTopGoal(bs->gs, &tmpgoal);
				//trap_BotGoalName(tmpgoal.number, buf, 144);
				//BotAI_Print(PRT_MESSAGE, "new nearby goal %s\n", buf);
				// time the bot gets to pick up the nearby goal item
				bs->nbg_time = FloatTime() + 8;
				AIEnter_Seek_NBG(bs, "BotLongTermGoal: go for air");
				return qfalse;
			}

			trap_BotResetAvoidReach(bs->ms);
			// check if the bot is blocking team mates
			BotCheckBlockedTeammates(bs);
			return qfalse;
		}
		// if the entity information is valid
		if (entinfo.valid) {
			areanum = BotPointAreaNum(entinfo.origin);

			if (areanum && trap_AAS_AreaReachability(areanum)) {
				// update team goal
				bs->teamgoal.entitynum = bs->teammate;
				bs->teamgoal.areanum = areanum;

				VectorCopy(entinfo.origin, bs->teamgoal.origin);
				VectorSet(bs->teamgoal.mins, -8, -8, -8);
				VectorSet(bs->teamgoal.maxs, 8, 8, 8);
			}
		}
		// the goal the bot should go for
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		// if the companion is NOT visible for too long
		if (bs->teammatevisible_time < FloatTime() - 60) {
			BotAI_BotInitialChat(bs, "accompany_cannotfind", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
			trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
			bs->ltgtype = 0;
			// just to make sure the bot won't spam this message
			bs->teammatevisible_time = FloatTime();
		}

		return qtrue;
	}

	if (bs->ltgtype == LTG_DEFENDKEYAREA) {
		if (trap_AAS_AreaTravelTimeToGoalArea(bs->areanum, bs->origin, bs->teamgoal.areanum, TFL_DEFAULT) > bs->defendaway_range) {
			bs->defendaway_time = 0;
		}
	}
	// if defending a key area
	if (bs->ltgtype == LTG_DEFENDKEYAREA && !retreat && bs->defendaway_time < FloatTime()) {
		// check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "defend_start", buf, NULL);
			trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
			BotVoiceChatOnly(bs, -1, VOICECHAT_ONDEFENSE);
			bs->teammessage_time = 0;
		}
		// set the bot goal
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		// stop after 2 minutes
		if (bs->teamgoal_time < FloatTime()) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "defend_stop", buf, NULL);
			trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
			bs->ltgtype = 0;
		}
		// if very close... go away for some time
		VectorSubtract(goal->origin, bs->origin, dir);

		if (VectorLengthSquared(dir) < Square(70)) {
			trap_BotResetAvoidReach(bs->ms);
			bs->defendaway_time = FloatTime() + 3 + 3 * random();

			if (BotHasPersistantPowerupAndWeapon(bs)) {
				bs->defendaway_range = 100;
			} else {
				bs->defendaway_range = 350;
			}
		}

		return qtrue;
	}
	// going to kill someone
	if (bs->ltgtype == LTG_KILL && !retreat) {
		// check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			EasyClientName(bs->teamgoal.entitynum, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "kill_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->teammessage_time = 0;
		}

		if (bs->killedenemy_time > bs->teamgoal_time - TEAM_KILL_SOMEONE && bs->lastkilledplayer == bs->teamgoal.entitynum) {
			EasyClientName(bs->teamgoal.entitynum, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "kill_done", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->ltgtype = 0;
		}
		// stop after some time
		if (bs->teamgoal_time < FloatTime()) {
			bs->ltgtype = 0;
		}
		// just roam around
		return BotGetItemLongTermGoal(bs, tfl, goal);
	}
	// get an item
	if (bs->ltgtype == LTG_GETITEM && !retreat) {
		// check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "getitem_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
			trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			bs->teammessage_time = 0;
		}
		// set the bot goal (the goal the bot should go for)
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		// stop after some time
		if (bs->teamgoal_time < FloatTime()) {
			bs->ltgtype = 0;
		}

		if (trap_BotItemGoalInVisButNotVisible(bs->entitynum, bs->eye, bs->viewangles, goal)) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "getitem_notthere", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->ltgtype = 0;
		} else if (BotReachedGoal(bs, goal)) {
			trap_BotGoalName(bs->teamgoal.number, buf, sizeof(buf));
			BotAI_BotInitialChat(bs, "getitem_gotit", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->ltgtype = 0;
		}

		return qtrue;
	}
	// if camping somewhere
	if ((bs->ltgtype == LTG_CAMP || bs->ltgtype == LTG_CAMPORDER) && !retreat) {
		// check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			if (bs->ltgtype == LTG_CAMPORDER) {
				BotAI_BotInitialChat(bs, "camp_start", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
				trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
				BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
				trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			}

			bs->teammessage_time = 0;
		}
		// set the bot goal
		memcpy(goal, &bs->teamgoal, sizeof(bot_goal_t));
		// stop after some time
		if (bs->teamgoal_time < FloatTime()) {
			if (bs->ltgtype == LTG_CAMPORDER) {
				BotAI_BotInitialChat(bs, "camp_stop", NULL);
				trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			}

			bs->ltgtype = 0;
		}
		// if the bot decided to camp
		if (!bs->ordered) {
			// if the bot should stop camping
			if (!BotCanCamp(bs)) {
				bs->ltgtype = 0;
			}
		}
		// if really near the camp spot
		VectorSubtract(goal->origin, bs->origin, dir);

		teammates = BotCountAllTeamMates(bs, 256);

		if (VectorLengthSquared(dir) < Square(60 + (teammates * 60))) {
			// if not arrived yet
			if (!bs->arrive_time) {
				if (bs->ltgtype == LTG_CAMPORDER) {
					BotAI_BotInitialChat(bs, "camp_arrive", EasyClientName(bs->teammate, netname, sizeof(netname)), NULL);
					trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
					BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_INPOSITION);
				}

				bs->arrive_time = FloatTime();
			}
			// look strategically around for enemies
			if (random() < bs->thinktime * 0.8) {
				BotRoamGoal(bs, target);
				VectorSubtract(target, bs->origin, dir);
				VectorToAngles(dir, bs->ideal_viewangles);
				bs->ideal_viewangles[2] *= 0.5;
			}
			// check if the bot wants to crouch, don't crouch if crouched less than 5 seconds ago
			if (bs->crouch_time < FloatTime() - 5) {
				croucher = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_CROUCHER, 0, 1);

				if (random() < bs->thinktime * croucher) {
					bs->crouch_time = FloatTime() + 5 + croucher * 15;
				}
			}
			// don't crouch when swimming
			if (trap_AAS_Swimming(bs->origin)) {
				bs->crouch_time = FloatTime() - 1;
			}
			// if the bot wants to crouch
			if (bs->crouch_time > FloatTime()) {
				trap_EA_Crouch(bs->client);
			}
			// make sure the bot is not gonna drown
			if (trap_PointContents(bs->eye, bs->entitynum) & (CONTENTS_WATER|CONTENTS_SLIME|CONTENTS_LAVA)) {
				if (bs->ltgtype == LTG_CAMPORDER) {
					BotAI_BotInitialChat(bs, "camp_stop", NULL);
					trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);

					if (bs->lastgoal_ltgtype == LTG_CAMPORDER) {
						bs->lastgoal_ltgtype = 0;
					}
				}

				bs->ltgtype = 0;
			}
			// FIXME: move around a bit
			trap_BotResetAvoidReach(bs->ms);
			// check if the bot is blocking team mates
			BotCheckBlockedTeammates(bs);
			return qfalse;
		}

		return qtrue;
	}
	// patrolling along several waypoints
	if (bs->ltgtype == LTG_PATROL && !retreat) {
		// check for bot typing status message
		if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
			strcpy(buf, "");

			for (wp = bs->patrolpoints; wp; wp = wp->next) {
				strcat(buf, wp->name);

				if (wp->next) {
					strcat(buf, " to ");
				}
			}

			BotAI_BotInitialChat(bs, "patrol_start", buf, NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			BotVoiceChatOnly(bs, bs->decisionmaker, VOICECHAT_YES);
			trap_EA_Action(bs->client, ACTION_AFFIRMATIVE);
			bs->teammessage_time = 0;
		}

		if (!bs->curpatrolpoint) {
			bs->ltgtype = 0;
			return qfalse;
		}
		// if the bot touches the current goal
		if (trap_BotTouchingGoal(bs->origin, &bs->curpatrolpoint->goal)) {
			if (bs->patrolflags & PATROL_BACK) {
				if (bs->curpatrolpoint->prev) {
					bs->curpatrolpoint = bs->curpatrolpoint->prev;
				} else {
					bs->curpatrolpoint = bs->curpatrolpoint->next;
					bs->patrolflags &= ~PATROL_BACK;
				}
			} else {
				if (bs->curpatrolpoint->next) {
					bs->curpatrolpoint = bs->curpatrolpoint->next;
				} else {
					bs->curpatrolpoint = bs->curpatrolpoint->prev;
					bs->patrolflags |= PATROL_BACK;
				}
			}
		}
		// stop after 5 minutes
		if (bs->teamgoal_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "patrol_stop", NULL);
			trap_BotEnterChat(bs->cs, bs->decisionmaker, CHAT_TELL);
			bs->ltgtype = 0;
		}

		if (!bs->curpatrolpoint) {
			bs->ltgtype = 0;
			return qfalse;
		}
		// set the bot goal
		memcpy(goal, &bs->curpatrolpoint->goal, sizeof(bot_goal_t));
		return qtrue;
	}

	if (gametype == GT_CTF) {
		// if going for enemy flag
		if (bs->ltgtype == LTG_GETFLAG) {
			// check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "captureflag_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONGETFLAG);
				bs->teammessage_time = 0;
			}
			// set the bot goal
			switch (BotTeam(bs)) {
				case TEAM_RED:
					memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t));
					break;
				case TEAM_BLUE:
					memcpy(goal, &ctf_redflag, sizeof(bot_goal_t));
					break;
				default:
					bs->ltgtype = 0;
					return qfalse;
			}
			// if touching the flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				// make sure the bot knows the flag isn't there anymore
				switch (BotTeam(bs)) {
					case TEAM_RED:
						bs->blueflagstatus = 1;
						break;
					case TEAM_BLUE:
						bs->redflagstatus = 1;
						break;
				}

				bs->ltgtype = 0;
			}
			// stop after 3 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}

			BotAlternateRoute(bs, goal);
			return qtrue;
		}
		// if rushing to the base
		if (bs->ltgtype == LTG_RUSHBASE && bs->rushbaseaway_time < FloatTime()) {
			// set the bot goal
			switch (BotTeam(bs)) {
				case TEAM_RED:
					memcpy(goal, &ctf_redflag, sizeof(bot_goal_t));
					break;
				case TEAM_BLUE:
					memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t));
					break;
				default:
					bs->ltgtype = 0;
					return qfalse;
			}
			// if not carrying the flag anymore
			if (!BotCTFCarryingFlag(bs)) {
				bs->ltgtype = 0;
			}
			// quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			// if touching the base flag the bot should loose the enemy flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				// if the bot is still carrying the enemy flag then the base flag is gone, now just walk near the base a bit
				if (BotCTFCarryingFlag(bs)) {
					trap_BotResetAvoidReach(bs->ms);
					bs->rushbaseaway_time = FloatTime() + 5 + 10 * random();
					// FIXME: add chat to tell the others to get back the flag
				} else {
					bs->ltgtype = 0;
				}
			}

			BotAlternateRoute(bs, goal);
			return qtrue;
		}
		// returning flag
		if (bs->ltgtype == LTG_RETURNFLAG) {
			// check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "returnflag_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONRETURNFLAG);
				bs->teammessage_time = 0;
			}
			// set the bot goal
			switch (BotTeam(bs)) {
				case TEAM_RED:
					memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t));
					break;
				case TEAM_BLUE:
					memcpy(goal, &ctf_redflag, sizeof(bot_goal_t));
					break;
				default:
					bs->ltgtype = 0;
					return qfalse;
			}
			// if touching the flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->ltgtype = 0;
			}
			// stop after 3 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}

			BotAlternateRoute(bs, goal);
			return qtrue;
		}
	} else if (gametype == GT_1FCTF) {
		if (bs->ltgtype == LTG_GETFLAG) {
			// check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "captureflag_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONGETFLAG);
				bs->teammessage_time = 0;
			}
			// set the bot goal
			memcpy(goal, &ctf_neutralflag, sizeof(bot_goal_t));
			// if touching the flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->ltgtype = 0;
			}
			// stop after 3 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}

			return qtrue;
		}
		// if rushing to the base
		if (bs->ltgtype == LTG_RUSHBASE) {
			// set the bot goal
			switch (BotTeam(bs)) {
				case TEAM_RED:
					memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t));
					break;
				case TEAM_BLUE:
					memcpy(goal, &ctf_redflag, sizeof(bot_goal_t));
					break;
				default:
					bs->ltgtype = 0;
					return qfalse;
			}
			// if not carrying the flag anymore
			if (!Bot1FCTFCarryingFlag(bs)) {
				bs->ltgtype = 0;
			}
			// quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			// if touching the base flag the bot should loose flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->ltgtype = 0;
			}

			BotAlternateRoute(bs, goal);
			return qtrue;
		}
		// attack the enemy base
		if (bs->ltgtype == LTG_ATTACKENEMYBASE && bs->attackaway_time < FloatTime()) {
			// check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "attackenemybase_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONOFFENSE);
				bs->teammessage_time = 0;
			}
			// set the bot goal
			switch (BotTeam(bs)) {
				case TEAM_RED:
					memcpy(goal, &ctf_blueflag, sizeof(bot_goal_t));
					break;
				case TEAM_BLUE:
					memcpy(goal, &ctf_redflag, sizeof(bot_goal_t));
					break;
				default:
					bs->ltgtype = 0;
					return qfalse;
			}
			// quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			// if touching the base flag the bot should loose the flag
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->attackaway_time = FloatTime() + 2 + 5 * random();
			}

			return qtrue;
		}
		// returning flag
		if (bs->ltgtype == LTG_RETURNFLAG) {
			// check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "returnflag_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONRETURNFLAG);
				bs->teammessage_time = 0;
			}
			// stop after some time
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			// just roam around
			return BotGetItemLongTermGoal(bs, tfl, goal);
		}
	} else if (gametype == GT_OBELISK) {
		if (bs->ltgtype == LTG_ATTACKENEMYBASE && bs->attackaway_time < FloatTime()) {
			// check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "attackenemybase_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONOFFENSE);
				bs->teammessage_time = 0;
			}
			// set the bot goal
			switch (BotTeam(bs)) {
				case TEAM_RED:
					memcpy(goal, &blueobelisk, sizeof(bot_goal_t));
					break;
				case TEAM_BLUE:
					memcpy(goal, &redobelisk, sizeof(bot_goal_t));
					break;
				default:
					bs->ltgtype = 0;
					return qfalse;
			}
			// if the bot no longer wants to attack the obelisk
			if (BotFeelingBad(bs)) {
				return BotGetItemLongTermGoal(bs, tfl, goal);
			}
			// if touching the obelisk
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->attackaway_time = FloatTime() + 3 + 5 * random();
			}
			// or very close to the obelisk
			VectorSubtract(bs->origin, goal->origin, dir);

			if (VectorLengthSquared(dir) < Square(60)) {
				bs->attackaway_time = FloatTime() + 3 + 5 * random();
			}
			// quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}

			BotAlternateRoute(bs, goal);
			// just move towards the obelisk
			return qtrue;
		}
	} else if (gametype == GT_HARVESTER) {
		// if rushing to the base
		if (bs->ltgtype == LTG_RUSHBASE) {
			// set the bot goal
			switch (BotTeam(bs)) {
				case TEAM_RED:
					memcpy(goal, &blueobelisk, sizeof(bot_goal_t));
					break;
				case TEAM_BLUE:
					memcpy(goal, &redobelisk, sizeof(bot_goal_t));
					break;
				default:
					BotGoHarvest(bs);
					return qfalse;
			}
			// if not carrying any cubes
			if (!BotHarvesterCarryingCubes(bs)) {
				BotGoHarvest(bs);
				return qfalse;
			}
			// quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				BotGoHarvest(bs);
				return qfalse;
			}
			// if touching the obelisk the bot should loose the skulls
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				BotGoHarvest(bs);
				return qfalse;
			}

			BotAlternateRoute(bs, goal);
			return qtrue;
		}
		// attack the enemy base
		if (bs->ltgtype == LTG_ATTACKENEMYBASE && bs->attackaway_time < FloatTime()) {
			// check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "attackenemybase_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONOFFENSE);
				bs->teammessage_time = 0;
			}
			// set the bot goal
			switch (BotTeam(bs)) {
				case TEAM_RED:
					memcpy(goal, &blueobelisk, sizeof(bot_goal_t));
					break;
				case TEAM_BLUE:
					memcpy(goal, &redobelisk, sizeof(bot_goal_t));
					break;
				default:
					bs->ltgtype = 0;
					return qfalse;
			}
			// quit rushing after 2 minutes
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			// if touching the goal
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->attackaway_time = FloatTime() + 2 + 5 * random();
			}

			return qtrue;
		}
		// harvest cubes
		if (bs->ltgtype == LTG_HARVEST && bs->harvestaway_time < FloatTime()) {
			// check for bot typing status message
			if (bs->teammessage_time && bs->teammessage_time < FloatTime()) {
				BotAI_BotInitialChat(bs, "harvest_start", NULL);
				trap_BotEnterChat(bs->cs, 0, CHAT_TEAM);
				BotVoiceChatOnly(bs, -1, VOICECHAT_ONOFFENSE);
				bs->teammessage_time = 0;
			}
			// set the bot goal
			memcpy(goal, &neutralobelisk, sizeof(bot_goal_t));
			// stop after some time
			if (bs->teamgoal_time < FloatTime()) {
				bs->ltgtype = 0;
			}
			// if touching the goal
			if (trap_BotTouchingGoal(bs->origin, goal)) {
				bs->harvestaway_time = FloatTime() + 4 + 3 * random();
			}

			return qtrue;
		}
	}
	// normal goal stuff
	return BotGetItemLongTermGoal(bs, tfl, goal);
}

/*
=======================================================================================================================================
BotLongTermGoal
=======================================================================================================================================
*/
int BotLongTermGoal(bot_state_t *bs, int tfl, int retreat, bot_goal_t *goal) {
	aas_entityinfo_t entinfo;
	char teammate[MAX_MESSAGE_SIZE];
	float squaredist;
	int areanum;
	vec3_t dir;

	// FIXME: also have air long term goals?

	// if the bot is leading someone and not retreating
	if (bs->lead_time > 0 && !retreat) {
		if (bs->lead_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "lead_stop", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
			trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
			bs->lead_time = 0;
			return BotGetLongTermGoal(bs, tfl, retreat, goal);
		}

		if (bs->leadmessage_time < 0 && -bs->leadmessage_time < FloatTime()) {
			BotAI_BotInitialChat(bs, "followme", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
			trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
			bs->leadmessage_time = FloatTime();
		}
		// get the entity information
		BotEntityInfo(bs->lead_teammate, &entinfo);
		// if the entity information is valid
		if (entinfo.valid) {
			areanum = BotPointAreaNum(entinfo.origin);

			if (areanum && trap_AAS_AreaReachability(areanum)) {
				// update team goal
				bs->lead_teamgoal.entitynum = bs->lead_teammate;
				bs->lead_teamgoal.areanum = areanum;

				VectorCopy(entinfo.origin, bs->lead_teamgoal.origin);
				VectorSet(bs->lead_teamgoal.mins, -8, -8, -8);
				VectorSet(bs->lead_teamgoal.maxs, 8, 8, 8);
			}
		}
		// if the team mate is visible
		if (BotEntityVisible(&bs->cur_ps, 360, bs->lead_teammate)) {
			bs->leadvisible_time = FloatTime();
		}
		// if the team mate is not visible for 1 seconds
		if (bs->leadvisible_time < FloatTime() - 1) {
			bs->leadbackup_time = FloatTime() + 2;
		}
		// distance towards the team mate
		VectorSubtract(bs->origin, bs->lead_teamgoal.origin, dir);

		squaredist = VectorLengthSquared(dir);
		// if backing up towards the team mate
		if (bs->leadbackup_time > FloatTime()) {
			if (bs->leadmessage_time < FloatTime() - 20) {
				BotAI_BotInitialChat(bs, "followme", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
				trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
				bs->leadmessage_time = FloatTime();
			}
			// if very close to the team mate
			if (squaredist < Square(100)) {
				bs->leadbackup_time = 0;
			}
			// the bot should go back to the team mate
			memcpy(goal, &bs->lead_teamgoal, sizeof(bot_goal_t));
			return qtrue;
		} else {
			// if quite distant from the team mate
			if (squaredist > Square(500)) {
				if (bs->leadmessage_time < FloatTime() - 20) {
					BotAI_BotInitialChat(bs, "followme", EasyClientName(bs->lead_teammate, teammate, sizeof(teammate)), NULL);
					trap_BotEnterChat(bs->cs, bs->teammate, CHAT_TELL);
					bs->leadmessage_time = FloatTime();
				}
				// look at the team mate
				VectorSubtract(entinfo.origin, bs->origin, dir);
				VectorToAngles(dir, bs->ideal_viewangles);
				bs->ideal_viewangles[2] *= 0.5;
				// just wait for the team mate
				return qfalse;
			}
		}
	}

	return BotGetLongTermGoal(bs, tfl, retreat, goal);
}

/*
=======================================================================================================================================
AIEnter_Intermission
=======================================================================================================================================
*/
void AIEnter_Intermission(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "INTERMISION", "", s);
	// reset the bot state
	BotResetState(bs);
	// check for end level chat
	if (BotChat_EndLevel(bs)) {
		trap_BotEnterChat(bs->cs, 0, bs->chatto);
	}

	bs->ainode = AINode_Intermission;
}

/*
=======================================================================================================================================
AINode_Intermission
=======================================================================================================================================
*/
int AINode_Intermission(bot_state_t *bs) {

	// if the intermission ended
	if (!BotIntermission(bs)) {
		BotChat_StartLevel(bs);
		bs->stand_time = 0;
	}

	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Observer
=======================================================================================================================================
*/
void AIEnter_Observer(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "OBSERVER", "", s);
	// reset the bot state
	BotResetState(bs);

	bs->ainode = AINode_Observer;
}

/*
=======================================================================================================================================
AINode_Observer
=======================================================================================================================================
*/
int AINode_Observer(bot_state_t *bs) {

	// if the bot left observer mode
	if (!BotIsObserver(bs)) {
		AIEnter_Stand(bs, "OBSERVER: left observer.");
	}

	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Stand
=======================================================================================================================================
*/
void AIEnter_Stand(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "STAND", "", s);

	bs->standfindenemy_time = FloatTime() + 1;
	bs->ainode = AINode_Stand;
}

/*
=======================================================================================================================================
AINode_Stand
=======================================================================================================================================
*/
int AINode_Stand(bot_state_t *bs) {

	if (bs->standfindenemy_time < FloatTime()) {
		if (BotFindEnemy(bs, -1)) {
			AIEnter_Battle_Fight(bs, "STAND: found enemy.");
			return qfalse;
		}

		bs->standfindenemy_time = FloatTime() + 1;
	}
	// when done standing
	if (bs->stand_time < FloatTime()) {
		trap_BotEnterChat(bs->cs, 0, bs->chatto);
		AIEnter_Seek_LTG(bs, "STAND: time out.");
		return qfalse;
	}

	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Respawn
=======================================================================================================================================
*/
void AIEnter_Respawn(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "RESPAWN", "", s);
	// reset some states
	trap_BotResetMoveState(bs->ms);
	trap_BotResetGoalState(bs->gs);
	trap_BotResetAvoidGoals(bs->gs);
	trap_BotResetAvoidReach(bs->ms);
	// if the bot wants to chat
	if (BotChat_Death(bs)) {
		bs->respawnchat_time = FloatTime();
	} else {
		bs->respawnchat_time = 0;
	}

	bs->respawn_time = FloatTime() + 2;
	// set respawn state
	bs->respawn_wait = qfalse;
	bs->ainode = AINode_Respawn;
}

/*
=======================================================================================================================================
AINode_Respawn
=======================================================================================================================================
*/
int AINode_Respawn(bot_state_t *bs) {

	// if waiting for the actual respawn
	if (bs->respawn_wait) {
		if (!BotIsDead(bs)) {
			AIEnter_Seek_LTG(bs, "RESPAWN: respawned.");
		} else {
			trap_EA_Respawn(bs->client);
		}
	} else if (bs->respawn_time < FloatTime()) {
		// wait until respawned
		bs->respawn_wait = qtrue;
		// elementary action respawn
		trap_EA_Respawn(bs->client);

		if (bs->respawnchat_time) {
			trap_BotEnterChat(bs->cs, 0, bs->chatto);
			bs->enemy = -1;
		}
	}

	return qtrue;
}

/*
=======================================================================================================================================
BotSelectActivateWeapon
=======================================================================================================================================
*/
int BotSelectActivateWeapon(bot_state_t *bs) {

	if (bs->inventory[INVENTORY_MACHINEGUN] > 0 && bs->inventory[INVENTORY_BULLETS] > 0) {
		return WEAPONINDEX_MACHINEGUN;
	} else if (bs->inventory[INVENTORY_CHAINGUN] > 0 && bs->inventory[INVENTORY_BELT] > 0) {
		return WEAPONINDEX_CHAINGUN;
	} else if (bs->inventory[INVENTORY_SHOTGUN] > 0 && bs->inventory[INVENTORY_SHELLS] > 0) {
		return WEAPONINDEX_SHOTGUN;
	} else if (bs->inventory[INVENTORY_NAILGUN] > 0 && bs->inventory[INVENTORY_NAILS] > 0) {
		return WEAPONINDEX_NAILGUN;
	} else if (bs->inventory[INVENTORY_PROXLAUNCHER] > 0 && bs->inventory[INVENTORY_MINES] > 0) {
		return WEAPONINDEX_PROXLAUNCHER;
	} else if (bs->inventory[INVENTORY_GRENADELAUNCHER] > 0 && bs->inventory[INVENTORY_GRENADES] > 0) {
		return WEAPONINDEX_GRENADELAUNCHER;
	} else if (bs->inventory[INVENTORY_NAPALMLAUNCHER] > 0 && bs->inventory[INVENTORY_CANISTERS] > 0) {
		return WEAPONINDEX_NAPALMLAUNCHER;
	} else if (bs->inventory[INVENTORY_ROCKETLAUNCHER] > 0 && bs->inventory[INVENTORY_ROCKETS] > 0) {
		return WEAPONINDEX_ROCKETLAUNCHER;
	} else if (bs->inventory[INVENTORY_BEAMGUN] > 0 && bs->inventory[INVENTORY_BEAMGUN_AMMO] > 0) {
		return WEAPONINDEX_BEAMGUN;
	} else if (bs->inventory[INVENTORY_RAILGUN] > 0 && bs->inventory[INVENTORY_SLUGS] > 0) {
		return WEAPONINDEX_RAILGUN;
	} else if (bs->inventory[INVENTORY_PLASMAGUN] > 0 && bs->inventory[INVENTORY_CELLS] > 0) {
		return WEAPONINDEX_PLASMAGUN;
	} else if (bs->inventory[INVENTORY_BFG10K] > 0 && bs->inventory[INVENTORY_BFG_AMMO] > 0) {
		return WEAPONINDEX_BFG;
	} else {
		return -1;
	}
}

/*
=======================================================================================================================================
BotClearPath

Try to deactivate obstacles like proximity mines on the bot's path.
=======================================================================================================================================
*/
void BotClearPath(bot_state_t *bs, bot_moveresult_t *moveresult) {
	int i, bestmine;
	float dist, bestdist;
	vec3_t target, dir;
	bsp_trace_t bsptrace;
	entityState_t state;

	// if there is a dead body wearing kamikaze nearby
	if (bs->kamikazebody) {
		// if the bot's view angles and weapon are not used for movement
		if (!(moveresult->flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_MOVEMENTWEAPON))) {
			BotAI_GetEntityState(bs->kamikazebody, &state);
			VectorCopy(state.pos.trBase, target);

			target[2] += 8;

			VectorSubtract(target, bs->eye, dir);
			VectorToAngles(dir, moveresult->ideal_viewangles);

			moveresult->weapon = BotSelectActivateWeapon(bs);

			if (moveresult->weapon == -1) {
				// FIXME: run away!
				moveresult->weapon = 0;
			}

			if (moveresult->weapon) {
				moveresult->flags |= MOVERESULT_MOVEMENTVIEW|MOVERESULT_MOVEMENTWEAPON;
				// if holding the right weapon
				if (bs->cur_ps.weapon == moveresult->weapon) {
					// if the bot is pretty close with its aim
					if (InFieldOfVision(bs->viewangles, 20, moveresult->ideal_viewangles)) {
						BotAI_Trace(&bsptrace, bs->eye, NULL, NULL, target, bs->entitynum, MASK_SHOT);
						// if the corpse is visible from the current position
						if (bsptrace.fraction >= 1.0 || bsptrace.entityNum == state.number) {
							// shoot at the kamikaze corpse
							trap_EA_Attack(bs->client);
						}
					}
				}
			}
		}
	}

	if (moveresult->flags & MOVERESULT_BLOCKEDBYAVOIDSPOT) {
		bs->blockedbyavoidspot_time = FloatTime() + 5;
	}
	// if blocked by an avoid spot and the view angles and weapon are used for movement
	if (bs->blockedbyavoidspot_time > FloatTime() && !(moveresult->flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_MOVEMENTWEAPON))) {
		bestdist = 300;
		bestmine = -1;

		for (i = 0; i < bs->numproxmines; i++) {
			BotAI_GetEntityState(bs->proxmines[i], &state);
			VectorSubtract(state.pos.trBase, bs->origin, dir);
			dist = VectorLength(dir);

			if (dist < bestdist) {
				bestdist = dist;
				bestmine = i;
			}
		}

		if (bestmine != -1) {
			// deactivate prox mines in the bot's path by shooting rockets or plasma cells etc. at them
			BotAI_GetEntityState(bs->proxmines[bestmine], &state);
			VectorCopy(state.pos.trBase, target);

			target[2] += 2;

			VectorSubtract(target, bs->eye, dir);
			VectorToAngles(dir, moveresult->ideal_viewangles);
			// if the bot has a weapon that does splash damage
			if (bs->inventory[INVENTORY_PLASMAGUN] > 0 && bs->inventory[INVENTORY_CELLS] > 0) {
				moveresult->weapon = WEAPONINDEX_PLASMAGUN;
			} else if (bs->inventory[INVENTORY_ROCKETLAUNCHER] > 0 && bs->inventory[INVENTORY_ROCKETS] > 0) {
				moveresult->weapon = WEAPONINDEX_ROCKETLAUNCHER;
			} else if (bs->inventory[INVENTORY_BFG10K] > 0 && bs->inventory[INVENTORY_BFG_AMMO] > 0) {
				moveresult->weapon = WEAPONINDEX_BFG;
			} else {
				moveresult->weapon = 0;
			}

			if (moveresult->weapon) {
				moveresult->flags |= MOVERESULT_MOVEMENTVIEW|MOVERESULT_MOVEMENTWEAPON;
				// if holding the right weapon
				if (bs->cur_ps.weapon == moveresult->weapon) {
					// if the bot is pretty close with its aim
					if (InFieldOfVision(bs->viewangles, 20, moveresult->ideal_viewangles)) {
						BotAI_Trace(&bsptrace, bs->eye, NULL, NULL, target, bs->entitynum, MASK_SHOT);
						// if the mine is visible from the current position
						if (bsptrace.fraction >= 1.0 || bsptrace.entityNum == state.number) {
							// shoot at the mine
							trap_EA_Attack(bs->client);
						}
					}
				}
			}
		}
	}
}

/*
=======================================================================================================================================
AIEnter_Wait
=======================================================================================================================================
*/
void AIEnter_Wait(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "waiting", "", s);
	bs->wait_time = FloatTime();
	bs->ainode = AINode_Wait;
}

/*
=======================================================================================================================================
AINode_Wait
=======================================================================================================================================
*/
int AINode_Wait(bot_state_t *bs) {
	bot_goal_t goal;
	vec3_t target, dir;
	bot_moveresult_t moveresult;

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "waiting: observer");
		return qfalse;
	}
	// if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "waiting: intermission");
		return qfalse;
	}
	// respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "waiting: bot dead");
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	// if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA|TFL_SLIME;
	}
	// map specific code
	BotMapScripts(bs);
	// no enemy
	bs->enemy = -1;
	// if there is an enemy
	if (BotFindEnemy(bs, -1)) {
		if (BotWantsToRetreat(bs)) {
			// keep the current long term goal and retreat
			AIEnter_Battle_Retreat(bs, "waiting: found enemy");
			return qfalse;
		} else {
			trap_BotResetLastAvoidReach(bs->ms);
			// empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			// go fight
			AIEnter_Battle_Fight(bs, "waiting: found enemy");
			return qfalse;
		}
	}
	// check if the bot is blocking teammates
	BotCheckBlockedTeammates(bs);
	// if the view angles are used for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	// if waiting for something
	} else if (moveresult.flags & MOVERESULT_WAITING) {
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	} else if (!(bs->flags & BFL_IDEALVIEWSET)) {
		if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
		// FIXME: look at cluster portals?
		} else if (VectorLengthSquared(moveresult.movedir)) {
			VectorToAngles(moveresult.movedir, bs->ideal_viewangles);
		} else if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}

		bs->ideal_viewangles[2] *= 0.5;
	}
	// if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) {
		bs->weaponnum = moveresult.weapon;
	}
	// when done waiting
	if (bs->wait_time < FloatTime() - 0.5) {
		AIEnter_Seek_LTG(bs, "waiting: time out");
		return qfalse;
	}

	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Seek_ActivateEntity
=======================================================================================================================================
*/
void AIEnter_Seek_ActivateEntity(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "ACTIVATE ENTITY", "", s);
	bs->ainode = AINode_Seek_ActivateEntity;
}

/*
=======================================================================================================================================
AINode_Seek_ActivateEntity
=======================================================================================================================================
*/
int AINode_Seek_ActivateEntity(bot_state_t *bs) {
	bot_goal_t *goal;
	vec3_t target, dir, ideal_viewangles;
	bot_moveresult_t moveresult;
	int targetvisible;
	bsp_trace_t bsptrace;
	aas_entityinfo_t entinfo;

	if (BotIsObserver(bs)) {
		BotClearActivateGoalStack(bs);
		AIEnter_Observer(bs, "ACTIVATE ENTITY: joined observer.");
		return qfalse;
	}
	// if in the intermission
	if (BotIntermission(bs)) {
		BotClearActivateGoalStack(bs);
		AIEnter_Intermission(bs, "ACTIVATE ENTITY: joined intermission.");
		return qfalse;
	}
	// respawn if dead
	if (BotIsDead(bs)) {
		BotClearActivateGoalStack(bs);
		AIEnter_Respawn(bs, "ACTIVATE ENTITY: bot dead.");
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	// if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA|TFL_SLIME;
	}
	// map specific code
	BotMapScripts(bs);
	// no enemy
	bs->enemy = -1;
	// if the bot has no activate goal
	if (!bs->activatestack) {
		BotClearActivateGoalStack(bs);
		AIEnter_Seek_LTG(bs, "ACTIVATE ENTITY: no goal.");
		return qfalse;
	}

	goal = &bs->activatestack->goal;
	// initialize target being visible to false
	targetvisible = qfalse;
	// if the bot has to shoot at a target to activate something
	if (bs->activatestack->shoot) {
		BotAI_Trace(&bsptrace, bs->eye, NULL, NULL, bs->activatestack->target, bs->entitynum, MASK_SHOT);
		// if the shootable entity is visible from the current position
		if (bsptrace.fraction >= 1.0 || bsptrace.entityNum == goal->entitynum) {
			targetvisible = qtrue;
			// if holding the right weapon
			if (bs->cur_ps.weapon == bs->activatestack->weapon) {
				VectorSubtract(bs->activatestack->target, bs->eye, dir);
				VectorToAngles(dir, ideal_viewangles);
				// if the bot is pretty close with its aim
				if (InFieldOfVision(bs->viewangles, 20, ideal_viewangles)) {
					trap_EA_Attack(bs->client);
				}
			}
		}
	}
	// if the shoot target is visible
	if (targetvisible) {
		// get the entity information of the entity the bot is shooting at
		BotEntityInfo(goal->entitynum, &entinfo);
		// if the entity the bot shoots at moved
		if (!VectorCompare(bs->activatestack->origin, entinfo.origin)) {
#ifdef DEBUG
			BotAI_Print(PRT_MESSAGE, "AINode_Seek_ActivateEntity: hit shootable button or trigger.\n");
#endif // DEBUG
			bs->activatestack->time = 0;
		}
		// if the activate goal has been activated or the bot takes too long
		if (bs->activatestack->time < FloatTime()) {
			BotPopFromActivateGoalStack(bs);
			// if there are more activate goals on the stack
			if (bs->activatestack) {
				bs->activatestack->time = FloatTime() + 10;
				return qfalse;
			}

			AIEnter_Seek_NBG(bs, "activate entity: time out");
			return qfalse;
		}

		memset(&moveresult, 0, sizeof(bot_moveresult_t));
	} else {
		// if the bot has no goal
		if (!goal) {
			bs->activatestack->time = 0;
		// if the bot does not have a shoot goal
		} else if (!bs->activatestack->shoot) {
			// if the bot touches the current goal
			if (trap_BotTouchingGoal(bs->origin, goal)) {
#ifdef DEBUG
				BotAI_Print(PRT_MESSAGE, "AINode_Seek_ActivateEntity: touched button or trigger.\n");
#endif // DEBUG
				bs->activatestack->time = 0;
			}
		}
		// if the activate goal has been activated or the bot takes too long
		if (bs->activatestack->time < FloatTime()) {
			BotPopFromActivateGoalStack(bs);
			// if there are more activate goals on the stack
			if (bs->activatestack) {
				bs->activatestack->time = FloatTime() + 10;
				return qfalse;
			}

			AIEnter_Seek_NBG(bs, "activate entity: activated");
			return qfalse;
		}
		// move towards the goal
		trap_BotMoveToGoal(&moveresult, bs->ms, goal, bs->tfl);
		// if the movement failed
		if (moveresult.failure) {
			// reset the avoid reach, otherwise bot is stuck in current area
			trap_BotResetAvoidReach(bs->ms);
			//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
			bs->activatestack->time = 0;
		}
	}
	// check if the bot has to deactivate obstacles
	BotClearPath(bs, &moveresult);
	// check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qtrue);
	// predict obstacles
	if (BotAIPredictObstacles(bs, goal)) {
		return qfalse;
	}
	// if the bot has to shoot to activate
	if (bs->activatestack->shoot) {
		// if the view angles aren't yet used for the movement
		if (!(moveresult.flags & MOVERESULT_MOVEMENTVIEW)) {
			VectorSubtract(bs->activatestack->target, bs->eye, dir);
			VectorToAngles(dir, moveresult.ideal_viewangles);
			// set the movement view flag
			moveresult.flags |= MOVERESULT_MOVEMENTVIEW;
		}
		// if there's no weapon yet used for the movement
		if (!(moveresult.flags & MOVERESULT_MOVEMENTWEAPON)) {
			// set the movement view flag
			moveresult.flags |= MOVERESULT_MOVEMENTWEAPON;
			bs->activatestack->weapon = BotSelectActivateWeapon(bs);

			if (bs->activatestack->weapon == -1) {
				// FIXME: find a decent weapon first
				bs->activatestack->weapon = 0;
			}

			moveresult.weapon = bs->activatestack->weapon;
		}
	}
	// if the view angles are used for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	// if waiting for something
	} else if (moveresult.flags & MOVERESULT_WAITING) {
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	} else if (!(bs->flags & BFL_IDEALVIEWSET)) {
		if (trap_BotMovementViewTarget(bs->ms, goal, bs->tfl, 300, target)) {
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
		} else {
			VectorToAngles(moveresult.movedir, bs->ideal_viewangles);
		}

		bs->ideal_viewangles[2] *= 0.5;
	}
	// if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) {
		bs->weaponnum = moveresult.weapon;
	}
	// if there is an enemy
	if (BotFindEnemy(bs, -1)) {
		if (BotWantsToRetreat(bs)) {
			// keep the current long term goal and retreat
			AIEnter_Battle_NBG(bs, "ACTIVATE ENTITY: found enemy.");
		} else {
			trap_BotResetLastAvoidReach(bs->ms);
			// empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			// go fight
			AIEnter_Battle_Fight(bs, "ACTIVATE ENTITY: found enemy.");
		}

		BotClearActivateGoalStack(bs);
	}

	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Seek_NBG
=======================================================================================================================================
*/
void AIEnter_Seek_NBG(bot_state_t *bs, char *s) {
	bot_goal_t goal;
	char buf[144];

	if (trap_BotGetTopGoal(bs->gs, &goal)) {
		trap_BotGoalName(goal.number, buf, 144);
		BotRecordNodeSwitch(bs, "SEEK NBG", buf, s);
	} else {
		BotRecordNodeSwitch(bs, "SEEK NBG", "No goal", s);
	}

	bs->ainode = AINode_Seek_NBG;
}

/*
=======================================================================================================================================
AINode_Seek_NBG
=======================================================================================================================================
*/
int AINode_Seek_NBG(bot_state_t *bs) {
	bot_goal_t goal;
	vec3_t target, dir;
	bot_moveresult_t moveresult;

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "SEEK NBG: joined observer.");
		return qfalse;
	}
	// if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "SEEK NBG: joined intermission.");
		return qfalse;
	}
	// respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "SEEK NBG: bot dead.");
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	// if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA|TFL_SLIME;
	}

	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}
	// map specific code
	BotMapScripts(bs);
	// no enemy
	bs->enemy = -1;
	// if the bot has no goal
	if (!trap_BotGetTopGoal(bs->gs, &goal)) {
		bs->nbg_time = 0;
	// if the bot touches the current goal
	} else if (BotReachedGoal(bs, &goal)) {
		bs->nbg_time = 0;
		bs->check_time = FloatTime() + 0.05; // immediately check for another NBG
	}

	if (bs->nbg_time < FloatTime()) {
		// pop the current goal from the stack
		trap_BotPopGoal(bs->gs);
		// check for new nearby items right away
		// NOTE: we can NOT reset the check_time to zero because it would create an endless loop of node switches
		bs->check_time = FloatTime() + 0.05;
		// go back to seek ltg
		AIEnter_Seek_LTG(bs, "SEEK NBG: time out.");
		return qfalse;
	}
	// if the bot should wait
	if (BotCanWait(bs, &goal)) {
		AIEnter_Wait(bs, "seek nbg: waiting");
		return qfalse;
	}
	// move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);
	// if the movement failed
	if (moveresult.failure) {
		// reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->nbg_time = 0;
	}

	BotClearPath(bs, &moveresult);
	// check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qtrue);
	// predict obstacles
	if (BotAIPredictObstacles(bs, &goal)) {
		return qfalse;
	}
	// if the view angles are used for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	// if waiting for something
	} else if (moveresult.flags & MOVERESULT_WAITING) {
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	} else if (!(bs->flags & BFL_IDEALVIEWSET)) {
		if (!trap_BotGetSecondGoal(bs->gs, &goal)) {
			trap_BotGetTopGoal(bs->gs, &goal);
		}

		if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
		// FIXME: look at cluster portals?
		} else {
			VectorToAngles(moveresult.movedir, bs->ideal_viewangles);
		}

		bs->ideal_viewangles[2] *= 0.5;
	}
	// if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) {
		bs->weaponnum = moveresult.weapon;
	}
	// if there is an enemy
	if (BotFindEnemy(bs, -1)) {
		if (BotWantsToRetreat(bs)) {
			// keep the current long term goal and retreat
			AIEnter_Battle_NBG(bs, "SEEK NBG: found enemy.");
		} else {
			trap_BotResetLastAvoidReach(bs->ms);
			// empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			// go fight
			AIEnter_Battle_Fight(bs, "SEEK NBG: found enemy.");
		}
	}

	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Seek_LTG
=======================================================================================================================================
*/
void AIEnter_Seek_LTG(bot_state_t *bs, char *s) {
	bot_goal_t goal;
	char buf[144];

	if (trap_BotGetTopGoal(bs->gs, &goal)) {
		trap_BotGoalName(goal.number, buf, 144);
		BotRecordNodeSwitch(bs, "SEEK LTG", buf, s);
	} else {
		BotRecordNodeSwitch(bs, "SEEK LTG", "No goal", s);
	}

	bs->ainode = AINode_Seek_LTG;
}

/*
=======================================================================================================================================
AINode_Seek_LTG
=======================================================================================================================================
*/
int AINode_Seek_LTG(bot_state_t *bs) {
	bot_goal_t goal;
	vec3_t target, dir;
	bot_moveresult_t moveresult;
	int range;
	//char buf[128];
	//bot_goal_t tmpgoal;

	range = trap_Characteristic_BInteger(bs->character, CHARACTERISTIC_GOAL_MULTIPLIER, 0, 10000);

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "SEEK LTG: joined observer.");
		return qfalse;
	}
	// if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "SEEK LTG: joined intermission.");
		return qfalse;
	}
	// respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "SEEK LTG: bot dead.");
		return qfalse;
	}

	BotChat_Random(bs);

	bs->tfl = TFL_DEFAULT;
	// if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA|TFL_SLIME;
	}

	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}

	BotTeamGoals(bs, qfalse);
	// map specific code
	BotMapScripts(bs);
	// no enemy
	bs->enemy = -1;
	// if there is an enemy
	if (BotFindEnemy(bs, -1)) {
		if (BotWantsToRetreat(bs)) {
			// keep the current long term goal and retreat
			AIEnter_Battle_Retreat(bs, "SEEK LTG: found enemy.");
			return qfalse;
		} else {
			trap_BotResetLastAvoidReach(bs->ms);
			// empty the goal stack
			trap_BotEmptyGoalStack(bs->gs);
			// go fight
			AIEnter_Battle_Fight(bs, "SEEK LTG: found enemy.");
			return qfalse;
		}
	}

	if (bs->killedenemy_time > FloatTime() - 2) {
		if (random() < bs->thinktime) {
			trap_EA_Gesture(bs->client);
		}
	}
	// check for nearby goals periodicly
	if (bs->check_time < FloatTime()) {
		bs->check_time = FloatTime() + 0.05;
		// check if the bot wants to camp
		BotWantsToCamp(bs);
		// if carrying a flag and the own flag is at base, or if the bot is trying to get the flag and the own flag is NOT at base the bot shouldn't be distracted too much and should ignore some items
		if (BotHasEmergencyGoal(bs)) {
			range *= 0.1; // 50
		} else if (!BotFeelingBad(bs)) {
			range *= 0.3; // 150
		}

		if (BotNearbyGoal(bs, bs->tfl, &goal, range)) {
			trap_BotResetLastAvoidReach(bs->ms);
			// get the goal at the top of the stack
			//trap_BotGetTopGoal(bs->gs, &tmpgoal);
			//trap_BotGoalName(tmpgoal.number, buf, 144);
			//BotAI_Print(PRT_MESSAGE, "new nearby goal %s\n", buf);
			// time the bot gets to pick up the nearby goal item
			bs->nbg_time = FloatTime() + 4 + range * 0.01;
			AIEnter_Seek_NBG(bs, "SEEK LTG: check for Nbg.");
			return qfalse;
		}
	}
	// get the current long term goal after checking for NBG's
	if (!BotLongTermGoal(bs, bs->tfl, qfalse, &goal)) {
		return qtrue;
	}
	// if the bot should wait
	if (BotCanWait(bs, &goal)) {
		AIEnter_Wait(bs, "seek ltg: waiting");
		return qfalse;
	}
	// move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);
	// if the movement failed
	if (moveresult.failure) {
		// reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	// check if the bot has to deactivate obstacles
	BotClearPath(bs, &moveresult);
	// check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qtrue);
	// predict obstacles
	if (BotAIPredictObstacles(bs, &goal)) {
		return qfalse;
	}
	// if the view angles are used for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	// if waiting for something
	} else if (moveresult.flags & MOVERESULT_WAITING) {
		if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}
	} else if (!(bs->flags & BFL_IDEALVIEWSET)) {
		if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
		// FIXME: look at cluster portals?
		} else if (VectorLengthSquared(moveresult.movedir)) {
			VectorToAngles(moveresult.movedir, bs->ideal_viewangles);
		} else if (random() < bs->thinktime * 0.8) {
			BotRoamGoal(bs, target);
			VectorSubtract(target, bs->origin, dir);
			VectorToAngles(dir, bs->ideal_viewangles);
			bs->ideal_viewangles[2] *= 0.5;
		}

		bs->ideal_viewangles[2] *= 0.5;
	}
	// if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) {
		bs->weaponnum = moveresult.weapon;
	}

	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Battle_Fight
=======================================================================================================================================
*/
void AIEnter_Battle_Fight(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "BATTLE FIGHT", "", s);
	trap_BotResetLastAvoidReach(bs->ms);

	bs->ainode = AINode_Battle_Fight;
	bs->flags &= ~BFL_FIGHTSUICIDAL;
}

/*
=======================================================================================================================================
AIEnter_Battle_SuicidalFight
=======================================================================================================================================
*/
void AIEnter_Battle_SuicidalFight(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "BATTLE FIGHT", "", s);
	trap_BotResetLastAvoidReach(bs->ms);

	bs->ainode = AINode_Battle_Fight;
	bs->flags |= BFL_FIGHTSUICIDAL;
}

/*
=======================================================================================================================================
AINode_Battle_Fight
=======================================================================================================================================
*/
int AINode_Battle_Fight(bot_state_t *bs) {
	int areanum, range;
	bot_goal_t goal;
	vec3_t target;
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;

	range = trap_Characteristic_BInteger(bs->character, CHARACTERISTIC_GOAL_MULTIPLIER, 0, 10000);

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "BATTLE FIGHT: joined observer.");
		return qfalse;
	}
	// if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "BATTLE FIGHT: joined intermission.");
		return qfalse;
	}
	// respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "BATTLE FIGHT: bot dead.");
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	// if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA|TFL_SLIME;
	}

	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}
	// if there is another better enemy
	if (BotFindEnemy(bs, bs->enemy)) {

	}
	// if no enemy
	if (bs->enemy < 0) {
		AIEnter_Seek_LTG(bs, "BATTLE FIGHT: no enemy.");
		return qfalse;
	}
	// get the entity information
	BotEntityInfo(bs->enemy, &entinfo);
	// if the enemy is dead
	if (bs->enemydeath_time) {
		if (bs->enemydeath_time < FloatTime() - 1.0) {
			bs->enemydeath_time = 0;

			if (bs->enemysuicide) {
				BotChat_EnemySuicide(bs);
			}

			if (bs->lastkilledplayer == bs->enemy) {
				BotChat_Kill(bs);
			}

			bs->ltg_time = 0;

			AIEnter_Seek_LTG(bs, "BATTLE FIGHT: enemy dead.");
			return qfalse;
		}
	} else {
		// if the entity isn't dead
		if (EntityIsDead(&entinfo)) {
			bs->enemydeath_time = FloatTime();
			bs->check_time = FloatTime() + 0.05; // the enemy might have lost something (weapons, flags, ...)
		}
	}

	VectorCopy(entinfo.origin, target);
	// if not a player enemy
	if (bs->enemy >= MAX_CLIENTS && (bs->enemy == redobelisk.entitynum || bs->enemy == blueobelisk.entitynum)) {
		target[2] += OBELISK_TARGET_HEIGHT;
	}
	// update the reachability area and origin if possible
	areanum = BotPointAreaNum(target);

	if (areanum && trap_AAS_AreaReachability(areanum)) {
		VectorCopy(target, bs->lastenemyorigin);
		bs->lastenemyareanum = areanum;
	}
	// if the bot's health decreased
	if (bs->lastframe_health > bs->inventory[INVENTORY_HEALTH]) {
		BotChat_HitNoDeath(bs);
	}
	// if the bot hit someone
	if (bs->cur_ps.persistant[PERS_HITS] > bs->lasthitcount) {
		BotChat_HitNoKill(bs);
	}
	// update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);
	// if the enemy is NOT visible
	if (!BotEntityVisible(&bs->cur_ps, 360, bs->enemy)) {
		// if the enemy is NOT visible for 1 second
		if (bs->enemyvisible_time < FloatTime() - 1) {
			if (BotWantsToChase(bs)) {
				AIEnter_Battle_Chase(bs, "BATTLE FIGHT: enemy out of sight.");
				return qfalse;
			} else {
				AIEnter_Seek_LTG(bs, "BATTLE FIGHT: enemy out of sight.");
				return qfalse;
			}
		}
	}
	// only check for nearby goals if not in water, it takes too long
	if (BotFeelingBad(bs) && !trap_AAS_Swimming(bs->origin) && !BotCTFCarryingFlag(bs) && !Bot1FCTFCarryingFlag(bs) && !BotHarvesterCarryingCubes(bs)) {
		// check for nearby goals periodicly
		if (bs->check_time < FloatTime()) {
			bs->check_time = FloatTime() + 0.05;
			range *= 0.6; // 300

			if (BotNearbyGoal(bs, bs->tfl, &goal, range)) {
				trap_BotResetLastAvoidReach(bs->ms);
				// time the bot gets to pick up the nearby goal item
				bs->nbg_time = FloatTime() + (range * 0.006);
				AIEnter_Battle_NBG(bs, "battle fight: going for NBG");
				return qfalse;
			}
		}
	}
	// do attack movements
	moveresult = BotAttackMove(bs, bs->tfl);
	// if the movement failed
	if (moveresult.failure) {
		// reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	// check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qfalse);
	// aim at the enemy
	BotAimAtEnemy(bs);
	// attack the enemy if possible
	BotCheckAttack(bs);
	// if the bot wants to retreat
	if (!(bs->flags & BFL_FIGHTSUICIDAL)) {
		if (BotWantsToRetreat(bs)) {
			AIEnter_Battle_Retreat(bs, "BATTLE FIGHT: wants to retreat.");
			return qtrue;
		}
	}

	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Battle_Chase
=======================================================================================================================================
*/
void AIEnter_Battle_Chase(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "BATTLE CHASE", "", s);
	bs->chase_time = FloatTime();
	bs->ainode = AINode_Battle_Chase;
}

/*
=======================================================================================================================================
AINode_Battle_Chase
=======================================================================================================================================
*/
int AINode_Battle_Chase(bot_state_t *bs) {
	bot_goal_t goal;
	vec3_t target, dir;
	bot_moveresult_t moveresult;
	int range;

	range = trap_Characteristic_BInteger(bs->character, CHARACTERISTIC_GOAL_MULTIPLIER, 0, 10000);

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "BATTLE CHASE: joined observer.");
		return qfalse;
	}
	// if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "BATTLE CHASE: joined intermission.");
		return qfalse;
	}
	// respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "BATTLE CHASE: bot dead.");
		return qfalse;
	}
	// if no enemy
	if (bs->enemy < 0) {
		AIEnter_Seek_LTG(bs, "BATTLE CHASE: no enemy.");
		return qfalse;
	}
	// if the enemy is visible
	if (BotEntityVisible(&bs->cur_ps, 360, bs->enemy)) {
		AIEnter_Battle_Fight(bs, "BATTLE CHASE: enemy visible.");
		return qfalse;
	}
	// if there is another enemy
	if (BotFindEnemy(bs, -1)) {
		AIEnter_Battle_Fight(bs, "BATTLE CHASE: better enemy.");
		return qfalse;
	}
	// there is no last enemy area
	if (!bs->lastenemyareanum) {
		AIEnter_Seek_LTG(bs, "BATTLE CHASE: no enemy area.");
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	// if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA|TFL_SLIME;
	}

	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}
	// map specific code
	BotMapScripts(bs);
	// create the chase goal
	goal.entitynum = bs->enemy;
	goal.areanum = bs->lastenemyareanum;

	VectorCopy(bs->lastenemyorigin, goal.origin);
	VectorSet(goal.mins, -8, -8, -8);
	VectorSet(goal.maxs, 8, 8, 8);
	// update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);
	// if the last seen enemy spot is reached the enemy could not be found
	if (trap_BotTouchingGoal(bs->origin, &goal)) {
		bs->chase_time = 0;
	}
	// if there's no chase time left
	if (!bs->chase_time || bs->chase_time < FloatTime() - 10) {
		AIEnter_Seek_LTG(bs, "BATTLE CHASE: time out.");
		return qfalse;
	}
	// check for nearby goals periodicly
	if (bs->check_time < FloatTime()) {
		bs->check_time = FloatTime() + 0.05;
		range *= 0.3; // 150

		if (BotNearbyGoal(bs, bs->tfl, &goal, range)) {
			trap_BotResetLastAvoidReach(bs->ms);
			// time the bot gets to pick up the nearby goal item
			bs->nbg_time = FloatTime() + 0.1 * range + 1;
			AIEnter_Battle_NBG(bs, "BATTLE CHASE: check for Nbg.");
			return qfalse;
		}
	}
	// if the bot should wait
	if (BotCanWait(bs, &goal)) {
		AIEnter_Wait(bs, "battle chase: waiting");
		return qfalse;
	}
	// move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);
	// if the movement failed
	if (moveresult.failure) {
		// reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	// check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qfalse);
	// if the view angles are used for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_MOVEMENTVIEWSET|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	} else if (!(bs->flags & BFL_IDEALVIEWSET)) {
		if (bs->chase_time > FloatTime() - 2) {
			BotAimAtEnemy(bs);
		} else {
			if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
				VectorSubtract(target, bs->origin, dir);
				VectorToAngles(dir, bs->ideal_viewangles);
			} else {
				VectorToAngles(moveresult.movedir, bs->ideal_viewangles);
			}
		}

		bs->ideal_viewangles[2] *= 0.5;
	}
	// if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) {
		bs->weaponnum = moveresult.weapon;
	}
	// if the bot is in the area the enemy was last seen in
	if (bs->areanum == bs->lastenemyareanum) {
		bs->chase_time = 0;
	}
	// if the bot wants to retreat (the bot could have been damage during the chase)
	if (BotWantsToRetreat(bs)) {
		AIEnter_Battle_Retreat(bs, "BATTLE CHASE: wants to retreat.");
		return qtrue;
	}

	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Battle_Retreat
=======================================================================================================================================
*/
void AIEnter_Battle_Retreat(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "BATTLE RETREAT", "", s);
	bs->ainode = AINode_Battle_Retreat;
}

/*
=======================================================================================================================================
AINode_Battle_Retreat
=======================================================================================================================================
*/
int AINode_Battle_Retreat(bot_state_t *bs) {
	bot_goal_t goal;
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;
	vec3_t target, dir;
	float attack_skill;
	int areanum, range;

	range = trap_Characteristic_BInteger(bs->character, CHARACTERISTIC_GOAL_MULTIPLIER, 0, 10000);

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "BATTLE RETREAT: joined observer.");
		return qfalse;
	}
	// if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "BATTLE RETREAT: joined intermission.");
		return qfalse;
	}
	// respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "BATTLE RETREAT: bot dead.");
		return qfalse;
	}
	// if no enemy
	if (bs->enemy < 0) {
		AIEnter_Seek_LTG(bs, "BATTLE RETREAT: no enemy.");
		return qfalse;
	}
	// get the entity information
	BotEntityInfo(bs->enemy, &entinfo);
	// if the entity isn't dead
	if (EntityIsDead(&entinfo)) {
		AIEnter_Seek_LTG(bs, "BATTLE RETREAT: enemy dead.");
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	// if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA|TFL_SLIME;
	}
	// if there is another better enemy
	if (BotFindEnemy(bs, bs->enemy)) {
#ifdef DEBUG
		BotAI_Print(PRT_MESSAGE, "AINode_Battle_Retreat: found new better enemy.\n");
#endif
	}

	BotTeamGoals(bs, qtrue);
	// map specific code
	BotMapScripts(bs);
	// update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);
	// if the bot doesn't want to retreat anymore... probably picked up some nice items
	if (BotWantsToChase(bs)) {
		// empty the goal stack, when chasing, only the enemy is the goal
		trap_BotEmptyGoalStack(bs->gs);
		// go chase the enemy
		AIEnter_Battle_Chase(bs, "BATTLE RETREAT: wants to chase.");
		return qfalse;
	}
	// update the last time the enemy was visible
	if (BotEntityVisible(&bs->cur_ps, 360, bs->enemy)) {
		bs->enemyvisible_time = FloatTime();

		VectorCopy(entinfo.origin, target);
		// if not a player enemy
		if (bs->enemy >= MAX_CLIENTS && (bs->enemy == redobelisk.entitynum || bs->enemy == blueobelisk.entitynum)) {
			target[2] += OBELISK_TARGET_HEIGHT;
		}
		// update the reachability area and origin if possible
		areanum = BotPointAreaNum(target);

		if (areanum && trap_AAS_AreaReachability(areanum)) {
			VectorCopy(target, bs->lastenemyorigin);
			bs->lastenemyareanum = areanum;
		}
	}
	// if the enemy is NOT visible for 4 seconds
	if (bs->enemyvisible_time < FloatTime() - 4) {
		AIEnter_Seek_LTG(bs, "BATTLE RETREAT: lost enemy.");
		return qfalse;
	// else if the enemy is NOT visible
	} else if (bs->enemyvisible_time < FloatTime()) {
		// if there is another enemy
		if (BotFindEnemy(bs, -1)) {
			AIEnter_Battle_Fight(bs, "BATTLE RETREAT: another enemy.");
			return qfalse;
		}
	}
	// check for nearby goals periodicly
	if (bs->check_time < FloatTime()) {
		bs->check_time = FloatTime() + 0.05;
		// if carrying a flag and the own flag is at base, or if the bot is trying to get the flag and the own flag is NOT at base the bot shouldn't be distracted too much and should ignore some items
		if (BotHasEmergencyGoal(bs)) {
			range *= 0.1; // 50
		} else if (!BotFeelingBad(bs)) {
			range *= 0.3; // 150
		}

		if (BotNearbyGoal(bs, bs->tfl, &goal, range)) {
			trap_BotResetLastAvoidReach(bs->ms);
			// time the bot gets to pick up the nearby goal item
			bs->nbg_time = FloatTime() + range / 100 + 1;
			AIEnter_Battle_NBG(bs, "BATTLE RETREAT: check for Nbg.");
			return qfalse;
		}
	}
	// get the current long term goal while retreating after checking for NBG's
	if (!BotLongTermGoal(bs, bs->tfl, qtrue, &goal)) {
		AIEnter_Battle_SuicidalFight(bs, "battle retreat: no way out");
		return qfalse;
	}
	// if the bot should wait
	if (BotCanWait(bs, &goal)) {
		AIEnter_Wait(bs, "battle retreat: waiting");
		return qfalse;
	}
	// move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);
	// if the movement failed
	if (moveresult.failure) {
		// reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->ltg_time = 0;
	}
	// check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qfalse);
	// if the view is fixed for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	} else if (!(moveresult.flags & MOVERESULT_MOVEMENTVIEWSET) && !(bs->flags & BFL_IDEALVIEWSET)) {
		attack_skill = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_ATTACK_SKILL, 0, 1);
		// if the bot is skilled enough
		if (attack_skill > 0.3) {
			//&& BotEntityVisible(&bs->cur_ps, 360, bs->enemy)
			BotAimAtEnemy(bs);
		} else {
			if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
				VectorSubtract(target, bs->origin, dir);
				VectorToAngles(dir, bs->ideal_viewangles);
			} else {
				VectorToAngles(moveresult.movedir, bs->ideal_viewangles);
			}

			bs->ideal_viewangles[2] *= 0.5;
		}
	}
	// if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) {
		bs->weaponnum = moveresult.weapon;
	}
	// attack the enemy if possible
	BotCheckAttack(bs);
	return qtrue;
}

/*
=======================================================================================================================================
AIEnter_Battle_NBG
=======================================================================================================================================
*/
void AIEnter_Battle_NBG(bot_state_t *bs, char *s) {

	BotRecordNodeSwitch(bs, "BATTLE NBG", "", s);
	bs->ainode = AINode_Battle_NBG;
}

/*
=======================================================================================================================================
AINode_Battle_NBG
=======================================================================================================================================
*/
int AINode_Battle_NBG(bot_state_t *bs) {
	int areanum;
	bot_goal_t goal;
	aas_entityinfo_t entinfo;
	bot_moveresult_t moveresult;
	float attack_skill;
	vec3_t target, dir;

	if (BotIsObserver(bs)) {
		AIEnter_Observer(bs, "BATTLE NBG: joined observer.");
		return qfalse;
	}
	// if in the intermission
	if (BotIntermission(bs)) {
		AIEnter_Intermission(bs, "BATTLE NBG: joined intermission.");
		return qfalse;
	}
	// respawn if dead
	if (BotIsDead(bs)) {
		AIEnter_Respawn(bs, "BATTLE NBG: bot dead.");
		return qfalse;
	}
	// if no enemy
	if (bs->enemy < 0) {
		AIEnter_Seek_NBG(bs, "BATTLE NBG: no enemy.");
		return qfalse;
	}
	// get the entity information
	BotEntityInfo(bs->enemy, &entinfo);
	// if the entity isn't dead
	if (EntityIsDead(&entinfo)) {
		AIEnter_Seek_NBG(bs, "BATTLE NBG: enemy dead.");
		return qfalse;
	}

	bs->tfl = TFL_DEFAULT;
	// if in lava or slime the bot should be able to get out
	if (BotInLavaOrSlime(bs)) {
		bs->tfl |= TFL_LAVA|TFL_SLIME;
	}

	if (BotCanAndWantsToRocketJump(bs)) {
		bs->tfl |= TFL_ROCKETJUMP;
	}
	// map specific code
	BotMapScripts(bs);
	// update the attack inventory values
	BotUpdateBattleInventory(bs, bs->enemy);
	// update the last time the enemy was visible
	if (BotEntityVisible(&bs->cur_ps, 360, bs->enemy)) {
		bs->enemyvisible_time = FloatTime();

		VectorCopy(entinfo.origin, target);
		// if not a player enemy
		if (bs->enemy >= MAX_CLIENTS && (bs->enemy == redobelisk.entitynum || bs->enemy == blueobelisk.entitynum)) {
			target[2] += OBELISK_TARGET_HEIGHT;
		}
		// update the reachability area and origin if possible
		areanum = BotPointAreaNum(target);

		if (areanum && trap_AAS_AreaReachability(areanum)) {
			VectorCopy(target, bs->lastenemyorigin);
			bs->lastenemyareanum = areanum;
		}
	}
	// if the bot has no goal or touches the current goal
	if (!trap_BotGetTopGoal(bs->gs, &goal)) {
		bs->nbg_time = 0;
	// if the bot touches the current goal
	} else if (BotReachedGoal(bs, &goal)) {
		bs->nbg_time = 0;
	}

	if (bs->nbg_time < FloatTime()) {
		// pop the current goal from the stack
		trap_BotPopGoal(bs->gs);
		// if the bot still has a goal
		if (trap_BotGetTopGoal(bs->gs, &goal)) {
			AIEnter_Battle_Retreat(bs, "BATTLE NBG: time out.");
		} else {
			AIEnter_Battle_Fight(bs, "BATTLE NBG: time out.");
		}

		return qfalse;
	}
	// move towards the goal
	trap_BotMoveToGoal(&moveresult, bs->ms, &goal, bs->tfl);
	// if the movement failed
	if (moveresult.failure) {
		// reset the avoid reach, otherwise bot is stuck in current area
		trap_BotResetAvoidReach(bs->ms);
		//BotAI_Print(PRT_MESSAGE, "movement failure %d\n", moveresult.traveltype);
		bs->nbg_time = 0;
	}
	// check if the bot is blocked
	BotAIBlocked(bs, &moveresult, qfalse);
	// if the view is fixed for the movement
	if (moveresult.flags & (MOVERESULT_MOVEMENTVIEW|MOVERESULT_SWIMVIEW)) {
		VectorCopy(moveresult.ideal_viewangles, bs->ideal_viewangles);
	} else if (!(moveresult.flags & MOVERESULT_MOVEMENTVIEWSET) && !(bs->flags & BFL_IDEALVIEWSET)) {
		attack_skill = trap_Characteristic_BFloat(bs->character, CHARACTERISTIC_ATTACK_SKILL, 0, 1);
		// if the bot is skilled enough
		if (attack_skill > 0.3) {
			//&& BotEntityVisible(&bs->cur_ps, 360, bs->enemy)
			BotAimAtEnemy(bs);
		} else {
			if (trap_BotMovementViewTarget(bs->ms, &goal, bs->tfl, 300, target)) {
				VectorSubtract(target, bs->origin, dir);
				VectorToAngles(dir, bs->ideal_viewangles);
			} else {
				VectorToAngles(moveresult.movedir, bs->ideal_viewangles);
			}

			bs->ideal_viewangles[2] *= 0.5;
		}
	}
	// if the weapon is used for the bot movement
	if (moveresult.flags & MOVERESULT_MOVEMENTWEAPON) {
		bs->weaponnum = moveresult.weapon;
	}
	// attack the enemy if possible
	BotCheckAttack(bs);
	return qtrue;
}
