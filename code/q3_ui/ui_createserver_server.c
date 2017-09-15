/*
=======================================================================================================================================
The work contained within this file is software written by various copyright holders. The initial contributor, Id Software holds all
copyright over their software. However, software used and written by and for UI Enhanced has copyrights held by the initial author of
the software.

The changes written by and for UI Enhanced are contained alongside the original work from Id Software for convenience and ease of
interoperability.

For the code contained herein that was written by Id Software, see the license agreement on their original archive for restrictions and
limitations.

The UI Enhanced copyright owner permit free reuse of his code contained herein, as long as the following terms are met:
---------------------------------------------------------------------------------------------------------------------------------------
1) Credit is given in a place where users of the mod may read it. (Title screen, credit screen or README will do). The recommended
   format is: "First, Last, alias, email"

2) There are no attempts to misrepresent the public as to who made the alterations. The UI Enhanced copyright owner does not give
   permission for others to release software under the UI Enhanced name.
---------------------------------------------------------------------------------------------------------------------------------------
Ian Jefferies - HypoThermia (uie@planetquake.com)
http://www.planetquake.com/uie
=======================================================================================================================================
*/

/*
=======================================================================================================================================

	CREATE SERVER MENU

=======================================================================================================================================
*/

#include "ui_local.h"
#include "ui_startserver_q3.h"

enum {
	ID_SERVER_GAMETYPE,
	ID_SERVER_HOSTNAME,
	ID_SERVER_AUTOJOIN,
	ID_SERVER_TEAMBALANCE,
	ID_SERVER_PURE,
	ID_SERVER_INACTIVITY,
	ID_SERVER_SAVE,
	ID_SERVER_LOAD,
	ID_SERVER_SMOOTHCLIENTS,
	ID_SERVER_MAXRATE,
	ID_SERVER_ALLOWDOWNLOAD,
	ID_SERVER_PASSWORD,
	ID_SERVER_ALLOWPASS,
	ID_SERVER_ALLOWVOTE,
	ID_SERVER_ALLOWMAXRATE,
	ID_SERVER_SYNCCLIENTS,
	ID_SERVER_MINPING,
	ID_SERVER_MAXPING,
	ID_SERVER_ALLOWMINPING,
	ID_SERVER_ALLOWMAXPING,
	ID_SERVER_CONFIGBUG
};

#define SERVER_SAVE0 "menu/art/save_0"
#define SERVER_SAVE1 "menu/art/save_1"
#define SERVER_LOAD0 "menu/art/load_0"
#define SERVER_LOAD1 "menu/art/load_1"

#define SERVERCOLUMN_X GAMETYPECOLUMN_X

enum {
	SRVCTRL_SPIN,
	SRVCTRL_RADIO,
	SRVCTRL_TEXTFIELD,
	SRVCTRL_NUMFIELD,
	SRVCTRL_BLANK
};

enum {
	SCRPOS_LEFT,
	SCRPOS_MIDDLE,
	SCRPOS_RIGHT,
	SCRPOS_LEFTOFFSET,
	SCRPOS_MIDDLEOFFSET,
	SCRPOS_RIGHTOFFSET,
	SCRPOS_COUNT
};
// list of created controls on page
typedef struct controlinit_s {
	int type;		// SRVCTRL_*
	int id;			// control id ID_SERVER_*
	int screenpos;	// SCRPOS_*
	char *title;	// text describing control
	// used by RADIO, NUMFIELD, or SPIN
	// min, max ignored if identical
	int *number;
	// used by NUMFIELD
	int min;		// min value of number
	int max;		// max value of number
	// used by TEXTFIELD
	char *array;
	// used by TEXTFIELD and NUMFIELD
	int displaysize;
	int arraysize;
	// used by SPIN
	const char **itemnames;
} controlinit_t;

typedef struct radiobuttoncontrol_s {
	menuradiobutton_s control;
	controlinit_t *init;
} radiobuttoncontrol_t;

typedef struct textfieldcontrol_s {
	menufield_s control;
	controlinit_t *init;
} textfieldcontrol_t;

typedef struct spincontrol_s {
	menulist_s control;
	controlinit_t *init;
} spincontrol_t;

#define MAX_SERVER_RADIO_CONTROL 24
#define MAX_SERVER_MFIELD_CONTROL 12
#define MAX_SERVER_SPIN_CONTROL 12

typedef struct servercontrols_s {
	menuframework_s menu;
	commoncontrols_t common;
	menulist_s gameType;
	menubitmap_s gameTypeIcon;
	radiobuttoncontrol_t radio[MAX_SERVER_RADIO_CONTROL];
	textfieldcontrol_t field[MAX_SERVER_MFIELD_CONTROL];
	spincontrol_t spin[MAX_SERVER_SPIN_CONTROL];
	int num_radio;
	int num_field;
	int num_spin;
	int control_height[SCRPOS_COUNT];
	menubitmap_s saveScript;
	menubitmap_s loadScript;
	int statusbar_height;
	int savetime;
	char statusbar_message[MAX_STATUSBAR_TEXT];
} servercontrols_t;

