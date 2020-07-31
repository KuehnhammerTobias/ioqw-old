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
 Definitions shared by both the server game and client game modules.
**************************************************************************************************************************************/

// because games can change separately from the main system version, we need a second version that must match between game and cgame

#define GAME_VERSION BASEGAME "-1"
#define DEFAULT_GRAVITY 800
#define GIB_HEALTH -40
#define ARMOR_PROTECTION 0.66
#define MAX_ITEMS 256
#define RANK_TIED_FLAG 0x4000
#define DEFAULT_SHOTGUN_SPREAD 700
#define DEFAULT_SHOTGUN_COUNT 11
#define ITEM_RADIUS 15 // item sizes are needed for client side pickup detection
#define BEAMGUN_RANGE 768
#define SCORE_NOT_PRESENT -9999 // for the CS_SCORES[12] when only one player is present
#define VOTE_TIME 30000 // 30 seconds before vote times out
#define MINS_Z -24
#define DEFAULT_VIEWHEIGHT 38
#define CROUCH_VIEWHEIGHT 22
#define DEAD_VIEWHEIGHT -16
#define OBELISK_TARGET_HEIGHT 56
#define MAX_DLIGHT_CONFIGSTRINGS 128
#define TIMER_GESTURE 2294

/**************************************************************************************************************************************

	config strings are a general means of communicating variable length strings from the server to all connected clients.

**************************************************************************************************************************************/

// CS_SERVERINFO and CS_SYSTEMINFO are defined in q_shared.h
#define CS_MUSIC			 2
#define CS_MESSAGE			 3 // from the map worldspawn's message field
#define CS_MOTD				 4 // g_motd string for server message of the day
#define CS_WARMUP			 5 // server time when the match will be restarted
#define CS_SCORES1			 6
#define CS_SCORES2			 7
#define CS_VOTE_TIME		 8
#define CS_VOTE_STRING		 9
#define CS_VOTE_YES			10
#define CS_VOTE_NO			11
#define CS_TEAMVOTE_TIME	12
#define CS_TEAMVOTE_STRING	14
#define CS_TEAMVOTE_YES		16
#define CS_TEAMVOTE_NO		18
#define CS_GAME_VERSION		20
#define CS_LEVEL_START_TIME	21 // so the timer only shows the current level
#define CS_INTERMISSION		22 // when 1, fraglimit/timelimit has been hit and intermission will start in a second or two
#define CS_FLAGSTATUS		23 // string indicating flag status in CTF
#define CS_SHADERSTATE		24
#define CS_ITEMS			27 // string of 0's and 1's that tell which items are present
#define CS_MODELS			32
#define CS_SOUNDS			(CS_MODELS + MAX_MODELS)
#define CS_PLAYERS			(CS_SOUNDS + MAX_SOUNDS)
#define CS_LOCATIONS		(CS_PLAYERS + MAX_CLIENTS)
#define CS_PARTICLES		(CS_LOCATIONS + MAX_LOCATIONS)
#define CS_DLIGHTS			(CS_PARTICLES + MAX_LOCATIONS)
#define CS_BOTINFO			(CS_DLIGHTS + MAX_DLIGHT_CONFIGSTRINGS)
#define CS_MAX				(CS_BOTINFO + MAX_CLIENTS)
#if (CS_MAX) > MAX_CONFIGSTRINGS
#error overflow: (CS_MAX) > MAX_CONFIGSTRINGS
#endif

typedef enum {
	GENDER_MALE,
	GENDER_FEMALE,
	GENDER_NEUTER
} gender_t;

/**************************************************************************************************************************************

	PMOVE MODULE

	The pmove code takes a player_state_t and a usercmd_t and generates a new player_state_t and some other output data. Used for local
	prediction on the client game and true movement on the server game.

**************************************************************************************************************************************/

typedef enum {
	PM_NORMAL,			// can accelerate and turn
	PM_NOCLIP,			// noclip movement
	PM_SPECTATOR,		// still run into walls
	PM_DEAD,			// no acceleration or turning, but free falling
	PM_FREEZE,			// stuck in place with no control
	PM_INTERMISSION,	// no movement or status bar
	PM_SPINTERMISSION	// no movement or status bar
} pmtype_t;

