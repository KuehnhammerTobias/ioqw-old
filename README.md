<img src="https://raw.githubusercontent.com/KuehnhammerTobias/ioqw-devel/master/misc/quakewars.png" width="128">

# Ioid3-qw 

**A second breath of life for Xreal, based on Zack Middleton's ioq3ztm engine**

Ioid3-qw is currently based on ioq3ztm and also contains code from [Spearmint](https://clover.moe/spearmint/) (the successor of ioq3ztm), and code from other repositories owned by [Zack Middleton](https://github.com/zturtleman?tab=repositories).


## License:

Ioid3-qw is licensed under a [modified version of the GNU GPLv3](COPYING.txt#L625) (or at your option, any later version). The license is also used by Return to Castle Wolfenstein, Wolfenstein: Enemy Territory, and Doom 3.


## Main features:

  * K&R (aka 1TBS/OTBS) formatted code.
  * Bloom rendering effect.
  * Enhanced BotAI.
  * Rotating gibs.
  * Slightly faster maths.
  * Improved UI (in-game server setup, unique bots per gametype,...).
  * New development cvars:
    *  `cl_drawping` (0/1) - Draw detailed ping.
    *  `cl_drawfps` (0/1) - Draw detailed fps.
    *  `cl_drawsnaps` (0/1) - Draw detailed snaps.
    *  `cl_drawpackets` (0/1) - Draw detailed packets.
    *  `cg_drawDebug` (0/1) - Disable/enable drawing some elements that are only useful while debugging bots (e.g.: team task sprites).
    *  `bot_report` (0/1/2) - Prints what the bot is doing and shows the node the bot is in (2). 0 = off, 1 = console report, 2 = report if a bot is being followed (as in Mint-Arena).
    *  `bot_shownodechanges` (0/1) - Shows the node the bots are in. 0 = off, 1 = console report.
    *  `bot_teambluestrategy` (0/1/2/3/4) - The strategy the blue team will choose in team gametypes (WIP).
    *  `bot_teamredstrategy` (0/1/2/3/4) - The strategy the red team will choose in team gametypes (WIP).
    *  `bot_noshoot` (0/1) - Bots will act as usual, but they suppress fire. They react and aim unaffected but they won't hit the trigger (added for various development benefits).
    *  `bot_equalize` (0/1) - (unknown/obsolet?)

## Main features from Spearmint:

  * Aspect correct widescreen.
  * High resolution font support (TrueType).
  * Enhanced model loading (incl. submodels).
  * Dynamic (damage) skin support.
  * Bullet marks on doors and moving platforms.
  * Gibs and bullet shells ride on moving platforms.
  * New shader keywords and game objects.
  * Foliage support.
  * Better external lightmap support.
  * Atmospheric effects, like rain and snow.
  * Dynamic lights have smoother edges.
  * Improved Bot AI.

## Goals:

  * Ragdoll physics.
  * Realtime lightning/shadowing.
  * 64 weapon support.
  * A new cooperative gamemode.
  * Even more improved Bot AI.
  * Advanced bot order menu.


## Current differences to Spearmint:

  * Splitscreen support is still missing due to some rendering issues.
  * Spearmint's gamepad/joystick support is missing.
  * Lot of bot AI code is still compiled into the engine, like it was by default.
  * The default code sctructure is kept alive, engine and game modules aren't seperated, so Ioid3-qw is less modding friendly.


## Credits:

* Zack Middleton
* Robert Beckebans
* And other contributors


## Ioid3-qw is based on ioq3ztm and also contains code from:

* Spearmint - Zack Middleton
* RTCW SP - Gray Matter Interactive
* RTCW MP - Nerve Software
* Wolfenstein: Enemy Territory - Splash Damage
* Tremulous - Dark Legion Development
* World of Padman - Padworld Entertainment
* ioquake3 Elite Force MP patch - Thilo Schulz
* NetRadiant's q3map2 - Rudolf Polzer
* OpenArena - OpenArena contributors
* OpenMoHAA - OpenMoHAA contributors
* Xreal (triangle mesh collision) - Robert Beckebans
