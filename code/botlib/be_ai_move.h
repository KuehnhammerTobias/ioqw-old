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
 Movement AI.
**************************************************************************************************************************************/

// movement types
#define MOVE_WALK		0x00000001
#define MOVE_JUMP		0x00000002
#define MOVE_CROUCH		0x00000004
#define MOVE_ROCKETJUMP	0x00000008
#define MOVE_BFGJUMP	0x00000010
// move flags
#define MFL_ONGROUND		0x00000001 // bot is in the ground
#define MFL_BARRIERJUMP		0x00000002 // bot is performing a barrier jump
#define MFL_WATERJUMP		0x00000004 // bot is waterjumping
#define MFL_SWIMMING		0x00000008 // bot is swimming
#define MFL_AGAINSTLADDER	0x00000010 // bot is against a ladder
#define MFL_SCOUT			0x00000020 // bot is using the scout powerup
#define MFL_WALK			0x00000040 // bot should walk slowly
#define MFL_TELEPORTED		0x00000080 // bot is being teleported
// move result flags
#define MOVERESULT_MOVEMENTVIEW			0x00000001 // bot uses view for movement
#define MOVERESULT_MOVEMENTVIEWSET		0x00000002 // bot has set the view in movement code
#define MOVERESULT_MOVEMENTWEAPON		0x00000004 // bot uses weapon for movement
#define MOVERESULT_BLOCKEDBYAVOIDSPOT	0x00000008 // bot is blocked by an avoid spot
#define MOVERESULT_SWIMVIEW				0x00000010 // bot uses view for swimming
#define MOVERESULT_WAITING				0x00000020 // bot is waiting for something
#define MOVERESULT_ONTOPOF_OBSTACLE		0x00000040 // bot is ontop of obstacle
#define MOVERESULT_ONTOPOF_FUNCBOB		0x00000080 // bot is ontop of a func_bobbing
#define MOVERESULT_ONTOPOF_ELEVATOR		0x00000100 // bot is ontop of an elevator (func_plat)

#define MAX_AVOIDREACH 1
#define MAX_AVOIDSPOTS 128
// avoid spot types
#define AVOID_CLEAR		0 // clear all avoid spots
#define AVOID_ALWAYS	1 // avoid always
#define AVOID_DONTBLOCK	2 // never totally block
// restult types
#define RESULTTYPE_ELEVATORUP			0x00000001 // elevator is up
#define RESULTTYPE_WAITFORFUNCBOBBING	0x00000002 // waiting for func_bobbing to arrive
#define RESULTTYPE_INSOLIDAREA			0x00000004 // stuck in solid area, this is bad
// structure used to initialize the movement state
// the or_moveflags MFL_ONGROUND, MFL_WATERJUMP and MFL_TELEPORTED come from the playerstate
typedef struct bot_initmove_s {
	vec3_t origin;		// origin of the bot
	vec3_t velocity;	// velocity of the bot
	vec3_t viewoffset;	// view offset
	int entitynum;		// entity number of the bot
	int client;			// client number of the bot
	float thinktime;	// time the bot thinks
	int presencetype;	// presencetype of the bot
	vec3_t viewangles;	// view angles of the bot
	int or_moveflags;	// values ored to the movement flags
} bot_initmove_t;
// NOTE: the ideal_viewangles are only valid if MFL_MOVEMENTVIEW is set
typedef struct bot_moveresult_s {
	int failure;				// true if movement failed all together
	int type;					// failure or blocked type
	int blocked;				// true if blocked by an entity
	int blockentity;			// entity blocking the bot
	int traveltype;				// last executed travel type
	int flags;					// result flags
	int weapon;					// weapon used for movement
	vec3_t movedir;				// movement direction
	vec3_t ideal_viewangles;	// ideal viewangles for the movement
} bot_moveresult_t;

#define bot_moveresult_t_cleared(x) bot_moveresult_t(x) = {0, 0, 0, 0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}}

typedef struct bot_avoidspot_s {
	vec3_t origin;
	float radius;
	int type;
} bot_avoidspot_t;
// resets the whole move state
void BotResetMoveState(int movestate);
// moves the bot to the given goal
void BotMoveToGoal(bot_moveresult_t *result, int movestate, bot_goal_t *goal, int travelflags);
// moves the bot in the specified direction using the specified type of movement
int BotMoveInDirection(int movestate, vec3_t dir, float speed, int type);
// reset avoid reachability
void BotResetAvoidReach(int movestate);
// resets the last avoid reachability
void BotResetLastAvoidReach(int movestate);
// returns a reachability area if the origin is in one
int BotReachabilityArea(vec3_t origin, int client);
// view target based on movement
int BotMovementViewTarget(int movestate, bot_goal_t *goal, int travelflags, float lookahead, vec3_t target);
// predict the position of a player based on movement towards a goal
int BotPredictVisiblePosition(vec3_t origin, int areanum, bot_goal_t *goal, int travelflags, vec3_t target);
// returns the handle of a newly allocated movestate
int BotAllocMoveState(void);
// frees the movestate with the given handle
void BotFreeMoveState(int handle);
// initialize movement state before performing any movement
void BotInitMoveState(int handle, bot_initmove_t *initmove);
// add a spot to avoid (if type == AVOID_CLEAR all spots are removed)
void BotAddAvoidSpot(int movestate, vec3_t origin, float radius, int type);
// must be called every map change
void BotSetBrushModelTypes(void);
// setup movement AI
int BotSetupMoveAI(void);
// shutdown movement AI
void BotShutdownMoveAI(void);
