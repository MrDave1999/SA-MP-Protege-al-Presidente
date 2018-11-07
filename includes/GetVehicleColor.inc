#if !defined _INC_GetVehicleColor
	#define _INC_GetVehicleColor

#include <YSI\y_hooks>

#define DestroyVehicleEx(%0) \
	DestroyVehicle(%0); \
		VehicleColor_Change[(%0)-1][COLOR_PRIMARY] = INVALID_VEHICLE_ID; \
			VehicleColor_Change[(%0)-1][COLOR_SECUNDARY] = INVALID_VEHICLE_ID
			
#define ChangeVehicleColorEx(%0,%1,%2) \
	ChangeVehicleColor(%0,%1,%2); \
		VehicleColor_Change[(%0)-1][COLOR_PRIMARY] = (%1); \
			VehicleColor_Change[(%0)-1][COLOR_SECUNDARY] = (%2)

new VehicleColor_Create[MAX_VEHICLES][2]; //Guardamos la ID del color cuando el vehículo se crea
new VehicleColor_Change[MAX_VEHICLES][2]; //Guardamos la ID del color cuando lo cambiamos con la función ChangeVehicleColor
 
const COLOR_PRIMARY = 0;
const COLOR_SECUNDARY = 1;

hook OnGameModeInit()
{
	for(new i = 0; i < MAX_VEHICLES; ++i)
	{
	    VehicleColor_Change[i][COLOR_PRIMARY] = INVALID_VEHICLE_ID;
	    VehicleColor_Change[i][COLOR_SECUNDARY] = INVALID_VEHICLE_ID;
	}
	return 1;
}

public OnVehicleSpawn(vehicleid)
{
	//printf("DEBUG: OnVehicleSpawn");
    VehicleColor_Change[vehicleid-1][COLOR_PRIMARY] = INVALID_VEHICLE_ID;
    VehicleColor_Change[vehicleid-1][COLOR_SECUNDARY] = INVALID_VEHICLE_ID;
	return 1;
}

hook_AddStaticVehicleEx(modelid, Float:spawn_x, Float:spawn_y, Float:spawn_z, Float:z_angle, color1, color2, time)
{
	new vehicleid = 0;
	vehicleid = AddStaticVehicleEx(modelid, spawn_x, spawn_y, spawn_z, z_angle, color1, color2, time);
	VehicleColor_Create[vehicleid-1][COLOR_PRIMARY] = ((color1 == -1) ? (VehicleColor_Create[vehicleid-1][COLOR_PRIMARY] = random(256)) : (VehicleColor_Create[vehicleid-1][COLOR_PRIMARY] = color1));
    VehicleColor_Create[vehicleid-1][COLOR_SECUNDARY] = ((color2 == -1) ? (VehicleColor_Create[vehicleid-1][COLOR_SECUNDARY] = random(256)) : (VehicleColor_Create[vehicleid-1][COLOR_SECUNDARY] = color2));
	return vehicleid;
}

GetVehicleColor(vehicleid, &color1, &color2)
{
	color1 = ((VehicleColor_Change[vehicleid-1][COLOR_PRIMARY] == INVALID_PLAYER_ID) ? (VehicleColor_Create[vehicleid-1][COLOR_PRIMARY]) : (VehicleColor_Change[vehicleid-1][COLOR_PRIMARY]));
	color2 = ((VehicleColor_Change[vehicleid-1][COLOR_SECUNDARY] == INVALID_PLAYER_ID) ? (VehicleColor_Create[vehicleid-1][COLOR_SECUNDARY]) : (VehicleColor_Change[vehicleid-1][COLOR_SECUNDARY]));
	return 1;
}

SetVehicleColorEx(playerid, vehicleid) //Función para establecer el color de un vehículo cuando se deja de usar el cmd /cc
{
	if(VIP[playerid][ColorPrimary] != -1) //Si el jugador tiene guardado un color primario y secundario...
	{
	    ChangeVehicleColorEx(vehicleid, VIP[playerid][ColorPrimary], VIP[playerid][ColorSecundary]);
	}
	    
	else if(VehicleColor_Change[vehicleid-1][COLOR_PRIMARY] != INVALID_VEHICLE_ID) //Si el vehículo fue cambiado por un color...
	{
	    ChangeVehicleColor(vehicleid, VehicleColor_Change[vehicleid-1][COLOR_PRIMARY], VehicleColor_Change[vehicleid-1][COLOR_SECUNDARY]);
	}

	else //De lo contrario, el vehículo seguirá con el mismo color...
	    ChangeVehicleColor(vehicleid, VehicleColor_Create[vehicleid-1][COLOR_PRIMARY], VehicleColor_Create[vehicleid-1][COLOR_SECUNDARY]);
}

cmd:colorid(playerid) //Command Test
{
    new vehicleid = GetPlayerVehicleID(playerid);
    if(vehicleid != 0)
    {
		new string[70];
		new color1 = 0;
		new color2 = 0;
		GetVehicleColor(vehicleid, color1, color2);
		format(string, sizeof(string), "ID Vehículo: %d - ID Color Primario: %d - ID Color Secundario: %d", vehicleid, color1, color2);
		SendClientMessage(playerid, -1, string);
	}
	return 1;
}

//========================================================
#if !defined _ALS_AddStaticVehicleEx
	#define _ALS_AddStaticVehicleEx
#else
	#undef AddStaticVehicleEx
#endif
#define AddStaticVehicleEx hook_AddStaticVehicleEx
//========================================================
#endif /* FIN _INC_GetVehicleColor */
