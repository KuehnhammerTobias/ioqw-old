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
 Master for refresh, status bar, console, chat, notify, etc.
**************************************************************************************************************************************/

#include "client.h"

qboolean scr_initialized; // ready to draw

cvar_t *cl_timegraph;
cvar_t *cl_debuggraph;
cvar_t *cl_graphheight;
cvar_t *cl_graphscale;
cvar_t *cl_graphshift;
// Tobias DEBUG
cvar_t *cl_drawping;
cvar_t *cl_drawpingfontsize;
cvar_t *cl_drawpingposx;
cvar_t *cl_drawpingposy;
cvar_t *cl_drawpingfirstinterval;
cvar_t *cl_drawpingsecondinterval;
cvar_t *cl_drawfps;
cvar_t *cl_drawfpsfontsize;
cvar_t *cl_drawfpsposx;
cvar_t *cl_drawfpsposy;
cvar_t *cl_drawfpsinterval;
cvar_t *cl_drawsnaps;
cvar_t *cl_drawsnapsfontsize;
cvar_t *cl_drawsnapsposx;
cvar_t *cl_drawsnapsposy;
cvar_t *cl_drawsnapsfirstinterval;
cvar_t *cl_drawsnapssecondinterval;
cvar_t *cl_drawpackets;
cvar_t *cl_drawpacketsposx;
cvar_t *cl_drawpacketsposy;
cvar_t *cl_drawpacketsfontsize;
cvar_t *cl_drawpacketsfirstinterval;
cvar_t *cl_drawpacketssecondinterval;
// Tobias END
/*
=======================================================================================================================================
SCR_DrawNamedPic

Coordinates are 640 * 480 virtual values.
=======================================================================================================================================
*/
void SCR_DrawNamedPic(float x, float y, float width, float height, const char *picname) {
	qhandle_t hShader;

	assert(width != 0);

	hShader = re.RegisterShader(picname);

	SCR_AdjustFrom640(&x, &y, &width, &height);
	re.DrawStretchPic(x, y, width, height, 0, 0, 1, 1, hShader);
}

/*
=======================================================================================================================================
SCR_AdjustFrom640

Adjusted for resolution and screen aspect ratio.
=======================================================================================================================================
*/
void SCR_AdjustFrom640(float *x, float *y, float *w, float *h) {
	float xscale;
	float yscale;
#if 0
	// adjust for wide screens
	if (cls.glconfig.vidWidth * 480 > cls.glconfig.vidHeight * 640) {
		*x += 0.5 * (cls.glconfig.vidWidth - (cls.glconfig.vidHeight * 640 / 480));
	}
#endif
	// scale for screen sizes
	xscale = cls.glconfig.vidWidth / 640.0;
	yscale = cls.glconfig.vidHeight / 480.0;

	if (x) {
		*x *= xscale;
	}

	if (y) {
		*y *= yscale;
	}

	if (w) {
		*w *= xscale;
	}

	if (h) {
		*h *= yscale;
	}
}

/*
=======================================================================================================================================
SCR_FillRect

Coordinates are 640 * 480 virtual values.
=======================================================================================================================================
*/
void SCR_FillRect(float x, float y, float width, float height, const float *color) {

	re.SetColor(color);

	SCR_AdjustFrom640(&x, &y, &width, &height);

	re.DrawStretchPic(x, y, width, height, 0, 0, 0, 0, cls.whiteShader);
	re.SetColor(NULL);
}

/*
=======================================================================================================================================
SCR_DrawPic

Coordinates are 640 * 480 virtual values.
=======================================================================================================================================
*/
void SCR_DrawPic(float x, float y, float width, float height, qhandle_t hShader) {

	SCR_AdjustFrom640(&x, &y, &width, &height);
	re.DrawStretchPic(x, y, width, height, 0, 0, 1, 1, hShader);
}

/*
=======================================================================================================================================
SCR_DrawChar

Chars are drawn at 640 * 480 virtual screen size.
=======================================================================================================================================
*/
static void SCR_DrawChar(int x, int y, float size, int ch) {
	int row, col;
	float frow, fcol;
	float ax, ay, aw, ah;

	ch &= 255;

	if (ch == ' ') {
		return;
	}

	if (y < -size) {
		return;
	}

	ax = x;
	ay = y;
	aw = size;
	ah = size;

	SCR_AdjustFrom640(&ax, &ay, &aw, &ah);

	row = ch >> 4;
	col = ch &15;
	frow = row * 0.0625;
	fcol = col * 0.0625;
	size = 0.0625;

	re.DrawStretchPic(ax, ay, aw, ah, fcol, frow, fcol + size, frow + size, cls.charSetShader);
}

/*
=======================================================================================================================================
SCR_DrawSmallChar

Small chars are drawn at native screen resolution.
=======================================================================================================================================
*/
void SCR_DrawSmallChar(int x, int y, int ch) {
	int row, col;
	float frow, fcol;
	float size;

	ch &= 255;

	if (ch == ' ') {
		return;
	}

	if (y < -SMALLCHAR_HEIGHT) {
		return;
	}

	row = ch >> 4;
	col = ch &15;
	frow = row * 0.0625;
	fcol = col * 0.0625;
	size = 0.0625;

	re.DrawStretchPic(x, y, SMALLCHAR_WIDTH, SMALLCHAR_HEIGHT, fcol, frow, fcol + size, frow + size, cls.charSetShader);
}