typedef enum {
	WEAPON_READY,
	WEAPON_RAISING,
	WEAPON_DROPPING,
	WEAPON_FIRING
} weaponstate_t;
// pmove->pm_flags (sent as max 16 bits in msg.c)
#define PMF_DUCKED			0x00000001
#define PMF_JUMP_HELD		0x00000002
#define PMF_BACKWARDS_JUMP	0x00000004 // go into backwards land
#define PMF_BACKWARDS_RUN	0x00000008 // coast down to backwards run
#define PMF_TIME_LAND		0x00000010 // pm_time is time before rejump
#define PMF_TIME_WATERJUMP	0x00000020 // pm_time is waterjump
#define PMF_TIME_KNOCKBACK	0x00000040 // pm_time is an air-accelerate only time
#define PMF_RESPAWNED		0x00000080 // clear after attack and jump buttons come up
#define PMF_USE_ITEM_HELD	0x00000100
#define PMF_FOLLOW			0x00000200 // spectate following another player
#define PMF_SCOREBOARD		0x00000400 // spectate as a scoreboard
#define PMF_ALL_TIMES (PMF_TIME_LAND|PMF_TIME_WATERJUMP|PMF_TIME_KNOCKBACK)

#define MAXTOUCH 32

typedef struct {
	// state (in/out)
	playerState_t *ps;
	// command (in)
	usercmd_t cmd;
	int tracemask;			// collide against these types of surfaces
	int debugLevel;			// if set, diagnostic output will be printed
	qboolean gauntletHit;	// true if a gauntlet attack would actually hit something
	int framecount;
	// results (out)
	int numtouch;
	int touchents[MAXTOUCH];
	vec3_t mins, maxs;		// bounding box size
	int watertype;
	int waterlevel;
	float xyspeed;
	// for fixed msec Pmove
	int pmove_fixed;
	int pmove_msec;
	// callbacks to test the world
	// these will be different functions during game and cgame
	void (*trace)(trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask);
	int (*pointcontents)(const vec3_t point, int passEntityNum);
} pmove_t;
// if a full pmove isn't done on the client, you can just update the angles
void PM_UpdateViewAngles(playerState_t *ps, const usercmd_t *cmd);
void Pmove(pmove_t *pmove);

/**************************************************************************************************************************************

	player_state->stats[] indexes

	NOTE: may not have more than MAX_STATS.

**************************************************************************************************************************************/

typedef enum {
	STAT_HEALTH,
	STAT_ARMOR,
	STAT_WEAPONS,		// 16 bit fields
	STAT_HOLDABLE_ITEM,
	STAT_PERSISTANT_POWERUP,
	STAT_DEAD_YAW,		// look this direction when dead (FIXME: get rid of?)
	STAT_CLIENTS_READY	// bit mask of clients wishing to exit the intermission (FIXME: configstring?)
} statIndex_t;

/**************************************************************************************************************************************

	player_state->persistant[] indexes

	These fields are the only part of player_state that isn't cleared on respawn.
	NOTE: may not have more than MAX_PERSISTANT.

**************************************************************************************************************************************/

