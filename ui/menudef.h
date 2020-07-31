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

#define ITEM_TYPE_TEXT			0 // simple text
#define ITEM_TYPE_BUTTON		1 // button, basically text with a border
#define ITEM_TYPE_RADIOBUTTON	2 // toggle button, may be grouped
#define ITEM_TYPE_CHECKBOX		3 // check box
#define ITEM_TYPE_EDITFIELD		4 // editable text, associated with a cvar
#define ITEM_TYPE_COMBO			5 // drop down list
#define ITEM_TYPE_LISTBOX		6 // scrollable list
#define ITEM_TYPE_MODEL			7 // model
#define ITEM_TYPE_OWNERDRAW		8 // owner draw, name specs what it is
#define ITEM_TYPE_NUMERICFIELD	9 // editable text, associated with a cvar
#define ITEM_TYPE_SLIDER		10 // mouse speed, volume, etc.
#define ITEM_TYPE_YESNO			11 // yes no cvar setting
#define ITEM_TYPE_MULTI			12 // multiple list setting, enumerated
#define ITEM_TYPE_BIND			13 // multiple list setting, enumerated

#define ITEM_ALIGN_LEFT		0 // left alignment
#define ITEM_ALIGN_CENTER	1 // center alignment
#define ITEM_ALIGN_RIGHT	2 // right alignment

#define ITEM_TEXTSTYLE_NORMAL			0 // normal text
#define ITEM_TEXTSTYLE_BLINK			1 // fast blinking
#define ITEM_TEXTSTYLE_PULSE			2 // slow pulsing
#define ITEM_TEXTSTYLE_SHADOWED			3 // drop shadow (need a color for this)
#define ITEM_TEXTSTYLE_OUTLINED			4 // drop shadow (need a color for this)
#define ITEM_TEXTSTYLE_OUTLINESHADOWED	5 // drop shadow (need a color for this)
#define ITEM_TEXTSTYLE_SHADOWEDMORE		6 // drop shadow (need a color for this)

#define WINDOW_BORDER_NONE			0 // no border
#define WINDOW_BORDER_FULL			1 // full border based on border color (single pixel)
#define WINDOW_BORDER_HORZ			2 // horizontal borders only
#define WINDOW_BORDER_VERT			3 // vertical borders only
#define WINDOW_BORDER_KCGRADIENT	4 // horizontal border using the gradient bars

#define WINDOW_STYLE_EMPTY		0 // no background
#define WINDOW_STYLE_FILLED		1 // filled with background color
#define WINDOW_STYLE_GRADIENT	2 // gradient bar based on background color
#define WINDOW_STYLE_SHADER		3 // gradient bar based on background color
#define WINDOW_STYLE_TEAMCOLOR	4 // team color
#define WINDOW_STYLE_CINEMATIC	5 // cinematic

#define MENU_TRUE	1 // uh.. true
#define MENU_FALSE	0 // and false

