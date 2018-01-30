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

#include "client.h"
#include "../botlib/botlib.h"

extern botlib_export_t *botlib_export;

vm_t *uivm;

/*
=======================================================================================================================================
GetClientState
=======================================================================================================================================
*/
static void GetClientState(uiClientState_t *state) {

	state->connectPacketCount = clc.connectPacketCount;
	state->connState = clc.state;

	Q_strncpyz(state->servername, clc.servername, sizeof(state->servername));
	Q_strncpyz(state->updateInfoString, cls.updateInfoString, sizeof(state->updateInfoString));
	Q_strncpyz(state->messageString, clc.serverMessage, sizeof(state->messageString));

	state->clientNum = cl.snap.ps.clientNum;
}

/*
=======================================================================================================================================
LAN_LoadCachedServers
=======================================================================================================================================
*/
void LAN_LoadCachedServers(void) {
	int size;
	fileHandle_t fileIn;

	cls.numglobalservers = cls.numfavoriteservers = 0;
	cls.numGlobalServerAddresses = 0;

	if (FS_SV_FOpenFileRead("servercache.dat", &fileIn)) {
		FS_Read(&cls.numglobalservers, sizeof(int), fileIn);
		FS_Read(&cls.numfavoriteservers, sizeof(int), fileIn);
		FS_Read(&size, sizeof(int), fileIn);

		if (size == sizeof(cls.globalServers) + sizeof(cls.favoriteServers)) {
			FS_Read(&cls.globalServers, sizeof(cls.globalServers), fileIn);
			FS_Read(&cls.favoriteServers, sizeof(cls.favoriteServers), fileIn);
		} else {
			cls.numglobalservers = cls.numfavoriteservers = 0;
			cls.numGlobalServerAddresses = 0;
		}

		FS_FCloseFile(fileIn);
	}
}

/*
=======================================================================================================================================
LAN_SaveServersToCache
=======================================================================================================================================
*/
void LAN_SaveServersToCache(void) {
	int size;
	fileHandle_t fileOut = FS_SV_FOpenFileWrite("servercache.dat");

	FS_Write(&cls.numglobalservers, sizeof(int), fileOut);
	FS_Write(&cls.numfavoriteservers, sizeof(int), fileOut);

	size = sizeof(cls.globalServers) + sizeof(cls.favoriteServers);

	FS_Write(&size, sizeof(int), fileOut);
	FS_Write(&cls.globalServers, sizeof(cls.globalServers), fileOut);
	FS_Write(&cls.favoriteServers, sizeof(cls.favoriteServers), fileOut);
	FS_FCloseFile(fileOut);
}

/*
=======================================================================================================================================
LAN_ResetPings
=======================================================================================================================================
*/
static void LAN_ResetPings(int source) {
	int count, i;
	serverInfo_t *servers = NULL;
	count = 0;

	switch (source) {
		case AS_LOCAL:
			servers = &cls.localServers[0];
			count = MAX_OTHER_SERVERS;
			break;
		case AS_GLOBAL:
			servers = &cls.globalServers[0];
			count = MAX_GLOBAL_SERVERS;
			break;
		case AS_FAVORITES:
			servers = &cls.favoriteServers[0];
			count = MAX_OTHER_SERVERS;
			break;
	}

	if (servers) {
		for (i = 0; i < count; i++) {
			servers[i].ping = -1;
		}
	}
}

/*
=======================================================================================================================================
LAN_AddServer
=======================================================================================================================================
*/
static int LAN_AddServer(int source, const char *name, const char *address) {
	int max, *count, i;
	netadr_t adr;
	serverInfo_t *servers = NULL;

	max = MAX_OTHER_SERVERS;
	count = NULL;

	switch (source) {
		case AS_LOCAL:
			count = &cls.numlocalservers;
			servers = &cls.localServers[0];
			break;
		case AS_GLOBAL:
			max = MAX_GLOBAL_SERVERS;
			count = &cls.numglobalservers;
			servers = &cls.globalServers[0];
			break;
		case AS_FAVORITES:
			count = &cls.numfavoriteservers;
			servers = &cls.favoriteServers[0];
			break;
	}

	if (servers && *count < max) {
		NET_StringToAdr(address, &adr, NA_UNSPEC);

		for (i = 0; i < *count; i++) {
			if (NET_CompareAdr(servers[i].adr, adr)) {
				break;
			}
		}

		if (i >= *count) {
			servers[*count].adr = adr;
			Q_strncpyz(servers[*count].hostName, name, sizeof(servers[*count].hostName));
			servers[*count].visible = qtrue;
			(*count)++;
			return 1;
		}

		return 0;
	}

	return -1;
}

