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

#define INVENTORY_NONE				  0
// health
#define INVENTORY_HEALTH			  1
// armor
#define INVENTORY_ARMOR				  2
// weapons
#define INVENTORY_GAUNTLET			  3
#define INVENTORY_MACHINEGUN		  4
#define INVENTORY_CHAINGUN			  5
#define INVENTORY_SHOTGUN			  6
#define INVENTORY_NAILGUN			  7
#define INVENTORY_PROXLAUNCHER		  8
#define INVENTORY_GRENADELAUNCHER	  9
#define INVENTORY_NAPALMLAUNCHER	 10
#define INVENTORY_ROCKETLAUNCHER	 11
#define INVENTORY_BEAMGUN			 12
#define INVENTORY_RAILGUN			 13
#define INVENTORY_PLASMAGUN			 14
#define INVENTORY_BFG10K			 15
// ammo
#define INVENTORY_BULLETS			 16
#define INVENTORY_BELT				 17
#define INVENTORY_SHELLS			 18
#define INVENTORY_NAILS				 19
#define INVENTORY_MINES				 20
#define INVENTORY_GRENADES			 21
#define INVENTORY_CANISTERS			 22
#define INVENTORY_ROCKETS			 23
#define INVENTORY_BEAMGUN_AMMO		 24
#define INVENTORY_SLUGS				 25
#define INVENTORY_CELLS				 26
#define INVENTORY_BFG_AMMO			 27
// holdables
#define INVENTORY_MEDKIT			 28
#define INVENTORY_KAMIKAZE			 29
// powerups
#define INVENTORY_QUAD				 30
#define INVENTORY_INVISIBILITY		 31
#define INVENTORY_REGEN				 32
#define INVENTORY_AMMOREGEN			 33
#define INVENTORY_GUARD				 34
#define INVENTORY_DOUBLER			 35
#define INVENTORY_SCOUT				 36
// team items
#define INVENTORY_REDFLAG			 37
#define INVENTORY_BLUEFLAG			 38
#define INVENTORY_NEUTRALFLAG		 39
#define INVENTORY_REDCUBE			 40
#define INVENTORY_BLUECUBE			 41
// bot stuff
#define BOT_IS_IN_HURRY				 42
#define NUM_VISIBLE_TEAMMATES		 43
// enemy stuff
#define NUM_VISIBLE_ENEMIES			 44
#define ENEMY_HORIZONTAL_DIST		 45
#define ENEMY_HEIGHT				 46
// entity stuff
#define ENTITY_IS_AN_OBELISK		 47
//************************************************************************
// ITEM NUMBERS (make sure they are in sync with bg_itemlist in bg_misc.c)
//************************************************************************
#define MODELINDEX_HEALTHSMALL		  1
#define MODELINDEX_HEALTH			  2
#define MODELINDEX_HEALTHLARGE		  3
#define MODELINDEX_HEALTHMEGA		  4
#define MODELINDEX_ARMORSHARD		  5
#define MODELINDEX_ARMORCOMBAT		  6
#define MODELINDEX_ARMORBODY		  7
#define MODELINDEX_ARMORFULL		  8
#define MODELINDEX_GAUNTLET			  9
#define MODELINDEX_MACHINEGUN		 10
#define MODELINDEX_CHAINGUN			 11
#define MODELINDEX_SHOTGUN			 12
#define MODELINDEX_NAILGUN			 13
#define MODELINDEX_PROXLAUNCHER		 14
#define MODELINDEX_GRENADELAUNCHER	 15
#define MODELINDEX_NAPALMLAUNCHER	 16
#define MODELINDEX_ROCKETLAUNCHER	 17
#define MODELINDEX_BEAMGUN			 18
#define MODELINDEX_RAILGUN			 19
#define MODELINDEX_PLASMAGUN		 20
#define MODELINDEX_BFG10K			 21
// ammo
#define MODELINDEX_BULLETS			 22
#define MODELINDEX_BELT				 23
#define MODELINDEX_SHELLS			 24
#define MODELINDEX_NAILS			 25
#define MODELINDEX_MINES			 26
#define MODELINDEX_GRENADES			 27
#define MODELINDEX_CANISTERS		 28
#define MODELINDEX_ROCKETS			 29
#define MODELINDEX_BEAMGUN_AMMO		 30
#define MODELINDEX_SLUGS			 31
#define MODELINDEX_CELLS			 32
#define MODELINDEX_BFG_AMMO			 33
// holdables
#define MODELINDEX_MEDKIT			 34
#define MODELINDEX_KAMIKAZE			 35
// powerups
#define MODELINDEX_QUAD				 36
#define MODELINDEX_INVISIBILITY		 37
#define MODELINDEX_REGEN			 38
#define MODELINDEX_AMMOREGEN		 39
#define MODELINDEX_GUARD			 40
#define MODELINDEX_DOUBLER			 41
#define MODELINDEX_SCOUT			 42
// team items
#define MODELINDEX_REDFLAG			 43
#define MODELINDEX_BLUEFLAG			 44
#define MODELINDEX_NEUTRALFLAG		 45
#define MODELINDEX_REDCUBE			 46
#define MODELINDEX_BLUECUBE			 47
//************************************************************************
// WEAPON NUMBERS
//************************************************************************
#define WEAPONINDEX_GAUNTLET			 1
#define WEAPONINDEX_MACHINEGUN			 2
#define WEAPONINDEX_CHAINGUN			 3
#define WEAPONINDEX_SHOTGUN				 4
#define WEAPONINDEX_NAILGUN				 5
#define WEAPONINDEX_PROXLAUNCHER		 6
#define WEAPONINDEX_GRENADELAUNCHER		 7
#define WEAPONINDEX_NAPALMLAUNCHER		 8
#define WEAPONINDEX_ROCKETLAUNCHER		 9
#define WEAPONINDEX_BEAMGUN				10
#define WEAPONINDEX_RAILGUN				11
#define WEAPONINDEX_PLASMAGUN			12
#define WEAPONINDEX_BFG					13