/*
=======================================================================================================================================
SCR_DrawStringExt

Draws a multi-colored string with a drop shadow, optionally forcing to a fixed color.
Coordinates are at 640 by 480 virtual resolution.
=======================================================================================================================================
*/
void SCR_DrawStringExt(int x, int y, float size, const char *string, float *setColor, qboolean forceColor, qboolean noColorEscape) {
	vec4_t color;
	const char *s;
	int xx;

	// draw the drop shadow
	color[0] = color[1] = color[2] = 0;
	color[3] = setColor[3];

	re.SetColor(color);

	s = string;
	xx = x;

	while (*s) {
		if (!noColorEscape && Q_IsColorString(s)) {
			s += 2;
			continue;
		}

		SCR_DrawChar(xx + 2, y + 2, size, *s);

		xx += size;
		s++;
	}
	// draw the colored text
	s = string;
	xx = x;

	re.SetColor(setColor);

	while (*s) {
		if (Q_IsColorString(s)) {
			if (!forceColor) {
				Com_Memcpy(color, g_color_table[ColorIndex(*(s + 1))], sizeof(color));
				color[3] = setColor[3];
				re.SetColor(color);
			}

			if (!noColorEscape) {
				s += 2;
				continue;
			}
		}

		SCR_DrawChar(xx, y, size, *s);

		xx += size;
		s++;
	}

	re.SetColor(NULL);
}

/*
=======================================================================================================================================
SCR_DrawBigString
=======================================================================================================================================
*/
void SCR_DrawBigString(int x, int y, const char *s, float alpha, qboolean noColorEscape) {
	float color[4];

	color[0] = color[1] = color[2] = 1.0;
	color[3] = alpha;

	SCR_DrawStringExt(x, y, BIGCHAR_WIDTH, s, color, qfalse, noColorEscape);
}

/*
=======================================================================================================================================
SCR_DrawBigStringColor
=======================================================================================================================================
*/
void SCR_DrawBigStringColor(int x, int y, const char *s, vec4_t color, qboolean noColorEscape) {
	SCR_DrawStringExt(x, y, BIGCHAR_WIDTH, s, color, qtrue, noColorEscape);
}

/*
=======================================================================================================================================
SCR_DrawSmallStringExt

Draws a multi-colored string with a drop shadow, optionally forcing to a fixed color.
=======================================================================================================================================
*/
void SCR_DrawSmallStringExt(int x, int y, const char *string, float *setColor, qboolean forceColor, qboolean noColorEscape) {
	vec4_t color;
	const char *s;
	int xx;

	// draw the colored text
	s = string;
	xx = x;
	re.SetColor(setColor);

	while (*s) {
		if (Q_IsColorString(s)) {
			if (!forceColor) {
				Com_Memcpy(color, g_color_table[ColorIndex(*(s + 1))], sizeof(color));
				color[3] = setColor[3];
				re.SetColor(color);
			}

			if (!noColorEscape) {
				s += 2;
				continue;
			}
		}

		SCR_DrawSmallChar(xx, y, *s);

		xx += SMALLCHAR_WIDTH;
		s++;
	}

	re.SetColor(NULL);
}

/*
=======================================================================================================================================
SCR_Strlen

Skips color escape codes.
=======================================================================================================================================
*/
static int SCR_Strlen(const char *str) {
	const char *s = str;
	int count = 0;

	while (*s) {
		if (Q_IsColorString(s)) {
			s += 2;
		} else {
			count++;
			s++;
		}
	}

	return count;
}

/*
=======================================================================================================================================
SCR_GetBigStringWidth
=======================================================================================================================================
*/
int SCR_GetBigStringWidth(const char *str) {
	return SCR_Strlen(str) * BIGCHAR_WIDTH;
}

/*
=======================================================================================================================================
SCR_DrawDemoRecording
=======================================================================================================================================
*/
void SCR_DrawDemoRecording(void) {
	char string[1024];
	int pos;

	if (!clc.demorecording) {
		return;
	}

	if (clc.spDemoRecording) {
		return;
	}

	pos = FS_FTell(clc.demofile);

	sprintf(string, "RECORDING %s: %ik", clc.demoName, pos / 1024);
	SCR_DrawStringExt(320 - strlen(string) * 4, 20, 8, string, g_color_table[7], qtrue, qfalse);
}
#ifdef USE_VOIP
/*
=======================================================================================================================================
SCR_DrawVoipMeter
=======================================================================================================================================
*/
void SCR_DrawVoipMeter(void) {
	char buffer[16];
	char string[256];
	int limit, i;

	if (!cl_voipShowMeter->integer) {
		return; // player doesn't want to show meter at all
	} else if (!cl_voipSend->integer) {
		return; // not recording at the moment
	} else if (clc.state != CA_ACTIVE) {
		return; // not connected to a server
	} else if (!clc.voipEnabled) {
		return; // server doesn't support VoIP
	} else if (clc.demoplaying) {
		return; // playing back a demo
	} else if (!cl_voip->integer) {
		return; // client has VoIP support disabled
	}

	limit = (int)(clc.voipPower * 10.0f);

	if (limit > 10) {
		limit = 10;
	}

	for (i = 0; i < limit; i++) {
		buffer[i] = '*';
	}

	while (i < 10) {
		buffer[i++] = ' ';
	}

	buffer[i] = '\0';

	sprintf(string, "VoIP: [%s]", buffer);
	SCR_DrawStringExt(320 - strlen(string) * 4, 10, 8, string, g_color_table[7], qtrue, qfalse);
}
#endif
/*
=======================================================================================================================================

	DEBUG GRAPH

=======================================================================================================================================
*/

static int current;
static float values[1024];
/*
=======================================================================================================================================
SCR_DebugGraph
=======================================================================================================================================
*/
void SCR_DebugGraph(float value) {

	values[current] = value;
	current = (current + 1) % ARRAY_LEN(values);
}