typedef enum {
	PERS_SCORE,					// !!! MUST NOT CHANGE, SERVER AND GAME BOTH REFERENCE !!!
	PERS_TEAM,					// player team
	PERS_RANK,					// player rank or team rank
	PERS_HITS,					// total points damage inflicted so damage beeps can sound on change
	PERS_ATTACKER,				// clientNum of last damage inflicter
	PERS_ATTACKEE_ARMOR,		// health/armor of last person we attacked
	PERS_PLAYEREVENTS,			// 16 bits that can be flipped for events
	PERS_SPAWN_COUNT,			// incremented every respawn
	PERS_KILLED,				// count of the number of times you died
	// player awards tracking
	PERS_EXCELLENT_COUNT,		// two successive kills in a short amount of time
	PERS_IMPRESSIVE_COUNT,		// two railgun hits in a row
	PERS_GAUNTLET_FRAG_COUNT,	// kills with the gauntlet
	PERS_CAPTURES,				// captures
	PERS_DEFEND_COUNT,			// defend awards
	PERS_ASSIST_COUNT			// assist awards
} persEnum_t;
// entityState_t->eFlags
#define EF_PLAYER_EVENT		0x00000001
#define EF_FIRING			0x00000002 // for beam gun
#define EF_DEAD				0x00000004 // don't draw a foe marker over players with EF_DEAD
#define EF_TICKING			0x00000008 // used to make players play the prox mine ticking sound
#define EF_TELEPORT_BIT		0x00000010 // toggled every time the origin abruptly changes
#define EF_BOUNCE			0x00000020 // for missiles
#define EF_BOUNCE_HALF		0x00000040 // for missiles
#define EF_MOVER_STOP		0x00000080 // will push otherwise
#define EF_NODRAW			0x00000100 // may have an event, but no model (unspawned items)
#define EF_KAMIKAZE			0x00000200
#define EF_VOTED			0x00000400 // already cast a vote
#define EF_TEAMVOTED		0x00000800 // already cast a team vote
#define EF_CONNECTION		0x00001000 // draw a connection trouble sprite
#define EF_TALK				0x00002000 // draw a talk balloon

/**************************************************************************************************************************************

	player_state->powerup[] indexes

	NOTE: may not have more than MAX_POWERUPS.

**************************************************************************************************************************************/

typedef enum {
	PW_NONE,
	PW_QUAD,
	PW_INVIS,
	PW_REGEN,
	PW_AMMOREGEN,
	PW_GUARD,
	PW_DOUBLER,
	PW_SCOUT,
	PW_REDFLAG,
	PW_BLUEFLAG,
	PW_NEUTRALFLAG,
	PW_NUM_POWERUPS
} powerup_t;

typedef enum {
	HI_NONE,
	HI_MEDKIT,
	HI_KAMIKAZE,
	HI_NUM_HOLDABLE
} holdable_t;

/**************************************************************************************************************************************

	player_state->weapon[] indexes

	NOTE: may not have more than MAX_WEAPONS.

**************************************************************************************************************************************/

typedef enum {
	WP_NONE,
	WP_GAUNTLET,
	WP_MACHINEGUN,
	WP_CHAINGUN,
	WP_SHOTGUN,
	WP_NAILGUN,
	WP_PROXLAUNCHER,
	WP_GRENADELAUNCHER,
	WP_NAPALMLAUNCHER,
	WP_ROCKETLAUNCHER,
	WP_BEAMGUN,
	WP_RAILGUN,
	WP_PLASMAGUN,
	WP_BFG,
	WP_NUM_WEAPONS
} weapon_t;
// reward sounds (stored in ps->persistant[PERS_PLAYEREVENTS])
#define PLAYEREVENT_DENIEDREWARD	0x00000001
#define PLAYEREVENT_GAUNTLETREWARD	0x00000002
#define PLAYEREVENT_HOLYSHIT		0x00000004
// entityState_t->event values
// entity events are for effects that take place relative to an existing entities origin. Very network efficient.
// two bits at the top of the entityState->event field will be incremented with each change in the event so
// that an identical event started twice in a row can be distinguished. And off the value with ~EV_EVENT_BITS
// to retrieve the actual event number
#define EV_EVENT_BIT1 0x00000100
#define EV_EVENT_BIT2 0x00000200
#define EV_EVENT_BITS (EV_EVENT_BIT1|EV_EVENT_BIT2)
#define EVENT_VALID_MSEC 300