/*
=======================================================================================================================================
LAN_RemoveServer
=======================================================================================================================================
*/
static void LAN_RemoveServer(int source, const char *addr) {
	int *count, i;
	serverInfo_t *servers = NULL;

	count = NULL;

	switch (source) {
		case AS_LOCAL:
			count = &cls.numlocalservers;
			servers = &cls.localServers[0];
			break;
		case AS_GLOBAL:
			count = &cls.numglobalservers;
			servers = &cls.globalServers[0];
			break;
		case AS_FAVORITES:
			count = &cls.numfavoriteservers;
			servers = &cls.favoriteServers[0];
			break;
	}

	if (servers) {
		netadr_t comp;

		NET_StringToAdr(addr, &comp, NA_UNSPEC);

		for (i = 0; i < *count; i++) {
			if (NET_CompareAdr(comp, servers[i].adr)) {
				int j = i;

				while (j < *count - 1) {
					Com_Memcpy(&servers[j], &servers[j + 1], sizeof(servers[j]));
					j++;
				}

				(*count)--;
				break;
			}
		}
	}
}

/*
=======================================================================================================================================
LAN_GetServerCount
=======================================================================================================================================
*/
static int LAN_GetServerCount(int source) {

	switch (source) {
		case AS_LOCAL:
			return cls.numlocalservers;
			break;
		case AS_GLOBAL:
			return cls.numglobalservers;
			break;
		case AS_FAVORITES:
			return cls.numfavoriteservers;
			break;
	}

	return 0;
}

/*
=======================================================================================================================================
LAN_GetServerAddressString
=======================================================================================================================================
*/
static void LAN_GetServerAddressString(int source, int n, char *buf, int buflen) {

	switch (source) {
		case AS_LOCAL:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				Q_strncpyz(buf, NET_AdrToStringwPort(cls.localServers[n].adr), buflen);
				return;
			}

			break;
		case AS_GLOBAL:
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				Q_strncpyz(buf, NET_AdrToStringwPort(cls.globalServers[n].adr), buflen);
				return;
			}

			break;
		case AS_FAVORITES:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				Q_strncpyz(buf, NET_AdrToStringwPort(cls.favoriteServers[n].adr), buflen);
				return;
			}

			break;
	}

	buf[0] = '\0';
}

/*
=======================================================================================================================================
LAN_GetServerInfo
=======================================================================================================================================
*/
static void LAN_GetServerInfo(int source, int n, char *buf, int buflen) {
	char info[MAX_STRING_CHARS];
	serverInfo_t *server = NULL;

	info[0] = '\0';

	switch (source) {
		case AS_LOCAL:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.localServers[n];
			}

			break;
		case AS_GLOBAL:
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				server = &cls.globalServers[n];
			}

			break;
		case AS_FAVORITES:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.favoriteServers[n];
			}

			break;
	}

	if (server && buf) {
		buf[0] = '\0';
		Info_SetValueForKey(info, "hostname", server->hostName);
		Info_SetValueForKey(info, "game", server->game);
		Info_SetValueForKey(info, "gametype", server->gameType);
		Info_SetValueForKey(info, "mapname", server->mapName);
		Info_SetValueForKey(info, "sv_maxclients", va("%i", server->maxClients));
		Info_SetValueForKey(info, "clients", va("%i", server->clients));
		Info_SetValueForKey(info, "nettype", va("%i", server->netType));
		Info_SetValueForKey(info, "minping", va("%i", server->minPing));
		Info_SetValueForKey(info, "maxping", va("%i", server->maxPing));
		Info_SetValueForKey(info, "ping", va("%i", server->ping));
		Info_SetValueForKey(info, "addr", NET_AdrToStringwPort(server->adr));
		Info_SetValueForKey(info, "g_needpass", va("%i", server->g_needpass));
		Info_SetValueForKey(info, "g_humanplayers", va("%i", server->g_humanplayers));
		Q_strncpyz(buf, info, buflen);
	} else {
		if (buf) {
			buf[0] = '\0';
		}
	}
}

