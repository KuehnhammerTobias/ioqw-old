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

#include "../qcommon/q_shared.h"
#include "../renderercommon/tr_types.h"
#include "../game/bg_public.h"
#include "cg_public.h"

/**************************************************************************************************************************************

	The entire cgame module is unloaded and reloaded on each level change, so there is NO persistant data between levels on the client
	side. If you absolutely need something stored, it can either be kept by the server in the server stored userinfos, or stashed in a
	cvar.

**************************************************************************************************************************************/

#define POWERUP_BLINKS 5
#define POWERUP_BLINK_TIME 1000
#define FADE_TIME 200
#define PULSE_TIME 200
#define DAMAGE_DEFLECT_TIME 100
#define DAMAGE_RETURN_TIME 400
#define DAMAGE_TIME 500
#define LAND_DEFLECT_TIME 150
#define LAND_RETURN_TIME 300
#define STEP_TIME 200
#define DUCK_TIME 100
#define PAIN_TWITCH_TIME 200
#define WEAPON_SELECT_TIME 1400
#define ITEM_SCALEUP_TIME 1000
#define ZOOM_TIME 150
#define MUZZLE_FLASH_TIME 20
#define SINK_TIME 1000 // time for fragments to sink into ground before going away
#define ATTACKER_HEAD_TIME 10000
#define PULSE_SCALE 1.5 // amount to scale up the icons when activating
#define MAX_STEP_CHANGE 32
#define MAX_VERTS_ON_POLY 10
#define MAX_MARK_POLYS 256
#define STAT_MINUS 10 // num frame for '-' stats digit
#define ICON_SIZE 48
#define CHAR_WIDTH 32
#define CHAR_HEIGHT 48
#define TEXT_ICON_SPACE 4
#define TEAMCHAT_WIDTH 80
#define TEAMCHAT_HEIGHT 8
// very large characters
#define GIANT_WIDTH 32
#define GIANT_HEIGHT 48
#define NUM_CROSSHAIRS 5
#define TEAM_OVERLAY_MAXNAME_WIDTH 12
#define TEAM_OVERLAY_MAXLOCATION_WIDTH 16

typedef enum {
	// default
	FOOTSTEP_DEFAULT_HARD,
	FOOTSTEP_DEFAULT_HARD_FROZEN,
	FOOTSTEP_DEFAULT_HARD_SNOW,
	FOOTSTEP_DEFAULT_HARD_SLUSH,
	FOOTSTEP_DEFAULT_PUDDLE,
	FOOTSTEP_DEFAULT_LEAVES,
	FOOTSTEP_DEFAULT_BUSH,
	FOOTSTEP_DEFAULT_GRASS,
	FOOTSTEP_DEFAULT_LONGGRASS,
	FOOTSTEP_DEFAULT_LONGGRASS_MUD,
	FOOTSTEP_DEFAULT_SAND,
	FOOTSTEP_DEFAULT_GRAVEL,
	FOOTSTEP_DEFAULT_RUBBLE,
	FOOTSTEP_DEFAULT_RUBBLE_WET,
	FOOTSTEP_DEFAULT_SOIL,
	FOOTSTEP_DEFAULT_MUD,
	FOOTSTEP_DEFAULT_SNOW_DEEP,
	FOOTSTEP_DEFAULT_ICE,
	FOOTSTEP_DEFAULT_METAL_HOLLOW,
	FOOTSTEP_DEFAULT_METAL_HOLLOW_FROZEN,
	FOOTSTEP_DEFAULT_METAL_HOLLOW_SNOW,
	FOOTSTEP_DEFAULT_METAL_HOLLOW_SLUSH,
	FOOTSTEP_DEFAULT_METAL_HOLLOW_SPLASH,
	FOOTSTEP_DEFAULT_GRATE_01,
	FOOTSTEP_DEFAULT_GRATE_02,
	FOOTSTEP_DEFAULT_DUCT,
	FOOTSTEP_DEFAULT_PLATE,
	FOOTSTEP_DEFAULT_FENCE,
	FOOTSTEP_DEFAULT_WOOD_HOLLOW,
	FOOTSTEP_DEFAULT_WOOD_HOLLOW_FROZEN,
	FOOTSTEP_DEFAULT_WOOD_HOLLOW_SNOW,
	FOOTSTEP_DEFAULT_WOOD_HOLLOW_SLUSH,
	FOOTSTEP_DEFAULT_WOOD_HOLLOW_SPLASH,
	FOOTSTEP_DEFAULT_WOOD_SOLID,
	FOOTSTEP_DEFAULT_WOOD_CREAKING,
	FOOTSTEP_DEFAULT_ROOF,
	FOOTSTEP_DEFAULT_SHINGLES,
	FOOTSTEP_DEFAULT_SOFT,
	FOOTSTEP_DEFAULT_GLASS_SHARDS,
	FOOTSTEP_DEFAULT_TRASH_GLASS,
	FOOTSTEP_DEFAULT_TRASH_DEBRIS,
	FOOTSTEP_DEFAULT_TRASH_WIRE,
	FOOTSTEP_DEFAULT_TRASH_PACKING,
	FOOTSTEP_DEFAULT_TRASH_PLASTIC,
	// boot_01
	FOOTSTEP_BOOT_01_HARD,
	FOOTSTEP_BOOT_01_BUSH,
	// boot_02
	FOOTSTEP_BOOT_02_HARD,
	// boot_03
	FOOTSTEP_BOOT_03_HARD,
	// flesh_01
	FOOTSTEP_FLESH_01_HARD,
	// flesh_02
	FOOTSTEP_FLESH_02_HARD,
	// heels_01
	FOOTSTEP_HEELS_01_HARD,
	// heels_02
	FOOTSTEP_HEELS_02_HARD,
	// heels_03
	FOOTSTEP_HEELS_03_HARD,
	// sandals_01
	FOOTSTEP_SANDALS_01_HARD,
	// step_01
	FOOTSTEP_STEP_01_HARD,
	// step_02
	FOOTSTEP_STEP_02_HARD,
	// step_03
	FOOTSTEP_STEP_03_HARD,
	// strogg_01
	FOOTSTEP_STROGG_01_HARD,
	// klesk
	FOOTSTEP_SPEC_KLESK_HARD,
	// sorlag
	FOOTSTEP_SPEC_SORLAG_HARD,
	// mission character (medium)
	FOOTSTEP_T2_MEDIUM_HARD,
	// mission character (heavy)
	FOOTSTEP_T2_HEAVY_HARD,
	// mission character (small)
	FOOTSTEP_T2_SMALL_HARD,
	// common footstep sounds (all characters use the same sound)
	FOOTSTEP_SPLASH,
	FOOTSTEP_WADE,
	FOOTSTEP_SWIM,
	FOOTSTEP_TOTAL
} footstep_t;

typedef enum {
	FOOTTYPE_DEFAULT,			// default footstep sound for (male/female) humans, zombies with boots etc.
	FOOTTYPE_BOOT_01,			// e.g.: Ranger
	FOOTTYPE_BOOT_02,			// e.g.: Sarge
	FOOTTYPE_BOOT_03,			// e.g.: Fritzkrieg
	FOOTTYPE_FLESH_01,			// e.g.: Xaero
	FOOTTYPE_FLESH_02,			// e.g.: Klansman
	FOOTTYPE_HEELS_01,			// e.g.: Mynx
	FOOTTYPE_HEELS_02,			// e.g.: Major
	FOOTTYPE_HEELS_03,			// e.g.: Lucy (and any other fat slut)
	FOOTTYPE_SANDALS_01,		// e.g.: Xaerena
	FOOTTYPE_STEP_01,			// e.g.: Grunt (default sound for non-mission characters?)
	FOOTTYPE_STEP_02,			// e.g.: Hossman
	FOOTTYPE_STEP_03,			// e.g.: Doom, Razor
	FOOTTYPE_STROGG_01,			// e.g.: Visor
	FOOTTYPE_SPEC_KLESK,		// e.g.: Klesk
	FOOTTYPE_SPEC_SORLAG,		// e.g.: Sorlag
	FOOTTYPE_T2_MEDIUM,			// mission character (normal)
	FOOTTYPE_T2_HEAVY,			// mission character (heavy) e.g.: Soldier
	FOOTTYPE_T2_SMALL			// mission character (lightweight) e.g.: Medic
} footType_t;

