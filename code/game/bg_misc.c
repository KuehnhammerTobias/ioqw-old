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
 Both games misc functions, all completely stateless.
**************************************************************************************************************************************/

#include "../qcommon/q_shared.h"
#include "bg_public.h"

/*QUAKED item_***** (0 0 0) (-16 -16 -16) (16 16 16) SUSPENDED
DO NOT USE THIS CLASS, IT JUST HOLDS GENERAL INFORMATION.
The suspended flag will allow items to hang in the air, otherwise they are dropped to the next surface.

If an item is the target of another entity, it will not spawn in until fired.

An item fires all of its targets when it is picked up. If the toucher can't carry it, the targets won't be fired.

"notfree" if set to 1, don't spawn in free for all games
"notteam" if set to 1, don't spawn in team games
"notsingle" if set to 1, don't spawn in single player games
"wait" override the default wait before respawning. -1 = never respawn automatically, which can be used with targeted spawning.
"random" random number of plus or minus seconds varied from the respawn time
"count" override quantity or duration on most items.
*/

/**************************************************************************************************************************************
	LEAVE INDEX 0 ALONE!
**************************************************************************************************************************************/

gitem_t bg_itemlist[] = {
	{
		NULL,
		NULL,
		{NULL, NULL, NULL, NULL},
/* icon */		NULL,
/* pickup */	NULL,
		0,
		0,
		0,
/* sounds */ ""
	},

/**************************************************************************************************************************************
	HEALTH
**************************************************************************************************************************************/

/*QUAKED item_health_small (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_health_small",
		"snd/i/puhs.wav",
		{"models/powerups/health/small_cross.md3", "models/powerups/health/small_sphere.md3", NULL, NULL},
/* icon */		"icons/iconh_green",
/* pickup */	"5 Health",
		5,
		IT_HEALTH,
		0,
/* sounds */ ""
	},

/*QUAKED item_health (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_health",
		"snd/i/puhn.wav",
		{"models/powerups/health/medium_cross.md3", "models/powerups/health/medium_sphere.md3", NULL, NULL},
/* icon */		"icons/iconh_yellow",
/* pickup */	"25 Health",
		25,
		IT_HEALTH,
		0,
/* sounds */ ""
	},

/*QUAKED item_health_large (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_health_large",
		"snd/i/puhl.wav",
		{"models/powerups/health/large_cross.md3", "models/powerups/health/large_sphere.md3", NULL, NULL},
/* icon */		"icons/iconh_red",
/* pickup */	"50 Health",
		50,
		IT_HEALTH,
		0,
/* sounds */ ""
	},

/*QUAKED item_health_mega (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_health_mega",
		"snd/i/puhm.wav",
		{"models/powerups/health/mega_cross.md3", "models/powerups/health/mega_sphere.md3", NULL, NULL},
/* icon */		"icons/iconh_mega",
/* pickup */	"Mega Health",
		100,
		IT_HEALTH,
		0,
/* sounds */ ""
	},

/**************************************************************************************************************************************
	ARMOR
**************************************************************************************************************************************/

/*QUAKED item_armor_shard (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_armor_shard",
		"snd/i/puas.wav",
		{"models/powerups/armor/shard.md3", NULL, NULL, NULL},
/* icon */		"icons/iconr_shard",
/* pickup */	"Armor Shard",
		5,
		IT_ARMOR,
		0,
/* sounds */ ""
	},

/*QUAKED item_armor_combat (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_armor_combat",
		"snd/i/puag.wav",
		{"models/powerups/armor/armor_gre.md3", NULL, NULL, NULL},
/* icon */		"icons/iconr_green",
/* pickup */	"Armor",
		50,
		IT_ARMOR,
		0,
/* sounds */ ""
	},

/*QUAKED item_armor_body (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_armor_body",
		"snd/i/puay.wav",
		{"models/powerups/armor/armor_yel.md3", NULL, NULL, NULL},
/* icon */		"icons/iconr_yellow",
/* pickup */	"Heavy Armor",
		100,
		IT_ARMOR,
		0,
/* sounds */ ""
	},

/*QUAKED item_armor_full (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_armor_full",
		"snd/i/puar.wav",
		{"models/powerups/armor/armor_red.md3", NULL, NULL, NULL},
/* icon */		"icons/iconr_red",
/* pickup */	"Full Armor",
		200,
		IT_ARMOR,
		0,
/* sounds */ ""
	},

/**************************************************************************************************************************************
	WEAPONS
**************************************************************************************************************************************/

/*QUAKED weapon_gauntlet (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_gauntlet",
		"snd/i/puw.wav",
		{"models/weapons2/gauntlet/gauntlet.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_gauntlet",
/* pickup */	"Gauntlet",
		0,
		IT_WEAPON,
		WP_GAUNTLET,
/* sounds */ ""
	},

/*QUAKED weapon_machinegun (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_machinegun",
		"snd/i/puw.wav",
		{"models/weapons2/machinegun/machinegun.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_machinegun",
/* pickup */	"Machine Gun",
		50,
		IT_WEAPON,
		WP_MACHINEGUN,
/* sounds */ ""
	},

/*QUAKED weapon_chaingun (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_chaingun",
		"snd/i/puw.wav",
		{"models/weapons/vulcan/vulcan.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_chaingun",
/* pickup */	"Chain Gun",
		100,
		IT_WEAPON,
		WP_CHAINGUN,
/* sounds */ "sound/weapons/vulcan/wvulwind.wav"
	},