/*
=======================================================================================================================================
LAN_GetServerPing
=======================================================================================================================================
*/
static int LAN_GetServerPing(int source, int n) {
	serverInfo_t *server = NULL;

	switch (source) {
		case AS_LOCAL:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.localServers[n];
			}

			break;
		case AS_GLOBAL:
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				server = &cls.globalServers[n];
			}

			break;
		case AS_FAVORITES:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				server = &cls.favoriteServers[n];
			}

			break;
	}

	if (server) {
		return server->ping;
	}

	return -1;
}

/*
=======================================================================================================================================
LAN_GetServerPtr
=======================================================================================================================================
*/
static serverInfo_t *LAN_GetServerPtr(int source, int n) {

	switch (source) {
		case AS_LOCAL:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return &cls.localServers[n];
			}

			break;
		case AS_GLOBAL:
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				return &cls.globalServers[n];
			}

			break;
		case AS_FAVORITES:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return &cls.favoriteServers[n];
			}

			break;
	}

	return NULL;
}

/*
=======================================================================================================================================
LAN_CompareServers
=======================================================================================================================================
*/
static int LAN_CompareServers(int source, int sortKey, int sortDir, int s1, int s2) {
	serverInfo_t *server1, *server2;
	int res, clients1, clients2;

	server1 = LAN_GetServerPtr(source, s1);
	server2 = LAN_GetServerPtr(source, s2);

	if (!server1 || !server2) {
		return 0;
	}

	res = 0;

	switch (sortKey) {
		case SORT_HOST:
			res = Q_stricmp(server1->hostName, server2->hostName);
			break;
		case SORT_MAP:
			res = Q_stricmp(server1->mapName, server2->mapName);
			break;
		case SORT_MAXCLIENTS:
		case SORT_CLIENTS:
		case SORT_HUMANS:
		case SORT_BOTS:
			if (sortKey == SORT_MAXCLIENTS) {
				clients1 = server1->maxClients;
				clients2 = server2->maxClients;
			} else if (sortKey == SORT_HUMANS) {
				clients1 = server1->g_humanplayers;
				clients2 = server2->g_humanplayers;
			} else if (sortKey == SORT_BOTS) {
				clients1 = server1->clients - server1->g_humanplayers;
				clients2 = server2->clients - server2->g_humanplayers;
			} else {
				clients1 = server1->clients;
				clients2 = server2->clients;
			}

			if (clients1 < clients2) {
				res = -1;
			} else if (clients1 > clients2) {
				res = 1;
			} else {
				res = 0;
			}

			break;
		case SORT_GAMETYPE:
			res = Q_stricmp(server1->gameType, server2->gameType);
			break;
		case SORT_PING:
			if (server1->ping < server2->ping) {
				res = -1;
			} else if (server1->ping > server2->ping) {
				res = 1;
			} else {
				res = 0;
			}

			break;
	}

	if (sortDir) {
		if (res < 0) {
			return 1;
		}

		if (res > 0) {
			return -1;
		}

		return 0;
	}

	return res;
}

/*
=======================================================================================================================================
LAN_GetPingQueueCount
=======================================================================================================================================
*/
static int LAN_GetPingQueueCount(void) {
	return (CL_GetPingQueueCount());
}

/*
=======================================================================================================================================
LAN_ClearPing
=======================================================================================================================================
*/
static void LAN_ClearPing(int n) {
	CL_ClearPing(n);
}

