#if !defined _INC_missingtime
	#define _INC_missingtime
#else
	#endinput
#endif

#include <YSI\y_hooks>

forward TimeVicePresident(playerid, name[], value[]);
forward TimePresident(playerid, name[], value[]);

static Day;

hook OnPlayerConnect(playerid)
{
	new NamePath1[47];
	new NamePath2[51];
	Route2(playerid, 1, NamePath1);
	Route2(playerid, 0, NamePath2);
	Data[playerid][President] = ((fexist(NamePath1)) ? (1) : (0));/* Si guarda 1 es porque existe un archivo con su nombre, caso contrario es porque no existe (0). */
	Data[playerid][VicePresident] = ((fexist(NamePath2)) ? (1) : (0));/* Si guarda 1 es porque existe un archivo con su nombre, caso contrario es porque no existe (0). */
	if(Data[playerid][President] == 1)
	{
		INI_ParseFile(NamePath1, "TimePresident",.bExtra = true, .extra = playerid);
		if((Day < getdate()) || Data[playerid][President] < gettime())
		{
			fremove(NamePath1);
			Data[playerid][President] = 0;
		}
	}
	if(Data[playerid][VicePresident] == 1)
	{
		INI_ParseFile(NamePath2, "TimeVicePresident",.bExtra = true, .extra = playerid);
		if((Day < getdate()) || (Data[playerid][VicePresident] < gettime()))
		{
			fremove(NamePath2);
			Data[playerid][VicePresident] = 0;
		}
	}
	return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
	if(Data[playerid][President] > 0)
	{
	    new NamePath[47];
	    Route2(playerid, 1, NamePath);
		if(Data[playerid][President] > gettime())
		{
			new INI:File = INI_Open(NamePath);
			INI_WriteInt(File, "Tiempo", Data[playerid][President]);
			INI_WriteInt(File, "Dias", getdate());
			INI_Close(File);
		}
		else if(fexist(NamePath))
				fremove(NamePath);
	}

	if(Data[playerid][VicePresident] > 0)
	{
	    new NamePath[51];
	    Route2(playerid, 0, NamePath);
		if(Data[playerid][VicePresident] > gettime())
		{
			new INI:File = INI_Open(NamePath);
			INI_WriteInt(File, "Tiempo", Data[playerid][VicePresident]);
			INI_WriteInt(File, "Dias", getdate());
			INI_Close(File);
		}
		else if(fexist(NamePath))
				fremove(NamePath);
	}
	return 1;
}

Route2(playerid, type, namepath[]) 
{
	new name[24];
	GetPlayerName(playerid, name, 24);
	if(type)
		format(namepath, 47, "Dadmin/Presidentes/%s.ini", name);
	else
		format(namepath, 51, "Dadmin/Vicepresidentes/%s.ini", name);
	return 1;
}

public TimePresident(playerid, name[], value[])
{
	INI_Int("Tiempo", Data[playerid][President]);
	INI_Int("Dias", Day);
	return 1;
}

public TimeVicePresident(playerid, name[], value[])
{
	INI_Int("Tiempo", Data[playerid][VicePresident]);
	INI_Int("Dias", Day);
	return 1;
}