typedef enum {
	EV_NONE,
	EV_FIRE_WEAPON,
	EV_BULLET_HIT_FLESH,
	EV_BULLET_HIT_WALL,
	EV_MISSILE_HIT,
	EV_MISSILE_MISS,
	EV_RAILTRAIL,
	EV_SHOTGUN,
	EV_PROXIMITY_MINE_STICK,
	EV_PROXIMITY_MINE_TRIGGER,
	EV_GRENADE_BOUNCE,		// eventParm will be the soundindex
	EV_KAMIKAZE,			// kamikaze explodes
	EV_OBELISKEXPLODE,		// obelisk explodes
	EV_OBELISKPAIN,			// obelisk is in pain
	EV_STOPLOOPINGSOUND,
	EV_GENERAL_SOUND,
	EV_GLOBAL_SOUND,		// no attenuation
	EV_GLOBAL_TEAM_SOUND,
	EV_PAIN,
	EV_DEATH1,
	EV_DEATH2,
	EV_DEATH3,
	EV_OBITUARY,
	EV_GIB_PLAYER,			// gib a previously living player
	EV_FALL_DIE,
	EV_FALL_DMG_50,
	EV_FALL_DMG_25,
	EV_FALL_DMG_15,
	EV_FALL_DMG_10,
	EV_FALL_DMG_5,
	EV_FALL_SHORT,
	EV_JUMP,
	EV_JUMP_PAD,			// boing sound at origin, jump sound on player
	EV_PLAYER_TELEPORT_IN,
	EV_PLAYER_TELEPORT_OUT,
	EV_NOAMMO,
	EV_CHANGE_WEAPON,
	EV_FOOTSTEP_HARD,
	EV_FOOTSTEP_HARD_FROZEN,
	EV_FOOTSTEP_HARD_SNOW,
	EV_FOOTSTEP_HARD_SLUSH,
	EV_FOOTSTEP_PUDDLE,
	EV_FOOTSTEP_LEAVES,
	EV_FOOTSTEP_BUSH,
	EV_FOOTSTEP_GRASS,
	EV_FOOTSTEP_LONGGRASS,
	EV_FOOTSTEP_LONGGRASS_MUD,
	EV_FOOTSTEP_SAND,
	EV_FOOTSTEP_GRAVEL,
	EV_FOOTSTEP_RUBBLE,
	EV_FOOTSTEP_RUBBLE_WET,
	EV_FOOTSTEP_SOIL,
	EV_FOOTSTEP_MUD,
	EV_FOOTSTEP_SNOW_DEEP,
	EV_FOOTSTEP_ICE,
	EV_FOOTSTEP_METAL_HOLLOW,
	EV_FOOTSTEP_METAL_HOLLOW_FROZEN,
	EV_FOOTSTEP_METAL_HOLLOW_SNOW,
	EV_FOOTSTEP_METAL_HOLLOW_SLUSH,
	EV_FOOTSTEP_METAL_HOLLOW_SPLASH,
	EV_FOOTSTEP_GRATE_01,
	EV_FOOTSTEP_GRATE_02,
	EV_FOOTSTEP_DUCT,
	EV_FOOTSTEP_PLATE,
	EV_FOOTSTEP_FENCE,
	EV_FOOTSTEP_WOOD_HOLLOW,
	EV_FOOTSTEP_WOOD_HOLLOW_FROZEN,
	EV_FOOTSTEP_WOOD_HOLLOW_SNOW,
	EV_FOOTSTEP_WOOD_HOLLOW_SLUSH,
	EV_FOOTSTEP_WOOD_HOLLOW_SPLASH,
	EV_FOOTSTEP_WOOD_SOLID,
	EV_FOOTSTEP_WOOD_CREAKING,
	EV_FOOTSTEP_ROOF,
	EV_FOOTSTEP_SHINGLES,
	EV_FOOTSTEP_SOFT,
	EV_FOOTSTEP_GLASS_SHARDS,
	EV_FOOTSTEP_TRASH_GLASS,
	EV_FOOTSTEP_TRASH_DEBRIS,
	EV_FOOTSTEP_TRASH_WIRE,
	EV_FOOTSTEP_TRASH_PACKING,
	EV_FOOTSTEP_TRASH_PLASTIC,
	EV_FOOTSPLASH,
	EV_FOOTWADE,
	EV_SWIM,
	EV_WATER_TOUCH,			// foot touches
	EV_WATER_LEAVE,			// foot leaves
	EV_WATER_UNDER,			// head touches
	EV_WATER_CLEAR,			// head leaves
	EV_TAUNT,
	EV_TAUNT_YES,
	EV_TAUNT_NO,
	EV_TAUNT_FOLLOWME,
	EV_TAUNT_GETFLAG,
	EV_TAUNT_GUARDBASE,
	EV_TAUNT_PATROL,
	EV_STEP_4,
	EV_STEP_8,
	EV_STEP_12,
	EV_STEP_16,
	EV_ITEM_POP,
	EV_ITEM_RESPAWN,
	EV_ITEM_PICKUP,			// normal item pickups are predictable
	EV_GLOBAL_ITEM_PICKUP,	// powerup/team sounds are broadcast to everyone
	EV_POWERUP_QUAD,
	EV_POWERUP_REGEN,
	EV_USE_ITEM0,
	EV_USE_ITEM1,
	EV_USE_ITEM2,
	EV_USE_ITEM3,
	EV_USE_ITEM4,
	EV_USE_ITEM5,
	EV_USE_ITEM6,
	EV_USE_ITEM7,
	EV_USE_ITEM8,
	EV_USE_ITEM9,
	EV_USE_ITEM10,
	EV_USE_ITEM11,
	EV_USE_ITEM12,
	EV_USE_ITEM13,
	EV_USE_ITEM14,
	EV_USE_ITEM15,
	EV_SCOREPLUM,			// score plum
	EV_LIGHTNINGBOLT,
	EV_DEBUG_LINE
} entity_event_t;

