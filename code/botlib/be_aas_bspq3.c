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
 BSP, Environment Sampling.
**************************************************************************************************************************************/

#include "../qcommon/q_shared.h"
#include "l_memory.h"
#include "l_script.h"
#include "l_precomp.h"
#include "l_struct.h"
#include "aasfile.h"
#include "botlib.h"
#include "be_aas.h"
#include "be_aas_funcs.h"
#include "be_interface.h"
#include "be_aas_def.h"

#define MAX_BSPENTITIES 2048

typedef struct rgb_s {
	int red;
	int green;
	int blue;
} rgb_t;
// bsp entity epair
typedef struct bsp_epair_s {
	char *key;
	char *value;
	struct bsp_epair_s *next;
} bsp_epair_t;
// bsp data entity
typedef struct bsp_entity_s {
	bsp_epair_t *epairs;
} bsp_entity_t;
// id Software BSP data
typedef struct bsp_s {
	// true when bsp file is loaded
	int loaded;
	// entity data
	int entdatasize;
	char *dentdata;
	// bsp entities
	int numentities;
	bsp_entity_t entities[MAX_BSPENTITIES];
} bsp_t;
// global bsp
bsp_t bspworld;

/*
=======================================================================================================================================
AAS_Trace

Traces axial boxes of any size through the world.
=======================================================================================================================================
*/
bsp_trace_t AAS_Trace(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int passent, int contentmask) {
	bsp_trace_t bsptrace;

	botimport.Trace(&bsptrace, start, mins, maxs, end, passent, contentmask);
	return bsptrace;
}

/*
=======================================================================================================================================
AAS_TraceEntities

Traces axial boxes of any size against entities.
=======================================================================================================================================
*/
bsp_trace_t AAS_TraceEntities(vec3_t start, vec3_t mins, vec3_t maxs, vec3_t end, int passent, int contentmask) {
	bsp_trace_t bsptrace;

	botimport.TraceEntities(&bsptrace, start, mins, maxs, end, passent, contentmask);
	return bsptrace;
}

/*
=======================================================================================================================================
AAS_PointContents

Returns the contents at the given point.
=======================================================================================================================================
*/
int AAS_PointContents(vec3_t point) {
	return botimport.PointContents(point);
}

/*
=======================================================================================================================================
AAS_EntityCollision
=======================================================================================================================================
*/
qboolean AAS_EntityCollision(int entnum, vec3_t start, vec3_t boxmins, vec3_t boxmaxs, vec3_t end, int contentmask, bsp_trace_t *trace) {
	bsp_trace_t enttrace;

	botimport.EntityTrace(&enttrace, start, boxmins, boxmaxs, end, entnum, contentmask);

	if (enttrace.fraction < trace->fraction) {
		Com_Memcpy(trace, &enttrace, sizeof(bsp_trace_t));
		return qtrue;
	}

	return qfalse;
}

/*
=======================================================================================================================================
AAS_inPVS

Returns true if in Potentially Visible Set.
=======================================================================================================================================
*/
qboolean AAS_inPVS(vec3_t p1, vec3_t p2) {
	return botimport.inPVS(p1, p2);
}

/*
=======================================================================================================================================
AAS_inPHS

Returns true if in Potentially Hearable Set.
=======================================================================================================================================
*/
qboolean AAS_inPHS(vec3_t p1, vec3_t p2) {
	return qtrue;
}

/*
=======================================================================================================================================
AAS_BSPModelMinsMaxsOrigin
=======================================================================================================================================
*/
void AAS_BSPModelMinsMaxsOrigin(int modelnum, vec3_t angles, vec3_t mins, vec3_t maxs, vec3_t origin) {
	botimport.BSPModelMinsMaxsOrigin(modelnum, angles, mins, maxs, origin);
}

/*
=======================================================================================================================================
AAS_UnlinkFromBSPLeaves

Unlinks the entity from all leaves.
=======================================================================================================================================
*/
void AAS_UnlinkFromBSPLeaves(bsp_link_t *leaves) {

}