char temporal[50];

servercontrols_t s_servercontrols;

static controlinit_t InitControls[] = {
	{SRVCTRL_TEXTFIELD, ID_SERVER_HOSTNAME, SCRPOS_MIDDLE, "Hostname:", NULL, 0, 0, s_scriptdata.server.hostname, 32, MAX_HOSTNAME_LENGTH, NULL},
	// controls on left side of page
	{SRVCTRL_BLANK, 0, SCRPOS_LEFT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_LEFT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_LEFT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_RADIO, ID_SERVER_ALLOWVOTE, SCRPOS_LEFT, "Allow voting:", &s_scriptdata.server.allowvote, 0, 1, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_LEFT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_RADIO, ID_SERVER_ALLOWPASS, SCRPOS_LEFT, "Require password:", &s_scriptdata.server.allowpass, 0, 1, NULL, 0, 0, NULL},
	{SRVCTRL_TEXTFIELD, ID_SERVER_PASSWORD, SCRPOS_LEFTOFFSET, "password:", NULL, 0, 0, s_scriptdata.server.password, 10, MAX_PASSWORD_LENGTH, NULL},
	{SRVCTRL_RADIO, ID_SERVER_ALLOWMAXRATE, SCRPOS_LEFT, "Server maxrate:", &s_scriptdata.server.allowmaxrate, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_NUMFIELD, ID_SERVER_MAXRATE, SCRPOS_LEFTOFFSET, "bytes/s:", &s_scriptdata.server.maxrate, 0, 0, NULL, 6, 6, NULL},
	{SRVCTRL_RADIO, ID_SERVER_ALLOWDOWNLOAD, SCRPOS_LEFT, "Allow download:", &s_scriptdata.server.allowdownload, 0, 1, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_LEFT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_RADIO, ID_SERVER_ALLOWMINPING, SCRPOS_LEFT, "Minimum ping:", &s_scriptdata.server.allowMinPing, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_NUMFIELD, ID_SERVER_MINPING, SCRPOS_LEFTOFFSET, "ping:", &s_scriptdata.server.minPing, 0, 999, NULL, 4, 4, NULL},
	{SRVCTRL_RADIO, ID_SERVER_ALLOWMAXPING, SCRPOS_LEFT, "Maximum ping:", &s_scriptdata.server.allowMaxPing, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_NUMFIELD, ID_SERVER_MAXPING, SCRPOS_LEFTOFFSET, "ping:", &s_scriptdata.server.maxPing, 0, 999, NULL, 4, 4, NULL},
	// controls on right side of page
	{SRVCTRL_BLANK, 0, SCRPOS_RIGHT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_RIGHT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_RADIO, ID_SERVER_PURE, SCRPOS_RIGHT, "Pure server:", &s_scriptdata.server.pureServer, 0, 1, NULL, 0, 0, NULL},
	{SRVCTRL_RADIO, ID_SERVER_SMOOTHCLIENTS, SCRPOS_RIGHT, "Smooth clients:", &s_scriptdata.server.smoothclients, 0, 1, NULL, 0, 0, NULL},
	{SRVCTRL_RADIO, ID_SERVER_SYNCCLIENTS, SCRPOS_RIGHT, "Sync clients:", &s_scriptdata.server.syncClients, 0, 1, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_RIGHT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_RIGHT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_RADIO, ID_SERVER_CONFIGBUG, SCRPOS_RIGHT, "Fix config bug:", &s_scriptdata.server.preventConfigBug, 0, 1, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_RIGHT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_RADIO, ID_SERVER_AUTOJOIN, SCRPOS_RIGHT, "Team auto join:", &s_scriptdata.server.autoJoin, 0, 1, NULL, 0, 0, NULL},
	{SRVCTRL_RADIO, ID_SERVER_TEAMBALANCE, SCRPOS_RIGHT, "Team balance:", &s_scriptdata.server.teamBalance, 0, 1, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_LEFT, NULL, NULL, 0, 0, NULL, 0, 0, NULL},
	{SRVCTRL_BLANK, 0, SCRPOS_LEFT, NULL, NULL, 0, 0, NULL, 0, 0, NULL}
};

static int InitControls_Size = sizeof(InitControls) / sizeof(InitControls[0]);
// additional controls shown in multiplayer version of this page
static controlinit_t InitMultiControls[] = {
	{SRVCTRL_NUMFIELD, ID_SERVER_INACTIVITY, SCRPOS_MIDDLE, "Inactivity timeout:", &s_scriptdata.server.inactivityTime, 0, 999, NULL, 3, 3, NULL}
};