/**************************************************************************************************************************************

	Player entities need to track more information than any other type of entity.

	Note that not every player entity is a client entity, because corpses after respawn are outside the normal client numbering range.

	When changing animation, set animationTime to frameTime + lerping time.
	The current lerp will finish out, then it will lerp to the new animation.

**************************************************************************************************************************************/

typedef struct {
	int oldFrame;
	int oldFrameTime;		// time when ->oldFrame was exactly on
	int frame;
	int frameTime;			// time when ->frame will be exactly on
	float backlerp;
	float yawAngle;
	qboolean yawing;
	float pitchAngle;
	qboolean pitching;
	int animationNumber;	// may include ANIM_TOGGLEBIT
	animation_t *animation;
	int animationTime;		// time when the first frame of the animation will be exact
} lerpFrame_t;

typedef struct {
	lerpFrame_t legs, torso, flag;
	int painTime;
	int painDirection; // flip from 0 to 1
	int beamgunFiring;
	int railFireTime;
	// machinegun spinning
	float barrelAngle;
	int barrelTime;
	qboolean barrelSpinning;
} playerEntity_t;

/**************************************************************************************************************************************

	centity_t have a direct correspondence with gentity_t in the game, but only the entityState_t is directly communicated to the cgame.

**************************************************************************************************************************************/

typedef struct centity_s {
	entityState_t currentState;	// from cg.frame
	entityState_t nextState;	// from cg.nextFrame, if available
	qboolean interpolate;		// true if next is valid to interpolate to
	qboolean currentValid;		// true if cg.frame holds this entity
	int muzzleFlashTime;		// move to playerEntity?
	int previousEvent;
	int trailTime;				// so missile trails can handle dropped initial packets
	int dustTrailTime;
	int miscTime;
	int snapShotTime;			// last time this entity was found in a snapshot
	playerEntity_t pe;
	int errorTime;				// decay the error from this time
	vec3_t rawOrigin;
	vec3_t rawAngles;
	// exact interpolated position of entity on this frame
	vec3_t lerpOrigin;
	vec3_t lerpAngles;
	// client side dlights
	int dl_frame;
	int dl_oldframe;
	float dl_backlerp;
	int dl_time;
	char dl_stylestring[64];
	int dl_sound;
	int dl_atten;
} centity_t;

/**************************************************************************************************************************************

	local entities are created as a result of events or predicted actions, and live independently from all server transmitted entities.

**************************************************************************************************************************************/

typedef struct markPoly_s {
	struct markPoly_s *prevMark, *nextMark;
	int time;
	qhandle_t markShader;
	qboolean alphaFade; // fade alpha instead of rgb
	float color[4];
	int numVerts;
	polyVert_t verts[MAX_VERTS_ON_POLY];
	int bmodelNum;
} markPoly_t;

typedef enum {
	LE_SPRITE_EXPLOSION,
	LE_EXPLOSION,
	LE_KAMIKAZE,
	LE_MARK,
	LE_FRAGMENT,
	LE_SCALE_FADE,
	LE_FADE_RGB,
	LE_FALL_SCALE_FADE,
	LE_MOVE_SCALE_FADE,
	LE_BUBBLE,
	LE_SCOREPLUM,
	LE_SHOWREFENTITY
} leType_t;

typedef enum {
	LEF_PUFF_DONT_SCALE = 0x0001, // do not scale size over time
	LEF_TUMBLE			= 0x0002, // tumble over time, used for plasma balls etc.
	LEF_GIBS			= 0x0004, // tumble over time, used for flying gibs
	LEF_SOUND1			= 0x0008, // sound 1 for kamikaze
	LEF_SOUND2			= 0x0010  // sound 2 for kamikaze
} leFlag_t;
// fragment local entities can leave marks on walls
typedef enum {
	LEMT_NONE,
	LEMT_BURN,
	LEMT_BLOOD
} leMarkType_t;
// fragment local entities can make sounds on impacts
typedef enum {
	LEBS_NONE,
	LEBS_BLOOD,
	LEBS_BRASS
} leBounceSoundType_t;

typedef struct localEntity_s {
	struct localEntity_s *prev, *next;
	leType_t leType;
	int leFlags;
	int startTime;
	int endTime;
	int fadeInTime;
	float lifeRate;				// 1.0 / (endTime - startTime)
	trajectory_t pos;
	trajectory_t angles;
	int groundEntityNum;
	float bounceFactor;			// 0.0 = no bounce, 1.0 = perfect
	float color[4];
	float radius;
	float light;
	vec3_t lightColor;
	leMarkType_t leMarkType;	// mark to leave on fragment impact
	leBounceSoundType_t leBounceSoundType;
	refEntity_t refEntity;
} localEntity_t;

typedef struct {
	int client;
	int score;
	int ping;
	int time;
	int scoreFlags;
	int accuracy;
	int excellentCount;
	int impressiveCount;
	int gauntletCount;
	int captures;
	int defendCount;
	int assistCount;
	qboolean perfect;
	int team;
} score_t;

/**************************************************************************************************************************************

	Each client has an associated clientInfo_t that contains media references necessary to present the client model and other color
	coded effects. This is regenerated each time a client's configstring changes, usually as a result of a userinfo (name, model, etc.)
	change.

**************************************************************************************************************************************/

#define MAX_CUSTOM_SOUNDS 32

typedef struct {
	qboolean infoValid;
	char name[MAX_QPATH];
	team_t team;
	int botSkill;			// 0 = not bot, 1-5 = bot
	vec3_t color1;
	vec3_t color2;
	byte c1RGBA[4];
	byte c2RGBA[4];
	int score;				// updated by score servercmds
	int location;			// location index for team mode
	int health;				// you only get this info about your teammates
	int armor;
	int curWeapon;
	int wins, losses;		// in tourney mode
	int teamTask;			// task in teamplay (offence/defence)
	qboolean teamLeader;	// true when this is a team leader
	int powerups;			// so can display quad/flag status
	int medkitUsageTime;
	int breathPuffTime;
	// when clientinfo is changed, the loading of models/skins/sounds can be deferred until you are dead, to prevent hitches in gameplay
	char modelName[MAX_QPATH];
	char skinName[MAX_QPATH];
	char headModelName[MAX_QPATH];
	char headSkinName[MAX_QPATH];
	char redTeam[MAX_TEAMNAME];
	char blueTeam[MAX_TEAMNAME];
	qboolean deferred;
	qboolean newAnims;		// true if using the new mission pack animations
	qboolean fixedlegs;		// true if legs yaw is always the same as torso yaw
	qboolean fixedtorso;	// true if torso never changes yaw
	vec3_t headOffset;		// move head in icon views
	footstep_t foottype;
	footstep_t footsteps4;
	footstep_t footsteps8;
	gender_t gender;		// from model
	qhandle_t legsModel;
	qhandle_t torsoModel;
	qhandle_t headModel;
	cgSkin_t modelSkin;
	qhandle_t modelIcon;
	animation_t animations[MAX_TOTALANIMATIONS];
	sfxHandle_t sounds[MAX_CUSTOM_SOUNDS];
} clientInfo_t;

/**************************************************************************************************************************************

	Each WP_* weapon enum has an associated weaponInfo_t that contains media references necessary to present the weapon and its effects.

**************************************************************************************************************************************/

typedef struct weaponInfo_s {
	qboolean registered;
	gitem_t *item;
	qhandle_t handsModel;		// the hands don't actually draw, they just position the weapon
	qhandle_t weaponModel;
	qhandle_t barrelModel;
	qhandle_t flashModel;
	vec3_t weaponMidpoint;		// so it will rotate centered instead of by tag
	float flashDlight;
	vec3_t flashDlightColor;
	sfxHandle_t flashSound[4];	// fast firing weapons randomly choose
	qhandle_t weaponIcon;
	qhandle_t ammoIcon;
	qhandle_t ammoModel;
	qhandle_t missileModel;
	sfxHandle_t missileSound;
	void (*missileTrailFunc)(centity_t *, const struct weaponInfo_s *wi);
	float missileDlight;
	vec3_t missileDlightColor;
	void (*ejectBrassFunc)(centity_t *);
	float trailRadius;
	float wiTrailTime;
	sfxHandle_t readySound;
	sfxHandle_t firingSound;
} weaponInfo_t;

/**************************************************************************************************************************************

	Each IT_* item has an associated itemInfo_t that constains media references necessary to present the item and its effects.

**************************************************************************************************************************************/