/*QUAKED weapon_shotgun (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_shotgun",
		"snd/i/puw.wav",
		{"models/weapons2/shotgun/shotgun.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_shotgun",
/* pickup */	"Shotgun",
		10,
		IT_WEAPON,
		WP_SHOTGUN,
/* sounds */ ""
	},

/*QUAKED weapon_nailgun (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_nailgun",
		"snd/i/puw.wav",
		{"models/weapons/nailgun/nailgun.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_nailgun",
/* pickup */	"Nail Gun",
		10,
		IT_WEAPON,
		WP_NAILGUN,
/* sounds */ ""
	},

/*QUAKED weapon_proxylauncher (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_proxylauncher",
		"snd/i/puw.wav",
		{"models/weapons/proxmine/proxmine.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_proxlauncher",
/* pickup */	"Proximity Launcher",
		5,
		IT_WEAPON,
		WP_PROXLAUNCHER,
/* sounds */ "sound/weapons/proxmine/wstbtick.wav sound/weapons/proxmine/wstbactv.wav sound/weapons/proxmine/wstbimpd.wav sound/weapons/proxmine/wstbactv.wav"
	},

/*QUAKED weapon_grenadelauncher (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_grenadelauncher",
		"snd/i/puw.wav",
		{"models/weapons2/grenadel/grenadel.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_grenade",
/* pickup */	"Grenade Launcher",
		10,
		IT_WEAPON,
		WP_GRENADELAUNCHER,
/* sounds */ "sound/weapons/grenade/hgrenb1a.wav sound/weapons/grenade/hgrenb2a.wav"
	},

/*QUAKED weapon_napalmlauncher (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_napalmlauncher",
		"snd/i/puw.wav",
		{"models/weapons2/napalml/napalml.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_napalm",
/* pickup */	"Napalm Launcher",
		10,
		IT_WEAPON,
		WP_NAPALMLAUNCHER,
/* sounds */ ""
	},

/*QUAKED weapon_rocketlauncher (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_rocketlauncher",
		"snd/i/puw.wav",
		{"models/weapons2/rocketl/rocketl.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_rocket",
/* pickup */	"Rocket Launcher",
		10,
		IT_WEAPON,
		WP_ROCKETLAUNCHER,
/* sounds */ ""
	},

/*QUAKED weapon_beamgun (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_beamgun",
		"snd/i/puw.wav",
		{"models/weapons2/lightning/lightning.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_lightning",
/* pickup */	"Beam Gun",
		100,
		IT_WEAPON,
		WP_BEAMGUN,
/* sounds */ ""
	},

/*QUAKED weapon_railgun (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_railgun",
		"snd/i/puw.wav",
		{"models/weapons2/railgun/railgun.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_railgun",
/* pickup */	"Railgun",
		10,
		IT_WEAPON,
		WP_RAILGUN,
/* sounds */ ""
	},

/*QUAKED weapon_plasmagun (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_plasmagun",
		"snd/i/puw.wav",
		{"models/weapons2/plasma/plasma.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_plasma",
/* pickup */	"Plasma Gun",
		50,
		IT_WEAPON,
		WP_PLASMAGUN,
/* sounds */ ""
	},

/*QUAKED weapon_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"weapon_bfg",
		"snd/i/puw.wav",
		{"models/weapons2/bfg/bfg.md3", NULL, NULL, NULL},
/* icon */		"icons/iconw_bfg",
/* pickup */	"BFG 10K",
		20,
		IT_WEAPON,
		WP_BFG,
/* sounds */ ""
	},

/**************************************************************************************************************************************
	AMMO
**************************************************************************************************************************************/

/*QUAKED ammo_bullets (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_bullets",
		"snd/i/pum.wav",
		{"models/powerups/ammo/machinegunam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_machinegun",
/* pickup */	"Bullets",
		50,
		IT_AMMO,
		WP_MACHINEGUN,
/* sounds */ ""
	},

/*QUAKED ammo_belt (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_belt",
		"snd/i/pum.wav",
		{"models/powerups/ammo/chaingunam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_chaingun",
/* pickup */	"Chain Gun Belt",
		100,
		IT_AMMO,
		WP_CHAINGUN,
/* sounds */ ""
	},

/*QUAKED ammo_shells (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_shells",
		"snd/i/pum.wav",
		{"models/powerups/ammo/shotgunam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_shotgun",
/* pickup */	"Shells",
		10,
		IT_AMMO,
		WP_SHOTGUN,
/* sounds */ ""
	},

/*QUAKED ammo_nails (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_nails",
		"snd/i/pum.wav",
		{"models/powerups/ammo/nailgunam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_nailgun",
/* pickup */	"Nails",
		5,
		IT_AMMO,
		WP_NAILGUN,
/* sounds */ ""
	},

/*QUAKED ammo_mines (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_mines",
		"snd/i/pum.wav",
		{"models/powerups/ammo/proxmineam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_proxlauncher",
/* pickup */	"Proximity Mines",
		5,
		IT_AMMO,
		WP_PROXLAUNCHER,
/* sounds */ ""
	},

/*QUAKED ammo_grenades (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_grenades",
		"snd/i/pum.wav",
		{"models/powerups/ammo/grenadeam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_grenade",
/* pickup */	"Grenades",
		5,
		IT_AMMO,
		WP_GRENADELAUNCHER,
/* sounds */ ""
	},

/*QUAKED ammo_canisters (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_canisters",
		"snd/i/pum.wav",
		{"models/powerups/ammo/napalmam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_napalm",
/* pickup */	"Canisters",
		5,
		IT_AMMO,
		WP_NAPALMLAUNCHER,
/* sounds */ ""
	},