static int InitMultiControls_Size = sizeof(InitMultiControls) / sizeof(InitMultiControls[0]);

/*
=======================================================================================================================================
CreateServer_ServerPage_SetControlState
=======================================================================================================================================
*/
static void CreateServer_ServerPage_SetControlState(menucommon_s *c, int type) {

	// set its appearance
	switch (type) {
		default:
			c->flags &= ~(QMF_HIDDEN|QMF_INACTIVE|QMF_GRAYED);
			break;
		case QMF_HIDDEN:
			c->flags &= ~(QMF_GRAYED);
			c->flags |= (QMF_HIDDEN|QMF_INACTIVE);
			break;
		case QMF_GRAYED:
			c->flags &= ~(QMF_HIDDEN|QMF_INACTIVE);
			c->flags |= (QMF_GRAYED);
	}
}

/*
=======================================================================================================================================
CreateServer_ServerPage_SetControlFromId
=======================================================================================================================================
*/
static void CreateServer_ServerPage_SetControlFromId(int id, int type) {
	menucommon_s *c;
	int i;

	// locate control
	c = NULL;

	for (i = 0; i < s_servercontrols.num_radio; i++) {
		if (s_servercontrols.radio[i].init->id == id) {
			c = &s_servercontrols.radio[i].control.generic;
			break;
		}
	}

	for (i = s_servercontrols.num_field - 1; i >= 0 && !c; i--) {
		if (s_servercontrols.field[i].init->id == id) {
			c = &s_servercontrols.field[i].control.generic;
		}
	}

	for (i = s_servercontrols.num_spin - 1; i >= 0 && !c; i--) {
		if (s_servercontrols.spin[i].init->id == id) {
			c = &s_servercontrols.spin[i].control.generic;
		}
	}

	if (!c) {
		return;
	}

	CreateServer_ServerPage_SetControlState(c, type);
}

/*
=======================================================================================================================================
CreateServer_ServerPage_UpdateInterface
=======================================================================================================================================
*/
static void CreateServer_ServerPage_UpdateInterface(void) {
	int flag;

	flag = 0;

	if (s_scriptdata.gametype < GT_TEAM) {
		flag = QMF_HIDDEN;
	}

	CreateServer_ServerPage_SetControlFromId(ID_SERVER_AUTOJOIN, flag);
	CreateServer_ServerPage_SetControlFromId(ID_SERVER_TEAMBALANCE, flag);

	CreateServer_SetIconFromGameType(&s_servercontrols.gameTypeIcon, s_scriptdata.gametype, qfalse);

	if (s_scriptdata.server.allowmaxrate) {
		CreateServer_ServerPage_SetControlFromId(ID_SERVER_MAXRATE, 0);
	} else {
		CreateServer_ServerPage_SetControlFromId(ID_SERVER_MAXRATE, QMF_HIDDEN);
	}

	if (s_scriptdata.server.allowpass) {
		CreateServer_ServerPage_SetControlFromId(ID_SERVER_PASSWORD, 0);
	} else {
		CreateServer_ServerPage_SetControlFromId(ID_SERVER_PASSWORD, QMF_HIDDEN);
	}

	if (s_scriptdata.server.allowMinPing) {
		CreateServer_ServerPage_SetControlFromId(ID_SERVER_MINPING, 0);
	} else {
		CreateServer_ServerPage_SetControlFromId(ID_SERVER_MINPING, QMF_HIDDEN);
	}

	if (s_scriptdata.server.allowMaxPing) {
		CreateServer_ServerPage_SetControlFromId(ID_SERVER_MAXPING, 0);
	} else {
		CreateServer_ServerPage_SetControlFromId(ID_SERVER_MAXPING, QMF_HIDDEN);
	}
	// enable fight button if possible
	CreateServer_CheckFightReady(&s_servercontrols.common);
}

/*
=======================================================================================================================================
CreateServer_ServerPage_Cache
=======================================================================================================================================
*/
void CreateServer_ServerPage_Cache(void) {

	trap_R_RegisterShaderNoMip(SERVER_SAVE0);
	trap_R_RegisterShaderNoMip(SERVER_SAVE1);
}

/*
=======================================================================================================================================
CreateServer_ServerPage_Save
=======================================================================================================================================
*/
static void CreateServer_ServerPage_Save(void) {
	CreateServer_SaveScriptData();
}