/*
=======================================================================================================================================
SCR_DrawDebugGraph
=======================================================================================================================================
*/
void SCR_DrawDebugGraph(void) {
	int a, x, y, w, i, h;
	float v;

	// draw the graph
	w = cls.glconfig.vidWidth;
	x = 0;
	y = cls.glconfig.vidHeight;

	re.SetColor(g_color_table[0]);
	re.DrawStretchPic(x, y - cl_graphheight->integer, w, cl_graphheight->integer, 0, 0, 0, 0, cls.whiteShader);
	re.SetColor(NULL);

	for (a = 0; a < w; a++) {
		i = (ARRAY_LEN(values) + current - 1 - (a % ARRAY_LEN(values))) % ARRAY_LEN(values);
		v = values[i];
		v = v * cl_graphscale->integer + cl_graphshift->integer;

		if (v < 0) {
			v += cl_graphheight->integer * (1 + (int)(-v / cl_graphheight->integer));
		}

		h = (int)v % cl_graphheight->integer;

		re.DrawStretchPic(x + w - 1 - a, y - h, 1, h, 0, 0, 0, 0, cls.whiteShader);
	}
}
// Tobias DEBUG
#if !defined (DEDICATED)
ioq3_urt_t clu; // most new console variables and functions are called via this
cvar_t *com_quiet; // excluded from the struct since it makes rebuilding hell.

/*
=======================================================================================================================================
SCR_DrawPing

Draw the ping, mean ping, spike from mean, mean spike and standard deviation of ping for two user-set intervals.

FIXME: on all meters: Max size of vars in relation to intervals.
=======================================================================================================================================
*/
void SCR_DrawPing(void) {
	char string1[64], string2[64], string3[64], string4[64], string5[64], string6[64];
	static int *pings, *pings2, timeRec, timeRec2, count, count2, acount, acount2, avgPing, avgPing2, flux, flux2, aflux, aflux2, std, std2, alloc_ok, alloc_ok2;
	static unsigned long long int sum, sum2, asum, asum2, stsum, stsum2; // some of those may be getting large
	int timeNew, currPing, interval, interval2, fontsize, posx, posy, posxx, posyy, i;

	// vars needing refreshing each global loop:
	timeNew = Sys_Milliseconds(); // not affected by timescale or timing alterations by the server.
	currPing = cl.snap.ping;
	// we let those able to be set on the fly for fun:
	interval = cl_drawpingfirstinterval->integer; // how long we keep collecting data before calcs
	interval2 = cl_drawpingsecondinterval->integer; // the same being repeated for another interval
	fontsize = cl_drawpingfontsize->integer;
	posx = cl_drawpingposx->integer;
	posy = cl_drawpingposy->integer;
	// preventing two potential devisions by zero and nonsensical results:
	if (interval <= 0) {
		interval = 1;
	}

	if (interval2 <= 0) {
		interval2 = 1;
	}
	// only set initially/once:
	if (!timeRec) {
		timeRec = timeRec2 = timeNew; // to assume initially that we are at a new interval(New and Rec's difference 0)
	}

	if ((timeNew - timeRec) / 1000 < interval) { // we're in the interval;collect info:
		if (!(count % 128)) { // realloc only every 128 entries and initially
			int *tmp = realloc(pings, (count + 128) * sizeof(int)); // giving space 

			if (tmp != NULL) {
				// alloc ok
				alloc_ok = 1;
				pings = tmp;
			} else {
			// alloc failed;reset.
				alloc_ok = 0;
				count = 0;
			}
		}

		if (alloc_ok) {
			pings[count] = currPing; // collecting all pings for interval-end
			// it's more accurate compared to doing it on the fly and using a 'previous mean' in later calculations
			count++;
		}
	} else if (count) { // we have hit the point the interval ends(time >= interval);let's do the calcs:
		// plus don't go further if there's no count(only if mem allocation totally failed above)
		// also avoids potential divisions by zero
		for (i = 0; i < count; i++) {
			sum += pings[i];
		}
	
		avgPing = sum / count; // mean
		flux = 0; // max spike has to reset at this point

		for (i = 0; i < count; i++) { // now that we have the mean, std deviation and spikes calcs
			// std dev sum:
			stsum += pow((double)(pings[i] - avgPing), 2); // (double) because more than 2 functions exist in library(+float)
		// spikes:
			if (pings[i] > avgPing) { // we had a spike(of the high side)
				acount++;
				asum += (pings[i] - avgPing); // summing the spikes for later avg of spikes

				if (pings[i] > flux + avgPing) { // we had a 'champion' spike 
					flux = pings[i] - avgPing; // for printing the current max spike for the interval;resets each loop
				}
			}
		}

		std = sqrt((double)(stsum / count)); // standard deviation

		if (acount) {
			aflux = asum / acount; // average of high spikes
		} else {
			aflux = 0; // there was no flux for this interval;this is common.
		}
		// + avoids division by zero
		timeRec = timeNew; // record a new point in time to check for next interval end
		// reset static vars that are being used in interval calculations:
		sum = count = asum = acount = stsum = 0;
		free(pings);
		pings = NULL; // don't forget this. free() is not enough for realloc() and debuggers will not be kind to explain why.
	}
	// pure repetition of the above(for 2nd interval):
	// FIXME: should have been a function
	if ((timeNew - timeRec2) / 1000 < interval2) {
		if (!(count2 % 128)) {
			int *tmp = realloc(pings2, (count2 + 128) * sizeof(int));

			if (tmp != NULL) {
				alloc_ok2 = 1;
				pings2 = tmp;
			} else {
				alloc_ok2 = 0;
				count2 = 0;
			}
		}

		if (alloc_ok2) {
			pings2[count2] = currPing;
			count2++;
		}
	} else if (count2) {
		for (i = 0; i < count2; i++) {
			sum2 += pings2[i];
		}

		avgPing2 = sum2 / count2;
		flux2 = 0;

		for (i = 0; i < count2; i++) {
			stsum2 += pow((double)(pings2[i] - avgPing2), 2);
		
			if (pings2[i] > avgPing2) {
				acount2++;
				asum2 += (pings2[i] - avgPing2);

				if (pings2[i] > flux2 + avgPing2) {
					flux2 = pings2[i] - avgPing2;
				}
			}
		}

		std2 = sqrt((double)(stsum2 / count2));
	
		if (acount2) {
			aflux2 = asum2 / acount2;
		} else {
			aflux2 = 0;
		}

		timeRec2 = timeNew;
		sum2 = count2 = asum2 = acount2 = stsum2 = 0;
		free(pings2);
		pings2 = NULL;
	}
	// end of pure repetition
	Com_sprintf(string1, sizeof(string1), "%3ims", currPing);
	Com_sprintf(string2, sizeof(string2), "Interval  %3is %3is", interval, interval2);
	Com_sprintf(string3, sizeof(string3), "Mean      %3ims %3ims", avgPing, avgPing2);
	Com_sprintf(string4, sizeof(string4), "Max Spike %3ims %3ims", flux, flux2);
	Com_sprintf(string5, sizeof(string5), "Avg Spike %3ims %3ims", aflux, aflux2);
	Com_sprintf(string6, sizeof(string6), "Std Dev   %3ims %3ims", std, std2);
	// positioning of printing(it could be one var for simplicity but this is more intuitive)
	if (posx >= 0 && posx < 21) { // well, you can't go beyond 640 virtual pixels;so avoiding hiding it
		posxx = posx * 30;
	} else {
		posxx = 0;
	}

	if (posy >= 0 && posy < 24) { // avoiding going beyond 480 virtual pixels
		posyy = posy * 20;
	} else {
		posyy = 0;
	}
	// fontsize + 2 came from experimentation, no harry potter involved
	SCR_DrawStringExt(posxx, posyy, fontsize, string1, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 1, fontsize, string2, g_color_table[5], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 2, fontsize, string3, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 3, fontsize, string4, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 4, fontsize, string5, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 5, fontsize, string6, g_color_table[7], qtrue, qtrue);
}

