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
 This file must be identical in the Quake and utils directories!
 Contents flags are separate bits. A given brush can contribute multiple content bits.
**************************************************************************************************************************************/

/*
=======================================================================================================================================
	CONTENTS
=======================================================================================================================================
*/

#define CONTENTS_SOLID			0x00000001 // an eye is never valid in a solid
#define CONTENTS_LAVA			0x00000008
#define CONTENTS_SLIME			0x00000010
#define CONTENTS_WATER			0x00000020
#define CONTENTS_FOG			0x00000040
#define CONTENTS_NOTTEAM1		0x00000080
#define CONTENTS_NOTTEAM2		0x00000100
#define CONTENTS_NOBOTCLIP		0x00000200
#define CONTENTS_LADDER			0x00000400
#define CONTENTS_AREAPORTAL		0x00008000
#define CONTENTS_PLAYERCLIP		0x00010000
#define CONTENTS_MONSTERCLIP	0x00020000
// bot specific contents types
#define CONTENTS_TELEPORTER		0x00040000
#define CONTENTS_JUMPPAD		0x00080000
#define CONTENTS_CLUSTERPORTAL	0x00100000
#define CONTENTS_DONOTENTER		0x00200000
#define CONTENTS_BOTCLIP		0x00400000
#define CONTENTS_MOVER			0x00800000
#define CONTENTS_ORIGIN			0x01000000 // removed before bsping an entity
#define CONTENTS_BODY			0x02000000 // should never be on a brush, only in game
#define CONTENTS_CORPSE			0x04000000
#define CONTENTS_DETAIL			0x08000000 // brushes not used for the bsp
#define CONTENTS_STRUCTURAL		0x10000000 // brushes used for the bsp
#define CONTENTS_TRANSLUCENT	0x20000000 // don't consume surface fragments inside
#define CONTENTS_TRIGGER		0x40000000
#define CONTENTS_NODROP			0x80000000 // don't leave bodies or items (death fog, lava)

#define CONTENTS_LIQUID_MASK	(CONTENTS_LAVA|CONTENTS_SLIME|CONTENTS_WATER)

/*
=======================================================================================================================================
	SURFACE/MATERIAL
=======================================================================================================================================
*/

#define SURF_NODAMAGE			0x00000001 // never give falling damage
#define SURF_SLICK				0x00000002 // effects game physics
#define SURF_SKY				0x00000004 // lighting from environment map
#define SURF_LADDER				0x00000008
#define SURF_NOIMPACT			0x00000010 // don't make missile explosions
#define SURF_NOMARKS			0x00000020 // don't leave missile marks
//#define SURF_FLESH				0x00000040 // make flesh sounds and effects // Tobias NOTE: replaced by MAT_*
#define SURF_NODRAW				0x00000080 // don't generate a drawsurface at all
#define SURF_HINT				0x00000100 // make a primary bsp splitter
#define SURF_SKIP				0x00000200 // completely ignore, allowing non-closed brushes
#define SURF_NOLIGHTMAP			0x00000400 // surface doesn't need a lightmap
#define SURF_POINTLIGHT			0x00000800 // generate lighting info at vertexes
//#define SURF_METALSTEPS			0x00001000 // clanking footsteps // Tobias NOTE: replaced by MAT_*
#define SURF_NOSTEPS			0x00002000 // no footstep sounds
#define SURF_NONSOLID			0x00004000 // don't collide against curves with this set
#define SURF_LIGHTFILTER		0x00008000 // act as a light filter during q3map -light
#define SURF_ALPHASHADOW		0x00010000 // do per-pixel light shadow casting in q3map
#define SURF_NODLIGHT			0x00020000 // don't dlight even if solid (solid lava, skies)
//#define SURF_DUST				0x00040000 // leave a dust trail when walking on this surface // Tobias NOTE: replaced by MAT_*

#define SURF_MATERIAL_MASK		0x0FF00000