/*
=======================================================================================================================================
CreateServer_ServerPage_InitControlsFromScript
=======================================================================================================================================
*/
static void CreateServer_ServerPage_InitControlsFromScript(void) {
	int i;
	radiobuttoncontrol_t *r;
	textfieldcontrol_t *t;

	// set radio controls
	for (i = 0; i < s_servercontrols.num_radio; i++) {
		r = &s_servercontrols.radio[i];
		r->control.curvalue = *(r->init->number);
	}
	// set text field controls
	for (i = 0; i < s_servercontrols.num_field; i++) {
		t = &s_servercontrols.field[i];

		if (t->init->type == SRVCTRL_NUMFIELD) {
			Q_strncpyz(t->control.field.buffer, va("%i", *t->init->number), t->init->arraysize);
		} else if (t->init->type == SRVCTRL_TEXTFIELD) {
			Q_strncpyz(t->control.field.buffer, t->init->array, t->init->arraysize);
		}

		t->control.field.cursor = 0;
		t->control.field.scroll = 0;
	}
}

/*
=======================================================================================================================================
CreateServer_ServerPage_Load
=======================================================================================================================================
*/
static void CreateServer_ServerPage_Load(void) {

	s_servercontrols.gameType.curvalue = gametype_remap2[s_scriptdata.gametype];

	CreateServer_ServerPage_InitControlsFromScript();
}

/*
=======================================================================================================================================
CreateServer_ServerPage_CommonEvent
=======================================================================================================================================
*/
static void CreateServer_ServerPage_CommonEvent(void *ptr, int event) {

	if (event != QM_ACTIVATED) {
		return;
	}

	CreateServer_ServerPage_Save();

	switch (((menucommon_s *)ptr)->id) {
		case ID_SERVERCOMMON_BOTS:
			UI_PopMenu();
			break;
		case ID_SERVERCOMMON_MAPS:
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_SERVERCOMMON_BACK:
			UI_PopMenu();
			UI_PopMenu();
			UI_PopMenu();
			break;
		case ID_SERVERCOMMON_FIGHT:
			CreateServer_ServerScript(NULL);
			break;
	}
}

/*
=======================================================================================================================================
CreateServer_ServerPage_DrawStatusBarText
=======================================================================================================================================
*/
static void CreateServer_ServerPage_DrawStatusBarText(void) {
	vec4_t color;
	int fade;
	float fadecol;

	if (uis.realtime > s_servercontrols.savetime) {
		return;
	}

	if (!s_servercontrols.statusbar_message[0]) {
		return;
	}

	fade = s_servercontrols.savetime - uis.realtime;
	fadecol = (float)fade / STATUSBAR_FADETIME;

	color[0] = 1.0;
	color[1] = 1.0;
	color[2] = 1.0;
	color[3] = fadecol;

	UI_DrawString(320, s_servercontrols.statusbar_height, s_servercontrols.statusbar_message, UI_CENTER|UI_SMALLFONT, color);
}

/*
=======================================================================================================================================
CreateServer_ServerPage_MenuKey
=======================================================================================================================================
*/
static sfxHandle_t CreateServer_ServerPage_MenuKey(int key) {

	switch (key) {
		case K_MOUSE2:
		case K_ESCAPE:
			CreateServer_ServerPage_Save();
			UI_PopMenu();
			UI_PopMenu();
			UI_PopMenu();
			break;
	}

	return (Menu_DefaultKey(&s_servercontrols.menu, key));
}

/*
=======================================================================================================================================
CreateServer_ServerPage_SetStatusBar
=======================================================================================================================================
*/
static void CreateServer_ServerPage_SetStatusBar(const char *text) {

	s_servercontrols.savetime = uis.realtime + STATUSBAR_FADETIME;

	if (text) {
		Q_strncpyz(s_servercontrols.statusbar_message, text, MAX_STATUSBAR_TEXT);
	}
}

/*
=======================================================================================================================================
CreateServer_ServerPage_Event
=======================================================================================================================================
*/
static void CreateServer_ServerPage_Event(void *ptr, int event) {

	switch (((menucommon_s *)ptr)->id) {
		case ID_SERVER_GAMETYPE:
			if (event != QM_ACTIVATED) {
				return;
			}

			CreateServer_SaveScriptData();
			CreateServer_LoadScriptDataFromType(gametype_remap[s_servercontrols.gameType.curvalue]);
			CreateServer_ServerPage_InitControlsFromScript();
			CreateServer_ServerPage_UpdateInterface();
			break;
		/*
		case ID_SERVER_SAVE:
			if (event != QM_ACTIVATED) {
				return;
			}

			CreateServer_ServerPage_Save();
			UI_ConfigMenu("SAVE SCRIPT", qfalse, CreateServer_ServerPage_SaveScript);
			break;
		case ID_SERVER_LOAD:
			if (event != QM_ACTIVATED) {
				return;
			}

			CreateServer_ServerPage_Save();
			UI_ConfigMenu("LOAD SCRIPT", qtrue, CreateServer_ServerPage_LoadScript);
			break;
		*/
	}
}