/*
=======================================================================================================================================
LAN_GetPing
=======================================================================================================================================
*/
static void LAN_GetPing(int n, char *buf, int buflen, int *pingtime) {
	CL_GetPing(n, buf, buflen, pingtime);
}

/*
=======================================================================================================================================
LAN_GetPingInfo
=======================================================================================================================================
*/
static void LAN_GetPingInfo(int n, char *buf, int buflen) {
	CL_GetPingInfo(n, buf, buflen);
}

/*
=======================================================================================================================================
LAN_MarkServerVisible
=======================================================================================================================================
*/
static void LAN_MarkServerVisible(int source, int n, qboolean visible) {

	if (n == -1) {
		int count = MAX_OTHER_SERVERS;
		serverInfo_t *server = NULL;

		switch (source) {
			case AS_LOCAL:
				server = &cls.localServers[0];
				break;
			case AS_GLOBAL:
				server = &cls.globalServers[0];
				count = MAX_GLOBAL_SERVERS;
				break;
			case AS_FAVORITES:
				server = &cls.favoriteServers[0];
				break;
		}

		if (server) {
			for (n = 0; n < count; n++) {
				server[n].visible = visible;
			}
		}
	} else {
		switch (source) {
			case AS_LOCAL:
				if (n >= 0 && n < MAX_OTHER_SERVERS) {
					cls.localServers[n].visible = visible;
				}

				break;
			case AS_GLOBAL:
				if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
					cls.globalServers[n].visible = visible;
				}

				break;
			case AS_FAVORITES:
				if (n >= 0 && n < MAX_OTHER_SERVERS) {
					cls.favoriteServers[n].visible = visible;
				}

				break;
		}
	}
}

/*
=======================================================================================================================================
LAN_ServerIsVisible
=======================================================================================================================================
*/
static int LAN_ServerIsVisible(int source, int n) {

	switch (source) {
		case AS_LOCAL:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return cls.localServers[n].visible;
			}

			break;
		case AS_GLOBAL:
			if (n >= 0 && n < MAX_GLOBAL_SERVERS) {
				return cls.globalServers[n].visible;
			}

			break;
		case AS_FAVORITES:
			if (n >= 0 && n < MAX_OTHER_SERVERS) {
				return cls.favoriteServers[n].visible;
			}

			break;
	}

	return qfalse;
}

/*
=======================================================================================================================================
LAN_UpdateVisiblePings
=======================================================================================================================================
*/
qboolean LAN_UpdateVisiblePings(int source) {
	return CL_UpdateVisiblePings_f(source);
}

/*
=======================================================================================================================================
LAN_GetServerStatus
=======================================================================================================================================
*/
int LAN_GetServerStatus(char *serverAddress, char *serverStatus, int maxLen) {
	return CL_ServerStatus(serverAddress, serverStatus, maxLen);
}

/*
=======================================================================================================================================
CL_GetGlconfig
=======================================================================================================================================
*/
static void CL_GetGlconfig(glconfig_t *config) {
	*config = cls.glconfig;
}

/*
=======================================================================================================================================
CL_GetClipboardData
=======================================================================================================================================
*/
static void CL_GetClipboardData(char *buf, int buflen) {
	char *cbd;

	cbd = Sys_GetClipboardData();

	if (!cbd) {
		*buf = 0;
		return;
	}

	Q_strncpyz(buf, cbd, buflen);
	Z_Free(cbd);
}

/*
=======================================================================================================================================
GetConfigString
=======================================================================================================================================
*/
static int GetConfigString(int index, char *buf, int size) {
	int offset;

	if (index < 0 || index >= MAX_CONFIGSTRINGS) {
		return qfalse;
	}

	offset = cl.gameState.stringOffsets[index];

	if (!offset) {
		if (size) {
			buf[0] = 0;
		}

		return qfalse;
	}

	Q_strncpyz(buf, cl.gameState.stringData + offset, size);

	return qtrue;
}

/*
=======================================================================================================================================
CL_AddUICommand
=======================================================================================================================================
*/
void CL_AddUICommand(const char *cmdName) {
	Cmd_AddCommand(cmdName, NULL);
}