typedef enum {
	GTS_RED_CAPTURE,
	GTS_BLUE_CAPTURE,
	GTS_RED_RETURN,
	GTS_BLUE_RETURN,
	GTS_RED_TAKEN,
	GTS_BLUE_TAKEN,
	GTS_REDOBELISK_ATTACKED,
	GTS_BLUEOBELISK_ATTACKED,
	GTS_REDTEAM_SCORED,
	GTS_BLUETEAM_SCORED,
	GTS_REDTEAM_TOOK_LEAD,
	GTS_BLUETEAM_TOOK_LEAD,
	GTS_TEAMS_ARE_TIED,
	GTS_KAMIKAZE
} global_team_sound_t;
// animations
typedef enum {
	BOTH_DEATH1,
	BOTH_DEAD1,
	BOTH_DEATH2,
	BOTH_DEAD2,
	BOTH_DEATH3,
	BOTH_DEAD3,
//-------------------------
	TORSO_GESTURE,
	TORSO_ATTACK,
	TORSO_ATTACK2,
	TORSO_DROP,
	TORSO_RAISE,
	TORSO_STAND,
	TORSO_STAND2,
//-------------------------
	LEGS_WALKCR,
	LEGS_WALK,
	LEGS_RUN,
	LEGS_BACK,
	LEGS_SWIM,
	LEGS_JUMP,
	LEGS_LAND,
	LEGS_JUMPB,
	LEGS_LANDB,
	LEGS_IDLE,
	LEGS_IDLECR,
	LEGS_TURN,
//-------------------------
	TORSO_GETFLAG,
	TORSO_GUARDBASE,
	TORSO_PATROL,
	TORSO_FOLLOWME,
	TORSO_AFFIRMATIVE,
	TORSO_NEGATIVE,
//-------------------------
	MAX_ANIMATIONS,
//-------------------------
	LEGS_BACKCR,
	LEGS_BACKWALK,
//-------------------------
	FLAG_RUN,
	FLAG_STAND,
	FLAG_STAND2RUN,
//-------------------------
	MAX_TOTALANIMATIONS
} animNumber_t;