/*QUAKED ammo_rockets (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_rockets",
		"snd/i/pum.wav",
		{"models/powerups/ammo/rocketam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_rocket",
/* pickup */	"Rockets",
		5,
		IT_AMMO,
		WP_ROCKETLAUNCHER,
/* sounds */ ""
	},

/*QUAKED ammo_cartridge (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_cartridge",
		"snd/i/pum.wav",
		{"models/powerups/ammo/lightningam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_lightning",
/* pickup */	"Cartridge",
		100,
		IT_AMMO,
		WP_BEAMGUN,
/* sounds */ ""
	},

/*QUAKED ammo_slugs (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_slugs",
		"snd/i/pum.wav",
		{"models/powerups/ammo/railgunam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_railgun",
/* pickup */	"Slugs",
		10,
		IT_AMMO,
		WP_RAILGUN,
/* sounds */ ""
	},

/*QUAKED ammo_cells (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_cells",
		"snd/i/pum.wav",
		{"models/powerups/ammo/plasmaam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_plasma",
/* pickup */	"Cells",
		50,
		IT_AMMO,
		WP_PLASMAGUN,
/* sounds */ ""
	},

/*QUAKED ammo_bfg (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"ammo_bfg",
		"snd/i/pum.wav",
		{"models/powerups/ammo/bfgam.md3", NULL, NULL, NULL},
/* icon */		"icons/icona_bfg",
/* pickup */	"Bfg Ammo",
		20,
		IT_AMMO,
		WP_BFG,
/* sounds */ ""
	},

/**************************************************************************************************************************************
	HOLDABLE ITEMS
**************************************************************************************************************************************/

/*QUAKED holdable_medkit (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"holdable_medkit",
		"snd/i/puh.wav",
		{"models/powerups/holdable/medkit.md3", NULL, NULL, NULL},
/* icon */		"icons/medkit",
/* pickup */	"Medkit",
		60,
		IT_HOLDABLE,
		HI_MEDKIT,
/* sounds */ "snd/i/um.wav"
	},

/*QUAKED holdable_kamikaze (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"holdable_kamikaze",
		"snd/i/puh.wav",
		{"models/powerups/kamikazi.md3", NULL, NULL, NULL},
/* icon */		"icons/kamikaze",
/* pickup */	"Kamikaze",
		60,
		IT_HOLDABLE,
		HI_KAMIKAZE,
/* sounds */ "snd/i/kam_sp.wav"
	},

/**************************************************************************************************************************************
	POWERUP ITEMS
**************************************************************************************************************************************/

/*QUAKED item_quad (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_quad",
		"snd/v/voc_quad.wav",
		{"models/powerups/instant/quad.md3", "models/powerups/instant/quad_ring.md3", NULL, NULL},
/* icon */		"icons/quad",
/* pickup */	"Quad Damage",
		30,
		IT_POWERUP,
		PW_QUAD,
/* sounds */ "snd/i/q.wav"
	},

/*QUAKED item_invis (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_invis",
		"snd/v/voc_invis.wav",
		{"models/powerups/instant/invis.md3", "models/powerups/instant/invis_ring.md3", NULL, NULL},
/* icon */		"icons/invis",
/* pickup */	"Invisibility",
		30,
		IT_POWERUP,
		PW_INVIS,
/* sounds */ ""
	},

/*QUAKED item_regen (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED
*/
	{
		"item_regen",
		"snd/v/voc_regen.wav",
		{"models/powerups/instant/regen.md3", "models/powerups/instant/regen_ring.md3", NULL, NULL},
/* icon */		"icons/regen",
/* pickup */	"Regeneration",
		30,
		IT_POWERUP,
		PW_REGEN,
/* sounds */ "snd/i/r.wav"
	},

/**************************************************************************************************************************************
	PERSISTANT POWERUP ITEMS
**************************************************************************************************************************************/

/*QUAKED item_ammoregen (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED REDTEAM BLUETEAM
*/
	{
		"item_ammoregen",
		"snd/i/pu.wav",
		{"models/powerups/ammo.md3", NULL, NULL, NULL},
/* icon */		"icons/ammo_regen",
/* pickup */	"Ammo Regen",
		30,
		IT_PERSISTANT_POWERUP,
		PW_AMMOREGEN,
/* sounds */ ""
	},

/*QUAKED item_guard (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED REDTEAM BLUETEAM
*/
	{
		"item_guard",
		"snd/i/pu.wav",
		{"models/powerups/guard.md3", NULL, NULL, NULL},
/* icon */		"icons/guard",
/* pickup */	"Guard",
		30,
		IT_PERSISTANT_POWERUP,
		PW_GUARD,
/* sounds */ ""
	},

/*QUAKED item_doubler (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED REDTEAM BLUETEAM
*/
	{
		"item_doubler",
		"snd/i/pu.wav",
		{"models/powerups/doubler.md3", NULL, NULL, NULL},
/* icon */		"icons/doubler",
/* pickup */	"Doubler",
		30,
		IT_PERSISTANT_POWERUP,
		PW_DOUBLER,
/* sounds */ ""
	},

/*QUAKED item_scout (.3 .3 1) (-16 -16 -16) (16 16 16) SUSPENDED REDTEAM BLUETEAM
*/
	{
		"item_scout",
		"snd/i/pu.wav",
		{"models/powerups/scout.md3", NULL, NULL, NULL},
/* icon */		"icons/scout",
/* pickup */	"Scout",
		30,
		IT_PERSISTANT_POWERUP,
		PW_SCOUT,
/* sounds */ ""
	},

