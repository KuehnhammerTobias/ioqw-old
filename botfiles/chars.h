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
 Unless specified, the higher the number, the better the characteristic, or the greater the likelyhood of the characteristic occuring.
 Also, if any characteristic is ommited, the default value will be used from the file in /bots/default_c.c.
**************************************************************************************************************************************/

// item weight file
#define CHARACTERISTIC_ITEMWEIGHTS					 0 // string
// weapon weight file
#define CHARACTERISTIC_WEAPONWEIGHTS				 1 // string
// file with chats
#define CHARACTERISTIC_CHAT_FILE					 2 // string
// name of the character with respect to the name that appears when the bot chats
#define CHARACTERISTIC_CHAT_NAME					 3 // string
// gender of the bot
#define CHARACTERISTIC_GENDER						 4 // string ("male", "female", "it")
//=====================================================================================================================================
// BATTLE SENSE
//=====================================================================================================================================
// reaction time in seconds, the lower the number the better
#define CHARACTERISTIC_REACTIONTIME					 5 // float [0, 5]
// how alert the bot is
#define CHARACTERISTIC_ALERTNESS					 6 // float [0, 1]
// the field of view of the bot
#define CHARACTERISTIC_FOV							 7 // int [0, 360]
// overreaction view model etc.
#define CHARACTERISTIC_VIEW_TYPE					 8 // int [0, 2]
// view angle difference to angle change factor
#define CHARACTERISTIC_VIEW_FACTOR					 9 // float [0, 1]
// maximum view angle change
#define CHARACTERISTIC_VIEW_MAXCHANGE				10 // float [1, 360]
// the aggression of the bot
// > 0.0 && <  0.2 = don't shoot at dead bodies
// > 0.0 && <= 0.9 = retreat if feeling bad
// > 0.9 && <= 1.0 = don't retreat even if low on health
#define CHARACTERISTIC_AGGRESSION					11 // float [0, 1]
// the self preservation of the bot (rockets near walls etc.)
#define CHARACTERISTIC_SELFPRESERVATION				12 // float [0, 1]
// the preservation of the bot's teammates (rockets near teammates etc.)
#define CHARACTERISTIC_TEAMPRESERVATION				13 // float [0, 1]
// how likely the bot is to take revenge
#define CHARACTERISTIC_VENGEFULNESS					14 // float [0, 1]
// what enemy a bot prefers most to attack
#define CHARACTERISTIC_ENEMY_PREFERENCE				15 // float [0, 1]
// tendency to get easy frags (such as on chatting players)
#define CHARACTERISTIC_EASY_FRAGGER					16 // float [0, 1]
// how overenthusiastic the bot is
#define CHARACTERISTIC_OBTRUSIVENESS				17 // float [0, 1]
// tendency to camp
#define CHARACTERISTIC_CAMPER						18 // float [0, 1]
//=====================================================================================================================================
// MOVEMENT
//=====================================================================================================================================
// tendency to crouch
#define CHARACTERISTIC_CROUCHER						19 // float [0, 1]
// tendency to jump
#define CHARACTERISTIC_JUMPER						20 // float [0, 1]
// tendency to walk
#define CHARACTERISTIC_WALKER						21 // float [0, 1]
// tendency to walk
#define CHARACTERISTIC_AGILITY						22 // float [0, 1]
// tendency to jump using a weapon
#define CHARACTERISTIC_WEAPONJUMPING				23 // float [0, 1]
// attack distance (+/- attack range determines the ideal attack distance)
#define CHARACTERISTIC_ATTACK_DISTANCE				24 // int [1, 1000]
// attack range (added/substracted to/from attack distance)
#define CHARACTERISTIC_ATTACK_RANGE					25 // int [1, 1000]
// attack skill
// >  0.0 && < 0.2 = don't move
// >  0.3 && < 1.0 = aim at enemy during retreat
// >  0.0 && < 0.4 = only move forward/backward
// >= 0.4 && < 1.0 = circle strafing
// >  0.7 && < 1.0 = random strafe direction change
#define CHARACTERISTIC_ATTACK_SKILL					26 // float [0, 1]
//=====================================================================================================================================
// WEAPON
//=====================================================================================================================================
// skill when aiming
// > 0.0 && <  0.9 = aim is affected by enemy movement
// > 0.4 && <= 0.8 = enemy linear leading
// > 0.8 && <= 1.0 = enemy exact movement leading
// > 0.5 && <= 1.0 = prediction shots when enemy is not visible
// > 0.6 && <= 1.0 = splash damage by shooting nearby geometry
#define CHARACTERISTIC_AIM_SKILL					27 // float [0, 1]
// weapon specific aim skill
#define CHARACTERISTIC_AIM_SKILL_NAILGUN			28 // float [0, 1]
#define CHARACTERISTIC_AIM_SKILL_PROXLAUNCHER		29 // float [0, 1]
#define CHARACTERISTIC_AIM_SKILL_GRENADELAUNCHER	30 // float [0, 1]
#define CHARACTERISTIC_AIM_SKILL_NAPALMLAUNCHER		31 // float [0, 1]
#define CHARACTERISTIC_AIM_SKILL_ROCKETLAUNCHER		32 // float [0, 1]
#define CHARACTERISTIC_AIM_SKILL_PLASMAGUN			33 // float [0, 1]
#define CHARACTERISTIC_AIM_SKILL_BFG10K				34 // float [0, 1]
// accuracy when aiming
#define CHARACTERISTIC_AIM_ACCURACY					35 // float [0, 1]
// weapon specific aim accuracy
#define CHARACTERISTIC_AIM_ACCURACY_MACHINEGUN		36 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_CHAINGUN		37 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_SHOTGUN			38 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_NAILGUN			39 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_PROXLAUNCHER	40 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_GRENADELAUNCHER	41 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_NAPALMLAUNCHER	42 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_ROCKETLAUNCHER	43 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_BEAMGUN			44 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_RAILGUN			45 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_PLASMAGUN		46 // float [0, 1]
#define CHARACTERISTIC_AIM_ACCURACY_BFG10K			47 // float [0, 1]
// how much the bot fires its weapon (continuous or bursts)
#define CHARACTERISTIC_FIRETHROTTLE					48 // float [0, 1]
//=====================================================================================================================================
// STRATEGY
//=====================================================================================================================================
#define CHARACTERISTIC_GOAL_MULTIPLIER				49 // int [1, 10000]
#define CHARACTERISTIC_LEADER_STRATEGY				50 // int [1, 4]
//=====================================================================================================================================
// CHAT
//=====================================================================================================================================
// tendency to insult/praise
#define CHARACTERISTIC_CHAT_INSULT					51 // float [0, 1]
// tendency to chat about miscellaneous subject
#define CHARACTERISTIC_CHAT_MISC					52 // float [0, 1]
// tendency to chat at start or end of level
#define CHARACTERISTIC_CHAT_STARTENDLEVEL			53 // float [0, 1]
// tendency to chat entering or exiting the game
#define CHARACTERISTIC_CHAT_ENTEREXITGAME			54 // float [0, 1]
// tendency to chat when the character killed someone
#define CHARACTERISTIC_CHAT_KILL					55 // float [0, 1]
// tendency to chat when the character died
#define CHARACTERISTIC_CHAT_DEATH					56 // float [0, 1]
// tendency to chat when enemy commits suicide
#define CHARACTERISTIC_CHAT_ENEMYSUICIDE			57 // float [0, 1]
// tendency to chat when bot was hit but didn't die
#define CHARACTERISTIC_CHAT_HITNODEATH				58 // float [0, 1]
// tendency to chat when bot hit the enemy but enemy didn't die
#define CHARACTERISTIC_CHAT_HITNOKILL				59 // float [0, 1]
// tendency to chat at random moments
#define CHARACTERISTIC_CHAT_RANDOM					60 // float [0, 1]
// tendency to reply to chats
#define CHARACTERISTIC_CHAT_REPLY					61 // float [0, 1]