/*
=======================================================================================================================================
SCR_DrawFPS

Equivalent to drawping for FPS.
=======================================================================================================================================
*/
void SCR_DrawFPS(void) {
	float fps, interval;
	char string1[64], string2[64], string3[64], string4[64], string5[64], string6[64];
	static float *fpses, avgfps, drop, dcount, stdfps, adrop, sumfps, stsumfps, dsum, alloc_fps;
	int fontsize, posx, posy, posxx, posyy, i;
	static int countfps;
	static unsigned long long microold, timeRecfps;
	unsigned long long micronew;
	long long div;

	micronew = Sys_Milliseconds() * 1000LL; // conversion with '(long long)'(or LL) or the calculation overflows.

	if (!microold) {
		microold = micronew;
	}

	div = micronew - microold; // fixme? ensure unoverflowability
	
	microold = micronew; // recall the previous time
	
	if (div) { // division by zero avoidance;theoreticaly possible on timedemo.
		fps = 1000000.0 / div; /* how long did it take it to come back here? We don't buffer on purpose, 	that's why we have an interval in this. */
	} else {
		fps = 1000.0;
	}

	interval = cl_drawfpsinterval->value;
	fontsize = cl_drawfpsfontsize->integer;
	posx = cl_drawfpsposx->integer;
	posy = cl_drawfpsposy->integer;
	
	if (interval <= 0.0) {
		interval = 0.02;
	}

	if (!timeRecfps) {
		timeRecfps = micronew;
	}

	if ((float)(micronew - timeRecfps) / 1000000 < interval) {
		if (!(countfps % 128)) {
			float *tmp = realloc(fpses, (countfps + 128) * sizeof(float));

			if (tmp != NULL) {
				alloc_fps = 1;
				fpses = tmp;
			} else {
				alloc_fps = 0;
				countfps = 0;
			}
		}
		if (alloc_fps) {
			fpses[countfps] = fps;
			countfps++;
		}
	} else if (countfps) {
		for (i = 0; i < countfps; i++) {
			sumfps += fpses[i];
		}

		avgfps = sumfps / countfps;
		drop = 0.0;

		for (i = 0; i < countfps; i++) {
			stsumfps += pow(fpses[i] - avgfps, 2);

			if (fpses[i] < avgfps) {
				dcount++;
				dsum += (avgfps - fpses[i]);

				if (fpses[i] < avgfps - drop) {
					drop = avgfps - fpses[i];
				}
			}
		}

		stdfps = sqrt(stsumfps / countfps);

		if (dcount) {
			adrop = dsum / dcount;
		} else {
			adrop = 0.0;
		}

		timeRecfps = micronew;
		sumfps = countfps = stsumfps = dsum = dcount = 0.0;
		free(fpses);
		fpses = NULL;
	} else {
		timeRecfps = micronew;/*	If the interval is small enough to not let it satisfy either of the previous two conditions,
					timeRecfps is not replenished and that makes the 'new time - old' check impossible to be ever
					satisfied again, unless a big enough interval is set in time. Hence this assignment here
					allows for extremely low intervals without explicitely disallowing them, even if they won't
					let it print much. */
		Com_Printf("cl_drawFPS: interval too low to be meaningful on this FPS. FPS is %f, consider an interval above 1/FPS = %fs\n", fps, 1.0/fps);
	}

	Com_sprintf(string1, sizeof(string1), "%7.3fFPS", fps);
	Com_sprintf(string2, sizeof(string2), "Interval %.2fs", interval);
	Com_sprintf(string3, sizeof(string3), "Mean     %7.3f", avgfps);
	Com_sprintf(string4, sizeof(string4), "Max Drop %7.3f", drop);
	Com_sprintf(string5, sizeof(string5), "Avg Drop %7.3f", adrop);
	Com_sprintf(string6, sizeof(string6), "Std Dev  %7.3f", stdfps);
	
	if (posx >= 0 && posx < 21) {
		posxx = posx * 30;
	} else {
		posxx = 0;
	}

	if (posy >= 0 && posy < 24) {
		posyy = posy * 20;
	} else {
		posyy = 0;
	}

	SCR_DrawStringExt(posxx, posyy, fontsize, string1, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 1, fontsize, string2, g_color_table[1], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 2, fontsize, string3, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 3, fontsize, string4, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 4, fontsize, string5, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 5, fontsize, string6, g_color_table[7], qtrue, qtrue);
}