/*
=======================================================================================================================================
CreateServer_ServerPage_CheckMinMaxFail
=======================================================================================================================================
*/
static qboolean CreateServer_ServerPage_CheckMinMaxFail(controlinit_t *c) {
	int value;

	if (c->min == c->max) {
		return qfalse;
	}

	value = *(c->number);

	if (value < c->min) {
		*(c->number) = c->min;
		return qtrue;
	}

	if (value > c->max) {
		*(c->number) = c->max;
		return qtrue;
	}

	return qfalse;
}

/*
=======================================================================================================================================
CreateServer_ServerPage_SpinEvent
=======================================================================================================================================
*/
static void CreateServer_ServerPage_SpinEvent(void *ptr, int event) {
	int id;
	spincontrol_t *s;

	if (event != QM_ACTIVATED) {
		return;
	}

	id = ((menucommon_s *)ptr)->id;
	s = &s_servercontrols.spin[id];
	*(s->init->number) = s_servercontrols.spin[id].control.curvalue;

	CreateServer_ServerPage_UpdateInterface();
}

/*
=======================================================================================================================================
CreateServer_ServerPage_RadioEvent
=======================================================================================================================================
*/
static void CreateServer_ServerPage_RadioEvent(void *ptr, int event) {
	int id;
	radiobuttoncontrol_t *r;

	if (event != QM_ACTIVATED) {
		return;
	}

	id = ((menucommon_s *)ptr)->id;
	r = &s_servercontrols.radio[id];
	*(r->init->number) = s_servercontrols.radio[id].control.curvalue;

	CreateServer_ServerPage_UpdateInterface();
}

/*
=======================================================================================================================================
CreateServer_ServerPage_FieldEvent
=======================================================================================================================================
*/
static void CreateServer_ServerPage_FieldEvent(void *ptr, int event) {
	int id;
	textfieldcontrol_t *r;
	controlinit_t *c;

	if (event != QM_LOSTFOCUS) {
		return;
	}

	id = ((menucommon_s *)ptr)->id;
	r = &s_servercontrols.field[id];
	c = r->init;

	if (c->type == SRVCTRL_NUMFIELD) {
		*(c->number) = atoi(r->control.field.buffer);

		if (CreateServer_ServerPage_CheckMinMaxFail(c)) {
			Q_strncpyz(r->control.field.buffer, va("%i", *(c->number)), c->arraysize);
		}
	} else if (c->type == SRVCTRL_TEXTFIELD) {
		Q_strncpyz(c->array, r->control.field.buffer, c->arraysize);
	}

	CreateServer_ServerPage_UpdateInterface();
}

/*
=======================================================================================================================================
CreateServer_ServerPage_MenuDraw
=======================================================================================================================================
*/
static void CreateServer_ServerPage_MenuDraw(void) {

	if (uis.firstdraw) {
		CreateServer_ServerPage_Load();
		CreateServer_ServerPage_UpdateInterface();
	}

	//CreateServer_BackgroundDraw(qfalse);
	// draw the controls
	Menu_Draw(&s_servercontrols.menu);

	CreateServer_ServerPage_DrawStatusBarText();
}

/*
=======================================================================================================================================
CreateServer_ServerPage_StatusBar
=======================================================================================================================================
*/
/*
static void CreateServer_ServerPage_StatusBar(void *ptr) {

	switch (((menucommon_s *)ptr)->id) {
		case ID_SERVER_SAVE:
			CreateServer_ServerPage_SetStatusBar("Create a script for later use with \\exec");
			break;
		case ID_SERVER_LOAD:
			CreateServer_ServerPage_SetStatusBar("Load a previously saved UI script");
			break;
	}
}
*/
/*
=======================================================================================================================================
CreateServer_ServerPage_ControlListStatusBar
=======================================================================================================================================
*/
static void CreateServer_ServerPage_ControlListStatusBar(void *ptr) {
	menucommon_s *m;
	controlinit_t *c;

	m = (menucommon_s *)ptr;

	switch (m->type) {
		case MTYPE_RADIOBUTTON:
			c = s_servercontrols.radio[m->id].init;
			break;
		case MTYPE_SPINCONTROL:
			c = s_servercontrols.spin[m->id].init;
			break;
		case MTYPE_FIELD:
			c = s_servercontrols.field[m->id].init;
			break;
		default:
			Com_Printf("Status bar: unsupported control type (%i)\n", m->id);
			return;
	}

	switch (c->id) {
		case ID_SERVER_CONFIGBUG:
			CreateServer_ServerPage_SetStatusBar("prevents unexpected change of time and frag limits");
			return;
		case ID_SERVER_ALLOWMAXRATE:
			CreateServer_ServerPage_SetStatusBar("max quantity of data sent to a single client");
			return;
		case ID_SERVER_MAXRATE:
			CreateServer_ServerPage_SetStatusBar("8000 or 10000 are typical, 0 = no limit");
			return;
		case ID_SERVER_SMOOTHCLIENTS:
			CreateServer_ServerPage_SetStatusBar("on = server allows a client to predict other player movement");
			return;
		case ID_SERVER_SYNCCLIENTS:
			CreateServer_ServerPage_SetStatusBar("on = allows client to record demos, may affect performance");
			return;
		case ID_SERVER_ALLOWMINPING:
		case ID_SERVER_ALLOWMAXPING:
		case ID_SERVER_MINPING:
		case ID_SERVER_MAXPING:
			CreateServer_ServerPage_SetStatusBar("Client ping limit, tested on first connection");
			return;
	}
}

