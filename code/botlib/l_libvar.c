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
 Bot library variables.
**************************************************************************************************************************************/

#include "../qcommon/q_shared.h"
#include "l_memory.h"
#include "l_libvar.h"

// list with library variables
libvar_t *libvarlist = NULL;

/*
=======================================================================================================================================
LibVarStringValue
=======================================================================================================================================
*/
float LibVarStringValue(const char *string) {
	int dotfound = 0;
	float value = 0;

	while (*string) {
		if (*string < '0' || *string > '9') {
			if (dotfound || *string != '.') {
				return 0;
			} else {
				dotfound = 10;
				string++;
			}
		}

		if (dotfound) {
			value = value + (float)(*string - '0') / (float)dotfound;
			dotfound *= 10;
		} else {
			value = value * 10.0 + (float)(*string - '0');
		}

		string++;
	}

	return value;
}

/*
=======================================================================================================================================
LibVarAlloc
=======================================================================================================================================
*/
libvar_t *LibVarAlloc(const char *var_name) {
	libvar_t *v;

	v = (libvar_t *)GetMemory(sizeof(libvar_t));

	Com_Memset(v, 0, sizeof(libvar_t));

	v->name = (char *)GetMemory(strlen(var_name) + 1);
	strcpy(v->name, var_name);
	// add the variable in the list
	v->next = libvarlist;
	libvarlist = v;
	return v;
}

/*
=======================================================================================================================================
LibVarDeAlloc
=======================================================================================================================================
*/
void LibVarDeAlloc(libvar_t *v) {

	if (v->string) {
		FreeMemory(v->string);
	}

	FreeMemory(v->name);
	FreeMemory(v);
}

/*
=======================================================================================================================================
LibVarDeAllocAll
=======================================================================================================================================
*/
void LibVarDeAllocAll(void) {
	libvar_t *v;

	for (v = libvarlist; v; v = libvarlist) {
		libvarlist = libvarlist->next;
		LibVarDeAlloc(v);
	}

	libvarlist = NULL;
}

/*
=======================================================================================================================================
LibVarGet
=======================================================================================================================================
*/
libvar_t *LibVarGet(const char *var_name) {
	libvar_t *v;

	for (v = libvarlist; v; v = v->next) {
		if (!Q_stricmp(v->name, var_name)) {
			return v;
		}
	}

	return NULL;
}

/*
=======================================================================================================================================
LibVarGetString
=======================================================================================================================================
*/
char *LibVarGetString(const char *var_name) {
	libvar_t *v;

	v = LibVarGet(var_name);

	if (v) {
		return v->string;
	} else {
		return "";
	}
}

/*
=======================================================================================================================================
LibVarGetValue
=======================================================================================================================================
*/
float LibVarGetValue(const char *var_name) {
	libvar_t *v;

	v = LibVarGet(var_name);

	if (v) {
		return v->value;
	} else {
		return 0;
	}
}

/*
=======================================================================================================================================
LibVar
=======================================================================================================================================
*/
libvar_t *LibVar(const char *var_name, const char *value) {
	libvar_t *v;

	v = LibVarGet(var_name);

	if (v) {
		return v;
	}
	// create new variable
	v = LibVarAlloc(var_name);
	// variable string
	v->string = (char *)GetMemory(strlen(value) + 1);
	strcpy(v->string, value);
	// the value
	v->value = LibVarStringValue(v->string);
	// variable is modified
	v->modified = qtrue;
	return v;
}

/*
=======================================================================================================================================
LibVarString
=======================================================================================================================================
*/
char *LibVarString(const char *var_name, const char *value) {
	libvar_t *v;

	v = LibVar(var_name, value);
	return v->string;
}

/*
=======================================================================================================================================
LibVarValue
=======================================================================================================================================
*/
float LibVarValue(const char *var_name, const char *value) {
	libvar_t *v;

	v = LibVar(var_name, value);
	return v->value;
}

/*
=======================================================================================================================================
LibVarSet
=======================================================================================================================================
*/
void LibVarSet(const char *var_name, const char *value) {
	libvar_t *v;

	v = LibVarGet(var_name);

	if (v) {
		FreeMemory(v->string);
	} else {
		v = LibVarAlloc(var_name);
	}
	// variable string
	v->string = (char *)GetMemory(strlen(value) + 1);
	strcpy(v->string, value);
	// the value
	v->value = LibVarStringValue(v->string);
	// variable is modified
	v->modified = qtrue;
}

/*
=======================================================================================================================================
LibVarChanged
=======================================================================================================================================
*/
qboolean LibVarChanged(const char *var_name) {
	libvar_t *v;

	v = LibVarGet(var_name);

	if (v) {
		return v->modified;
	} else {
		return qfalse;
	}
}

/*
=======================================================================================================================================
LibVarSetNotModified
=======================================================================================================================================
*/
void LibVarSetNotModified(const char *var_name) {
	libvar_t *v;

	v = LibVarGet(var_name);

	if (v) {
		v->modified = qfalse;
	}
}