typedef struct {
	qboolean registered;
	qhandle_t models[MAX_ITEM_MODELS];
	qhandle_t icon;
} itemInfo_t;

#define MAX_SKULLTRAIL 10

typedef struct {
	vec3_t positions[MAX_SKULLTRAIL];
	int numpositions;
} skulltrail_t;

#define MAX_SOUNDBUFFER 32

/**************************************************************************************************************************************

	All cg.stepTime, cg.duckTime, cg.landTime, etc are set to cg.time when the action occurs, and they will have visible effects for
	#define STEP_TIME or whatever msec after.

**************************************************************************************************************************************/

#define MAX_PREDICTED_EVENTS 16

typedef struct {
	int clientFrame;				// incremented each frame
	int clientNum;
	qboolean demoPlayback;
	qboolean levelShot;				// taking a level menu screenshot
	int deferredPlayerLoading;
	qboolean loading;				// don't defer players at initial startup
	qboolean intermissionStarted;	// don't play voice rewards, because game will end shortly
	// there are only one or two snapshot_t that are relevant at a time
	int latestSnapshotNum;			// the number of snapshots the client system has received
	int latestSnapshotTime;			// the time from latestSnapshotNum, so we don't need to read the snapshot yet
	snapshot_t *snap;				// cg.snap->serverTime <= cg.time
	snapshot_t *nextSnap;			// cg.nextSnap->serverTime > cg.time, or NULL
	snapshot_t activeSnapshots[2];
	float frameInterpolation;		// (float)(cg.time - cg.frame->serverTime) / (cg.nextFrame->serverTime - cg.frame->serverTime)
	qboolean thisFrameTeleport;
	qboolean nextFrameTeleport;
	int frametime;					// cg.time - cg.oldTime
	int time;						// this is the server time value that the client is rendering at
	int oldTime;					// time at last frame, used for missile trails and prediction checking
	int physicsTime;				// either cg.snap->time or cg.nextSnap->time
	int timelimitWarnings;			// 5 min, 1 min, overtime
	int fraglimitWarnings;
	qboolean mapRestart;			// set on a map restart to set back the weapon
	qboolean renderingThirdPerson;	// during deaths, chasecams, etc.
	// prediction state
	qboolean hyperspace;			// true if prediction has hit a trigger_teleport
	playerState_t predictedPlayerState;
	centity_t predictedPlayerEntity;
	qboolean validPPS;				// clear until the first call to CG_PredictPlayerState
	int predictedErrorTime;
	vec3_t predictedError;
	int eventSequence;
	int predictableEvents[MAX_PREDICTED_EVENTS];
	float stepChange;				// for stair up smoothing
	int stepTime;
	float duckChange;				// for duck viewheight smoothing
	int duckTime;
	float landChange;				// for landing hard
	int landTime;
	// input state sent to server
	int weaponSelect;
	// auto rotating items
	vec3_t autoAngles;
	vec3_t autoAxis[3];
	vec3_t autoAnglesFast;
	vec3_t autoAxisFast[3];
	// view rendering
	refdef_t refdef;
	vec3_t refdefViewAngles;		// will be converted to refdef.viewaxis
	float fov;						// either range checked cg_fov or forced value
	// spawn variables
	qboolean spawning;				// the CG_Spawn *() functions are valid
	int numSpawnVars;
	char *spawnVars[MAX_SPAWN_VARS][2]; // key/value pairs
	int numSpawnVarChars;
	char spawnVarChars[MAX_SPAWN_VARS_CHARS];
	int spawnEntityOffset;
	vec2_t mapcoordsMins;
	vec2_t mapcoordsMaxs;
	qboolean mapcoordsValid;
	int numMiscGameModels;
	qboolean hasSkyPortal;
	vec3_t skyPortalOrigin;
	vec3_t skyPortalFogColor;
	int skyPortalFogDepthForOpaque;
	float skyAlpha;
	// zoom key
	qboolean zoomed;
	int zoomTime;
	float zoomSensitivity;
	// information screen text during loading
	char infoScreenText[MAX_STRING_CHARS];
	qboolean lightstylesInited;
	// scoreboard
	int scoresRequestTime;
	int numScores;
	int selectedScore;
	int teamScores[2];
	score_t scores[MAX_CLIENTS];
	qboolean showScores;
	qboolean scoreBoardShowing;
	int scoreFadeTime;
	char killerName[MAX_NAME_LENGTH];
	char spectatorList[MAX_STRING_CHARS];	// list of names
	int spectatorTime;						// last time offset
	float spectatorOffset;					// current offset from start
	// skull trails
	skulltrail_t skulltrails[MAX_CLIENTS];
	// centerprinting
	int centerPrintTime;
	int centerPrintCharWidth;
	int centerPrintY;
	char centerPrint[1024];
	int centerPrintLines;
	// low ammo warning state
	int lowAmmoWarning;						// 1 = low, 2 = empty
	// crosshair client ID
	int crosshairClientNum;
	int crosshairClientTime;
	// powerup active flashing
	int powerupActive;
	int powerupTime;
	// attacking player
	int attackerTime;
	int voiceTime;
	// sound buffer mainly for announcer sounds
	int soundBufferIn;
	int soundBufferOut;
	int soundTime;
	qhandle_t soundBuffer[MAX_SOUNDBUFFER];
	qhandle_t soundPlaying;
	// for voice chat buffer
	int voiceChatTime;
	int voiceChatBufferIn;
	int voiceChatBufferOut;
	// warmup countdown
	int warmup;
	int warmupCount;
	qboolean warmupCounterShowing;
#ifndef BASEGAME
	int itemPickup;
	int itemPickupTime;
	int itemPickupBlendTime;				// the pulse around the crosshair is timed separately
#endif
	int weaponSelectTime;
	int weaponAnimation;
	int weaponAnimationTime;
	// blend blobs
	float damageTime;
	float damageX, damageY, damageValue;
	// status bar head
	float headEndPitch;
	float headEndYaw;
	int headEndTime;
	float headStartPitch;
	float headStartYaw;
	int headStartTime;
	// view movement
	float v_dmg_time;
	float v_dmg_pitch;
	float v_dmg_roll;
	// temp working variables for player view
	float bobfracsin;
	int bobcycle;
	float xyspeed;
	//qboolean cameraMode;					// if rendering from a loaded camera
	// development tool
	refEntity_t testModelEntity;
	char testModelName[MAX_QPATH];
	qboolean testGun;
} cg_t;

/**************************************************************************************************************************************

	All of the model, shader, and sound references that are loaded at gamestate time are stored in cgMedia_t. Other media that can be
	tied to clients, weapons, or items are stored in the clientInfo_t, weaponInfo_t, or itemInfo_t.

**************************************************************************************************************************************/