/*
=======================================================================================================================================
CreateServer_ServerPage_GetControlX
=======================================================================================================================================
*/
static int CreateServer_ServerPage_GetControlX(int type) {

	switch (type) {
		case SCRPOS_LEFT:
			return 160;
		default:
		case SCRPOS_MIDDLE:
			return SERVERCOLUMN_X;
		case SCRPOS_RIGHT:
			return 520;
		case SCRPOS_LEFTOFFSET:
			return 160 + 17 * SMALLCHAR_WIDTH;
		case SCRPOS_MIDDLEOFFSET:
			return SERVERCOLUMN_X + 17 * SMALLCHAR_WIDTH;
		case SCRPOS_RIGHTOFFSET:
			return 520 + 17 * SMALLCHAR_WIDTH;
	}
}

/*
=======================================================================================================================================
CreateServer_ServerPage_GetControlY
=======================================================================================================================================
*/
static int CreateServer_ServerPage_GetControlY(int type) {
	int height, *cy;

	cy = s_servercontrols.control_height;

	switch (type) {
		case SCRPOS_LEFT:
			if (cy[SCRPOS_LEFTOFFSET] > cy[SCRPOS_LEFT]) {
				cy[SCRPOS_LEFT] = cy[SCRPOS_LEFTOFFSET];
			}

			break;
		case SCRPOS_MIDDLE:
			if (cy[SCRPOS_MIDDLEOFFSET] > cy[SCRPOS_MIDDLE]) {
				cy[SCRPOS_MIDDLE] = cy[SCRPOS_MIDDLEOFFSET];
			}

			break;
		case SCRPOS_RIGHT:
			if (cy[SCRPOS_RIGHTOFFSET] > cy[SCRPOS_RIGHT]) {
				cy[SCRPOS_RIGHT] = cy[SCRPOS_RIGHTOFFSET];
			}

			break;
	}

	height = cy[type];

	switch (type) {
		case SCRPOS_LEFT:
			cy[SCRPOS_LEFTOFFSET] = cy[SCRPOS_LEFT];
			break;
		case SCRPOS_MIDDLE:
			cy[SCRPOS_MIDDLEOFFSET] = cy[SCRPOS_MIDDLE];
			break;
		case SCRPOS_RIGHT:
			cy[SCRPOS_RIGHTOFFSET] = cy[SCRPOS_RIGHT];
			break;
	}
	// set position for next use
	cy[type] += LINE_HEIGHT;

	return height;
}

/*
=======================================================================================================================================
CreateServer_ServerPage_InitSpinCtrl
=======================================================================================================================================
*/
static void CreateServer_ServerPage_InitSpinCtrl(controlinit_t *c) {
	spincontrol_t *s;

	if (s_servercontrols.num_spin == MAX_SERVER_SPIN_CONTROL) {
		Com_Printf("ServerPage: Max spin controls (%i) reached\n", s_servercontrols.num_spin);
		return;
	}

	if (!c->number) {
		Com_Printf("ServerPage: Spin control (id:%i) has no data\n", c->id);
		return;
	}

	if (!c->itemnames) {
		Com_Printf("ServerPage: Spin control (id:%i) has no strings\n", c->id);
		return;
	}

	s = &s_servercontrols.spin[s_servercontrols.num_spin];
	s->init = c;
	s->control.generic.type = MTYPE_SPINCONTROL;
	s->control.generic.name = c->title;
	s->control.generic.id = s_servercontrols.num_spin; // self reference
	s->control.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s->control.generic.callback = CreateServer_ServerPage_SpinEvent;
	s->control.generic.statusbar = CreateServer_ServerPage_ControlListStatusBar;
	s->control.generic.x = CreateServer_ServerPage_GetControlX(c->screenpos);
	s->control.generic.y = CreateServer_ServerPage_GetControlY(c->screenpos);
	s->control.itemnames = c->itemnames;
	s->control.curvalue = *(c->number);
	s_servercontrols.num_spin++;
}