/**************************************************************************************************************************************
	TEAM ITEMS
**************************************************************************************************************************************/

/*QUAKED team_CTF_redflag (1 0 0) (-16 -16 -16) (16 16 16)
Only in CTF games
*/
	{
		"team_CTF_redflag",
		NULL,
		{"models/flags/r_flag.md3", NULL, NULL, NULL},
/* icon */		"icons/iconf_red1",
/* pickup */	"Red Flag",
		0,
		IT_TEAM,
		PW_REDFLAG,
/* sounds */ ""
	},

/*QUAKED team_CTF_blueflag (0 0 1) (-16 -16 -16) (16 16 16)
Only in CTF games
*/
	{
		"team_CTF_blueflag",
		NULL,
		{"models/flags/b_flag.md3", NULL, NULL, NULL},
/* icon */		"icons/iconf_blu1",
/* pickup */	"Blue Flag",
		0,
		IT_TEAM,
		PW_BLUEFLAG,
/* sounds */ ""
	},

/*QUAKED team_CTF_neutralflag (0 0 1) (-16 -16 -16) (16 16 16)
Only in One Flag CTF games
*/
	{
		"team_CTF_neutralflag",
		NULL,
		{"models/flags/n_flag.md3", NULL, NULL, NULL},
/* icon */		"icons/iconf_neutral1",
/* pickup */	"Neutral Flag",
		0,
		IT_TEAM,
		PW_NEUTRALFLAG,
/* sounds */ ""
	},

/*QUAKED item_redcube (0 0 1) (-16 -16 -16) (16 16 16)
*/
	{
		"item_redcube",
		"snd/i/pu.wav",
		{"models/powerups/orb/r_orb.md3", NULL, NULL, NULL},
/* icon */		"icons/iconh_rorb",
/* pickup */	"Red Cube",
		0,
		IT_TEAM,
		0,
/* sounds */ ""
	},

/*QUAKED item_bluecube (0 0 1) (-16 -16 -16) (16 16 16)
*/
	{
		"item_bluecube",
		"snd/i/pu.wav",
		{"models/powerups/orb/b_orb.md3", NULL, NULL, NULL},
/* icon */		"icons/iconh_borb",
/* pickup */	"Blue Cube",
		0,
		IT_TEAM,
		0,
/* sounds */ ""
	},
	// end of list marker
	{NULL}
};

int bg_numItems = ARRAY_LEN(bg_itemlist) - 1;

/*
=======================================================================================================================================
BG_CheckSpawnEntity
=======================================================================================================================================
*/
qboolean BG_CheckSpawnEntity(const bgEntitySpawnInfo_t *info) {
	int i, gametype;
	char *s, *value, *gametypeName;
	static char *gametypeNames[GT_MAX_GAME_TYPE] = {"single", "ffa", "tournament", "team", "ctf", "oneflag", "obelisk", "harvester"};

	gametype = info->gametype;
	// check for "notsingle" flag
	if (gametype == GT_SINGLE_PLAYER) {
		info->spawnInt("notsingle", "0", &i);

		if (i) {
			return qfalse;
		}
	}
	// check for "notteam" flag (GT_SINGLE_PLAYER, GT_FFA, GT_TOURNAMENT)
	if (gametype > GT_TOURNAMENT) {
		info->spawnInt("notteam", "0", &i);

		if (i) {
			return qfalse;
		}
	} else {
		info->spawnInt("notfree", "0", &i);

		if (i) {
			return qfalse;
		}
	}

	if (info->spawnString("!gametype", NULL, &value)) {
		if (gametype >= 0 && gametype < GT_MAX_GAME_TYPE) {
			gametypeName = gametypeNames[gametype];
			s = strstr(value, gametypeName);

			if (s) {
				return qfalse;
			}
		}
	}

	if (info->spawnString("gametype", NULL, &value)) {
		if (gametype >= 0 && gametype < GT_MAX_GAME_TYPE) {
			gametypeName = gametypeNames[gametype];
			s = strstr(value, gametypeName);

			if (!s) {
				return qfalse;
			}
		}
	}

	return qtrue;
}

/*
=======================================================================================================================================
BG_FindItemForPowerup
=======================================================================================================================================
*/
gitem_t *BG_FindItemForPowerup(powerup_t pw) {
	int i;

	for (i = 0; i < bg_numItems; i++) {
		if ((bg_itemlist[i].giType == IT_POWERUP || bg_itemlist[i].giType == IT_TEAM || bg_itemlist[i].giType == IT_PERSISTANT_POWERUP) && bg_itemlist[i].giTag == pw) {
			return &bg_itemlist[i];
		}
	}

	return NULL;
}

/*
=======================================================================================================================================
BG_FindItemForHoldable
=======================================================================================================================================
*/
gitem_t *BG_FindItemForHoldable(holdable_t pw) {
	int i;

	for (i = 0; i < bg_numItems; i++) {
		if (bg_itemlist[i].giType == IT_HOLDABLE && bg_itemlist[i].giTag == pw) {
			return &bg_itemlist[i];
		}
	}

	Com_Error(ERR_DROP, "HoldableItem not found");
	return NULL;
}

/*
=======================================================================================================================================
BG_FindItemForWeapon
=======================================================================================================================================
*/
gitem_t *BG_FindItemForWeapon(weapon_t weapon) {
	gitem_t *it;

	for (it = bg_itemlist + 1; it->classname; it++) {
		if (it->giType == IT_WEAPON && it->giTag == weapon) {
			return it;
		}
	}

	Com_Error(ERR_DROP, "Couldn't find item for weapon %i", weapon);
	return NULL;
}