typedef struct {
	qhandle_t charsetShader;
	qhandle_t charsetProp;
	qhandle_t charsetPropGlow;
	qhandle_t charsetPropB;
	qhandle_t whiteShader;
	qhandle_t nodrawShader;
	qhandle_t cursor;
	qhandle_t selectCursor;
	qhandle_t sizeCursor;
	qhandle_t redCubeModel;
	qhandle_t blueCubeModel;
	qhandle_t redCubeIcon;
	qhandle_t blueCubeIcon;
	qhandle_t redFlagModel;
	qhandle_t blueFlagModel;
	qhandle_t neutralFlagModel;
	qhandle_t redFlagShader[3];
	qhandle_t blueFlagShader[3];
	qhandle_t flagShader[4];
	qhandle_t flagPoleModel;
	qhandle_t flagFlapModel;
	cgSkin_t redFlagFlapSkin;
	cgSkin_t blueFlagFlapSkin;
	cgSkin_t neutralFlagFlapSkin;
	qhandle_t redFlagBaseModel;
	qhandle_t blueFlagBaseModel;
	qhandle_t neutralFlagBaseModel;
	qhandle_t overloadBaseModel;
	qhandle_t overloadTargetModel;
	qhandle_t overloadLightsModel;
	qhandle_t overloadEnergyModel;
	qhandle_t harvesterModel;
	cgSkin_t harvesterRedSkin;
	cgSkin_t harvesterBlueSkin;
	qhandle_t harvesterNeutralModel;
	qhandle_t armorModel;
	qhandle_t armorIcon;
	qhandle_t teamStatusBar;
	qhandle_t deferShader;
	// gib explosions
	qhandle_t gibAbdomen;
	qhandle_t gibArm;
	qhandle_t gibChest;
	qhandle_t gibFist;
	qhandle_t gibFoot;
	qhandle_t gibForearm;
	qhandle_t gibIntestine;
	qhandle_t gibLeg;
	qhandle_t gibSkull;
	qhandle_t gibBrain;
	qhandle_t smoke2;
	qhandle_t machinegunBrassModel;
	qhandle_t shotgunBrassModel;
	qhandle_t railRingsShader;
	qhandle_t railCoreShader;
	qhandle_t lightningShader;
	qhandle_t redFriendShader;
	qhandle_t blueFriendShader;
	qhandle_t balloonShader;
	qhandle_t connectionShader;
	qhandle_t coronaShader;
	qhandle_t selectShader;
	qhandle_t viewBloodShader;
	qhandle_t tracerShader;
	qhandle_t crosshairShader[NUM_CROSSHAIRS];
	qhandle_t lagometerShader;
	qhandle_t backTileShader;
	qhandle_t noammoShader;
	qhandle_t smokePuffShader;
	qhandle_t shotgunSmokePuffShader;
	qhandle_t plasmaBallShader;
	qhandle_t waterBubbleShader;
	qhandle_t bloodTrailShader;
	qhandle_t nailPuffShader;
	qhandle_t blueProxMine;
	qhandle_t numberShaders[11];
	qhandle_t shadowMarkShader;
	qhandle_t botSkillShaders[5];
	// wall mark shaders
	qhandle_t wakeMarkShader;
	qhandle_t bloodMarkShader;
	qhandle_t bulletMarkShader;
	qhandle_t burnMarkShader;
	qhandle_t holeMarkShader;
	qhandle_t energyMarkShader;
	// powerup shaders
	qhandle_t quadShader;
	qhandle_t quadWeaponShader;
	qhandle_t invisRedShader;
	qhandle_t invisBlueShader;
	qhandle_t invisShader;
	qhandle_t regenShader;
	// weapon effect models
	qhandle_t bulletFlashModel;
	qhandle_t ringFlashModel;
	qhandle_t dishFlashModel;
	qhandle_t lightningExplosionModel;
	// weapon effect shaders
	qhandle_t railExplosionShader;
	qhandle_t plasmaExplosionShader;
	qhandle_t bulletExplosionShader;
	qhandle_t rocketExplosionShader;
	qhandle_t grenadeExplosionShader;
	qhandle_t bfgExplosionShader;
	qhandle_t bloodExplosionShader;
	// special effects models
	qhandle_t teleportEffectModel;
	qhandle_t teleportEffectShader;
	qhandle_t medkitUsageModel;
	qhandle_t kamikazeEffectModel;
	qhandle_t kamikazeShockWave;
	qhandle_t kamikazeHeadModel;
	qhandle_t kamikazeHeadTrail;
	qhandle_t guardPowerupModel;
	qhandle_t scoutPowerupModel;
	qhandle_t doublerPowerupModel;
	qhandle_t ammoRegenPowerupModel;
	qhandle_t dustPuffShader;
	qhandle_t heartShader;
	// scoreboard headers
	qhandle_t scoreboardName;
	qhandle_t scoreboardPing;
	qhandle_t scoreboardScore;
	qhandle_t scoreboardTime;
	// medals shown during gameplay
	qhandle_t medalAccuracy;
	qhandle_t medalExcellent;
	qhandle_t medalImpressive;
	qhandle_t medalGauntlet;
	qhandle_t medalCapture;
	qhandle_t medalDefend;
	qhandle_t medalAssist;
	// flag shaders
	qhandle_t flagShaders[3];
	// task shaders
	qhandle_t roamShader; // Tobias DEBUG
	qhandle_t patrolShader;
	qhandle_t assaultShader;
	qhandle_t campShader;
	qhandle_t followShader;
	qhandle_t defendShader;
	qhandle_t teamLeaderShader;
	qhandle_t retrieveShader;
	qhandle_t escortShader;
// Tobias BULLET
	// debug utils
	qhandle_t debugPlayerAABB;
	qhandle_t debugPlayerAABB_twoSided;
// Tobias END
	// sounds
	sfxHandle_t talkSound;
	sfxHandle_t killSound;
	sfxHandle_t hitSound;
	sfxHandle_t hitSoundHighArmor;
	sfxHandle_t hitSoundLowArmor;
	sfxHandle_t impressiveSound;
	sfxHandle_t excellentSound;
	sfxHandle_t deniedSound;
	sfxHandle_t humiliationSound;
	sfxHandle_t assistSound;
	sfxHandle_t defendSound;
	sfxHandle_t firstImpressiveSound;
	sfxHandle_t firstExcellentSound;
	sfxHandle_t firstHumiliationSound;
	sfxHandle_t takenLeadSound;
	sfxHandle_t tiedLeadSound;
	sfxHandle_t lostLeadSound;
	sfxHandle_t voteNow;
	sfxHandle_t votePassed;
	sfxHandle_t voteFailed;
	sfxHandle_t oneMinuteSound;
	sfxHandle_t fiveMinuteSound;
	sfxHandle_t suddenDeathSound;
	sfxHandle_t oneFragSound;
	sfxHandle_t twoFragSound;
	sfxHandle_t threeFragSound;
	// tournament sounds
	sfxHandle_t count3Sound;
	sfxHandle_t count2Sound;
	sfxHandle_t count1Sound;
	sfxHandle_t countFightSound;
	sfxHandle_t countPrepareSound;
	// teamplay sounds
	sfxHandle_t hitTeamSound;
	sfxHandle_t countPrepareTeamSound;
	sfxHandle_t redScoredSound;
	sfxHandle_t blueScoredSound;
	sfxHandle_t redLeadsSound;
	sfxHandle_t blueLeadsSound;
	sfxHandle_t teamsTiedSound;
	sfxHandle_t redWinsSound;
	sfxHandle_t blueWinsSound;
	sfxHandle_t captureYourTeamSound;
	sfxHandle_t captureOpponentSound;
	sfxHandle_t returnYourTeamSound;
	sfxHandle_t returnOpponentSound;
	sfxHandle_t takenYourTeamSound;
	sfxHandle_t takenOpponentSound;
	sfxHandle_t redFlagReturnedSound;
	sfxHandle_t blueFlagReturnedSound;
	sfxHandle_t enemyTookYourFlagSound;
	sfxHandle_t yourTeamTookEnemyFlagSound;
	sfxHandle_t neutralFlagReturnedSound;
	sfxHandle_t yourTeamTookTheFlagSound;
	sfxHandle_t enemyTookTheFlagSound;
	sfxHandle_t youHaveFlagSound;
	sfxHandle_t holyShitSound;
	sfxHandle_t yourBaseIsUnderAttackSound;
	sfxHandle_t winnerSound;
	sfxHandle_t loserSound;
	// item sounds
	sfxHandle_t itemPickupSounds[MAX_ITEMS];
	sfxHandle_t pickupSound;
	sfxHandle_t medkitSound;
	sfxHandle_t kamikazeExplodeSound;
	sfxHandle_t kamikazeImplodeSound;
	sfxHandle_t kamikazeFarSound;
	sfxHandle_t useNothingSound;
	sfxHandle_t quadSound;
	sfxHandle_t regenSound;
	sfxHandle_t wearOffSound;
	sfxHandle_t ammoregenSound;
	sfxHandle_t doublerSound;
	sfxHandle_t guardSound;
	sfxHandle_t scoutSound;
	sfxHandle_t obeliskHitSound1;
	sfxHandle_t obeliskHitSound2;
	sfxHandle_t obeliskHitSound3;
	// weapon sounds
	sfxHandle_t sfx_ric1;
	sfxHandle_t sfx_ric2;
	sfxHandle_t sfx_ric3;
	sfxHandle_t tracerSound;
	sfxHandle_t sfx_chghit;
	sfxHandle_t sfx_chgstop;
	sfxHandle_t sfx_nghit;
	sfxHandle_t sfx_proxexp;
	sfxHandle_t wstbimpdSound;
	sfxHandle_t wstbactvSound;
	sfxHandle_t hgrenb1aSound;
	sfxHandle_t hgrenb2aSound;
	sfxHandle_t sfx_rockexp;
	sfxHandle_t sfx_lghit1;
	sfxHandle_t sfx_lghit2;
	sfxHandle_t sfx_lghit3;
	sfxHandle_t sfx_plasmaexp;
	sfxHandle_t selectSound;
	sfxHandle_t noAmmoSound;
	sfxHandle_t respawnSound[3];
	sfxHandle_t obeliskRespawnSound[3];
	// character sounds
	sfxHandle_t teleInSound[3];
	sfxHandle_t gibSound[12];
	sfxHandle_t gibBounce1Sound;
	sfxHandle_t gibBounce2Sound;
	sfxHandle_t gibBounce3Sound;
	sfxHandle_t teleOutSound;
	sfxHandle_t jumpPadSound; // Tobias CHECK: move this out from 'world' folder?
	sfxHandle_t landSound;
	sfxHandle_t watrInSound;
	sfxHandle_t watrOutSound;
	sfxHandle_t watrUnSound;
	sfxHandle_t footsteps8[FOOTSTEP_TOTAL][8];
	sfxHandle_t footsteps4[FOOTSTEP_TOTAL][4];
} cgMedia_t;