#define HUD_VERTICAL	0x00
#define HUD_HORIZONTAL	0x01
// list box element types
#define LISTBOX_TEXT	0x00
#define LISTBOX_IMAGE	0x01
// list feeders
#define FEEDER_HEADS			0x00 // model heads
#define FEEDER_MAPS				0x01 // text maps based on game type
#define FEEDER_SERVERS			0x02 // servers
#define FEEDER_CLANS			0x03 // clan names
#define FEEDER_ALLMAPS			0x04 // all maps available, in graphic format
#define FEEDER_REDTEAM_LIST		0x05 // red team members
#define FEEDER_BLUETEAM_LIST	0x06 // blue team members
#define FEEDER_PLAYER_LIST		0x07 // players
#define FEEDER_TEAM_LIST		0x08 // team members for team voting
#define FEEDER_MODS				0x09
#define FEEDER_DEMOS			0x0a
#define FEEDER_SCOREBOARD		0x0b
#define FEEDER_Q3HEADS			0x0c // model heads
#define FEEDER_SERVERSTATUS		0x0d // server status
#define FEEDER_FINDPLAYER		0x0e // find player
#define FEEDER_CINEMATICS		0x0f // cinematics
// display flags
#define CG_SHOW_BLUE_TEAM_HAS_REDFLAG	0x00000001
#define CG_SHOW_RED_TEAM_HAS_BLUEFLAG	0x00000002
#define CG_SHOW_ANYTEAMGAME				0x00000004
#define CG_SHOW_HARVESTER				0x00000008
#define CG_SHOW_ONEFLAG					0x00000010
#define CG_SHOW_CTF						0x00000020
#define CG_SHOW_OBELISK					0x00000040
#define CG_SHOW_HEALTHCRITICAL			0x00000080
#define CG_SHOW_SINGLEPLAYER			0x00000100
#define CG_SHOW_TOURNAMENT				0x00000200
#define CG_SHOW_DURINGINCOMINGVOICE		0x00000400
#define CG_SHOW_IF_PLAYER_HAS_FLAG		0x00000800
#define CG_SHOW_LANPLAYONLY				0x00001000
#define CG_SHOW_MINED					0x00002000
#define CG_SHOW_HEALTHOK				0x00004000
#define CG_SHOW_TEAMINFO				0x00008000
#define CG_SHOW_NOTEAMINFO				0x00010000
#define CG_SHOW_OTHERTEAMHASFLAG		0x00020000
#define CG_SHOW_YOURTEAMHASENEMYFLAG	0x00040000
#define CG_SHOW_ANYNONTEAMGAME			0x00080000
#define CG_SHOW_2DONLY					0x10000000

#define UI_SHOW_LEADER				0x00000001
#define UI_SHOW_NOTLEADER			0x00000002
#define UI_SHOW_FAVORITESERVERS		0x00000004
#define UI_SHOW_ANYNONTEAMGAME		0x00000008
#define UI_SHOW_ANYTEAMGAME			0x00000010
#define UI_SHOW_NEWHIGHSCORE		0x00000020
#define UI_SHOW_DEMOAVAILABLE		0x00000040
#define UI_SHOW_NEWBESTTIME			0x00000080
#define UI_SHOW_FFA					0x00000100
#define UI_SHOW_NOTFFA				0x00000200
#define UI_SHOW_NETANYNONTEAMGAME	0x00000400
#define UI_SHOW_NETANYTEAMGAME		0x00000800
#define UI_SHOW_NOTFAVORITESERVERS	0x00001000

// owner draw types