/*
=======================================================================================================================================
BG_FindItem
=======================================================================================================================================
*/
gitem_t *BG_FindItem(const char *pickupName) {
	gitem_t *it;

	for (it = bg_itemlist + 1; it->classname; it++) {
		if (!Q_stricmp(it->pickup_name, pickupName)) {
			return it;
		}
	}

	return NULL;
}

/*
=======================================================================================================================================
BG_PlayerTouchesItem

Items can be picked up without actually touching their physical bounds to make grabbing them easier.
=======================================================================================================================================
*/
qboolean BG_PlayerTouchesItem(playerState_t *ps, entityState_t *item, int atTime) {
	vec3_t origin;

	BG_EvaluateTrajectory(&item->pos, atTime, origin);
	// we are ignoring ducked differences here
	if (ps->origin[0] - origin[0] > 36 || ps->origin[0] - origin[0] < -36 || ps->origin[1] - origin[1] > 36 || ps->origin[1] - origin[1] < -36 || ps->origin[2] - origin[2] > 36 || ps->origin[2] - origin[2] < -36) {
		return qfalse;
	}

	return qtrue;
}

/*
=======================================================================================================================================
BG_CanItemBeGrabbed

Returns false if the item should not be picked up. This needs to be the same for client side prediction and server use.
=======================================================================================================================================
*/
qboolean BG_CanItemBeGrabbed(int gametype, const entityState_t *ent, const playerState_t *ps) {
	gitem_t *item;

	if (ent->modelindex < 1 || ent->modelindex >= bg_numItems) {
		Com_Error(ERR_DROP, "BG_CanItemBeGrabbed: index out of range");
	}

	item = &bg_itemlist[ent->modelindex];

	switch (item->giType) {
		case IT_HEALTH:
			if (ps->stats[STAT_HEALTH] >= 100) {
				return qfalse;
			}

			return qtrue;
		case IT_ARMOR:
			if (ps->stats[STAT_ARMOR] >= 200) {
				return qfalse;
			}

			return qtrue;
		case IT_WEAPON:
			return qtrue; // weapons are always picked up
		case IT_AMMO:
			if (ps->ammo[item->giTag] >= 200) {
				return qfalse; // can't hold any more
			}

			return qtrue;
		case IT_HOLDABLE:
			// can only hold one item at a time
			if (ps->stats[STAT_HOLDABLE_ITEM]) {
				return qfalse;
			}

			return qtrue;
		case IT_POWERUP:
			return qtrue; // powerups are always picked up
		case IT_PERSISTANT_POWERUP:
			// can only hold one item at a time
			if (ps->stats[STAT_PERSISTANT_POWERUP]) {
				return qfalse;
			}
			// check team only
			if (ent->team != 255 && (ps->persistant[PERS_TEAM] != ent->team)) {
				return qfalse;
			}

			return qtrue;
		case IT_TEAM: // team items, such as flags
			if (gametype == GT_CTF) {
				// ent->modelindex2 is non-zero on items if they are dropped
				// we need to know this because we can pick up our dropped flag (and return it) but we can't pick up our flag at base
				if (ps->persistant[PERS_TEAM] == TEAM_RED) {
					if (item->giTag == PW_BLUEFLAG || (item->giTag == PW_REDFLAG && ent->modelindex2) || (item->giTag == PW_REDFLAG && ps->powerups[PW_BLUEFLAG])) {
						return qtrue;
					}
				} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
					if (item->giTag == PW_REDFLAG || (item->giTag == PW_BLUEFLAG && ent->modelindex2) || (item->giTag == PW_BLUEFLAG && ps->powerups[PW_REDFLAG])) {
						return qtrue;
					}
				}
			}

			if (gametype == GT_1FCTF) {
				// neutral flag can always be picked up
				if (item->giTag == PW_NEUTRALFLAG) {
					return qtrue;
				}

				if (ps->persistant[PERS_TEAM] == TEAM_RED) {
					if (item->giTag == PW_BLUEFLAG && ps->powerups[PW_NEUTRALFLAG]) {
						return qtrue;
					}
				} else if (ps->persistant[PERS_TEAM] == TEAM_BLUE) {
					if (item->giTag == PW_REDFLAG && ps->powerups[PW_NEUTRALFLAG]) {
						return qtrue;
					}
				}
			}

			if (gametype == GT_HARVESTER) {
				return qtrue;
			}

			return qfalse;
		case IT_BAD:
			Com_Error(ERR_DROP, "BG_CanItemBeGrabbed: IT_BAD");
		default:
			Com_Printf("BG_CanItemBeGrabbed: unknown enum %d\n", item->giType);
	}

	return qfalse;
}

/*
=======================================================================================================================================
BG_EvaluateTrajectory
=======================================================================================================================================
*/
void BG_EvaluateTrajectory(const trajectory_t *tr, int atTime, vec3_t result) {
	float deltaTime;
	float phase;

	switch (tr->trType) {
		case TR_STATIONARY:
		case TR_INTERPOLATE:
			VectorCopy(tr->trBase, result);
			break;
		case TR_LINEAR:
			deltaTime = (atTime - tr->trTime) * 0.001; // milliseconds to seconds
			VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
			break;
		case TR_LINEAR_STOP:
			if (atTime > tr->trTime + tr->trDuration) {
				atTime = tr->trTime + tr->trDuration;
			}

			deltaTime = (atTime - tr->trTime) * 0.001; // milliseconds to seconds

			if (deltaTime < 0) {
				deltaTime = 0;
			}

			VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
			break;
		case TR_GRAVITY:
			deltaTime = (atTime - tr->trTime) * 0.001; // milliseconds to seconds
			VectorMA(tr->trBase, deltaTime, tr->trDelta, result);
			result[2] -= 0.5 * DEFAULT_GRAVITY * deltaTime * deltaTime; // FIXME: local gravity...
			break;
		case TR_SINE:
			deltaTime = (atTime - tr->trTime) / (float)tr->trDuration;
			phase = sin(deltaTime * M_PI * 2);
			VectorMA(tr->trBase, phase, tr->trDelta, result);
			break;
		default:
			Com_Error(ERR_DROP, "BG_EvaluateTrajectory: unknown trType: %i", tr->trType);
			break;
	}
}