/*
=======================================================================================================================================
AAS_BSPLinkEntity
=======================================================================================================================================
*/
bsp_link_t *AAS_BSPLinkEntity(vec3_t absmins, vec3_t absmaxs, int entnum, int modelnum) {
	return NULL;
}

/*
=======================================================================================================================================
AAS_BoxEntities
=======================================================================================================================================
*/
int AAS_BoxEntities(vec3_t absmins, vec3_t absmaxs, int *list, int maxcount) {
	return 0;
}

/*
=======================================================================================================================================
AAS_NextBSPEntity
=======================================================================================================================================
*/
int AAS_NextBSPEntity(int ent) {

	ent++;

	if (ent >= 1 && ent < bspworld.numentities) {
		return ent;
	}

	return 0;
}

/*
=======================================================================================================================================
AAS_BSPEntityInRange
=======================================================================================================================================
*/
int AAS_BSPEntityInRange(int ent) {

	if (ent <= 0 || ent >= bspworld.numentities) {
		botimport.Print(PRT_MESSAGE, "bsp entity out of range\n");
		return qfalse;
	}

	return qtrue;
}

/*
=======================================================================================================================================
AAS_ValueForBSPEpairKey
=======================================================================================================================================
*/
int AAS_ValueForBSPEpairKey(int ent, char *key, char *value, int size) {
	bsp_epair_t *epair;

	value[0] = '\0';

	if (!AAS_BSPEntityInRange(ent)) {
		return qfalse;
	}

	for (epair = bspworld.entities[ent].epairs; epair; epair = epair->next) {
		if (!strcmp(epair->key, key)) {
			Q_strncpyz(value, epair->value, size);
			return qtrue;
		}
	}

	return qfalse;
}

/*
=======================================================================================================================================
AAS_VectorForBSPEpairKey
=======================================================================================================================================
*/
int AAS_VectorForBSPEpairKey(int ent, char *key, vec3_t v) {
	char buf[MAX_EPAIRKEY];
	double v1, v2, v3;

	VectorClear(v);

	if (!AAS_ValueForBSPEpairKey(ent, key, buf, MAX_EPAIRKEY)) {
		return qfalse;
	}
	// scanf into doubles, then assign, so it is vec_t size independent
	v1 = v2 = v3 = 0;

	sscanf(buf, "%lf %lf %lf", &v1, &v2, &v3);

	v[0] = v1;
	v[1] = v2;
	v[2] = v3;
	return qtrue;
}

/*
=======================================================================================================================================
AAS_FloatForBSPEpairKey
=======================================================================================================================================
*/
int AAS_FloatForBSPEpairKey(int ent, char *key, float *value) {
	char buf[MAX_EPAIRKEY];

	*value = 0;

	if (!AAS_ValueForBSPEpairKey(ent, key, buf, MAX_EPAIRKEY)) {
		return qfalse;
	}

	*value = atof(buf);
	return qtrue;
}

/*
=======================================================================================================================================
AAS_IntForBSPEpairKey
=======================================================================================================================================
*/
int AAS_IntForBSPEpairKey(int ent, char *key, int *value) {
	char buf[MAX_EPAIRKEY];

	*value = 0;

	if (!AAS_ValueForBSPEpairKey(ent, key, buf, MAX_EPAIRKEY)) {
		return qfalse;
	}

	*value = atoi(buf);
	return qtrue;
}

/*
=======================================================================================================================================
AAS_FreeBSPEntities
=======================================================================================================================================
*/
void AAS_FreeBSPEntities(void) {
	int i;
	bsp_entity_t *ent;
	bsp_epair_t *epair, *nextepair;

	for (i = 1; i < bspworld.numentities; i++) {
		ent = &bspworld.entities[i];

		for (epair = ent->epairs; epair; epair = nextepair) {
			nextepair = epair->next;

			if (epair->key) {
				FreeMemory(epair->key);
			}

			if (epair->value) {
				FreeMemory(epair->value);
			}

			FreeMemory(epair);
		}
	}

	bspworld.numentities = 0;
}