#define MAX_STATIC_GAMEMODELS 1024

typedef struct cg_gamemodel_s {
	qhandle_t model;
	cgSkin_t skin;
	vec3_t org;
	vec3_t axes[3];
	vec_t radius;
} cg_gamemodel_t;

/**************************************************************************************************************************************

	The client game static (cgs) structure hold everything loaded or calculated from the gamestate. It will NOT be cleared when a
	tournament restart is done, allowing all clients to begin playing instantly.

**************************************************************************************************************************************/

typedef struct {
	gameState_t gameState;			// gamestate from server
	glconfig_t glconfig;			// rendering configuration
	float screenXScale;				// derived from glconfig
	float screenYScale;
	float screenXBias;
	float screenYBias;
	float screenXScaleStretch;
	float screenYScaleStretch;
	int serverCommandSequence;		// reliable command stream counter
	int processedSnapshotNum;		// the number of snapshots cgame has requested
	qboolean localServer;			// detected on startup by checking sv_running
	// parsed from serverinfo
	gametype_t gametype;
	int dmflags;
	int fraglimit;
	int capturelimit;
	int timelimit;
	int maxclients;
	char mapname[MAX_QPATH];
	int voteTime;
	int voteYes;
	int voteNo;
	qboolean voteModified;			// beep whenever changed
	char voteString[MAX_STRING_TOKENS];
	int teamVoteTime[2];
	int teamVoteYes[2];
	int teamVoteNo[2];
	qboolean teamVoteModified[2];	// beep whenever changed
	char teamVoteString[2][MAX_STRING_TOKENS];
	int levelStartTime;
	int scores1, scores2;			// from configstrings
	int redflag, blueflag;			// flag status from configstrings
	int flagStatus;
	qboolean newHud;
	// locally derived information from gamestate
	qhandle_t gameModels[MAX_MODELS];
	sfxHandle_t gameSounds[MAX_SOUNDS];
	int numInlineModels;
	qhandle_t inlineDrawModel[MAX_SUBMODELS];
	vec3_t inlineModelMidpoints[MAX_SUBMODELS];
	clientInfo_t clientinfo[MAX_CLIENTS];
	// teamchat width is * 3 because of embedded color codes
	char teamChatMsgs[TEAMCHAT_HEIGHT][TEAMCHAT_WIDTH * 3 + 1];
	int teamChatMsgTimes[TEAMCHAT_HEIGHT];
	int teamChatPos;
	int teamLastChatPos;
	int cursorX;
	int cursorY;
	qboolean eventHandling;
	qboolean mouseCaptured;
	qboolean sizingHud;
	void *capturedItem;
	qhandle_t activeCursor;
	// orders
	int currentOrder;
	qboolean orderPending;
	int orderTime;
	int currentVoiceClient;
	int acceptOrderTime;
	int acceptTask;
	int acceptLeader;
	char acceptVoice[MAX_NAME_LENGTH];
	// default global fog from bsp or fogvars in a shader
	fogType_t globalFogType;
	vec3_t globalFogColor;
	float globalFogDepthForOpaque;
	float globalFogDensity;
	float globalFogFarClip;
	cg_gamemodel_t miscGameModels[MAX_STATIC_GAMEMODELS];
	// media
	cgMedia_t media;
} cgs_t;

extern cgs_t cgs;
extern cg_t cg;
extern centity_t cg_entities[MAX_GENTITIES];
extern weaponInfo_t cg_weapons[MAX_WEAPONS];
extern itemInfo_t cg_items[MAX_ITEMS];
extern markPoly_t cg_markPolys[MAX_MARK_POLYS];