typedef struct animation_s {
	int firstFrame;
	int numFrames;
	int loopFrames;		// 0 to numFrames
	int frameLerp;		// msec between frames
	int initialLerp;	// msec to get to first frame
	int reversed;		// true if animation is reversed
	int flipflop;		// true if animation should flipflop back to base
} animation_t;
// flip the togglebit every time an animation changes so a restart of the same anim can be detected
#define ANIM_TOGGLEBIT 128
// skin surfaces array shouldn't be dynamically allocated because players reuse the same skin structure when changing models
#define MAX_CG_SKIN_SURFACES 100
#define MAX_CG_SKIN_SURFACE_SHADERS 10
typedef struct {
	qhandle_t surfaces[MAX_CG_SKIN_SURFACE_SHADERS]; // allocated skin surfaces (mesh name + shader)
	int numShaders;
} cgSkinMesh_t;

typedef struct {
	int numMeshes;
	cgSkinMesh_t meshes[MAX_CG_SKIN_SURFACES];
} cgSkin_t;

#define DEFAULT_PLAYER_NAME "UnnamedPlayer"
// default player model names
#define DEFAULT_MODEL "sarge"
#define DEFAULT_HEAD "sarge"
// for fallback player and gender-specific fallback sounds
#define DEFAULT_MODEL_MALE "sarge"
#define DEFAULT_MODEL_FEMALE "major"
// default team player model names
#define DEFAULT_TEAM_MODEL "james"
#define DEFAULT_TEAM_HEAD "*james"
// for fallback (team) player and gender-specific fallback sounds, also used for UI's character base model and CGame player pre-caching
#define DEFAULT_TEAM_MODEL_MALE "james"
#define DEFAULT_TEAM_HEAD_MALE "*james"
#define DEFAULT_TEAM_MODEL_FEMALE "janet"
#define DEFAULT_TEAM_HEAD_FEMALE "*janet"
#define DEFAULT_REDTEAM_NAME "Stroggs"
#define DEFAULT_BLUETEAM_NAME "Pagans"

typedef enum {
	TEAM_RED,
	TEAM_BLUE,
	TEAM_FREE,
	TEAM_SPECTATOR,
	TEAM_NUM_TEAMS
} team_t;
// time between location updates
#define TEAM_LOCATION_UPDATE_TIME 1000
// how many players on the overlay
#define TEAM_MAXOVERLAY 32
// team task
typedef enum {
	TEAMTASK_NONE,
	TEAMTASK_OFFENSE,
	TEAMTASK_DEFENSE,
	TEAMTASK_RETRIEVE,
	TEAMTASK_ESCORT,
	TEAMTASK_FOLLOW,
	TEAMTASK_CAMP,
	TEAMTASK_PATROL
} teamtask_t;
// means of death
typedef enum {
	MOD_GAUNTLET,
	MOD_MACHINEGUN,
	MOD_CHAINGUN,
	MOD_SHOTGUN,
	MOD_NAIL,
	MOD_PROXIMITY_MINE,
	MOD_GRENADE_SPLASH,
	MOD_NAPALM,
	MOD_NAPALM_SPLASH,
	MOD_ROCKET,
	MOD_ROCKET_SPLASH,
	MOD_BEAMGUN,
	MOD_RAILGUN,
	MOD_PLASMA,
	MOD_PLASMA_SPLASH,
	MOD_BFG,
	MOD_BFG_SPLASH,
	MOD_KAMIKAZE,
	MOD_TELEFRAG,
	MOD_WATER,
	MOD_SLIME,
	MOD_LAVA,
	MOD_FALLING,
	MOD_GOOMBA,
	MOD_TRIGGER_HURT,
	MOD_CRUSH,
	MOD_TARGET_LASER,
	MOD_SUICIDE,
	MOD_SUICIDE_TEAM_CHANGE,
	MOD_UNKNOWN
} meansOfDeath_t;
// gitem_t->type
typedef enum {
	IT_HEALTH,		// EFX: static external sphere + rotating internal
	IT_ARMOR,		// EFX: rotate + minlight
	IT_WEAPON,		// EFX: rotate + upscale + minlight
	IT_AMMO,		// EFX: rotate
	IT_HOLDABLE,	// single use, holdable item. EFX: rotate + bob
	IT_POWERUP,		// instant on, timer based. EFX: rotate + external ring that rotates
	IT_PERSISTANT_POWERUP,
	IT_TEAM,
	IT_BAD
} itemType_t;