/*
=======================================================================================================================================
AAS_ParseBSPEntities
=======================================================================================================================================
*/
void AAS_ParseBSPEntities(void) {
	script_t *script;
	token_t token;
	bsp_entity_t *ent;
	bsp_epair_t *epair;

	script = LoadScriptMemory(bspworld.dentdata, bspworld.entdatasize, "entdata");

	SetScriptFlags(script, SCFL_NOSTRINGWHITESPACES|SCFL_NOSTRINGESCAPECHARS); // SCFL_PRIMITIVE

	bspworld.numentities = 1;

	while (PS_ReadToken(script, &token)) {
		if (strcmp(token.string, "{")) {
			ScriptError(script, "invalid %s", token.string);
			AAS_FreeBSPEntities();
			FreeScript(script);
			return;
		}

		if (bspworld.numentities >= MAX_BSPENTITIES) {
			botimport.Print(PRT_MESSAGE, "AAS_ParseBSPEntities: too many entities in BSP file\n");
			break;
		}

		ent = &bspworld.entities[bspworld.numentities];

		bspworld.numentities++;

		ent->epairs = NULL;

		while (PS_ReadToken(script, &token)) {
			if (!strcmp(token.string, "}")) {
				break;
			}

			epair = (bsp_epair_t *)GetClearedHunkMemory(sizeof(bsp_epair_t));
			epair->next = ent->epairs;
			ent->epairs = epair;

			if (token.type != TT_STRING) {
				ScriptError(script, "invalid %s", token.string);
				AAS_FreeBSPEntities();
				FreeScript(script);
				return;
			}

			StripDoubleQuotes(token.string);

			epair->key = (char *)GetHunkMemory(strlen(token.string) + 1);

			strcpy(epair->key, token.string);

			if (!PS_ExpectTokenType(script, TT_STRING, 0, &token)) {
				AAS_FreeBSPEntities();
				FreeScript(script);
				return;
			}

			StripDoubleQuotes(token.string);

			epair->value = (char *)GetHunkMemory(strlen(token.string) + 1);

			strcpy(epair->value, token.string);
		}

		if (strcmp(token.string, "}")) {
			ScriptError(script, "missing }");
			AAS_FreeBSPEntities();
			FreeScript(script);
			return;
		}
	}

	FreeScript(script);
}

/*
=======================================================================================================================================
AAS_BSPTraceLight
=======================================================================================================================================
*/
int AAS_BSPTraceLight(vec3_t start, vec3_t end, vec3_t endpos, int *red, int *green, int *blue) {
	return 0;
}

/*
=======================================================================================================================================
AAS_DumpBSPData
=======================================================================================================================================
*/
void AAS_DumpBSPData(void) {

	AAS_FreeBSPEntities();

	if (bspworld.dentdata) {
		FreeMemory(bspworld.dentdata);
	}

	bspworld.dentdata = NULL;
	bspworld.entdatasize = 0;
	bspworld.loaded = qfalse;

	Com_Memset(&bspworld, 0, sizeof(bspworld));
}

/*
=======================================================================================================================================
AAS_LoadBSPFile

Load a .bsp file.
=======================================================================================================================================
*/
int AAS_LoadBSPFile(void) {

	AAS_DumpBSPData();

	bspworld.entdatasize = strlen(botimport.BSPEntityData()) + 1;
	bspworld.dentdata = (char *)GetClearedHunkMemory(bspworld.entdatasize);

	Com_Memcpy(bspworld.dentdata, botimport.BSPEntityData(), bspworld.entdatasize);

	AAS_ParseBSPEntities();

	bspworld.loaded = qtrue;
	return BLERR_NOERROR;
}