/*
=======================================================================================================================================
FloatAsInt
=======================================================================================================================================
*/
static int FloatAsInt(float f) {
	floatint_t fi;

	fi.f = f;
	return fi.i;
}

/*
=======================================================================================================================================
CL_UISystemCalls

The ui module is making a system call.
=======================================================================================================================================
*/
intptr_t CL_UISystemCalls(intptr_t *args) {

	switch (args[0]) {
		case TRAP_MEMSET:
			Com_Memset(VMA(1), args[2], args[3]);
			return args[1];
		case TRAP_MEMCPY:
			Com_Memcpy(VMA(1), VMA(2), args[3]);
			return args[1];
		case TRAP_STRNCPY:
			strncpy(VMA(1), VMA(2), args[3]);
			return args[1];
		case TRAP_SIN:
			return FloatAsInt(sin(VMF(1)));
		case TRAP_COS:
			return FloatAsInt(cos(VMF(1)));
		case TRAP_ATAN2:
			return FloatAsInt(atan2(VMF(1), VMF(2)));
		case TRAP_SQRT:
			return FloatAsInt(sqrt(VMF(1)));
		case TRAP_FLOOR:
			return FloatAsInt(floor(VMF(1)));
		case TRAP_CEIL:
			return FloatAsInt(ceil(VMF(1)));
		case TRAP_ACOS:
			return FloatAsInt(Q_acos(VMF(1)));
		case TRAP_ASIN:
			return FloatAsInt(Q_asin(VMF(1)));
		case TRAP_TAN:
			return FloatAsInt(tan(VMF(1)));
		case TRAP_ATAN:
			return FloatAsInt(atan(VMF(1)));
		case TRAP_POW:
			return FloatAsInt(pow(VMF(1), VMF(2)));
		case TRAP_EXP:
			return FloatAsInt(exp(VMF(1)));
		case TRAP_LOG:
			return FloatAsInt(log(VMF(1)));
		case TRAP_LOG10:
			return FloatAsInt(log10(VMF(1)));
		case UI_PRINT:
			Com_Printf("%s", (const char *)VMA(1));
			return 0;
		case UI_ERROR:
			Com_Error(ERR_DROP, "%s", (const char *)VMA(1));
			return 0;
		case UI_MILLISECONDS:
			return Sys_Milliseconds();
		case UI_REAL_TIME:
			return Com_RealTime(VMA(1));
		case UI_SNAPVECTOR:
			Q_SnapVector(VMA(1));
			return 0;
		case UI_ARGC:
			return Cmd_Argc();
		case UI_ARGV:
			Cmd_ArgvBuffer(args[1], VMA(2), args[3]);
			return 0;
		case UI_CVAR_RESET:
			Cvar_Reset(VMA(1));
			return 0;
		case UI_ARGS:
			Cmd_ArgsBuffer(VMA(1), args[2]);
			return 0;
		case UI_ADDCOMMAND:
			CL_AddUICommand(VMA(1));
			return 0;
		case UI_REMOVECOMMAND:
			Cmd_RemoveCommandSafe(VMA(1));
			return 0;
		case UI_CMD_EXECUTETEXT:
			Cbuf_ExecuteTextSafe(args[1], VMA(2));
			return 0;
		case UI_CVAR_REGISTER:
			Cvar_Register(VMA(1), VMA(2), VMA(3), args[4]);
			return 0;
		case UI_CVAR_UPDATE:
			Cvar_Update(VMA(1));
			return 0;
		case UI_CVAR_SET:
			Cvar_SetSafe(VMA(1), VMA(2));
			return 0;
		case UI_CVAR_SET_VALUE:
			Cvar_SetValueSafe(VMA(1), VMF(2));
			return 0;
		case UI_CVAR_VARIABLE_VALUE:
			return FloatAsInt(Cvar_VariableValue(VMA(1)));
		case UI_CVAR_INFO_STRING_BUFFER:
			Cvar_InfoStringBuffer(args[1], VMA(2), args[3]);
			return 0;
		case UI_CVAR_VARIABLE_INTEGER_VALUE:
			return Cvar_VariableIntegerValue(VMA(1));
		case UI_CVAR_VARIABLE_STRING_BUFFER:
			Cvar_VariableStringBuffer(VMA(1), VMA(2), args[3]);
			return 0;
		case UI_CVAR_LATCHED_VARIABLE_STRING_BUFFER:
			Cvar_LatchedVariableStringBuffer(VMA(1), VMA(2), args[3]);
			return 0;
		case UI_FS_FOPENFILE:
			return FS_FOpenFileByMode(VMA(1), VMA(2), args[3]);
		case UI_FS_READ:
			FS_Read(VMA(1), args[2], args[3]);
			return 0;
		case UI_FS_WRITE:
			FS_Write(VMA(1), args[2], args[3]);
			return 0;
		case UI_FS_SEEK:
			return FS_Seek(args[1], args[2], args[3]);
		case UI_FS_FCLOSEFILE:
			FS_FCloseFile(args[1]);
			return 0;
		case UI_FS_GETFILELIST:
			return FS_GetFileList(VMA(1), VMA(2), VMA(3), args[4]);
		case UI_PC_ADD_GLOBAL_DEFINE:
			return botlib_export->PC_AddGlobalDefine(VMA(1));
		case UI_PC_REMOVE_ALL_GLOBAL_DEFINES:
			botlib_export->PC_RemoveAllGlobalDefines();
			return 0;
		case UI_PC_LOAD_SOURCE:
			return botlib_export->PC_LoadSourceHandle(VMA(1));
		case UI_PC_FREE_SOURCE:
			return botlib_export->PC_FreeSourceHandle(args[1]);
		case UI_PC_READ_TOKEN:
			return botlib_export->PC_ReadTokenHandle(args[1], VMA(2));
		case UI_PC_UNREAD_TOKEN:
			botlib_export->PC_UnreadLastTokenHandle(args[1]);
			return 0;
		case UI_PC_SOURCE_FILE_AND_LINE:
			return botlib_export->PC_SourceFileAndLine(args[1], VMA(2), VMA(3));
		case UI_GETGLCONFIG:
			CL_GetGlconfig(VMA(1));
			return 0;
		case UI_MEMORY_REMAINING:
			return Hunk_MemoryRemaining();
		case UI_UPDATESCREEN:
			SCR_UpdateScreen();
			return 0;
		case UI_GETCLIENTSTATE:
			GetClientState(VMA(1));
			return 0;
		case UI_GETCONFIGSTRING:
			return GetConfigString(args[1], VMA(2), args[3]);
		case UI_LAN_GETSERVERCOUNT:
			return LAN_GetServerCount(args[1]);
		case UI_LAN_GETSERVERADDRESSSTRING:
			LAN_GetServerAddressString(args[1], args[2], VMA(3), args[4]);
			return 0;
		case UI_LAN_GETSERVERINFO:
			LAN_GetServerInfo(args[1], args[2], VMA(3), args[4]);
			return 0;
		case UI_LAN_MARKSERVERVISIBLE:
			LAN_MarkServerVisible(args[1], args[2], args[3]);
			return 0;
		case UI_LAN_UPDATEVISIBLEPINGS:
			return LAN_UpdateVisiblePings(args[1]);
		case UI_LAN_RESETPINGS:
			LAN_ResetPings(args[1]);
			return 0;
		case UI_LAN_LOADCACHEDSERVERS:
			LAN_LoadCachedServers();
			return 0;
		case UI_LAN_SAVECACHEDSERVERS:
			LAN_SaveServersToCache();
			return 0;
		case UI_LAN_ADDSERVER:
			return LAN_AddServer(args[1], VMA(2), VMA(3));
		case UI_LAN_REMOVESERVER:
			LAN_RemoveServer(args[1], VMA(2));
			return 0;
		case UI_LAN_SERVERSTATUS:
			return LAN_GetServerStatus(VMA(1), VMA(2), args[3]);
		case UI_LAN_GETSERVERPING:
			return LAN_GetServerPing(args[1], args[2]);
		case UI_LAN_SERVERISVISIBLE:
			return LAN_ServerIsVisible(args[1], args[2]);
		case UI_LAN_COMPARESERVERS:
			return LAN_CompareServers(args[1], args[2], args[3], args[4], args[5]);
		case UI_LAN_GETPING:
			LAN_GetPing(args[1], VMA(2), args[3], VMA(4));
			return 0;
		case UI_LAN_GETPINGINFO:
			LAN_GetPingInfo(args[1], VMA(2), args[3]);
			return 0;
		case UI_LAN_GETPINGQUEUECOUNT:
			return LAN_GetPingQueueCount();
		case UI_LAN_CLEARPING:
			LAN_ClearPing(args[1]);
			return 0;
		case UI_R_REGISTERMODEL:
			return re.RegisterModel(VMA(1));
		case UI_R_REGISTERSKIN:
			return re.RegisterSkin(VMA(1));
		case UI_R_REGISTERSHADER:
			return re.RegisterShader(VMA(1));
		case UI_R_REGISTERSHADERNOMIP:
			return re.RegisterShaderNoMip(VMA(1));
		case UI_R_REGISTERFONT:
			re.RegisterFont(VMA(1), args[2], VMF(3), args[4], VMA(5));
			return 0;
		case UI_R_CLEARSCENE:
			re.ClearScene();
			return 0;
		case UI_R_ADDREFENTITYTOSCENE:
			re.AddRefEntityToScene(VMA(1));
			return 0;
		case UI_R_ADDPOLYTOSCENE:
			re.AddPolyToScene(args[1], args[2], VMA(3), 1);
			return 0;
		case UI_R_ADDLIGHTTOSCENE:
			re.AddLightToScene(VMA(1), VMF(2), VMF(3), VMF(4), VMF(5));
			return 0;
		case UI_R_RENDERSCENE:
			re.RenderScene(VMA(1));
			return 0;
		case UI_R_SETCOLOR:
			re.SetColor(VMA(1));
			return 0;
		case UI_R_DRAWSTRETCHPIC:
			re.DrawStretchPic(VMF(1), VMF(2), VMF(3), VMF(4), VMF(5), VMF(6), VMF(7), VMF(8), args[9]);
			return 0;
		case UI_R_DRAWSTRETCHPIC_GRADIENT:
			re.DrawStretchPicGradient(VMF(1), VMF(2), VMF(3), VMF(4), VMF(5), VMF(6), VMF(7), VMF(8), args[9], VMA(10));
			return 0;
		case UI_R_DRAWROTATEDPIC:
			re.DrawRotatedPic(VMF(1), VMF(2), VMF(3), VMF(4), VMF(5), VMF(6), VMF(7), VMF(8), args[9], VMF(10));
			return 0;
		case UI_R_LERPTAG:
			return re.LerpTag(VMA(1), args[2], args[3], args[4], VMF(5), VMA(6));
		case UI_R_MODELBOUNDS:
			re.ModelBounds(args[1], VMA(2), VMA(3));
			return 0;
		case UI_R_REMAP_SHADER:
			re.RemapShader(VMA(1), VMA(2), VMA(3));
			return 0;
		case UI_R_SETCLIPREGION:
			re.SetClipRegion(VMA(1));
			return 0;
		case UI_CL_TRANSLATE_STRING:
			CL_TranslateString(VMA(1), VMA(2));
			return 0;
		case UI_S_REGISTERSOUND:
			return S_RegisterSound(VMA(1), args[2]);
		case UI_S_SOUNDDURATION:
			return S_SoundDuration(args[1]);
		case UI_S_STARTLOCALSOUND:
			S_StartLocalSound(args[1], args[2]);
			return 0;
		case UI_S_STARTBACKGROUNDTRACK:
			S_StartBackgroundTrack(VMA(1), VMA(2));
			return 0;
		case UI_S_STOPBACKGROUNDTRACK:
			S_StopBackgroundTrack();
			return 0;
		case UI_KEY_KEYNUMTOSTRINGBUF:
			Key_KeynumToStringBuf(args[1], VMA(2), args[3]);
			return 0;
		case UI_KEY_GETBINDINGBUF:
			Key_GetBindingBuf(args[1], VMA(2), args[3]);
			return 0;
		case UI_KEY_SETBINDING:
			Key_SetBinding(args[1], VMA(2));
			return 0;
		case UI_KEY_ISDOWN:
			return Key_IsDown(args[1]);
		case UI_KEY_GETOVERSTRIKEMODE:
			return Key_GetOverstrikeMode();
		case UI_KEY_SETOVERSTRIKEMODE:
			Key_SetOverstrikeMode(args[1]);
			return 0;
		case UI_KEY_CLEARSTATES:
			Key_ClearStates();
			return 0;
		case UI_KEY_GETCATCHER:
			return Key_GetCatcher();
		case UI_KEY_SETCATCHER:
			// don't allow the ui module to close the console
			Key_SetCatcher(args[1]|(Key_GetCatcher() & KEYCATCH_CONSOLE));
			return 0;
		case UI_KEY_GETKEY:
			return Key_GetKey(VMA(1), args[2]);
		case UI_GETCLIPBOARDDATA:
			CL_GetClipboardData(VMA(1), args[2]);
			return 0;
		case UI_CIN_PLAYCINEMATIC:
			Com_DPrintf("UI_CIN_PlayCinematic\n");
			return CIN_PlayCinematic(VMA(1), args[2], args[3], args[4], args[5], args[6]);
		case UI_CIN_STOPCINEMATIC:
			return CIN_StopCinematic(args[1]);
		case UI_CIN_RUNCINEMATIC:
			return CIN_RunCinematic(args[1]);
		case UI_CIN_DRAWCINEMATIC:
			CIN_DrawCinematic(args[1]);
			return 0;
		case UI_CIN_SETEXTENTS:
			CIN_SetExtents(args[1], args[2], args[3], args[4], args[5]);
			return 0;
		default:
			Com_Error(ERR_DROP, "Bad UI system trap: %ld", (long int)args[0]);
	}

	return 0;
}