typedef struct { // could be in clu.
	int sps;
	int fps; // for internal calculation of delayed/sec etc.
	int state;
	qboolean extrapolated;
}s_snaps;

/*
=======================================================================================================================================
SCR_DrawSnaps

Equivalent to drawping for snapshots received from server - most comments in drawping.
=======================================================================================================================================
*/
void SCR_DrawSnaps(void) {
	char string1[64], string2[64], string3[64], string4[64], string5[64], string6[64], string7[64], string8[64];
	static int sps, omsg, otime, timeRecsnaps, timeRecsnaps2, countsnaps, countsnaps2, avgsnaps, avgsnaps2, sdrop, sdrop2, asdrop, asdrop2, dcount, dcount2, stdsnaps, stdsnaps2, delayedpacks, delayedpacks2, alloc_snaps, alloc_snaps2, extrapolatedsnaps, extrapolatedsnaps2, fps, otimefps, avgfps, avgfps2;
	static float coef, coef2, delayedpacksf, delayedpacksf2, extrapolatedsnapsf, extrapolatedsnapsf2;
	int cmsg, ctime, interval, interval2, fontsize, posx, posy, posxx, posyy, i;
	static unsigned long long int sumsnaps, sumsnaps2, snsum, snsum2, stsnapssum, stsnapssum2, sumfps, sumfps2;
	static s_snaps *snapses, *snapses2;

	cmsg = cl.snap.messageNum;
	ctime = Sys_Milliseconds(); // not affected by timescale or timing alterations by the server.
	
	if (!omsg) { // initially
		omsg = cmsg;
		otime = ctime;
		otimefps = ctime;
	}

	if (ctime - otimefps) {
		fps = 1000 / (ctime - otimefps); // for internal use only
		otimefps = ctime;
	}

	if (ctime - otime && cmsg-omsg) {
		sps = 1000 *(cmsg - omsg) / (ctime - otime);
		omsg = cmsg;
		otime = ctime;
	}

	interval = cl_drawsnapsfirstinterval->integer;
	interval2 = cl_drawsnapssecondinterval->integer;
	fontsize = cl_drawsnapsfontsize->integer;
	posx = cl_drawsnapsposx->integer;
	posy = cl_drawsnapsposy->integer;

	if (interval <= 0) {
		interval = 1;
	}

	if (interval2 <= 0) {
		interval2 = 1;
	}

	if (!timeRecsnaps) {
		timeRecsnaps = timeRecsnaps2 = ctime;
	}

	if ((ctime - timeRecsnaps) / 1000 < interval) {
		if (!(countsnaps % 128)) {
			s_snaps *tmp = realloc(snapses, (countsnaps + 128) * sizeof(s_snaps));

			if (tmp != NULL) {
				alloc_snaps = 1;
				snapses = tmp;
			} else {
				alloc_snaps = 0;
				countsnaps = 0;
			}
		}

		if (alloc_snaps) {
			snapses[countsnaps].sps = sps;
			snapses[countsnaps].state = cl.snap.snapFlags;
			snapses[countsnaps].extrapolated = cl.extrapolatedSnapshot;
			snapses[countsnaps].fps = fps;
			countsnaps++;
		}
	} else if (countsnaps) {
		for (i = 0; i < countsnaps; i++) {
			sumsnaps += snapses[i].sps;
			sumfps +=snapses[i].fps;
		}

		avgsnaps = sumsnaps / countsnaps;
		avgfps = sumfps / countsnaps;
		coef = (float)avgsnaps / (float)avgfps / (float)interval; // (float) required because they are integers and the result would be different
		sdrop = delayedpacks = extrapolatedsnaps = 0;

		for (i = 0; i < countsnaps; i++) {
			stsnapssum += pow((double)(snapses[i].sps - avgsnaps), 2);

			if (snapses[i].sps < avgsnaps) {
				dcount++;
				snsum += (avgsnaps - snapses[i].sps);

				if (snapses[i].sps < avgsnaps - sdrop) {
					sdrop = avgsnaps - snapses[i].sps;
				}
			}
			// state:
			if (snapses[i].state & SNAPFLAG_RATE_DELAYED) {
				delayedpacks++;
			}

			delayedpacksf = (float)delayedpacks * coef;

			if (snapses[i].extrapolated == qtrue) {
				extrapolatedsnaps++;
			}

			extrapolatedsnapsf = (float) extrapolatedsnaps * coef;
		}

		stdsnaps = sqrt((double)(stsnapssum / countsnaps));

		if (dcount) {
			asdrop = snsum / dcount;
		} else {
			asdrop = 0;
		}

		timeRecsnaps = ctime;
		sumsnaps = countsnaps = stsnapssum = snsum = dcount = sumfps = 0;
		free(snapses);
		snapses = NULL;
	}
	// 2
	if ((ctime - timeRecsnaps2) / 1000 < interval2) {
		if (!(countsnaps2 % 128)) {

			s_snaps *tmp = realloc(snapses2, (countsnaps2 + 128) * sizeof(s_snaps));

			if (tmp != NULL) {
				alloc_snaps2 = 1;
				snapses2 = tmp;
			} else {
				alloc_snaps2 = 0;
				countsnaps2 = 0;
			}
		}

		if (alloc_snaps2) {
			snapses2[countsnaps2].sps = sps;
			snapses2[countsnaps2].state = cl.snap.snapFlags;
			snapses2[countsnaps2].extrapolated = cl.extrapolatedSnapshot;
			snapses2[countsnaps2].fps = fps;
			countsnaps2++;
		}
	} else if (countsnaps2) {
		for (i = 0; i < countsnaps2; i++) {
			sumsnaps2 += snapses2[i].sps;
			sumfps2 +=snapses2[i].fps;
		}

		avgsnaps2 = sumsnaps2 / countsnaps2;
		avgfps2 = sumfps2 / countsnaps2;
		coef2 = (float)avgsnaps2 / (float)avgfps2 / (float)interval2;
		sdrop2 = delayedpacks2 = extrapolatedsnaps2 = 0;

		for (i = 0; i < countsnaps2; i++) {
			stsnapssum2 += pow((double)(snapses2[i].sps - avgsnaps2), 2);

			if (snapses2[i].sps < avgsnaps2) {
				dcount2++;
				snsum2 += (avgsnaps2 - snapses2[i].sps);

				if (snapses2[i].sps < avgsnaps2 - sdrop2) {
					sdrop2 = avgsnaps2 - snapses2[i].sps;
				}
			}
		
			// state:
			if (snapses2[i].state & SNAPFLAG_RATE_DELAYED) {
				delayedpacks2++;
			}

			delayedpacksf2 = (float)delayedpacks2 * coef2;

			if (snapses2[i].extrapolated == qtrue) {
				extrapolatedsnaps2++;
			}
		
			extrapolatedsnapsf2 = (float) extrapolatedsnaps2 * coef2;
		}

		stdsnaps2 = sqrt((double)(stsnapssum2 / countsnaps2));

		if (dcount2) {
			asdrop2 = snsum2 / dcount2;
		} else {
			asdrop2 = 0;
		}

		timeRecsnaps2 = ctime;
		sumsnaps2 = countsnaps2 = stsnapssum2 = snsum2 = dcount2 = sumfps2 = 0;
		free(snapses2);
		snapses2 = NULL;
	}

	Com_sprintf(string1, sizeof(string1), "%3iSPS", sps);
	Com_sprintf(string2, sizeof(string2), "Interval %3is  %3is", interval, interval2);
	Com_sprintf(string3, sizeof(string3), "Mean     %3iSPS %3iSPS", avgsnaps, avgsnaps2);
	Com_sprintf(string4, sizeof(string4), "Max Drop %3iSPS %3iSPS", sdrop, sdrop2);
	Com_sprintf(string5, sizeof(string5), "Avg Drop %3iSPS %3iSPS", asdrop, asdrop2);
	Com_sprintf(string6, sizeof(string6), "Std Dev  %3iSPS %3iSPS", stdsnaps, stdsnaps2);
	Com_sprintf(string7, sizeof(string7), "Delayed  %.1fSPS %.1fSPS", delayedpacksf, delayedpacksf2);
	Com_sprintf(string8, sizeof(string8), "Extrap.  %.1fSPS %.1fSPS", extrapolatedsnapsf, extrapolatedsnapsf2);
	
	if (posx >= 0 && posx < 21) {
		posxx = posx * 30;
	} else {
		posxx = 0;
	}

	if (posy >= 0 && posy < 24) {
		posyy = posy * 20;
	} else {
		posyy = 0;
	}

	SCR_DrawStringExt(posxx, posyy, fontsize, string1, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 1, fontsize, string2, g_color_table[3], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 2, fontsize, string3, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 3, fontsize, string4, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 4, fontsize, string5, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 5, fontsize, string6, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 6, fontsize, string7, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 7, fontsize, string8, g_color_table[7], qtrue, qtrue);
}