// ideally these should be done outside of this file but this makes it much easier for the macro expansion to convert them for the designers (from the .menu files)
#define CG_OWNERDRAW_BASE			1
#define CG_PLAYER_ARMOR_ICON		2
#define CG_PLAYER_ARMOR_VALUE		3
#define CG_PLAYER_HEAD				4
#define CG_PLAYER_HEALTH			5
#define CG_PLAYER_AMMO_ICON			6
#define CG_PLAYER_AMMO_VALUE		7
#define CG_SELECTEDPLAYER_HEAD		8
#define CG_SELECTEDPLAYER_NAME		9
#define CG_SELECTEDPLAYER_LOCATION	10
#define CG_SELECTEDPLAYER_STATUS	11
#define CG_SELECTEDPLAYER_WEAPON	12
#define CG_SELECTEDPLAYER_POWERUP	13
#define CG_FLAGCARRIER_HEAD			14
#define CG_FLAGCARRIER_NAME			15
#define CG_FLAGCARRIER_LOCATION		16
#define CG_FLAGCARRIER_STATUS		17
#define CG_FLAGCARRIER_WEAPON		18
#define CG_FLAGCARRIER_POWERUP		19
#define CG_PLAYER_ITEM				20
#define CG_PLAYER_SCORE				21
#define CG_BLUE_FLAGHEAD			22
#define CG_BLUE_FLAGSTATUS			23
#define CG_BLUE_FLAGNAME			24
#define CG_RED_FLAGHEAD				25
#define CG_RED_FLAGSTATUS			26
#define CG_RED_FLAGNAME				27
#define CG_BLUE_SCORE				28
#define CG_RED_SCORE				29
#define CG_RED_NAME					30
#define CG_BLUE_NAME				31
#define CG_HARVESTER_SKULLS			32 // only shows in harvester
#define CG_ONEFLAG_STATUS			33 // only shows in one flag
#define CG_PLAYER_LOCATION			34
#define CG_TEAM_COLOR				35
#define CG_CTF_POWERUP				36
#define CG_AREA_POWERUP				37
#define CG_AREA_LAGOMETER			38 // painted with old system
#define CG_PLAYER_HASFLAG			39
#define CG_GAME_TYPE				40 // not done
#define CG_SELECTEDPLAYER_ARMOR		41
#define CG_SELECTEDPLAYER_HEALTH	42
#define CG_PLAYER_STATUS			43
#define CG_FRAGGED_MSG				44 // painted with old system
#define CG_PROXMINED_MSG			45 // painted with old system
#define CG_AREA_FPSINFO				46 // painted with old system
#define CG_AREA_SYSTEMCHAT			47 // painted with old system
#define CG_AREA_TEAMCHAT			48 // painted with old system
#define CG_AREA_CHAT				49 // painted with old system
#define CG_GAME_STATUS				50
#define CG_KILLER					51
#define CG_PLAYER_ARMOR_ICON2D		52
#define CG_PLAYER_AMMO_ICON2D		53
#define CG_ACCURACY					54
#define CG_ASSISTS					55
#define CG_DEFEND					56
#define CG_EXCELLENT				57
#define CG_IMPRESSIVE				58
#define CG_PERFECT					59
#define CG_GAUNTLET					60
#define CG_SPECTATORS				61
#define CG_TEAMINFO					62
#define CG_VOICE_HEAD				63
#define CG_VOICE_NAME				64
#define CG_PLAYER_HASFLAG2D			65
#define CG_HARVESTER_SKULLS2D		66 // only shows in harvester
#define CG_CAPFRAGLIMIT				67
#define CG_1STPLACE					68
#define CG_2NDPLACE					69
#define CG_CAPTURES					70

#define UI_OWNERDRAW_BASE		200
#define UI_EFFECTS				201
#define UI_PLAYERMODEL			202
#define UI_CLANNAME				203
#define UI_CLANLOGO				204
#define UI_GAMETYPE				205
#define UI_MAPPREVIEW			206
#define UI_SKILL				207
#define UI_BLUETEAMNAME			208
#define UI_REDTEAMNAME			209
#define UI_BLUETEAM1			210
#define UI_BLUETEAM2			211
#define UI_BLUETEAM3			212
#define UI_BLUETEAM4			213
#define UI_BLUETEAM5			214
#define UI_BLUETEAM6			215
#define UI_BLUETEAM7			216
#define UI_REDTEAM1				217
#define UI_REDTEAM2				218
#define UI_REDTEAM3				219
#define UI_REDTEAM4				220
#define UI_REDTEAM5				221
#define UI_REDTEAM6				222
#define UI_REDTEAM7				223
#define UI_NOTTEAM1				224
#define UI_NOTTEAM2				225
#define UI_NOTTEAM3				226
#define UI_NOTTEAM4				227
#define UI_NOTTEAM5				228
#define UI_NOTTEAM6				229
#define UI_NOTTEAM7				230
#define UI_NOTTEAM8				231
#define UI_NOTTEAM9				232
#define UI_NOTTEAM10			233
#define UI_NOTTEAM11			234
#define UI_NOTTEAM12			235
#define UI_NOTTEAM13			236
#define UI_NOTTEAM14			237
#define UI_NETSOURCE			238
#define UI_NETMAPPREVIEW		239
#define UI_NETFILTER			240
#define UI_TIER					241
#define UI_OPPONENTMODEL		242
#define UI_TIERMAP1				243
#define UI_TIERMAP2				244
#define UI_TIERMAP3				245
#define UI_PLAYERLOGO			246
#define UI_OPPONENTLOGO			247
#define UI_PLAYERLOGO_METAL		248
#define UI_OPPONENTLOGO_METAL	249
#define UI_PLAYERLOGO_NAME		250
#define UI_OPPONENTLOGO_NAME	251
#define UI_TIER_MAPNAME			252
#define UI_TIER_GAMETYPE		253
#define UI_ALLMAPS_SELECTION	254
#define UI_OPPONENT_NAME		255
#define UI_VOTE_KICK			256
#define UI_BOTNAME				257
#define UI_BOTSKILL				258
#define UI_REDBLUE				259
#define UI_CROSSHAIR			260
#define UI_SELECTEDPLAYER		261
#define UI_MAPCINEMATIC			262
#define UI_NETGAMETYPE			263
#define UI_NETMAPCINEMATIC		264
#define UI_SERVERREFRESHDATE	265
#define UI_SERVERMOTD			266
#define UI_GLINFO				267
#define UI_KEYBINDSTATUS		268
#define UI_CLANCINEMATIC		269
#define UI_MAP_TIMETOBEAT		270
#define UI_JOINGAMETYPE			271
#define UI_PREVIEWCINEMATIC		272
#define UI_STARTMAPCINEMATIC	273
#define UI_MAPS_SELECTION		274

