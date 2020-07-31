<img src="https://raw.githubusercontent.com/ioid3-games/ioid3-qw/branches/0.01/misc/quakewars.png" width="128">

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

## Main features from ioquake3:

  * SDL 2 backend.
  * OpenAL sound API support (multiple speaker support and better sound quality).
  * Full x86_64 support on Linux.
  * VoIP support, both in-game and external support through Mumble.
  * MinGW compilation support on Windows and cross compilation support on Linux.
  * AVI video capture of demos.
  * Much improved console autocompletion.
  * Persistent console history.
  * Colorized terminal output.
  * Optional Ogg Vorbis support.
  * Much improved QVM tools.
  * Support for various esoteric operating systems.
  * cl_guid support.
  * HTTP/FTP download redirection (using cURL).
  * Multiuser support on Windows systems.
  * HDR Rendering, and support for HDR lightmaps.
  * Tone mapping and auto-exposure.
  * Cascaded shadow maps.
  * Multisample anti-aliasing.
  * Anisotropic texture filtering.
  * Advanced materials support.
  * Advanced shading and specular methods.
  * Screen-space ambient occlusion.
  * Rendering 'Sunrays'.
  * DDS and PNG texture support.
  * Many, many bug fixes.

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
* Enemy Territory Fortress - Ensiform
* Wolfenstein: Enemy Territory - Splash Damage
* Tremulous - Dark Legion Development
* World of Padman - Padworld Entertainment
* ioquake3 Elite Force MP patch - Thilo Schulz
* NetRadiant's q3map2 - Rudolf Polzer
* OpenArena - OpenArena contributors
* OpenMoHAA - OpenMoHAA contributors
* Xreal (triangle mesh collision) - Robert Beckebans