/*
=======================================================================================================================================
SCR_DrawPackets

Equivalent to drawping for packets from client to server.
=======================================================================================================================================
*/
void SCR_DrawPackets(void) {
	int posx, posy, posxx, posyy, fontsize, sent, newtime, interval, interval2, i;
	char string1[64], string2[64], string3[64], string4[64], string5[64], string6[64];
	static int *packs, *packs2, pps, count, count2, timerec	, timerec2, lastrecorded, avgpps, avgpps2, mdrop, mdrop2, adrop, adrop2, dcount, dcount2, std, std2, alloc_ok, alloc_ok2;
	static unsigned long long int sum, sum2, stdsum, stdsum2, dsum, dsum2;

	interval = cl_drawpacketsfirstinterval->integer;
	interval2 = cl_drawpacketssecondinterval->integer;
	fontsize = cl_drawpacketsfontsize->integer;
	posx = cl_drawpacketsposx->integer;
	posy = cl_drawpacketsposy->integer;
	newtime = Sys_Milliseconds(); // not affected by timescale or timing alterations by the server.
	sent = clu.sent;
	!timerec ? timerec = timerec2 = lastrecorded = newtime : 0; // initially
	interval <= 0 ? interval = 1 : 0;
	interval2 <= 0 ? interval2 = 1 :0;

	if (sent &&(newtime - lastrecorded)) {
		pps = 1000 / (newtime - lastrecorded);
		lastrecorded = newtime;
	}

	if ((newtime - timerec) / 1000 < interval) {
		if (!(count % 128)) {
			int *tmp = realloc(packs, (count + 128) * sizeof(int));

			if (tmp != NULL) {
				alloc_ok = 1;
				packs = tmp;
			} else {
				alloc_ok = 0;
				count = 0;
			}
		}

		if (alloc_ok) {
			packs[count] = pps;
			count++;
		}
	} else if (count) {
		for (i = 0; i < count; i++) {
			sum += packs[i];
		}

		avgpps = sum / count;
		mdrop = 0;

		for (i = 0; i < count; i++) {
			stdsum += pow((double)(packs[i] - avgpps), 2);

			if (packs[i] < avgpps) {
				dcount++;
				dsum += (avgpps - packs[i]);

				if (packs[i] < avgpps - mdrop) {
					mdrop = avgpps - packs[i];
				}
			}
		}

		std = sqrt((double)(stdsum / count));

		if (dcount) {
			adrop = dsum / dcount;
		} else {
			adrop = 0;
		}

		timerec = newtime;
		sum = count = stdsum = dsum = dcount = 0;
		free(packs);
		packs = NULL;
	}
	// 2
	if ((newtime - timerec2) / 1000 < interval2) {
		if (!(count2 % 128)) {

			int *tmp = realloc(packs2, (count2 + 128) * sizeof(int));

			if (tmp != NULL) {
				alloc_ok2 = 1;
				packs2 = tmp;
			} else {
				alloc_ok2 = 0;
				count2 = 0;
			}
		}

		if (alloc_ok2) {
			packs2[count2] = pps;
			count2++;
		}
	} else if (count2) {
		for (i = 0; i < count2; i++) {
			sum2 += packs2[i];
		}

		avgpps2 = sum2 / count2;
		mdrop2 = 0;
	
		for (i = 0; i < count2; i++) {
			stdsum2 += pow((double)(packs2[i] - avgpps2), 2);

			if (packs2[i] < avgpps2) {
				dcount2++;
				dsum2 += (avgpps2 - packs2[i]);

				if (packs2[i] < avgpps2 - mdrop2) {
					mdrop2 = avgpps2 - packs2[i];
				}
			}
		}

		std2 = sqrt((double)(stdsum2 / count2));

		if (dcount2) {
			adrop2 = dsum2 / dcount2;
		} else {
			adrop2 = 0;
		}

		timerec2 = newtime;
	
		sum2 = count2 = stdsum2 = dsum2 = dcount2 = 0;
		free(packs2);
		packs2 = NULL;
	}

	Com_sprintf(string1, sizeof(string1), "%3iPPS", pps);
	Com_sprintf(string2, sizeof(string2), "Interval %3is  %3is", interval, interval2);
	Com_sprintf(string3, sizeof(string3), "Mean     %3iPPS %3iPPS", avgpps, avgpps2);
	Com_sprintf(string4, sizeof(string4), "Max Drop %3iPPS %3iPPS", mdrop, mdrop2);
	Com_sprintf(string5, sizeof(string5), "Avg Drop %3iPPS %3iPPS", adrop, adrop2);
	Com_sprintf(string6, sizeof(string6), "Std Dev  %3iPPS %3iPPS", std, std2);
	
	if (posx >= 0 && posx < 21) {
		posxx = posx * 30;
	} else {
		posxx = 0;
	}

	if (posy >= 0 && posy < 24) {
		posyy = posy * 20;
	} else {
		posyy = 0;
	}

	SCR_DrawStringExt(posxx, posyy, fontsize, string1, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 1, fontsize, string2, g_color_table[2], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 2, fontsize, string3, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 3, fontsize, string4, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 4, fontsize, string5, g_color_table[7], qtrue, qtrue);
	SCR_DrawStringExt(posxx, posyy + (fontsize + 2) * 5, fontsize, string6, g_color_table[7], qtrue, qtrue);
}