#define VOICECHAT_GETFLAG "getflag" // command someone to get the flag
#define VOICECHAT_OFFENSE "offense" // command someone to go on offense
#define VOICECHAT_DEFENDFLAG "defendflag" // command someone to defend the flag
#define VOICECHAT_DEFEND "defend" // command someone to go on defense
#define VOICECHAT_RETURNFLAG "returnflag" // command someone to return our flag
#define VOICECHAT_FOLLOWFLAGCARRIER "followflagcarrier" // command someone to follow the flag carrier
#define VOICECHAT_FOLLOWME "followme" // command someone to follow you
#define VOICECHAT_CAMP "camp" // command someone to camp (we don't have sounds for this one)
#define VOICECHAT_PATROL "patrol" // command someone to go on patrol (roam)
#define VOICECHAT_WANTONOFFENSE "wantonoffense" // I want to be on offense
#define VOICECHAT_WANTONDEFENSE "wantondefense" // I want to be on defense
#define VOICECHAT_WHOISLEADER "whoisleader" // who is the team leader
#define VOICECHAT_STOPLEADER "stopleader" // I resign leadership
#define VOICECHAT_STARTLEADER "startleader" // I'm the leader
#define VOICECHAT_YES "yes" // yes, affirmative, etc.
#define VOICECHAT_NO "no" // no, negative, etc.
#define VOICECHAT_ONGETFLAG "ongetflag" // I'm getting the flag
#define VOICECHAT_ONOFFENSE "onoffense" // I'm on offense
#define VOICECHAT_ONDEFENSE "ondefense" // I'm on defense
#define VOICECHAT_ONRETURNFLAG "onreturnflag" // I'm returning our flag
#define VOICECHAT_ONFOLLOWCARRIER "onfollowcarrier" // I'm following the flag carrier
#define VOICECHAT_ONFOLLOW "onfollow" // I'm following
#define VOICECHAT_ONCAMPING "oncamp" // I'm camping somewhere
#define VOICECHAT_ONPATROL "onpatrol" // I'm on patrol (roaming)
#define VOICECHAT_INPOSITION "inposition" // I'm in position
#define VOICECHAT_IHAVEFLAG "ihaveflag" // I have the flag
#define VOICECHAT_ENEMYHASFLAG "enemyhasflag" // the enemy has our flag (CTF)
#define VOICECHAT_BASEATTACK "baseattack" // the base is under attack
#define VOICECHAT_PRAISE "praise" // you did something good
#define VOICECHAT_TAUNT "taunt" // I want to taunt you
#define VOICECHAT_KILLGAUNTLET "kill_gauntlet" // I just killed you with the gauntlet
#define VOICECHAT_KILLINSULT "kill_insult" // I just killed you
#define VOICECHAT_DEATHINSULT "death_insult" // you just killed me
#define VOICECHAT_TRASH "trash" // lots of trash talk
