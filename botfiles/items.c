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

#include "inv.h"

#define ITEM_NONE		0
#define ITEM_HEALTH		1
#define ITEM_ARMOR		2
#define ITEM_WEAPON		3
#define ITEM_AMMO		4
#define ITEM_POWERUP	5
#define ITEM_FLAG		6
#define ITEM_ROAM		7
#define ITEM_KEY		8

//===================================
// HEALTH
//===================================
iteminfo "item_health_small"
{
	name		"5 Health"
	model		"models/powerups/health/small_cross.md3"
	modelindex	MODELINDEX_HEALTHSMALL
	type		ITEM_HEALTH
	index		INVENTORY_HEALTH
	respawntime	35
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_health"
{
	name		"25 Health"
	model		"models/powerups/health/medium_cross.md3"
	modelindex	MODELINDEX_HEALTH
	type		ITEM_HEALTH
	index		INVENTORY_HEALTH
	respawntime	35
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_health_large"
{
	name		"50 Health"
	model		"models/powerups/health/large_cross.md3"
	modelindex	MODELINDEX_HEALTHLARGE
	type		ITEM_HEALTH
	index		INVENTORY_HEALTH
	respawntime	35
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_health_mega"
{
	name		"Mega Health"
	model		"models/powerups/health/mega_cross.md3"
	modelindex	MODELINDEX_HEALTHMEGA
	type		ITEM_HEALTH
	index		INVENTORY_HEALTH
	respawntime	35
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

//===================================
// ARMOR
//===================================
iteminfo "item_armor_shard"
{
	name		"Armor Shard"
	model		"models/powerups/armor/shard.md3"
	modelindex	MODELINDEX_ARMORSHARD
	type		ITEM_ARMOR
	index		INVENTORY_ARMOR
	respawntime	25
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_armor_combat"
{
	name		"Armor"
	model		"models/powerups/armor/armor_gre.md3"
	modelindex	MODELINDEX_ARMORCOMBAT
	type		ITEM_ARMOR
	index		INVENTORY_ARMOR
	respawntime	25
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_armor_body"
{
	name		"Heavy Armor"
	model		"models/powerups/armor/armor_yel.md3"
	modelindex	MODELINDEX_ARMORBODY
	type		ITEM_ARMOR
	index		INVENTORY_ARMOR
	respawntime	25
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_armor_full"
{
	name		"Full Armor"
	model		"models/powerups/armor/armor_red.md3"
	modelindex	MODELINDEX_ARMORFULL
	type		ITEM_ARMOR
	index		INVENTORY_ARMOR
	respawntime	25
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

//===================================
// WEAPONS
//===================================
iteminfo "weapon_gauntlet"
{
	name		"Gauntlet"
	model		"models/weapons2/gauntlet/gauntlet.md3"
	modelindex	MODELINDEX_GAUNTLET
	type		ITEM_WEAPON
	index		INVENTORY_GAUNTLET
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_machinegun"
{
	name		"Machine Gun"
	model		"models/weapons2/machinegun/machinegun.md3"
	modelindex	MODELINDEX_MACHINEGUN
	type		ITEM_WEAPON
	index		INVENTORY_MACHINEGUN
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_chaingun"
{
	name		"Chain Gun"
	model		"models/weapons/vulcan/vulcan.md3"
	modelindex	MODELINDEX_CHAINGUN
	type		ITEM_WEAPON
	index		INVENTORY_CHAINGUN
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_shotgun"
{
	name		"Shotgun"
	model		"models/weapons2/shotgun/shotgun.md3"
	modelindex	MODELINDEX_SHOTGUN
	type		ITEM_WEAPON
	index		INVENTORY_SHOTGUN
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_nailgun"
{
	name		"Nail Gun"
	model		"models/weapons/nailgun/nailgun.md3"
	modelindex	MODELINDEX_NAILGUN
	type		ITEM_WEAPON
	index		INVENTORY_NAILGUN
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_proxylauncher"
{
	name		"Proximity Launcher"
	model		"models/weapons/proxmine/proxmine.md3"
	modelindex	MODELINDEX_PROXLAUNCHER
	type		ITEM_WEAPON
	index		INVENTORY_PROXLAUNCHER
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_grenadelauncher"
{
	name		"Grenade Launcher"
	model		"models/weapons2/grenadel/grenadel.md3"
	modelindex	MODELINDEX_GRENADELAUNCHER
	type		ITEM_WEAPON
	index		INVENTORY_GRENADELAUNCHER
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_napalmlauncher"
{
	name		"Napalm Launcher"
	model		"models/weapons2/napalml/napalml.md3"
	modelindex	MODELINDEX_NAPALMLAUNCHER
	type		ITEM_WEAPON
	index		INVENTORY_NAPALMLAUNCHER
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_rocketlauncher"
{
	name		"Rocket Launcher"
	model		"models/weapons2/rocketl/rocketl.md3"
	modelindex	MODELINDEX_ROCKETLAUNCHER
	type		ITEM_WEAPON
	index		INVENTORY_ROCKETLAUNCHER
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_beamgun"
{
	name		"Beam Gun"
	model		"models/weapons2/lightning/lightning.md3"
	modelindex	MODELINDEX_BEAMGUN
	type		ITEM_WEAPON
	index		INVENTORY_BEAMGUN
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_railgun"
{
	name		"Railgun"
	model		"models/weapons2/railgun/railgun.md3"
	modelindex	MODELINDEX_RAILGUN
	type		ITEM_WEAPON
	index		INVENTORY_RAILGUN
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_plasmagun"
{
	name		"Plasma Gun"
	model		"models/weapons2/plasma/plasma.md3"
	modelindex	MODELINDEX_PLASMAGUN
	type		ITEM_WEAPON
	index		INVENTORY_PLASMAGUN
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "weapon_bfg"
{
	name		"BFG 10K"
	model		"models/weapons2/bfg/bfg.md3"
	modelindex	MODELINDEX_BFG10K
	type		ITEM_WEAPON
	index		INVENTORY_BFG10K
	respawntime	10
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

//===================================
// AMMO
//===================================
iteminfo "ammo_bullets"
{
	name		"Bullets"
	model		"models/powerups/ammo/machinegunam.md3"
	modelindex	MODELINDEX_BULLETS
	type		ITEM_AMMO
	index		INVENTORY_BULLETS
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_belt"
{
	name		"Chain Gun Belt"
	model		"models/powerups/ammo/chaingunam.md3"
	modelindex	MODELINDEX_BELT
	type		ITEM_AMMO
	index		INVENTORY_BELT
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_shells"
{
	name		"Shells"
	model		"models/powerups/ammo/shotgunam.md3"
	modelindex	MODELINDEX_SHELLS
	type		ITEM_AMMO
	index		INVENTORY_SHELLS
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_nails"
{
	name		"Nails"
	model		"models/powerups/ammo/nailgunam.md3"
	modelindex	MODELINDEX_NAILS
	type		ITEM_AMMO
	index		INVENTORY_NAILS
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_mines"
{
	name		"Proximity Mines"
	model		"models/powerups/ammo/proxmineam.md3"
	modelindex	MODELINDEX_MINES
	type		ITEM_AMMO
	index		INVENTORY_MINES
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_grenades"
{
	name		"Grenades"
	model		"models/powerups/ammo/grenadeam.md3"
	modelindex	MODELINDEX_GRENADES
	type		ITEM_AMMO
	index		INVENTORY_GRENADES
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_canisters"
{
	name		"Canisters"
	model		"models/powerups/ammo/napalmam.md3"
	modelindex	MODELINDEX_CANISTERS
	type		ITEM_AMMO
	index		INVENTORY_CANISTERS
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_rockets"
{
	name		"Rockets"
	model		"models/powerups/ammo/rocketam.dm3"
	modelindex	MODELINDEX_ROCKETS
	type		ITEM_AMMO
	index		INVENTORY_ROCKETS
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_cartridge"
{
	name		"Cartridge"
	model		"models/powerups/ammo/lightningam.md3"
	modelindex	MODELINDEX_BEAMGUN_AMMO
	type		ITEM_AMMO
	index		INVENTORY_BEAMGUN_AMMO
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_slugs"
{
	name		"Slugs"
	model		"models/powerups/ammo/railgunam.md3"
	modelindex	MODELINDEX_SLUGS
	type		ITEM_AMMO
	index		INVENTORY_SLUGS
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_cells"
{
	name		"Cells"
	model		"models/powerups/ammo/plasmaam.md3"
	modelindex	MODELINDEX_CELLS
	type		ITEM_AMMO
	index		INVENTORY_CELLS
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "ammo_bfg"
{
	name		"Bfg Ammo"
	model		"models/powerups/ammo/bfgam.md3"
	modelindex	MODELINDEX_BFG_AMMO
	type		ITEM_AMMO
	index		INVENTORY_BFG_AMMO
	respawntime	40
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

//===================================
// HOLDABLES
//===================================
iteminfo "holdable_medkit"
{
	name		"Medkit"
	model		"models/powerups/holdable/medkit.md3"
	modelindex	MODELINDEX_MEDKIT
	type		ITEM_POWERUP
	index		INVENTORY_MEDKIT
	respawntime	60
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "holdable_kamikaze"
{
	name		"Kamikaze"
	model		"models/powerups/kamikazi.md3"
	modelindex	MODELINDEX_KAMIKAZE
	type		ITEM_POWERUP
	index		INVENTORY_KAMIKAZE
	respawntime	60
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

//===================================
// POWERUPS
//===================================
iteminfo "item_quad"
{
	name		"Quad Damage"
	model		"models/powerups/instant/quad.md3"
	modelindex	MODELINDEX_QUAD
	type		ITEM_POWERUP
	index		INVENTORY_QUAD
	respawntime	120
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_invisibility"
{
	name		"Invisibility"
	model		"models/powerups/instant/invis.md3"
	modelindex	MODELINDEX_INVISIBILITY
	type		ITEM_POWERUP
	index		INVENTORY_INVISIBILITY
	respawntime	120
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_regen"
{
	name		"Regeneration"
	model		"models/powerups/instant/regen_ring.md3"
	modelindex	MODELINDEX_REGEN
	type		ITEM_POWERUP
	index		INVENTORY_REGEN
	respawntime	120
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

//===================================
// PERSISTANT POWERUPS
//===================================
iteminfo "item_ammoregen"
{
	name		"Ammo Regen"
	model		"models/powerups/ammo.md3"
	modelindex	MODELINDEX_AMMOREGEN
	type		ITEM_POWERUP
	index		INVENTORY_AMMOREGEN
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_guard"
{
	name		"Guard"
	model		"models/powerups/guard.md3"
	modelindex	MODELINDEX_GUARD
	type		ITEM_POWERUP
	index		INVENTORY_GUARD
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_doubler"
{
	name		"Doubler"
	model		"models/powerups/doubler.md3"
	modelindex	MODELINDEX_DOUBLER
	type		ITEM_POWERUP
	index		INVENTORY_DOUBLER
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_scout"
{
	name		"Scout"
	model		"models/powerups/scout.md3"
	modelindex	MODELINDEX_SCOUT
	type		ITEM_POWERUP
	index		INVENTORY_SCOUT
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

//===================================
// TEAM ITEMS
//===================================
iteminfo "team_CTF_redflag"
{
	name		"Red Flag"
	model		"models/flags/r_flag.md3"
	modelindex	MODELINDEX_REDFLAG
	type		ITEM_FLAG
	index		INVENTORY_REDFLAG
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "team_CTF_blueflag"
{
	name		"Blue Flag"
	model		"models/flags/b_flag.md3"
	modelindex	MODELINDEX_BLUEFLAG
	type		ITEM_FLAG
	index		INVENTORY_BLUEFLAG
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "team_CTF_neutralflag"
{
	name		"Neutral Flag"
	model		"models/flags/n_flag.md3"
	modelindex	MODELINDEX_NEUTRALFLAG
	type		ITEM_FLAG
	index		INVENTORY_NEUTRALFLAG
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "team_redobelisk"
{
	name		"Red Obelisk"
	model		"models/powerups/obelisk/obelisk.md3"
	modelindex	0
	type		ITEM_NONE
	index		0
	mins		{-15, -15, 1}
	maxs		{15, 15, 87}
}

iteminfo "team_blueobelisk"
{
	name		"Blue Obelisk"
	model		"models/powerups/obelisk/obelisk.md3"
	modelindex	0
	type		ITEM_NONE
	index		0
	mins		{-15, -15, 1}
	maxs		{15, 15, 87}
}

iteminfo "team_neutralobelisk"
{
	name		"Neutral Obelisk"
	model		"models/powerups/obelisk/obelisk.md3"
	modelindex	0
	type		ITEM_NONE
	index		0
	mins		{-15, -15, 1}
	maxs		{15, 15, 87}
}

iteminfo "item_redcube"
{
	name		"Red Cube"
	model		"models/powerups/orb/r_orb.md3"
	modelindex	MODELINDEX_REDCUBE
	type		ITEM_FLAG
	index		INVENTORY_REDCUBE
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

iteminfo "item_bluecube"
{
	name		"Blue Cube"
	model		"models/powerups/orb/b_orb.md3"
	modelindex	MODELINDEX_BLUECUBE
	type		ITEM_FLAG
	index		INVENTORY_BLUECUBE
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}

//===================================
// bot roam item
//===================================
iteminfo "item_botroam"
{
	name		"Bot Roam Goal"
	model		""
	modelindex	0
	type		ITEM_ROAM
	index		0
	mins		{-15,-15,-15}
	maxs		{15,15,15}
}