extern vmCvar_t cg_centertime;
extern vmCvar_t cg_runpitch;
extern vmCvar_t cg_runroll;
extern vmCvar_t cg_runyaw;
extern vmCvar_t cg_bobup;
extern vmCvar_t cg_bobpitch;
extern vmCvar_t cg_bobroll;
extern vmCvar_t cg_bobyaw;
extern vmCvar_t cg_swingSpeed;
extern vmCvar_t cg_statusScale;
extern vmCvar_t cg_stretch;
extern vmCvar_t cg_shadows;
extern vmCvar_t cg_gibs;
extern vmCvar_t cg_drawTimer;
extern vmCvar_t cg_drawClock;
extern vmCvar_t cg_drawFPS;
extern vmCvar_t cg_drawSnapshot;
extern vmCvar_t cg_draw3dIcons;
extern vmCvar_t cg_drawIcons;
extern vmCvar_t cg_drawAmmoWarning;
extern vmCvar_t cg_hitSounds;
extern vmCvar_t cg_drawCrosshair;
extern vmCvar_t cg_drawCrosshairNames;
extern vmCvar_t cg_drawScores;
extern vmCvar_t cg_drawPickups;
extern vmCvar_t cg_drawWeaponBar;
extern vmCvar_t cg_drawStatusHead;
extern vmCvar_t cg_drawTeamOverlay;
extern vmCvar_t cg_teamOverlayUserinfo;
extern vmCvar_t cg_crosshairX;
extern vmCvar_t cg_crosshairY;
extern vmCvar_t cg_crosshairSize;
extern vmCvar_t cg_crosshairHealth;
extern vmCvar_t cg_drawStatus;
extern vmCvar_t cg_draw2D;
extern vmCvar_t cg_animSpeed;
// Tobias DEBUG
extern vmCvar_t cg_drawDebug;
extern vmCvar_t cg_drawStatusDebug;
extern vmCvar_t cg_drawObstacleDebug;
// Tobias END
extern vmCvar_t cg_debugAnim;
extern vmCvar_t cg_debugPosition;
extern vmCvar_t cg_debugEvents;
extern vmCvar_t cg_railTrailTime;
extern vmCvar_t cg_errorDecay;
extern vmCvar_t cg_nopredict;
extern vmCvar_t cg_noPlayerAnims;
extern vmCvar_t cg_showmiss;
extern vmCvar_t cg_footsteps;
extern vmCvar_t cg_addMarks;
extern vmCvar_t cg_brassTime;
extern vmCvar_t cg_gun_frame;
extern vmCvar_t cg_gun_x;
extern vmCvar_t cg_gun_y;
extern vmCvar_t cg_gun_z;
extern vmCvar_t cg_drawGun;
extern vmCvar_t cg_viewsize;
extern vmCvar_t cg_tracerChance;
extern vmCvar_t cg_tracerWidth;
extern vmCvar_t cg_tracerLength;
extern vmCvar_t cg_autoswitch;
extern vmCvar_t cg_ignore;
extern vmCvar_t cg_simpleItems;
extern vmCvar_t cg_fov;
extern vmCvar_t cg_fovAspectAdjust;
extern vmCvar_t cg_fovGunAdjust;
extern vmCvar_t cg_zoomFov;
extern vmCvar_t cg_thirdPersonRange;
extern vmCvar_t cg_thirdPersonAngle;
extern vmCvar_t cg_thirdPerson;
extern vmCvar_t cg_drawLagometer;
extern vmCvar_t cg_drawAttacker;
extern vmCvar_t cg_synchronousClients;
extern vmCvar_t cg_singlePlayer;
extern vmCvar_t cg_teamChatTime;
extern vmCvar_t cg_teamChatHeight;
extern vmCvar_t cg_stats;
extern vmCvar_t cg_forceModel;
extern vmCvar_t cg_buildScript;
extern vmCvar_t cg_paused;
extern vmCvar_t cg_blood;
extern vmCvar_t cg_predictItems;
extern vmCvar_t cg_deferPlayers;
extern vmCvar_t cg_drawFriend;
extern vmCvar_t cg_teamChatsOnly;
extern vmCvar_t cg_noVoiceChats;
extern vmCvar_t cg_noVoiceText;
extern vmCvar_t cg_scorePlum;
extern vmCvar_t cg_smoothClients;
extern vmCvar_t pmove_fixed;
extern vmCvar_t pmove_msec;
//extern vmCvar_t cg_pmove_fixed;
extern vmCvar_t cg_cameraOrbit;
extern vmCvar_t cg_timescaleFadeEnd;
extern vmCvar_t cg_timescaleFadeSpeed;
extern vmCvar_t cg_timescale;
extern vmCvar_t cg_cameraMode;
extern vmCvar_t cg_smallFont;
extern vmCvar_t cg_bigFont;
extern vmCvar_t cg_noTaunt;
extern vmCvar_t cg_noProjectileTrail;
extern vmCvar_t cg_oldRail;
extern vmCvar_t cg_oldRocket;
extern vmCvar_t cg_oldPlasma;
extern vmCvar_t cg_enableDust;
extern vmCvar_t cg_enableBreath;
extern vmCvar_t cg_obeliskRespawnDelay;
extern vmCvar_t cg_trueLightning;
extern vmCvar_t cg_atmosphericEffects;
extern vmCvar_t cg_coronafardist;
extern vmCvar_t cg_coronas;
extern vmCvar_t cg_fadeExplosions;
extern vmCvar_t cg_skybox;
extern vmCvar_t cg_redTeamName;
extern vmCvar_t cg_blueTeamName;
extern vmCvar_t cg_currentSelectedPlayer;
extern vmCvar_t cg_currentSelectedPlayerName;
extern vmCvar_t cg_recordSPDemo;
extern vmCvar_t cg_recordSPDemoName;
// cg_main.c
const char *CG_ConfigString(int index);
const char *CG_Argv(int arg);
void QDECL CG_Printf(const char *msg, ...) __attribute__((format(printf, 1, 2)));
void QDECL CG_Error(const char *msg, ...) __attribute__((noreturn, format(printf, 1, 2)));
void CG_StartMusic(void);
void CG_UpdateCvars(void);
int CG_CrosshairPlayer(void);
int CG_LastAttacker(void);
void CG_LoadMenus(const char *menuFile);
void CG_HudMenuHacks(void);
void CG_KeyEvent(int key, qboolean down);
void CG_MouseEvent(int x, int y);
void CG_EventHandling(int type);
void CG_RankRunFrame(void);
void CG_SetScoreSelection(void *menu);
score_t *CG_GetSelectedScore(void);
void CG_BuildSpectatorString(void);
void CG_SetupDlightstyles(void);
// cg_view.c
void CG_TestModel_f(void);
void CG_TestGun_f(void);
void CG_TestModelNextFrame_f(void);
void CG_TestModelPrevFrame_f(void);
void CG_TestModelNextSkin_f(void);
void CG_TestModelPrevSkin_f(void);
void CG_ZoomDown_f(void);
void CG_ZoomUp_f(void);
void CG_AddBufferedAnnouncerSound(sfxHandle_t sfx);
void CG_SetupFrustum(void);
qboolean CG_CullPoint(vec3_t pt);
qboolean CG_CullPointAndRadius(const vec3_t pt, vec_t radius);
qboolean CG_HasBufferedAnnouncerSound(void);
void CG_DrawActiveFrame(int serverTime, stereoFrame_t stereoView, qboolean demoPlayback);
// cg_drawtools.c
// ugly workaround for having it in cg_local.h and ui_shared.h
#ifndef HAVE_SCREEN_PLACEMENT
#define HAVE_SCREEN_PLACEMENT
typedef enum {
	PLACE_STRETCH,
	PLACE_CENTER,
	// horizontal only
	PLACE_LEFT,
	PLACE_RIGHT,
	// vertical only
	PLACE_TOP,
	PLACE_BOTTOM
} screenPlacement_e;
#endif
void CG_SetScreenPlacement(screenPlacement_e hpos, screenPlacement_e vpos);
void CG_PopScreenPlacement(void);
screenPlacement_e CG_GetScreenHorizontalPlacement(void);
screenPlacement_e CG_GetScreenVerticalPlacement(void);
void CG_AdjustFrom640(float *x, float *y, float *w, float *h);
void CG_FillRect(float x, float y, float width, float height, const float *color);
void CG_DrawPic(float x, float y, float width, float height, qhandle_t hShader);
void CG_SetClipRegion(float x, float y, float w, float h);
void CG_ClearClipRegion(void);
void CG_Text_Paint(float x, float y, float scale, vec4_t color, const char *text, float adjust, int limit, int style);
void CG_Text_PaintChar(float x, float y, float width, float height, float scale, float s, float t, float s2, float t2, qhandle_t hShader);
int CG_Text_Width(const char *text, float scale, int limit);
int CG_Text_Height(const char *text, float scale, int limit);
// Base Q3 text drawing
void CG_DrawString(float x, float y, const char *string, float charWidth, float charHeight, const float *modulate);
void CG_DrawStringExt(int x, int y, const char *string, const float *setColor, qboolean forceColor, qboolean shadow, int charWidth, int charHeight, int maxChars);
void CG_DrawBigString(int x, int y, const char *s, float alpha);
void CG_DrawBigStringColor(int x, int y, const char *s, vec4_t color);
void CG_DrawSmallString(int x, int y, const char *s, float alpha);
void CG_DrawSmallStringColor(int x, int y, const char *s, vec4_t color);
int CG_DrawStrlen(const char *str);
float *CG_FadeColor(int startMsec, int totalMsec);
float *CG_TeamColor(int team);
void CG_TileClear(void);
void CG_ColorForHealth(vec4_t hcolor);
void CG_GetColorForHealth(int health, int armor, vec4_t hcolor);
void UI_DrawProportionalString(int x, int y, const char *str, int style, vec4_t color);
void CG_DrawRect(float x, float y, float width, float height, float size, const float *color);
void CG_DrawSides(float x, float y, float w, float h, float size);
void CG_DrawTopBottom(float x, float y, float w, float h, float size);
// cg_draw.c, cg_newDraw.c
typedef enum {
	SYSTEM_PRINT,
	CHAT_PRINT,
	TEAMCHAT_PRINT
} q3print_t;

extern int sortedTeamPlayers[TEAM_MAXOVERLAY];
extern int numSortedTeamPlayers;
extern int drawTeamOverlayModificationCount;
extern char systemChat[256];
extern char teamChat1[256];
extern char teamChat2[256];