#define MAT_NONE					0x00000000 // for when the artist hasn't set anything up
//#define MAT_						0x00100000
//#define MAT_						0x00200000
#define MAT_LEAVES_01_GR_COL_01		0x00300000 // dried up leaves on the floor
#define MAT_LEAVES_01_GR_COL_02		0x00400000
#define MAT_LEAVES_01_GR_COL_03		0x00500000
#define MAT_LEAVES_01_GR_COL_04		0x00600000
#define MAT_BUSH_01_GR_COL_01		0x00700000
#define MAT_BUSH_01_GR_COL_02		0x00800000
#define MAT_BUSH_01_GR_COL_03		0x00900000
#define MAT_BUSH_01_GR_COL_04		0x00A00000
#define MAT_BUSH_02_GR_COL_01		0x00B00000
#define MAT_BUSH_02_GR_COL_02		0x00C00000
#define MAT_BUSH_02_GR_COL_03		0x00D00000
#define MAT_BUSH_02_GR_COL_04		0x00E00000
#define MAT_SHORTGRASS_01_GR_COL_01	0x00F00000 // e.g.: manicured lawn
#define MAT_SHORTGRASS_01_GR_COL_02	0x01000000
#define MAT_SHORTGRASS_01_GR_COL_03	0x01100000
#define MAT_SHORTGRASS_01_GR_COL_04	0x01200000
#define MAT_SHORTGRASS_02_GR_COL_01	0x01300000
#define MAT_SHORTGRASS_02_GR_COL_02	0x01400000
#define MAT_SHORTGRASS_02_GR_COL_03	0x01500000
#define MAT_SHORTGRASS_02_GR_COL_04	0x01600000
#define MAT_LONGGRASS_01_GR_COL_01	0x01700000 // long grass
#define MAT_LONGGRASS_01_GR_COL_02	0x01800000
#define MAT_LONGGRASS_01_GR_COL_03	0x01900000
#define MAT_LONGGRASS_01_GR_COL_04	0x01A00000
#define MAT_LONGGRASS_02_GR_COL_01	0x01B00000
#define MAT_LONGGRASS_02_GR_COL_02	0x01C00000
#define MAT_LONGGRASS_02_GR_COL_03	0x01D00000
#define MAT_LONGGRASS_02_GR_COL_04	0x01E00000
#define MAT_LONGGRASS_MUD_GR_COL_01	0x01F00000
#define MAT_LONGGRASS_MUD_GR_COL_02	0x02000000
#define MAT_LONGGRASS_MUD_GR_COL_03	0x02100000
#define MAT_LONGGRASS_MUD_GR_COL_04	0x02200000
#define MAT_SAND_GR_COL_01			0x02300000 // sandy beach
#define MAT_SAND_GR_COL_02			0x02400000
#define MAT_SAND_GR_COL_03			0x02500000
#define MAT_SAND_GR_COL_04			0x02600000
#define MAT_GRAVEL_GR_COL_01		0x02700000 // lots of small stones
#define MAT_GRAVEL_GR_COL_02		0x02800000
#define MAT_GRAVEL_GR_COL_03		0x02900000
#define MAT_GRAVEL_GR_COL_04		0x02A00000
#define MAT_RUBBLE_GR_COL_01		0x02B00000
#define MAT_RUBBLE_GR_COL_02		0x02C00000
#define MAT_RUBBLE_GR_COL_03		0x02D00000
#define MAT_RUBBLE_GR_COL_04		0x02E00000
#define MAT_RUBBLE_WET_GR_COL_01	0x02F00000
#define MAT_RUBBLE_WET_GR_COL_02	0x03000000
#define MAT_RUBBLE_WET_GR_COL_03	0x03100000
#define MAT_RUBBLE_WET_GR_COL_04	0x03200000
#define MAT_SOIL_GR_COL_01			0x03300000 // hard mud
#define MAT_SOIL_GR_COL_02			0x03400000
#define MAT_SOIL_GR_COL_03			0x03500000
#define MAT_SOIL_GR_COL_04			0x03600000
#define MAT_MUD_GR_COL_01			0x03700000 // wet soil
#define MAT_MUD_GR_COL_02			0x03800000
#define MAT_MUD_GR_COL_03			0x03900000
#define MAT_MUD_GR_COL_04			0x03A00000
#define MAT_SNOW_GR_COL_01			0x03B00000 // freshly laid snow
#define MAT_SNOW_GR_COL_02			0x03C00000
#define MAT_SNOW_GR_COL_03			0x03D00000
#define MAT_SNOW_GR_COL_04			0x03E00000
#define MAT_SNOW_DEEP_GR_COL_01		0x03F00000 // packed/deep snow
#define MAT_SNOW_DEEP_GR_COL_02		0x04000000
#define MAT_SNOW_DEEP_GR_COL_03		0x04100000
#define MAT_SNOW_DEEP_GR_COL_04		0x04200000
#define MAT_ICE						0x04300000
#define MAT_PUDDLE_GR_COL_01		0x04400000
#define MAT_PUDDLE_GR_COL_02		0x04500000
#define MAT_PUDDLE_GR_COL_03		0x04600000
#define MAT_PUDDLE_GR_COL_04		0x04700000
#define MAT_STONE_GR_COL_01			0x04800000
#define MAT_STONE_GR_COL_02			0x04900000
#define MAT_STONE_GR_COL_03			0x04A00000
#define MAT_STONE_GR_COL_04			0x04B00000
#define MAT_STONE_FROZEN			0x04C00000
#define MAT_STONE_SNOW				0x04D00000
#define MAT_STONE_SLUSH				0x04E00000
#define MAT_STONE_SPLASH			0x04F00000 // light covering of water on a stone surface
#define MAT_STONE_HOT				0x05000000
#define MAT_LAVACRACKS				0x05100000
#define MAT_TILES_BROWN				0x05200000
#define MAT_TILES_CYAN				0x05300000
#define MAT_TILES_GREEN				0x05400000
#define MAT_TILES_GREY				0x05500000
#define MAT_TILES_RED				0x05600000
#define MAT_TILES_YELLOW			0x05700000
#define MAT_TILES_WHITE				0x05800000
#define MAT_TILES_BROWN_FROZEN		0x05900000
#define MAT_TILES_CYAN_FROZEN		0x05A00000
#define MAT_TILES_GREEN_FROZEN		0x05B00000
#define MAT_TILES_GREY_FROZEN		0x05C00000
#define MAT_TILES_RED_FROZEN		0x05D00000
#define MAT_TILES_YELLOW_FROZEN		0x05E00000
#define MAT_TILES_WHITE_FROZEN		0x05F00000
#define MAT_TILES_BROWN_SNOW		0x06000000
#define MAT_TILES_CYAN_SNOW			0x06100000
#define MAT_TILES_GREEN_SNOW		0x06200000
#define MAT_TILES_GREY_SNOW			0x06300000
#define MAT_TILES_RED_SNOW			0x06400000
#define MAT_TILES_YELLOW_SNOW		0x06500000
#define MAT_TILES_WHITE_SNOW		0x06600000
#define MAT_TILES_BROWN_SLUSH		0x06700000
#define MAT_TILES_CYAN_SLUSH		0x06800000
#define MAT_TILES_GREEN_SLUSH		0x06900000
#define MAT_TILES_GREY_SLUSH		0x06A00000
#define MAT_TILES_RED_SLUSH			0x06B00000
#define MAT_TILES_YELLOW_SLUSH		0x06C00000
#define MAT_TILES_WHITE_SLUSH		0x06D00000
#define MAT_TILES_BROWN_SPLASH		0x06E00000 // light covering of water on a tiled surface
#define MAT_TILES_CYAN_SPLASH		0x06F00000
#define MAT_TILES_GREEN_SPLASH		0x07000000
#define MAT_TILES_GREY_SPLASH		0x07100000
#define MAT_TILES_RED_SPLASH		0x07200000
#define MAT_TILES_YELLOW_SPLASH		0x07300000
#define MAT_TILES_WHITE_SPLASH		0x07400000
#define MAT_CONCRETE				0x07500000 // hardened concrete pavement
#define MAT_CONCRETE_FROZEN			0x07600000
#define MAT_CONCRETE_SNOW			0x07700000
#define MAT_CONCRETE_SLUSH			0x07800000
#define MAT_CONCRETE_SPLASH			0x07900000 // light covering of water on a concrete surface
#define MAT_ASPHALT					0x07A00000
#define MAT_ASPHALT_FROZEN			0x07B00000
#define MAT_ASPHALT_SNOW			0x07C00000
#define MAT_ASPHALT_SLUSH			0x07D00000
#define MAT_ASPHALT_SPLASH			0x07E00000 // light covering of water on asphalt
#define MAT_BRICK_BLACK				0x07F00000
#define MAT_BRICK_BROWN				0x08000000
#define MAT_BRICK_GREY				0x08100000
#define MAT_BRICK_RED				0x08200000
#define MAT_METAL_HOLLOW			0x08300000 // hollow metal machines
#define MAT_METAL_HOLLOW_FROZEN		0x08400000
#define MAT_METAL_HOLLOW_SNOW		0x08500000
#define MAT_METAL_HOLLOW_SLUSH		0x08600000
#define MAT_METAL_HOLLOW_SPLASH		0x08700000
#define MAT_METAL_HOLLOW_PAINTED	0x08800000 // hollow metal painted
#define MAT_METAL_SOLID				0x08900000
#define MAT_METAL_SOLID_FROZEN		0x08A00000
#define MAT_METAL_SOLID_SNOW		0x08B00000
#define MAT_METAL_SOLID_SLUSH		0x08C00000
#define MAT_METAL_SOLID_SPLASH		0x08D00000
#define MAT_METAL_SOLID_PAINTED		0x08E00000
#define MAT_METAL_COPPER			0x08F00000
#define MAT_GRATE_01				0x09000000 // grating
#define MAT_GRATE_02				0x09100000
#define MAT_DUCT					0x09200000
#define MAT_PLATE					0x09300000 // solid metal plate
#define MAT_FENCE					0x09400000
#define MAT_BARREL					0x09500000
#define MAT_POT						0x09600000
#define MAT_WOOD_HOLLOW_DARK		0x09700000
#define MAT_WOOD_HOLLOW_BRIGHT		0x09800000
#define MAT_WOOD_HOLLOW_FROZEN		0x09900000
#define MAT_WOOD_HOLLOW_SNOW		0x09A00000
#define MAT_WOOD_HOLLOW_SLUSH		0x09B00000
#define MAT_WOOD_HOLLOW_SPLASH		0x09C00000
#define MAT_WOOD_SOLID_DARK			0x09D00000
#define MAT_WOOD_SOLID_DARK_FROZEN	0x09E00000
#define MAT_WOOD_SOLID_DARK_SNOW	0x09F00000
#define MAT_WOOD_SOLID_DARK_SLUSH	0x0A000000
#define MAT_WOOD_SOLID_DARK_SPLASH	0x0A100000 // light covering of water on a wooden surface
#define MAT_WOOD_SOLID_BRIGHT		0x0A200000 // freshly cut timber
#define MAT_WOOD_CREAKING_DARK		0x0A300000 // termite infested dark creaky wood
#define MAT_WOOD_CREAKING_BRIGHT	0x0A400000 // termite infested fresh creaky wood
#define MAT_ROOF					0x0A500000
#define MAT_SHINGLES_CERAMIC_RED	0x0A600000
#define MAT_SHINGLES_CERAMIC_GREY	0x0A700000
#define MAT_SHINGLES_WOOD			0x0A800000
#define MAT_INSULATION_01			0x0A900000
#define MAT_INSULATION_02			0x0AA00000
#define MAT_WALLPAPER				0x0AB00000
#define MAT_TEXTILES				0x0AC00000
#define MAT_CARPET					0x0AD00000 // lush carpet
#define MAT_PLASTIC_SOFT			0x0AE00000
#define MAT_CANVAS					0x0AF00000 // tent material
#define MAT_RUBBER					0x0B000000 // hard tire like rubber
#define MAT_FLESH					0x0B100000 // hung meat, corpses in the world
#define MAT_GLASS					0x0B200000
#define MAT_GLASS_SHARDS			0x0B300000
#define MAT_TRASH_GLASS				0x0B400000
#define MAT_TRASH_DEBRIS			0x0B500000
#define MAT_TRASH_WIRE				0x0B600000
#define MAT_TRASH_PACKING			0x0B700000
#define MAT_TRASH_PLASTIC			0x0B800000
#define MAT_PLASTIC_HARD			0x0B900000
#define MAT_COMPUTER				0x0BA00000 // computers/electronic equipment
#define MAT_BONES					0x0BB00000
#define MAT_PORCELAIN				0x0BC00000
//#define MAT_						0x0BD00000
//#define MAT_						0x0BE00000
//#define MAT_						0x0BF00000
//#define MAT_						0x0C000000
//#define MAT_						0x0C100000
//#define MAT_						0x0C200000
//#define MAT_						0x0C300000
//#define MAT_						0x0C400000
//#define MAT_						0x0C500000
//#define MAT_						0x0C600000
//#define MAT_						0x0C700000
//#define MAT_						0x0C800000
//#define MAT_						0x0C900000
//#define MAT_						0x0CA00000
//#define MAT_						0x0CB00000
//#define MAT_						0x0CC00000
//#define MAT_						0x0CD00000
//#define MAT_						0x0CE00000
//#define MAT_						0x0CF00000
//#define MAT_						0x0D000000
//#define MAT_						0x0D100000
//#define MAT_						0x0D200000
//#define MAT_						0x0D300000
//#define MAT_						0x0D400000
//#define MAT_						0x0D500000
//#define MAT_						0x0D600000
//#define MAT_						0x0D700000
//#define MAT_						0x0D800000
//#define MAT_						0x0D900000
//#define MAT_						0x0DA00000
//#define MAT_						0x0DB00000
//#define MAT_						0x0DC00000
//#define MAT_						0x0DD00000
//#define MAT_						0x0DE00000
//#define MAT_						0x0DF00000
//#define MAT_						0x0E000000
//#define MAT_						0x0E100000
//#define MAT_						0x0E200000
//#define MAT_						0x0E300000
//#define MAT_						0x0E400000
//#define MAT_						0x0E500000
//#define MAT_						0x0E600000
//#define MAT_						0x0E700000
//#define MAT_						0x0E800000
//#define MAT_						0x0E900000
//#define MAT_						0x0EA00000
//#define MAT_						0x0EB00000
//#define MAT_						0x0EC00000
//#define MAT_						0x0ED00000
//#define MAT_						0x0EE00000
//#define MAT_						0x0EF00000
//#define MAT_						0x0F000000
//#define MAT_						0x0F100000
//#define MAT_						0x0F200000
//#define MAT_						0x0F300000
//#define MAT_						0x0F400000
//#define MAT_						0x0F500000
//#define MAT_						0x0F600000
//#define MAT_						0x0F700000
//#define MAT_						0x0F800000
//#define MAT_						0x0F900000
//#define MAT_						0x0FA00000
//#define MAT_						0x0FB00000
//#define MAT_						0x0FC00000
//#define MAT_						0x0FD00000
//#define MAT_						0x0FE00000
//#define MAT_						0x0FF00000