#define MAX_ITEM_MODELS 4

typedef struct gitem_s {
	char *classname;	// spawning name
	char *pickup_sound;
	char *world_model[MAX_ITEM_MODELS];
	char *icon;
	char *pickup_name;	// for printing on pickup
	int quantity;		// for ammo how much, or duration of powerup
	itemType_t giType;	// IT_* flags
	int giTag;
	char *sounds;		// string of all sounds this item will use
} gitem_t;
// included in both the game dll and the client
extern gitem_t bg_itemlist[];
extern int bg_numItems;

gitem_t *BG_FindItem(const char *pickupName);
gitem_t *BG_FindItemForWeapon(weapon_t weapon);
gitem_t *BG_FindItemForPowerup(powerup_t pw);
gitem_t *BG_FindItemForHoldable(holdable_t pw);

#define ITEM_INDEX(x) ((x) - bg_itemlist)

qboolean BG_CanItemBeGrabbed(int gametype, const entityState_t *ent, const playerState_t *ps);

typedef struct {
	int gametype;
	// callbacks to test the entity, these will be different functions during game and cgame
	qboolean (*spawnInt)(const char *key, const char *defaultString, int *out);
	qboolean (*spawnString)(const char *key, const char *defaultString, char **out);
} bgEntitySpawnInfo_t;

qboolean BG_CheckSpawnEntity(const bgEntitySpawnInfo_t *info);

#define MAX_MAP_SIZE 65536
// bg_tracemap.c
typedef struct {
	// callbacks to test the world, these will be different functions during game and cgame
	void (*trace)(trace_t *results, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int passEntityNum, int contentMask);
	int (*pointcontents)(const vec3_t point, int passEntityNum);
} bgGenTracemap_t;

void BG_GenerateTracemap(const char *mapname, vec3_t mapcoordsMins, vec3_t mapcoordsMaxs, bgGenTracemap_t *gen);
qboolean BG_LoadTraceMap(char *rawmapname, vec2_t world_mins, vec2_t world_maxs);
float BG_GetSkyHeightAtPoint(vec3_t pos);
float BG_GetSkyGroundHeightAtPoint(vec3_t pos);
float BG_GetGroundHeightAtPoint(vec3_t pos);
int BG_GetTracemapGroundFloor(void);
int BG_GetTracemapGroundCeil(void);
void FinalizeTracemapClamp(int *x, int *y);
// g_dmflags->integer flags
#define DF_NO_FALLING	0x00000001
#define DF_FIXED_FOV	0x00000002
// content masks
#define MASK_ALL			(-1)
#define MASK_SOLID			(CONTENTS_SOLID)
#define MASK_PLAYERSOLID	(CONTENTS_SOLID|CONTENTS_PLAYERCLIP|CONTENTS_BODY)
#define MASK_DEADSOLID		(CONTENTS_SOLID|CONTENTS_PLAYERCLIP)
#define MASK_WATER			(CONTENTS_WATER|CONTENTS_LAVA|CONTENTS_SLIME)
#define MASK_OPAQUE			(CONTENTS_SOLID|CONTENTS_SLIME|CONTENTS_LAVA)
#define MASK_SHOT			(CONTENTS_SOLID|CONTENTS_BODY|CONTENTS_CORPSE)

void BG_EvaluateTrajectory(const trajectory_t *tr, int atTime, vec3_t result);
void BG_EvaluateTrajectoryDelta(const trajectory_t *tr, int atTime, vec3_t result);
void BG_AddPredictableEventToPlayerstate(int newEvent, int eventParm, playerState_t *ps);
void BG_TouchJumpPad(playerState_t *ps, entityState_t *jumppad);
void BG_PlayerStateToEntityState(playerState_t *ps, entityState_t *s, qboolean snap);
void BG_PlayerStateToEntityStateExtraPolate(playerState_t *ps, entityState_t *s, int time, qboolean snap);
qboolean BG_PlayerTouchesItem(playerState_t *ps, entityState_t *item, int atTime);
void SnapVectorTowards(vec3_t v, vec3_t to);