void CG_AddLagometerFrameInfo(void);
void CG_AddLagometerSnapshotInfo(snapshot_t *snap);
void CG_CenterPrint(const char *str, int y, int charWidth);
void CG_DrawHead(float x, float y, float w, float h, int clientNum, vec3_t headAngles);
void CG_DrawActive(stereoFrame_t stereoView);
void CG_DrawFlagModel(float x, float y, float w, float h, int team, qboolean force2D);
void CG_DrawTeamBackground(int x, int y, int w, int h, float alpha, int team);
void CG_OwnerDraw(float x, float y, float w, float h, float text_x, float text_y, int ownerDraw, int ownerDrawFlags, int align, float special, float scale, vec4_t color, qhandle_t shader, int textStyle);
void CG_SelectPrevPlayer(void);
void CG_SelectNextPlayer(void);
float CG_GetValue(int ownerDraw);
qboolean CG_OwnerDrawVisible(int flags);
void CG_RunMenuScript(char **args);
void CG_ShowResponseHead(void);
void CG_SetPrintString(int type, const char *p);
void CG_InitTeamChat(void);
void CG_GetTeamColor(vec4_t *color);
const char *CG_GetGameStatusText(void);
const char *CG_GetKillerText(void);
void CG_Draw3DModel(float x, float y, float w, float h, qhandle_t model, cgSkin_t *skin, vec3_t origin, vec3_t angles);
void CG_CheckOrderPending(void);
const char *CG_GameTypeString(void);
qboolean CG_YourTeamHasFlag(void);
qboolean CG_OtherTeamHasFlag(void);
qhandle_t CG_StatusHandle(int task);
// Tobias DEBUG
qhandle_t CG_ObstacleHandle(int task);
// Tobias END
// cg_particles.c
void CG_ClearParticles(void);
void CG_AddParticles(void);
void CG_ParticleSnow(qhandle_t pshader, vec3_t origin, vec3_t origin2, int turb, float range, int snum);
void CG_ParticleSmoke(qhandle_t pshader, centity_t *cent);
void CG_AddParticleShrapnel(localEntity_t *le);
void CG_ParticleSnowFlurry(qhandle_t pshader, centity_t *cent);
void CG_ParticleBulletDebris(vec3_t org, vec3_t vel, int duration);
void CG_ParticleSparks(vec3_t org, vec3_t vel, int duration, float x, float y, float speed);
void CG_ParticleDust(centity_t *cent, vec3_t origin, vec3_t dir);
void CG_ParticleMisc(qhandle_t pshader, vec3_t origin, int size, int duration, float alpha);
void CG_ParticleExplosion(char *animStr, vec3_t origin, vec3_t vel, int duration, int sizeStart, int sizeEnd);
extern qboolean initparticles;
int CG_NewParticleArea(int num);
// cg_player.c
void CG_Player(centity_t *cent);
void CG_ResetPlayerEntity(centity_t *cent);
void CG_AddRefEntityWithPowerups(refEntity_t *ent, entityState_t *state);
qhandle_t CG_AddSkinToFrame(const cgSkin_t *skin, entityState_t *state);
qboolean CG_RegisterSkin(const char *name, cgSkin_t *skin, qboolean append);
void CG_NewClientInfo(int clientNum);
sfxHandle_t CG_CustomSound(int clientNum, const char *soundName);
void CG_CachePlayerSounds(const char *modelName);
void CG_CachePlayerModels(const char *modelName, const char *headModelName);
// cg_predict.c
void CG_BuildSolidList(void);
int CG_PointContents(const vec3_t point, int passEntityNum);
void CG_Trace(trace_t *result, const vec3_t start, const vec3_t mins, const vec3_t maxs, const vec3_t end, int skipNumber, int mask);
void CG_PredictPlayerState(void);
void CG_LoadDeferredPlayers(void);
// cg_events.c
void CG_CheckEvents(centity_t *cent);
const char *CG_PlaceString(int rank);
void CG_EntityEvent(centity_t *cent, vec3_t position);
void CG_PainEvent(centity_t *cent, int health);
// cg_ents.c
void CG_SetEntitySoundPosition(centity_t *cent);
void CG_AddPacketEntities(void);
void CG_Beam(centity_t *cent);
void CG_AdjustPositionForMover(const vec3_t in, int moverNum, int fromTime, int toTime, vec3_t out, vec3_t angles_in, vec3_t angles_out);
qboolean CG_PositionEntityOnTag(refEntity_t *entity, const refEntity_t *parent, qhandle_t parentModel, char *tagName);
qboolean CG_PositionRotatedEntityOnTag(refEntity_t *entity, const refEntity_t *parent, qhandle_t parentModel, char *tagName);
// cg_weapons.c
void CG_NextWeapon_f(void);
void CG_PrevWeapon_f(void);
void CG_Weapon_f(void);
void CG_RegisterWeapon(int weaponNum);
void CG_RegisterItemVisuals(int itemNum);
void CG_FireWeapon(centity_t *cent);
void CG_MissileHitWall(int weapon, int clientNum, vec3_t origin, vec3_t dir);
void CG_MissileHitPlayer(int weapon, vec3_t origin, vec3_t dir, int entityNum);
void CG_ShotgunFire(entityState_t *es);
void CG_Bullet(vec3_t origin, int sourceEntityNum, vec3_t normal, qboolean flesh, int fleshEntityNum);
void CG_RailTrail(clientInfo_t *ci, vec3_t start, vec3_t end);
void CG_AddViewWeapon(playerState_t *ps);
void CG_AddPlayerWeapon(refEntity_t *parent, playerState_t *ps, centity_t *cent, int team);
void CG_DrawWeaponSelect(void);
void CG_OutOfAmmoChange(void); // should this be in pmove?
// cg_marks.c
void CG_InitMarkPolys(void);
void CG_AddMarks(void);
void CG_ImpactMark(qhandle_t markShader, const vec3_t origin, const vec3_t dir, float orientation, float r, float g, float b, float a, qboolean alphaFade, float markRadius, qboolean temporary);
// cg_localents.c
void CG_InitLocalEntities(void);
localEntity_t *CG_AllocLocalEntity(void);
void CG_AddLocalEntities(void);
// cg_effects.c
localEntity_t *CG_SmokePuff(const vec3_t p, const vec3_t vel, float radius, float r, float g, float b, float a, float duration, int startTime, int fadeInTime, int leFlags, qhandle_t hShader);
void CG_BubbleTrail(vec3_t start, vec3_t end, float spacing);
int CG_SpawnBubbles(localEntity_t **bubbles, vec3_t origin, float baseSize, int numBubbles);
void CG_SpawnEffectDefault(vec3_t org);
void CG_SpawnEffectSmall(vec3_t org);
void CG_KamikazeEffect(vec3_t org);
void CG_ObeliskExplode(vec3_t org, int entityNum);
void CG_ObeliskPain(vec3_t org);
void CG_LightningBolt(vec3_t start, vec3_t end);
void CG_ScorePlum(int client, vec3_t org, int score);
void CG_GibPlayer(vec3_t playerOrigin);
void CG_BigExplode(vec3_t playerOrigin);
void CG_Bleed(vec3_t origin, int entityNum);
localEntity_t *CG_MakeExplosion(vec3_t origin, vec3_t dir, qhandle_t hModel, qhandle_t shader, int msec, qboolean isSprite);
// cg_snapshot.c
void CG_ProcessSnapshots(void);
// cg_spawn.c
qboolean CG_SpawnString(const char *key, const char *defaultString, char **out);
// spawn string returns a temporary reference, you must CopyString() if you want to keep it
qboolean CG_SpawnFloat(const char *key, const char *defaultString, float *out);
qboolean CG_SpawnInt(const char *key, const char *defaultString, int *out);
qboolean CG_SpawnVector(const char *key, const char *defaultString, float *out);
void CG_ParseEntitiesFromString(void);
// cg_info.c
void CG_LoadingString(const char *s);
void CG_LoadingItem(int itemNum);
void CG_LoadingClient(int clientNum);
void CG_DrawInformation(void);
// cg_consolecmds.c
qboolean CG_ConsoleCommand(void);
void CG_InitConsoleCommands(void);
// cg_servercmds.c
void CG_ExecuteNewServerCommands(int latestSequence);
void CG_ParseServerinfo(void);
void CG_SetConfigValues(void);
void CG_ShaderStateChanged(void);
void CG_LoadVoiceChats(void);
void CG_VoiceChatLocal(int mode, qboolean voiceOnly, int clientNum, int color, const char *cmd);
void CG_PlayBufferedVoiceChats(void);
// cg_playerstate.c
void CG_Respawn(void);
void CG_TransitionPlayerState(playerState_t *ps, playerState_t *ops);
void CG_CheckChangedPredictableEvents(playerState_t *ps);
// cg_atmospheric.c
void CG_EffectParse(const char *effectstr);
void CG_AddAtmosphericEffects(void);
// cg_polybus.c
polyBuffer_t *CG_PB_FindFreePolyBuffer(qhandle_t shader, int numVerts, int numIndicies);
void CG_PB_ClearPolyBuffers(void);
void CG_PB_RenderPolyBuffers(void);

/*
=======================================================================================================================================

	SYSTEM TRAPS

	These functions are how the cgame communicates with the main game system.

=======================================================================================================================================
*/