/*
=======================================================================================================================================
SCR_ioq3_urt

Collect ioq3-urt scr stuff.
=======================================================================================================================================
*/
void SCR_ioq3_urt(void) {

	if (cl_drawping->integer) {
		SCR_DrawPing();
	}

	if (cl_drawfps->integer) {
		SCR_DrawFPS();
	}

	if (cl_drawsnaps->integer) {
		SCR_DrawSnaps();
	}

	if (cl_drawpackets->integer) {
		SCR_DrawPackets();
	}
}
#endif
// Tobias END
/*
=======================================================================================================================================
SCR_Init
=======================================================================================================================================
*/
void SCR_Init(void) {

	cl_timegraph = Cvar_Get("timegraph", "0", CVAR_CHEAT);
	cl_debuggraph = Cvar_Get("debuggraph", "0", CVAR_CHEAT);
	cl_graphheight = Cvar_Get("graphheight", "32", CVAR_CHEAT);
	cl_graphscale = Cvar_Get("graphscale", "1", CVAR_CHEAT);
	cl_graphshift = Cvar_Get("graphshift", "0", CVAR_CHEAT);
// Tobias DEBUG
	cl_drawping = Cvar_Get ("cl_drawPing", "0", CVAR_ARCHIVE);
	cl_drawpingfontsize = Cvar_Get ("cl_drawPingFontSize", "7", CVAR_ARCHIVE);
	cl_drawpingposx = Cvar_Get ("cl_drawPingPosX", "0", CVAR_ARCHIVE);
	cl_drawpingposy = Cvar_Get ("cl_drawPingPosY", "14", CVAR_ARCHIVE);
	cl_drawpingfirstinterval = Cvar_Get ("cl_drawPingFirstInterval", "2", CVAR_ARCHIVE);
	cl_drawpingsecondinterval = Cvar_Get ("cl_drawPingSecondInterval", "10", CVAR_ARCHIVE);
	cl_drawfps = Cvar_Get ("cl_drawFPS", "0", CVAR_ARCHIVE);
	cl_drawfpsfontsize = Cvar_Get ("cl_drawFPSfontSize", "7", CVAR_ARCHIVE);
	cl_drawfpsposx = Cvar_Get ("cl_drawFPSposx", "0", CVAR_ARCHIVE);
	cl_drawfpsposy = Cvar_Get ("cl_drawFPSposy", "11", CVAR_ARCHIVE);
	cl_drawfpsinterval = Cvar_Get ("cl_drawFPSinterval", "0.9", CVAR_ARCHIVE);
	cl_drawsnaps = Cvar_Get ("cl_drawSnaps", "0", CVAR_ARCHIVE);
	cl_drawsnapsfontsize = Cvar_Get ("cl_drawSnapsFontSize", "7", CVAR_ARCHIVE);
	cl_drawsnapsposx = Cvar_Get ("cl_drawSnapsPosX", "0", CVAR_ARCHIVE);
	cl_drawsnapsposy = Cvar_Get ("cl_drawSnapsPosY", "7", CVAR_ARCHIVE);
	cl_drawsnapsfirstinterval = Cvar_Get ("cl_drawSnapsFirstInterval", "2", CVAR_ARCHIVE);
	cl_drawsnapssecondinterval = Cvar_Get ("cl_drawSnapsSecondInterval", "10", CVAR_ARCHIVE);
	cl_drawpackets = Cvar_Get ("cl_drawPackets", "0", CVAR_ARCHIVE);
	cl_drawpacketsposx = Cvar_Get ("cl_drawpacketsposx", "0", CVAR_ARCHIVE);
	cl_drawpacketsposy = Cvar_Get ("cl_drawpacketsposy", "2", CVAR_ARCHIVE);
	cl_drawpacketsfontsize = Cvar_Get ("cl_drawpacketsfontsize", "7", CVAR_ARCHIVE);
	cl_drawpacketsfirstinterval = Cvar_Get ("cl_drawpacketsfirstinterval", "2", CVAR_ARCHIVE);
	cl_drawpacketssecondinterval = Cvar_Get ("cl_drawpacketssecondtinterval", "10", CVAR_ARCHIVE);
// Tobias END
	scr_initialized = qtrue;
}

