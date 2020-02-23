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

// script procedure (script with the name "main" is initial script)
script "name" {}
// point definition (default positions "entity1", "entity2" etc.)
point("name", x, y, z);
// box definition
box("name", minx, miny, minz, maxx, maxy, maxz);
// move a box
movebox("boxname", "pointname");
// move bot to a certain position
moveto("boxname");
// aim at a certain point
aim("point");
// say something
say("text", "wav filename");
// do a specific model animation
wave("wave type");
// select weapon with number 'x'
selectweapon(x);
// fire the current weapon
fireweapon();
// wait 'x' seconds
wait(time(x));
// wait until entity with number 'entitynum' touches 'box'
wait(touch(entitynum, "box"));

script "main"
{
	point("red armor point", 120, 120, 120);
	box("red armor box", -16, -16, -16, 16, 16, 16);
	movebox("red armor box", "red armor point");
	//
	point("near red armor point", 100, 100, 100);
	box("near red armor box", -16, -16, -16, 16, 16, 16);
	movebox("near red armor box", "near red armor point");
	//
	say("I'm going to the red armor", NULL);
	say("follow me", NULL);
	moveto("red armor box");
	wait(touch(1, "red armor box"));
	moveto("near red armor box");
	wait(touch(0, "red armor box"));
}