// the glconfig_t will not change during the life of a cgame. If it needs to change, the entire cgame will be restarted, because all the qhandle_t are then invalid
void trap_GetGlconfig(glconfig_t *glconfig);
// force a screen update, only used during gamestate load
void trap_UpdateScreen(void);
int trap_MemoryRemaining(void);
// the gamestate should be grabbed at startup, and whenever a configstring changes
void trap_GetGameState(gameState_t *gamestate);
// cgame will poll each frame to see if a newer snapshot has arrived that it is interested in. The time is returned separately so that snapshot latency can be calculated
void trap_GetCurrentSnapshotNumber(int *snapshotNumber, int *serverTime);
// a snapshot get can fail if the snapshot (or the entties it holds) is so old that it has fallen out of the client system queue
qboolean trap_GetSnapshot(int snapshotNumber, snapshot_t *snapshot);
// retrieve a text command from the server stream the current snapshot will hold the number of the most recent command qfalse can be returned if the client system handled the command
// argc()/argv()can be used to examine the parameters of the command
qboolean trap_GetServerCommand(int serverCommandNumber);
// returns the most recent command number that can be passed to GetUserCmd
// this will always be at least one higher than the number in the current snapshot, and it may be quite a few higher if it is a fast computer on a lagged connection
int trap_GetCurrentCmdNumber(void);
qboolean trap_GetUserCmd(int cmdNumber, usercmd_t *ucmd);
// used for the weapon select and zoom
void trap_SetUserCmdValue(int stateValue, float sensitivityScale);
// send a string to the server over the network
void trap_SendClientCommand(const char *s);
// model collision
void trap_CM_LoadMap(const char *mapname);
int trap_CM_NumInlineModels(void);
clipHandle_t trap_CM_InlineModel(int index); // 0 = world, 1+ = bmodels
clipHandle_t trap_CM_TempBoxModel(const vec3_t mins, const vec3_t maxs);
int trap_CM_PointContents(const vec3_t p, clipHandle_t model);
int trap_CM_TransformedPointContents(const vec3_t p, clipHandle_t model, const vec3_t origin, const vec3_t angles);
void trap_CM_BoxTrace(trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask);
void trap_CM_CapsuleTrace(trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask);
void trap_CM_TransformedBoxTrace(trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles);
void trap_CM_TransformedCapsuleTrace(trace_t *results, const vec3_t start, const vec3_t end, const vec3_t mins, const vec3_t maxs, clipHandle_t model, int brushmask, const vec3_t origin, const vec3_t angles);
// returns the projection of a polygon onto the solid brushes in the world
int trap_CM_MarkFragments(int numPoints, const vec3_t *points, const vec3_t projection, int maxPoints, vec3_t pointBuffer, int maxFragments, markFragment_t *fragmentBuffer);
void trap_R_LoadWorldMap(const char *mapname);
qboolean trap_GetEntityToken(char *buffer, int bufferSize);
// all media should be registered during level startup to prevent hitches during gameplay
qhandle_t trap_R_RegisterModel(const char *name); // returns rgb axis if not found
qhandle_t trap_R_RegisterShaderEx(const char *name, int lightmapIndex, qboolean mipRawImage); // returns all white if not found
qhandle_t trap_R_RegisterShader(const char *name); // returns all white if not found
qhandle_t trap_R_RegisterShaderNoMip(const char *name); // returns all white if not found
// a scene is built up by calls to R_ClearScene and the various R_Add functions.
// nothing is drawn until R_RenderScene is called
void trap_R_RenderScene(const refdef_t *fd);
void trap_R_ClearScene(void);
void trap_R_SetColor(const float *rgba); // NULL = 1, 1, 1, 1
void trap_R_AddRefEntityToScene(const refEntity_t *re);
// polys are intended for simple wall marks, not really for doing significant construction
void trap_R_AddPolyToScene(qhandle_t hShader, int numVerts, const polyVert_t *verts, int bmodelNum, int sortLevel);
void trap_R_AddPolysToScene(qhandle_t hShader, int numVerts, const polyVert_t *verts, int numPolys, int bmodelNum, int sortLevel);
void trap_R_AddPolyBufferToScene(polyBuffer_t *pPolyBuffer);
int trap_R_LightForPoint(vec3_t point, vec3_t ambientLight, vec3_t directedLight, vec3_t lightDir);
void trap_R_AddLightToScene(const vec3_t org, float radius, float intensity, float r, float g, float b, qhandle_t hShader);
void trap_R_AddAdditiveLightToScene(const vec3_t org, float radius, float intensity, float r, float g, float b);
void trap_R_AddVertexLightToScene(const vec3_t org, float radius, float intensity, float r, float g, float b);
void trap_R_AddJuniorLightToScene(const vec3_t org, float radius, float intensity, float r, float g, float b);
void trap_R_AddDirectedLightToScene(const vec3_t normal, float intensity, float r, float g, float b);
void trap_R_AddCoronaToScene(const vec3_t org, float r, float g, float b, float scale, int id, qboolean visible, qhandle_t hShader);
void trap_R_SetClipRegion(const float *region);
void trap_R_DrawStretchPic(float x, float y, float w, float h, float s1, float t1, float s2, float t2, qhandle_t hShader);
int trap_R_ModelBounds(clipHandle_t handle, vec3_t mins, vec3_t maxs, int startFrame, int endFrame, float frac);
int trap_R_LerpTag(orientation_t *tag, clipHandle_t mod, int startFrame, int endFrame, float frac, const char *tagName);
int trap_R_LerpTagFrameModel(orientation_t *tag, clipHandle_t mod, clipHandle_t frameModel, int startFrame, clipHandle_t endFrameModel, int endFrame, float frac, const char *tagName, int *tagIndex);
int trap_R_LerpTagTorso(orientation_t *tag, clipHandle_t mod, clipHandle_t frameModel, int startFrame, clipHandle_t endFrameModel, int endFrame, float frac, const char *tagName, int *tagIndex, const vec3_t *torsoAxis, qhandle_t torsoFrameModel, int torsoFrame, qhandle_t oldTorsoFrameModel, int oldTorsoFrame, float torsoFrac);
// skin (entity model surface remap) management
qhandle_t trap_R_AllocSkinSurface(const char *surface, qhandle_t hShader);
qhandle_t trap_R_AddSkinToFrame(int numSurfaces, const qhandle_t *surfaces);
void trap_R_RemapShader(const char *oldShader, const char *newShader, const char *timeOffset);
qboolean trap_R_inPVS(const vec3_t p1, const vec3_t p2);
// normal sounds will have their volume dynamically changed as their entity moves and the listener moves
void trap_S_StartSound(vec3_t origin, int entityNum, int entchannel, sfxHandle_t sfx, int range);
// a local sound is always played full volume
void trap_S_StartLocalSound(sfxHandle_t sfx, int channelNum);
void trap_S_StopLoopingSound(int entnum);
void trap_S_ClearLoopingSounds(qboolean killall);
void trap_S_AddLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx, int range);
void trap_S_AddRealLoopingSound(int entityNum, const vec3_t origin, const vec3_t velocity, sfxHandle_t sfx, int range);
void trap_S_UpdateEntityPosition(int entityNum, const vec3_t origin);
// respatialize recalculates the volumes of sound as they should be heard by the given entityNum and position
void trap_S_Respatialize(int entityNum, const vec3_t origin, vec3_t axis[3], int inwater);
sfxHandle_t trap_S_RegisterSound(const char *sample, qboolean compressed); // returns buzz if not found
void trap_S_StartBackgroundTrack(const char *intro, const char *loop); // empty name stops music
void trap_S_StopBackgroundTrack(void);
void trap_R_GetGlobalFog(fogType_t *type, vec3_t color, float *depthForOpaque, float *density, float *farClip);
void trap_R_GetViewFog(const vec3_t origin, fogType_t *type, vec3_t color, float *depthForOpaque, float *density, float *farClip, qboolean inwater);
void trap_R_GetViewFog(const vec3_t origin, fogType_t *type, vec3_t color, float *depthForOpaque, float *density, float *farClip, qboolean inwater);
qboolean trap_Key_IsDown(int keynum);
int trap_Key_GetCatcher(void);
void trap_Key_SetCatcher(int catcher);
int trap_Key_GetKey(const char *binding);
// this returns a handle. arg0 is the name in the format "idlogo.roq", set arg1 to NULL, altered states to qfalse (do not alter gamestate)
int trap_CIN_PlayCinematic(const char *arg0, int xpos, int ypos, int width, int height, int bits);
// stops playing the cinematic and ends it. should always return FMV_EOF
// cinematics must be stopped in reverse order of when they are started
e_status trap_CIN_StopCinematic(int handle);
// will run a frame of the cinematic but will not draw it. Will return FMV_EOF if the end of the cinematic has been reached
e_status trap_CIN_RunCinematic(int handle);
// draws the current frame
void trap_CIN_DrawCinematic(int handle);
// allows you to resize the animation dynamically
void trap_CIN_SetExtents(int handle, int x, int y, int w, int h);
qboolean trap_loadCamera(const char *name);
void trap_startCamera(int time);
qboolean trap_getCameraInfo(int time, vec3_t *origin, vec3_t *angles);