/*
=======================================================================================================================================
CreateServer_ServerPage_InitRadioCtrl
=======================================================================================================================================
*/
static void CreateServer_ServerPage_InitRadioCtrl(controlinit_t *c) {
	radiobuttoncontrol_t *r;

	if (s_servercontrols.num_radio == MAX_SERVER_RADIO_CONTROL) {
		Com_Printf("ServerPage: Max radio controls (%i) reached\n", s_servercontrols.num_radio);
		return;
	}

	if (!c->number) {
		Com_Printf("ServerPage: Radio control (id:%i) has no data\n", c->id);
		return;
	}

	r = &s_servercontrols.radio[s_servercontrols.num_radio];
	r->init = c;
	r->control.generic.type = MTYPE_RADIOBUTTON;
	r->control.generic.name = c->title;
	r->control.generic.id = s_servercontrols.num_radio; // self reference
	r->control.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	r->control.generic.callback = CreateServer_ServerPage_RadioEvent;
	r->control.generic.statusbar = CreateServer_ServerPage_ControlListStatusBar;
	r->control.generic.x = CreateServer_ServerPage_GetControlX(c->screenpos);
	r->control.generic.y = CreateServer_ServerPage_GetControlY(c->screenpos);
	r->control.curvalue = *(c->number);

	s_servercontrols.num_radio++;
}

/*
=======================================================================================================================================
CreateServer_ServerPage_InitFieldCtrl
=======================================================================================================================================
*/
static void CreateServer_ServerPage_InitFieldCtrl(controlinit_t *c) {
	textfieldcontrol_t *f;
	int flags;

	if (s_servercontrols.num_field == MAX_SERVER_MFIELD_CONTROL) {
		Com_Printf("ServerPage: Max field controls (%i) reached\n", s_servercontrols.num_field);
		return;
	}

	if (c->type == SRVCTRL_NUMFIELD && !c->number) {
		Com_Printf("ServerPage: NumField control (id:%i) has no data\n", c->id);
		return;
	}

	if (c->type == SRVCTRL_TEXTFIELD && !c->array) {
		Com_Printf("ServerPage: TextField control (id:%i) has no data\n", c->id);
		return;
	}

	f = &s_servercontrols.field[s_servercontrols.num_field];
	f->init = c;
	flags = QMF_SMALLFONT|QMF_PULSEIFFOCUS;

	if (c->type == SRVCTRL_NUMFIELD) {
		flags |= QMF_NUMBERSONLY;
	}

	f->control.generic.type = MTYPE_FIELD;
	f->control.generic.name = c->title;
	f->control.generic.id = s_servercontrols.num_field; // self reference
	f->control.generic.callback = CreateServer_ServerPage_FieldEvent;
	f->control.generic.statusbar = CreateServer_ServerPage_ControlListStatusBar;
	f->control.generic.flags = flags;
	f->control.generic.x = CreateServer_ServerPage_GetControlX(c->screenpos);
	f->control.generic.y = CreateServer_ServerPage_GetControlY(c->screenpos);
	f->control.field.widthInChars = c->displaysize;
	f->control.field.maxchars = c->arraysize;

	if (c->type == SRVCTRL_NUMFIELD) {
		Q_strncpyz(f->control.field.buffer, va("%i", *c->number), c->arraysize);
	} else if (c->type == SRVCTRL_TEXTFIELD) {
		Q_strncpyz(f->control.field.buffer, c->array, c->arraysize);
	}

	s_servercontrols.num_field++;
}

/*
=======================================================================================================================================
CreateServer_ServerPage_InitControlList
=======================================================================================================================================
*/
static void CreateServer_ServerPage_InitControlList(controlinit_t *list, int size) {
	int i;

	for (i = 0; i < size; i++) {
		switch (list[i].type) {
			case SRVCTRL_SPIN:
				CreateServer_ServerPage_InitSpinCtrl(&list[i]);
				break;
			case SRVCTRL_RADIO:
				CreateServer_ServerPage_InitRadioCtrl(&list[i]);
				break;
			case SRVCTRL_TEXTFIELD:
			case SRVCTRL_NUMFIELD:
				CreateServer_ServerPage_InitFieldCtrl(&list[i]);
				break;
			case SRVCTRL_BLANK:
				s_servercontrols.control_height[list[i].screenpos] += LINE_HEIGHT;
				break;
		}
	}
}