/*
=======================================================================================================================================
CL_ShutdownUI
=======================================================================================================================================
*/
void CL_ShutdownUI(void) {

	Key_SetCatcher(Key_GetCatcher() & ~KEYCATCH_UI);

	cls.uiStarted = qfalse;

	if (!uivm) {
		return;
	}

	VM_Call(uivm, UI_SHUTDOWN);
	VM_Free(uivm);

	uivm = NULL;
}

/*
=======================================================================================================================================
CL_InitUI
=======================================================================================================================================
*/
void CL_InitUI(void) {
	int v;
	vmInterpret_t interpret;

	// load the dll or bytecode
	interpret = Cvar_VariableValue("vm_ui");

	if (cl_connectedToPureServer) {
		// if sv_pure is set we only allow qvms to be loaded
		if (interpret != VMI_COMPILED && interpret != VMI_BYTECODE) {
			interpret = VMI_COMPILED;
		}
	}

	uivm = VM_Create("ui", CL_UISystemCalls, interpret);

	if (!uivm) {
		Com_Error(ERR_FATAL, "VM_Create on UI failed");
	}
	// sanity check
	v = VM_Call(uivm, UI_GETAPIVERSION);

	if (v != UI_API_VERSION) {
		// Free uivm now, so UI_SHUTDOWN doesn't get called later.
		VM_Free(uivm);
		uivm = NULL;

		Com_Error(ERR_DROP, "User Interface is version %d, expected %d", v, UI_API_VERSION);
		cls.uiStarted = qfalse;
	} else {
		// init for this gamestate
		VM_Call(uivm, UI_INIT, (clc.state >= CA_CONNECTING && clc.state < CA_ACTIVE));
	}
}

/*
=======================================================================================================================================
UI_GameCommand

See if the current console command is claimed by the ui.
=======================================================================================================================================
*/
qboolean UI_GameCommand(void) {

	if (!uivm) {
		return qfalse;
	}

	return VM_Call(uivm, UI_CONSOLE_COMMAND, cls.realtime);
}