/*
=======================================================================================================================================
BG_EvaluateTrajectoryDelta

For determining velocity at a given time.
=======================================================================================================================================
*/
void BG_EvaluateTrajectoryDelta(const trajectory_t *tr, int atTime, vec3_t result) {
	float deltaTime;
	float phase;

	switch (tr->trType) {
		case TR_STATIONARY:
		case TR_INTERPOLATE:
			VectorClear(result);
			break;
		case TR_LINEAR:
			VectorCopy(tr->trDelta, result);
			break;
		case TR_LINEAR_STOP:
			if (atTime > tr->trTime + tr->trDuration) {
				VectorClear(result);
				return;
			}

			VectorCopy(tr->trDelta, result);
			break;
		case TR_GRAVITY:
			deltaTime = (atTime - tr->trTime) * 0.001; // milliseconds to seconds
			VectorCopy(tr->trDelta, result);
			result[2] -= DEFAULT_GRAVITY * deltaTime; // FIXME: local gravity...
			break;
		case TR_SINE:
			deltaTime = (atTime - tr->trTime) / (float)tr->trDuration;
			phase = cos(deltaTime * M_PI * 2); // derivative of sin = cos
			phase *= 0.5;
			VectorScale(tr->trDelta, phase, result);
			break;
		default:
			Com_Error(ERR_DROP, "BG_EvaluateTrajectoryDelta: unknown trType: %i", tr->trType);
			break;
	}
}

char *eventnames[] = {
	"EV_NONE",
	"EV_FIRE_WEAPON",
	"EV_BULLET_HIT_FLESH",
	"EV_BULLET_HIT_WALL",
	"EV_MISSILE_HIT",
	"EV_MISSILE_MISS",
	"EV_RAILTRAIL",
	"EV_SHOTGUN",
	"EV_PROXIMITY_MINE_STICK",
	"EV_PROXIMITY_MINE_TRIGGER",
	"EV_GRENADE_BOUNCE",		// eventParm will be the soundindex
	"EV_KAMIKAZE",				// kamikaze explodes
	"EV_OBELISKEXPLODE",		// obelisk explodes
	"EV_OBELISKPAIN",			// obelisk is in pain
	"EV_STOPLOOPINGSOUND",
	"EV_GENERAL_SOUND",
	"EV_GLOBAL_SOUND",			// no attenuation
	"EV_GLOBAL_TEAM_SOUND",
	"EV_PAIN",
	"EV_DEATH1",
	"EV_DEATH2",
	"EV_DEATH3",
	"EV_OBITUARY",
	"EV_GIB_PLAYER",			// gib a previously living player
	"EV_FALL_DIE",
	"EV_FALL_DMG_50",
	"EV_FALL_DMG_25",
	"EV_FALL_DMG_15",
	"EV_FALL_DMG_10",
	"EV_FALL_DMG_5",
	"EV_FALL_SHORT",
	"EV_JUMP",
	"EV_JUMP_PAD",				// boing sound at origin, jump sound on player
	"EV_PLAYER_TELEPORT_IN",
	"EV_PLAYER_TELEPORT_OUT",
	"EV_NOAMMO",
	"EV_CHANGE_WEAPON",
	"EV_FOOTSTEP_HARD",
	"EV_FOOTSTEP_HARD_FROZEN",
	"EV_FOOTSTEP_HARD_SNOW",
	"EV_FOOTSTEP_HARD_SLUSH",
	"EV_FOOTSTEP_PUDDLE",
	"EV_FOOTSTEP_LEAVES",
	"EV_FOOTSTEP_BUSH",
	"EV_FOOTSTEP_GRASS",
	"EV_FOOTSTEP_LONGGRASS",
	"EV_FOOTSTEP_LONGGRASS_MUD",
	"EV_FOOTSTEP_SAND",
	"EV_FOOTSTEP_GRAVEL",
	"EV_FOOTSTEP_RUBBLE",
	"EV_FOOTSTEP_RUBBLE_WET",
	"EV_FOOTSTEP_SOIL",
	"EV_FOOTSTEP_MUD",
	"EV_FOOTSTEP_SNOW_DEEP",
	"EV_FOOTSTEP_ICE",
	"EV_FOOTSTEP_METAL_HOLLOW",
	"EV_FOOTSTEP_METAL_HOLLOW_FROZEN",
	"EV_FOOTSTEP_METAL_HOLLOW_SNOW",
	"EV_FOOTSTEP_METAL_HOLLOW_SLUSH",
	"EV_FOOTSTEP_METAL_HOLLOW_SPLASH",
	"EV_FOOTSTEP_GRATE_01",
	"EV_FOOTSTEP_GRATE_02",
	"EV_FOOTSTEP_DUCT",
	"EV_FOOTSTEP_PLATE",
	"EV_FOOTSTEP_FENCE",
	"EV_FOOTSTEP_WOOD_HOLLOW",
	"EV_FOOTSTEP_WOOD_HOLLOW_FROZEN",
	"EV_FOOTSTEP_WOOD_HOLLOW_SNOW",
	"EV_FOOTSTEP_WOOD_HOLLOW_SLUSH",
	"EV_FOOTSTEP_WOOD_HOLLOW_SPLASH",
	"EV_FOOTSTEP_WOOD_SOLID",
	"EV_FOOTSTEP_WOOD_CREAKING",
	"EV_FOOTSTEP_ROOF",
	"EV_FOOTSTEP_SHINGLES",
	"EV_FOOTSTEP_SOFT",
	"EV_FOOTSTEP_GLASS_SHARDS",
	"EV_FOOTSTEP_TRASH_GLASS",
	"EV_FOOTSTEP_TRASH_DEBRIS",
	"EV_FOOTSTEP_TRASH_WIRE",
	"EV_FOOTSTEP_TRASH_PACKING",
	"EV_FOOTSTEP_TRASH_PLASTIC",
	"EV_FOOTSPLASH",
	"EV_FOOTWADE",
	"EV_SWIM",
	"EV_WATER_TOUCH",			// foot touches
	"EV_WATER_LEAVE",			// foot leaves
	"EV_WATER_UNDER",			// head touches
	"EV_WATER_CLEAR",			// head leaves
	"EV_TAUNT",
	"EV_TAUNT_YES",
	"EV_TAUNT_NO",
	"EV_TAUNT_FOLLOWME",
	"EV_TAUNT_GETFLAG",
	"EV_TAUNT_GUARDBASE",
	"EV_TAUNT_PATROL",
	"EV_STEP_4",
	"EV_STEP_8",
	"EV_STEP_12",
	"EV_STEP_16",
	"EV_ITEM_POP",
	"EV_ITEM_RESPAWN",
	"EV_ITEM_PICKUP",			// normal item pickups are predictable
	"EV_GLOBAL_ITEM_PICKUP",	// powerup/team sounds are broadcast to everyone
	"EV_POWERUP_QUAD",
	"EV_POWERUP_REGEN",
	"EV_USE_ITEM0",
	"EV_USE_ITEM1",
	"EV_USE_ITEM2",
	"EV_USE_ITEM3",
	"EV_USE_ITEM4",
	"EV_USE_ITEM5",
	"EV_USE_ITEM6",
	"EV_USE_ITEM7",
	"EV_USE_ITEM8",
	"EV_USE_ITEM9",
	"EV_USE_ITEM10",
	"EV_USE_ITEM11",
	"EV_USE_ITEM12",
	"EV_USE_ITEM13",
	"EV_USE_ITEM14",
	"EV_USE_ITEM15",
	"EV_SCOREPLUM",				// score plum
	"EV_LIGHTNINGBOLT",			// lightning bolt
	"EV_DEBUG_LINE"
};