/*
=======================================================================================================================================
CreateServer_ServerPage_MenuInit
=======================================================================================================================================
*/
void CreateServer_ServerPage_MenuInit(void) {
	menuframework_s *menuptr;
	int y, ymax, i;

	memset(&s_servercontrols, 0, sizeof(servercontrols_t));

	CreateServer_ServerPage_Cache();

	menuptr = &s_servercontrols.menu;
	menuptr->key = CreateServer_ServerPage_MenuKey;
	menuptr->wrapAround = qtrue;
	menuptr->fullscreen = qtrue;
	menuptr->draw = CreateServer_ServerPage_MenuDraw;

	CreateServer_CommonControls_Init(menuptr, &s_servercontrols.common, CreateServer_ServerPage_CommonEvent, COMMONCTRL_SERVER);
	// the user controlled server params
	y = GAMETYPEROW_Y;
	s_servercontrols.gameType.generic.type = MTYPE_SPINCONTROL;
	s_servercontrols.gameType.generic.id = ID_SERVER_GAMETYPE;
	s_servercontrols.gameType.generic.flags = QMF_PULSEIFFOCUS|QMF_SMALLFONT;
	s_servercontrols.gameType.generic.callback = CreateServer_ServerPage_Event;
	s_servercontrols.gameType.generic.x = GAMETYPECOLUMN_X;
	s_servercontrols.gameType.generic.y = y;
	s_servercontrols.gameType.generic.name = "Game Type:";
	s_servercontrols.gameType.itemnames = gametype_items;

	s_servercontrols.gameTypeIcon.generic.type = MTYPE_BITMAP;
	s_servercontrols.gameTypeIcon.generic.flags = QMF_INACTIVE;
	s_servercontrols.gameTypeIcon.generic.x = GAMETYPEICON_X;
	s_servercontrols.gameTypeIcon.generic.y = y;
	s_servercontrols.gameTypeIcon.width = 32;
	s_servercontrols.gameTypeIcon.height = 32;

	s_servercontrols.num_radio = 0;
	s_servercontrols.num_field = 0;
	s_servercontrols.num_spin = 0;

	y += LINE_HEIGHT;

	for (i = 0; i < SCRPOS_COUNT; i++) {
		s_servercontrols.control_height[i] = y;
	}

	CreateServer_ServerPage_InitControlList(InitControls, InitControls_Size);

	if (s_scriptdata.multiplayer) {
		ymax = 0;

		for (i = 0; i < SCRPOS_COUNT; i++) {
			if (s_servercontrols.control_height[i] > ymax) {
				ymax = s_servercontrols.control_height[i];
			}
		}

		for (i = 0; i < SCRPOS_COUNT; i++) {
			s_servercontrols.control_height[i] = ymax;
		}

		CreateServer_ServerPage_InitControlList(InitMultiControls, InitMultiControls_Size);
	}
	/*
	s_servercontrols.saveScript.generic.type = MTYPE_BITMAP;
	s_servercontrols.saveScript.generic.name = SERVER_SAVE0;
	s_servercontrols.saveScript.generic.flags = QMF_PULSEIFFOCUS;
	s_servercontrols.saveScript.generic.x = 320 - 128;
	s_servercontrols.saveScript.generic.y = 480 - 64;
	s_servercontrols.saveScript.generic.id = ID_SERVER_SAVE;
	s_servercontrols.saveScript.generic.callback = CreateServer_ServerPage_Event;
	s_servercontrols.saveScript.generic.statusbar = CreateServer_ServerPage_StatusBar;
	s_servercontrols.saveScript.width = 128;
	s_servercontrols.saveScript.height = 64;
	s_servercontrols.saveScript.focuspic = SERVER_SAVE1;

	s_servercontrols.loadScript.generic.type = MTYPE_BITMAP;
	s_servercontrols.loadScript.generic.name = SERVER_LOAD0;
	s_servercontrols.loadScript.generic.flags = QMF_PULSEIFFOCUS;
	s_servercontrols.loadScript.generic.x = 320;
	s_servercontrols.loadScript.generic.y = 480 - 64;
	s_servercontrols.loadScript.generic.id = ID_SERVER_LOAD;
	s_servercontrols.loadScript.generic.callback = CreateServer_ServerPage_Event;
	s_servercontrols.loadScript.generic.statusbar = CreateServer_ServerPage_StatusBar;
	s_servercontrols.loadScript.width = 128;
	s_servercontrols.loadScript.height = 64;
	s_servercontrols.loadScript.focuspic = SERVER_LOAD1;
	*/
	s_servercontrols.statusbar_height = 480 - 64 - LINE_HEIGHT;
	// register controls
	Menu_AddItem(menuptr, &s_servercontrols.gameType);
	Menu_AddItem(menuptr, &s_servercontrols.gameTypeIcon);

	for (i = 0; i < s_servercontrols.num_radio; i++) {
		Menu_AddItem(menuptr, &s_servercontrols.radio[i].control);
	}

	for (i = 0; i < s_servercontrols.num_field; i++) {
		Menu_AddItem(menuptr, &s_servercontrols.field[i].control);
	}

	for (i = 0; i < s_servercontrols.num_spin; i++) {
		Menu_AddItem(menuptr, &s_servercontrols.spin[i].control);
	}
	/*
	Menu_AddItem(menuptr, &s_servercontrols.saveScript);
	Menu_AddItem(menuptr, &s_servercontrols.loadScript);
	*/
	UI_PushMenu(&s_servercontrols.menu);
}