#define ARENAS_PER_TIER 4

#define MAX_ARENAS 1024
#define MAX_ARENAS_TEXT 8192
#define MAX_BOTS 1024
#define MAX_BOTS_TEXT 8192
// Kamikaze
// 1st shockwave times
#define KAMI_SHOCKWAVE_STARTTIME 0
#define KAMI_SHOCKWAVEFADE_STARTTIME 1500
#define KAMI_SHOCKWAVE_ENDTIME 2000
// explosion/implosion times
#define KAMI_EXPLODE_STARTTIME 250
#define KAMI_IMPLODE_STARTTIME 2000
#define KAMI_IMPLODE_ENDTIME 2250
// 2nd shockwave times
#define KAMI_SHOCKWAVE2_STARTTIME 2000
#define KAMI_SHOCKWAVE2FADE_STARTTIME 2500
#define KAMI_SHOCKWAVE2_ENDTIME 3000
// radius of the models without scaling
#define KAMI_SHOCKWAVEMODEL_RADIUS 88
#define KAMI_BOOMSPHEREMODEL_RADIUS 72
// maximum radius of the models during the effect
#define KAMI_SHOCKWAVE_MAXRADIUS 1320
#define KAMI_BOOMSPHERE_MAXRADIUS 720
#define KAMI_SHOCKWAVE2_MAXRADIUS 704

typedef struct {
	const char *name;
} dummyCmd_t;

int cmdcmp(const void *a, const void *b);

/*
=======================================================================================================================================

	SYSTEM TRAPS

	System calls shared by game, cgame, and ui.

=======================================================================================================================================
*/

// print message on the local console
void trap_Print(const char *text);
// abort the game
void trap_Error(const char *text) __attribute__((noreturn));
// milliseconds should only be used for performance tuning, never for anything game related. Get time from the CG_DrawActiveFrame parameter
int trap_Milliseconds(void);
int trap_RealTime(qtime_t *qtime);
void trap_SnapVector(float *v);
// ServerCommand and ConsoleCommand parameter access
int trap_Argc(void);
void trap_Argv(int n, char *buffer, int bufferLength);
void trap_Args(char *buffer, int bufferLength);
// register a command name so the console can perform command completion.
void trap_AddCommand(const char *cmdName);
void trap_RemoveCommand(const char *cmdName);
void trap_Cmd_ExecuteText(int exec_when, const char *text); // ui: don't use EXEC_NOW!
void trap_R_RegisterFont(const char *fontName, int pointSize, float borderWidth, qboolean forceAutoHint, fontInfo_t *font);
// console variable interaction
void trap_Cvar_Register(vmCvar_t *cvar, const char *var_name, const char *value, int flags);
void trap_Cvar_Update(vmCvar_t *cvar);
void trap_Cvar_Set(const char *var_name, const char *value);
void trap_Cvar_SetValue(const char *var_name, float value);
int trap_Cvar_VariableIntegerValue(const char *var_name);
float trap_Cvar_VariableValue(const char *var_name);
void trap_Cvar_VariableStringBuffer(const char *var_name, char *buffer, int bufsize);
// filesystem access
// returns length of file
int trap_FS_FOpenFile(const char *qpath, fileHandle_t *f, fsMode_t mode);
void trap_FS_Read(void *buffer, int len, fileHandle_t f);
void trap_FS_Write(const void *buffer, int len, fileHandle_t f);
int trap_FS_Seek(fileHandle_t f, long offset, int origin); // fsOrigin_t
void trap_FS_FCloseFile(fileHandle_t f);
int trap_FS_GetFileList(const char *path, const char *extension, char *listbuf, int bufsize);
int trap_PC_AddGlobalDefine(const char *define);
int trap_PC_LoadSource(const char *filename);
int trap_PC_ReadToken(int handle, pc_token_t *pc_token);
int trap_PC_SourceFileAndLine(int handle, char *filename, int *line);