/*
=======================================================================================================================================
SCR_DrawScreenField

This will be called twice if rendering in stereo mode.
=======================================================================================================================================
*/
void SCR_DrawScreenField(stereoFrame_t stereoFrame) {
	qboolean uiFullscreen;

	re.BeginFrame(stereoFrame);

	uiFullscreen = (uivm && VM_Call(uivm, UI_IS_FULLSCREEN));
	// wide aspect ratio screens need to have the sides cleared unless they are displaying game renderings
	if (uiFullscreen || clc.state < CA_LOADING) {
		if (cls.glconfig.vidWidth * 480 > cls.glconfig.vidHeight * 640) {
			re.SetColor(g_color_table[0]);
			re.DrawStretchPic(0, 0, cls.glconfig.vidWidth, cls.glconfig.vidHeight, 0, 0, 0, 0, cls.whiteShader);
			re.SetColor(NULL);
		}
	}
	// if the menu is going to cover the entire screen, we don't need to render anything under it
	if (uivm && !uiFullscreen) {
		switch (clc.state) {
			default:
				Com_Error(ERR_FATAL, "SCR_DrawScreenField: bad clc.state");
				break;
			case CA_CINEMATIC:
				SCR_DrawCinematic();
				break;
			case CA_DISCONNECTED:
				// force menu up
				S_StopAllSounds();
				VM_Call(uivm, UI_SET_ACTIVE_MENU, UIMENU_MAIN);
				break;
			case CA_CONNECTING:
			case CA_CHALLENGING:
			case CA_CONNECTED:
				// connecting clients will only show the connection dialog
				// refresh to update the time
				VM_Call(uivm, UI_REFRESH, cls.realtime);
				VM_Call(uivm, UI_DRAW_CONNECT_SCREEN, qfalse);
				break;
			case CA_LOADING:
			case CA_PRIMED:
				// draw the game information screen and loading progress
				CL_CGameRendering(stereoFrame);
				// also draw the connection information, so it doesn't flash away too briefly on local or lan games
				// refresh to update the time
				VM_Call(uivm, UI_REFRESH, cls.realtime);
				VM_Call(uivm, UI_DRAW_CONNECT_SCREEN, qtrue);
				break;
			case CA_ACTIVE:
				// always supply STEREO_CENTER as vieworg offset is now done by the engine.
				CL_CGameRendering(stereoFrame);
				SCR_DrawDemoRecording();
#ifdef USE_VOIP
				SCR_DrawVoipMeter();
#endif
				SCR_ioq3_urt(); // Tobias DEBUG
				break;
		}
	}
	// the menu draws next
	if (Key_GetCatcher() & KEYCATCH_UI && uivm) {
		VM_Call(uivm, UI_REFRESH, cls.realtime);
	}
	// console draws next
	Con_DrawConsole();
	// debug graph can be drawn on top of anything
	if (cl_debuggraph->integer || cl_timegraph->integer || cl_debugMove->integer) {
		SCR_DrawDebugGraph();
	}
}

/*
=======================================================================================================================================
SCR_UpdateScreen

This is called every frame, and can also be called explicitly to flush text to the screen.
=======================================================================================================================================
*/
void SCR_UpdateScreen(void) {
	static int recursive;

	if (!scr_initialized) {
		return; // not initialized yet
	}

	if (++recursive > 2) {
		Com_Error(ERR_FATAL, "SCR_UpdateScreen: recursively called");
	}

	recursive = 1;
	// if there is no VM, there are also no rendering commands issued. Stop the renderer in that case
	if (uivm || com_dedicated->integer) {
		// XXX
		int in_anaglyphMode = Cvar_VariableIntegerValue("r_anaglyphMode");
		// if running in stereo, we need to draw the frame twice
		if (cls.glconfig.stereoEnabled || in_anaglyphMode) {
			SCR_DrawScreenField(STEREO_LEFT);
			SCR_DrawScreenField(STEREO_RIGHT);
		} else {
			SCR_DrawScreenField(STEREO_CENTER);
		}

		if (com_speeds->integer) {
			re.EndFrame(&time_frontend, &time_backend);
		} else {
			re.EndFrame(NULL, NULL);
		}
	}

	recursive = 0;
}
