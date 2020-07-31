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

#ifndef DEATHMATCH
#define DEATHMATCH 1
#endif
#ifndef DMFLAGS
#define DMFLAGS 0
#endif

#define DF_NO_HEALTH		    1 // Health, Adrenaline and the AncientHead are removed
#define DF_NO_ITEMS			    2 // all powerups are removed
#define DF_WEAPONS_STAY		    4 // weapons stay
#define DF_NO_FALLING		    8 // no falling damage
#define DF_INSTANT_ITEMS	   16 // powerups are used instantly when picked up
#define DF_SAME_LEVEL		   32 // same level is played over and over again
#define DF_SKINTEAMS		   64 // teamplay based on skin
#define DF_MODELTEAMS		  128 // eamplay based on model
#define DF_NO_FRIENDLY_FIRE	  256 // no friendly fire
#define DF_SPAWN_FARTHEST	  512 // spawn as far away from the death point
#define DF_FORCE_RESPAWN	 1024 // force players to respawn
#define DF_NO_ARMOR			 2048 // All armor is removed
#define DF_ALLOW_EXIT		 4096 // allow player to exit the map
#define DF_INFINITE_AMMO	 8192 // all ammo and the bfg are removed
#define DF_QUAD_DROP		16384 // drop the quad when died when using
#define DF_FIXED_FOV		32768 // field of vision is fixed