/*
=======================================================================================================================================
BG_AddPredictableEventToPlayerstate

Handles the sequence numbers.
=======================================================================================================================================
*/
void BG_AddPredictableEventToPlayerstate(int newEvent, int eventParm, playerState_t *ps) {
#ifdef _DEBUG
	{
		char buf[256];

		trap_Cvar_VariableStringBuffer("showevents", buf, sizeof(buf));

		if (atof(buf) != 0) {
#ifdef QAGAME
			Com_Printf(" game event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#else
			Com_Printf("Cgame event svt %5d -> %5d: num = %20s parm %d\n", ps->pmove_framecount/*ps->commandTime*/, ps->eventSequence, eventnames[newEvent], eventParm);
#endif
		}
	}
#endif
	ps->events[ps->eventSequence & (MAX_PS_EVENTS - 1)] = newEvent;
	ps->eventParms[ps->eventSequence & (MAX_PS_EVENTS - 1)] = eventParm;
	ps->eventSequence++;
}

/*
=======================================================================================================================================
BG_TouchJumpPad
=======================================================================================================================================
*/
void BG_TouchJumpPad(playerState_t *ps, entityState_t *jumppad) {
	vec3_t angles;
	float p;
	int effectNum;

	// spectators don't use jump pads
	if (ps->pm_type != PM_NORMAL) {
		return;
	}
	// if we didn't hit this same jumppad the previous frame then don't play the event sound again if we are in a fat trigger
	if (ps->jumppad_ent != jumppad->number) {
		VectorToAngles(jumppad->origin2, angles);

		p = fabs(AngleNormalize180(angles[PITCH]));

		if (p < 45) {
			effectNum = 0;
		} else {
			effectNum = 1;
		}

		BG_AddPredictableEventToPlayerstate(EV_JUMP_PAD, effectNum, ps);
	}
	// remember hitting this jumppad this frame
	ps->jumppad_ent = jumppad->number;
	ps->jumppad_frame = ps->pmove_framecount;
	// give the player the velocity from the jumppad
	VectorCopy(jumppad->origin2, ps->velocity);
}

/*
=======================================================================================================================================
BG_PlayerStateToEntityState

This is done after each set of usercmd_t on the server, and after local prediction on the client.
=======================================================================================================================================
*/
void BG_PlayerStateToEntityState(playerState_t *ps, entityState_t *s, qboolean snap) {
	int i;

	if (ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR) {
		s->eType = ET_INVISIBLE;
	} else if (ps->stats[STAT_HEALTH] <= GIB_HEALTH) {
		s->eType = ET_INVISIBLE;
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->clientNum;
	s->pos.trType = TR_INTERPOLATE;

	VectorCopy(ps->origin, s->pos.trBase);

	if (snap) {
		SnapVector(s->pos.trBase);
	}
	// set the trDelta for flag direction
	VectorCopy(ps->velocity, s->pos.trDelta);

	s->apos.trType = TR_INTERPOLATE;

	VectorCopy(ps->viewangles, s->apos.trBase);

	if (snap) {
		SnapVector(s->apos.trBase);
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum; // ET_PLAYER looks here instead of at number, so corpses can also reference the proper config
	s->eFlags = ps->eFlags;

	if (ps->stats[STAT_HEALTH] <= 0) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if (ps->externalEvent) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if (ps->entityEventSequence < ps->eventSequence) {
		int seq;

		if (ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}

		seq = ps->entityEventSequence & (MAX_PS_EVENTS - 1);
		s->event = ps->events[seq]|((ps->entityEventSequence & 3) << 8);
		s->eventParm = ps->eventParms[seq];
		ps->entityEventSequence++;
	}

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;
	s->powerups = 0;

	for (i = 0; i < MAX_POWERUPS; i++) {
		if (ps->powerups[i]) {
			s->powerups |= 1 << i;
		}
	}

	s->team = ps->persistant[PERS_TEAM];

	if (ps->stats[STAT_HEALTH] <= 0) {
		s->skinFraction = 1.0f;
	} else if (ps->stats[STAT_HEALTH] >= 100) {
		s->skinFraction = 0.0f;
	} else {
		s->skinFraction = 1.0f - ((float)ps->stats[STAT_HEALTH] * 0.01);
	}

	s->loopSound = ps->loopSound;
	s->tokens = ps->tokens;
}

/*
=======================================================================================================================================
BG_PlayerStateToEntityStateExtraPolate

This is done after each set of usercmd_t on the server, and after local prediction on the client.
=======================================================================================================================================
*/
void BG_PlayerStateToEntityStateExtraPolate(playerState_t *ps, entityState_t *s, int time, qboolean snap) {
	int i;

	if (ps->pm_type == PM_INTERMISSION || ps->pm_type == PM_SPECTATOR) {
		s->eType = ET_INVISIBLE;
	} else if (ps->stats[STAT_HEALTH] <= GIB_HEALTH) {
		s->eType = ET_INVISIBLE;
	} else {
		s->eType = ET_PLAYER;
	}

	s->number = ps->clientNum;
	s->pos.trType = TR_LINEAR_STOP;

	VectorCopy(ps->origin, s->pos.trBase);

	if (snap) {
		SnapVector(s->pos.trBase);
	}
	// set the trDelta for flag direction and linear prediction
	VectorCopy(ps->velocity, s->pos.trDelta);
	// set the time for linear prediction
	s->pos.trTime = time;
	// set maximum extra polation time
	s->pos.trDuration = 50; // 1000 / sv_fps (default = 20)
	s->apos.trType = TR_INTERPOLATE;

	VectorCopy(ps->viewangles, s->apos.trBase);

	if (snap) {
		SnapVector(s->apos.trBase);
	}

	s->angles2[YAW] = ps->movementDir;
	s->legsAnim = ps->legsAnim;
	s->torsoAnim = ps->torsoAnim;
	s->clientNum = ps->clientNum; // ET_PLAYER looks here instead of at number, so corpses can also reference the proper config
	s->eFlags = ps->eFlags;

	if (ps->stats[STAT_HEALTH] <= 0) {
		s->eFlags |= EF_DEAD;
	} else {
		s->eFlags &= ~EF_DEAD;
	}

	if (ps->externalEvent) {
		s->event = ps->externalEvent;
		s->eventParm = ps->externalEventParm;
	} else if (ps->entityEventSequence < ps->eventSequence) {
		int seq;

		if (ps->entityEventSequence < ps->eventSequence - MAX_PS_EVENTS) {
			ps->entityEventSequence = ps->eventSequence - MAX_PS_EVENTS;
		}

		seq = ps->entityEventSequence & (MAX_PS_EVENTS - 1);
		s->event = ps->events[seq]|((ps->entityEventSequence & 3) << 8);
		s->eventParm = ps->eventParms[seq];
		ps->entityEventSequence++;
	}

	s->weapon = ps->weapon;
	s->groundEntityNum = ps->groundEntityNum;
	s->powerups = 0;

	for (i = 0; i < MAX_POWERUPS; i++) {
		if (ps->powerups[i]) {
			s->powerups |= 1 << i;
		}
	}

	s->team = ps->persistant[PERS_TEAM];

	if (ps->stats[STAT_HEALTH] <= 0) {
		s->skinFraction = 1.0f;
	} else if (ps->stats[STAT_HEALTH] >= 100) {
		s->skinFraction = 0.0f;
	} else {
		s->skinFraction = 1.0f - ((float)ps->stats[STAT_HEALTH] * 0.01);
	}

	s->loopSound = ps->loopSound;
	s->tokens = ps->tokens;
}

/*
=======================================================================================================================================
SnapVectorTowards

Round a vector to integers for more efficient network transmission, but make sure that it rounds towards a given point rather than
blindly truncating. This prevents it from truncating into a wall.
=======================================================================================================================================
*/
void SnapVectorTowards(vec3_t v, vec3_t to) {
	int i;

	for (i = 0; i < 3; i++) {
		if (to[i] <= v[i]) {
			v[i] = floor(v[i]);
		} else {
			v[i] = ceil(v[i]);
		}
	}
}

/*
=======================================================================================================================================
cmdcmp
=======================================================================================================================================
*/
int cmdcmp(const void *a, const void *b) {
	return Q_stricmp((const char *)a, ((dummyCmd_t *)b)->name);
}
