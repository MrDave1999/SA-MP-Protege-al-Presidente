//Funciones globales de PAP (f_pap.inc)

#if !defined _INC_fpap
	#define _INC_fpap
	
#define TIME_AD 					(60*5*1000) // Tiempo anuncio - 5 minutos
#define DestroyVehicleForAll(%0,%1) for(new vehicleid = 0; vehicleid < (%0); ++vehicleid){DestroyVehicleEx(%1);}

forward Commands(name[], value[]);

enum
{
    PRESIDENTE_SOBREVIVIDO = -2, //Cuando sobrevive los 15 minutos
    PRESIDENTE_MUERTO_1, //Murió por un Terrorista
    PRESIDENTE_MUERTO_2, //Murió por un Civil
    PRESIDENTE_MUERTO_3, //Murió en un accidente
    PRESIDENTE_DESAPARECIDO,  //Cuando se desconecta
    NO_HAY_PRESIDENTE = 0,
    SI_HAY_PRESIDENTE
};

udb_hash(buf[]) /* Esta función es un encriptador, que convierte un texto mediante una clave. Esta función la creó DrakoBlue. */
{
	new length=strlen(buf);
	new s1 = 1;
	new s2 = 0;
	new n;
	for (n=0; n<length; ++n)
	{
		s1 = (s1 + buf[n]) % 65521;
		s2 = (s2 + s1)     % 65521;
	}
	return (s2 << 16) + s1;
}

SetPlayerMessagePM(playerid, params, const message[])
{
	new string[128];
	new name1[24];
	new name2[24];
	GetPlayerName(playerid, name1, sizeof(name1));
	GetPlayerName(params, name2, sizeof(name2));
	format(string, sizeof(string), "[PM]: %s(%d) te dice: %s", name1, playerid, message);
	SendClientMessage(params, Amarillo, string);
	if(Data[params][Level] > 0)
		SendClientMessage(params, -1, "{FF0000}* Utiliza /r {FFFF00}para responder el mensaje privado");
	GameTextForPlayer(playerid, "MENSAJE ENVIADO", 5000, 4);
	GameTextForPlayer(params, "MENSAJE RECIBIDO", 5000, 4);
	PlayerPlaySound(playerid, 1058, 0.0, 0.0, 0.0);
	PlayerPlaySound(params, 1058, 0.0, 0.0, 0.0);
	/* playerid = id jugador quien envia el mensaje privado (esto solo pasa cuando es un administrador o moderador) */
	/* params = id jugador quien recibe el mensaje privado (esto solo pasa cuando es un administrador o moderador) */
	format(string, sizeof(string), "[PM]: %s le dice a %s: %s", name1, name2, message);
	foreach(new i : Player)
	{
		if(!(Data[i][Level] >= 3 && i != playerid && i != params && Data[playerid][Level] <= Data[i][Level])) continue;
		SendClientMessage(i, Amarillo, string);
	}
}

SetPlayerTimeCMD(playerid, const string[], time, total_time)
{
	new str[100];
	new Float:diferencia = time-gettime();
	new Float:minutos = diferencia/60;
	new Float:segundos = 0.0;
	if(diferencia > 60)
	{
		for(new i = 1; i < total_time; ++i)
		{
			if(minutos > i && minutos < i+1)
			{
				segundos = ((minutos-i)*100);
				break;
			}
		}
	}
	if(diferencia <= 60)
	{
		format(str, sizeof(str), "ERROR: Te falta %.0f %s %s.",
		diferencia,
		((diferencia == 1) ? ("segundo") : ("segundos")),
		string);
	}
	else
	{
		format(str, sizeof(str), "ERROR: Te falta %.0f %s con %.0f %s %s.",
		minutos,
		((minutos < 2) ? ("minuto") : ("minutos")),
		segundos,
		((segundos < 2) ? ("segundo") : ("segundos")),
		string);
	}
	SendClientMessage(playerid, Rojo, str);
	return 1;
}

IsPlayerSpaceText(&bool:value, const string[])
{
	new i;
	new len = strlen(string);
	for(i = 0; i < len; ++i)
	{
	    if(string[i] == ' ')
	    	value = true;
	    else
	    {
	        value = false;
			break;
	    }
	}
	return 0;
}

GetNameWheels(modelid, name[])
{
	new aux = ((modelid == 1025) ? (1086) : ((modelid == 1096) ? (1087) : ((modelid == 1097) ? (1088) : ((modelid == 1098) ? (1089) : (-1)))));
	if(aux != -1)
	    modelid = aux;
	new const NameWheels[17][9] =
	{
	    {"Shadow"},
	    {"Mega"},
	    {"Rimshine"},
	    {"Wires"},
	    {"Classic"},
	    {"Twist"},
     	{"Cutter"},
	    {"Switch"},
     	{"Grove"},
	    {"Import"},
	    {"Dollar"},
	    {"Trance"},
	    {"Atomic"}, //MODELID = 1085
	    {"Offroad"}, //MODELID = 1025
	    {"Ahab"}, //MODELID = 1096
	    {"Virtual"}, //MODELID = 1097
	    {"Access"} //MODELID = 1098
	};
	format(name, 9, "%s", NameWheels[modelid-1073]);
}

/*
Si la función de cada balance devuelve 0, quiere decir que no esta equilibrado.
Si devuelve 1, quiere decir que si esta equilibrado.

- Explicación de como funciona el balanceo:
Se podrá escoger cada clase, siempre y cuando la diferencia que haya entre ellos no sea mayor o igual a 3.
*/

TeamBalance(teamid)
{
	new Team_President = 0;
	new CountTeams[7] = {0,...};
	new i;
	foreach(i : Player)
	{
		if(IsPlayerInClassSelection(i) != 1)
		{
		    switch(Data[i][Teams])
		    {
				case PRESIDENT:
					++CountTeams[PRESIDENT-1];
				case VICEPRESIDENT:
					++CountTeams[VICEPRESIDENT-1];
				case SECURITY:
					++CountTeams[SECURITY-1];
				case POLI:
					++CountTeams[POLI-1];
				case SWAT:
					++CountTeams[SWAT-1];
				case TERRO:
					++CountTeams[TERRO-1];
				case CIVIL:
					++CountTeams[CIVIL-1];
		    }
		}
	}
	for(i = 0; i < 5; ++i)
	    Team_President += CountTeams[i];
	switch(teamid)
	{
		case SECURITY..SWAT:
		{
			if((Team_President-CountTeams[TERRO-1]) >= 3)
				return 0;
		}
		case TERRO:
		{
			if((CountTeams[TERRO-1]-Team_President) >= 3)
				return 0;
		}
		case CIVIL:
		{
			if(((CountTeams[CIVIL-1]-Team_President) >= 5) && ((CountTeams[CIVIL-1]-CountTeams[TERRO-1]) >= 5))
				return 0;
		}
	}
	return 1;
}

SetPlayerSelectTeam(playerid)
{
	new string[435];
	new Team_President = 0;
	new CountTeams[7] = {0,...};
	new i;
	foreach(i : Player)
	{
		if(IsPlayerInClassSelection(i) != 1)
		{
		    switch(Data[i][Teams])
		    {
				case PRESIDENT:
					++CountTeams[PRESIDENT-1];
				case VICEPRESIDENT:
					++CountTeams[VICEPRESIDENT-1];
				case SECURITY:
					++CountTeams[SECURITY-1];
				case POLI:
					++CountTeams[POLI-1];
				case SWAT:
					++CountTeams[SWAT-1];
				case TERRO:
					++CountTeams[TERRO-1];
				case CIVIL:
					++CountTeams[CIVIL-1];
		    }
		}
	}
	for(i = 0; i < 5; ++i)
	    Team_President += CountTeams[i];
	format(string, sizeof(string),
	"Clase:\tUsuarios:\tAccesibilidad:\n\
	{FFFF00}Presidente\t{FFFFFF}%d\t{FF0000}%s\n\
	{FFFF00}VicePresidente\t{FFFFFF}%d\t{FF0000}%s\n\
	{FFFF00}Seguridad\t{FFFFFF}%d\t{FF0000}%s\n\
	{FFFF00}Policía\t{FFFFFF}%d\t{FF0000}%s\n\
	{FFFF00}SWAT\t{FFFFFF}%d\t{FF0000}%s\n\
	{FFFF00}Terrorista\t{FFFFFF}%d\t{FF0000}%s\n\
	{FFFF00}Civiles\t{FFFFFF}%d\t{FF0000}%s",
	CountTeams[0], ((CountTeams[0] == 0) ? ("Disponible") : ("No Disponible")),
	CountTeams[1], ((CountTeams[1] == 0) ? ("Disponible") : ("No Disponible")),
	CountTeams[2], ((Team_President-CountTeams[TERRO-1] < 3) ? ("Disponible") : ("No Disponible")),
	CountTeams[3], ((Team_President-CountTeams[TERRO-1] < 3) ? ("Disponible") : ("No Disponible")),
	CountTeams[4], ((Team_President-CountTeams[TERRO-1] < 3) ? ("Disponible") : ("No Disponible")),
	CountTeams[5], ((CountTeams[TERRO-1]-Team_President < 3) ? ("Disponible") : ("No Disponible")),
	CountTeams[6], ((((CountTeams[CIVIL-1]-Team_President) < 5) && ((CountTeams[CIVIL-1]-CountTeams[TERRO-1]) < 5)) ? ("Disponible") : ("No Disponible")));
	ShowPlayerDialog(playerid, DIALOG_TEAM, DIALOG_STYLE_TABLIST_HEADERS, "Selecciona una Clase", string, "Seleccionar", "Cerrar");
	return 1;
}

SetPlayerDuty(playerid, teamid)
{
    switch(teamid)
	{
		case PRESIDENT:
		{
			new duty_presi[497+100];
			strcat(duty_presi, "{FFFFFF}Tu deber es sobrevivir la ronda de 15 minutos. Puedes hacer esto de la manera que quieras (acampar, correr, ...).\n");
			strcat(duty_presi, "{FFFFFF}La {41A323}seguridad {FFFFFF}son tus guardaespaldas personales. Deben permanecer contigo y protegerte de los {8D1515}terroristas.\n");
			strcat(duty_presi, "{FFFFFF}La {0066FF}policía {FFFFFF}y {0066FF}SWAT {FFFFFF}también te ayudarán matando a los terroristas.\n\n");
			strcat(duty_presi, "{FFFFFF}* Aliados: {FFFF00}VicePresidente, {41A323}Seguridad, {0066FF}Policía, {0066FF}SWAT\n");
			strcat(duty_presi, "{FFFFFF}* Enemigos: {8D1515}Terroristas\n\n");
			strcat(duty_presi, "{FFFFFF}- Armas que usa el {FFFF00}Presidente:\n");
			strcat(duty_presi, "{FFFFFF}1. Desert Eagle\n");
			strcat(duty_presi, "2. Flores\n");
			strcat(duty_presi, "3. Palo de golf");
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Deber: {FFFF00}Presidente", duty_presi, "Cerrar", "");
		}
		case VICEPRESIDENT:
		{
		    new duty_vice[380+113];
			strcat(duty_vice,"{FFFFFF}Tu deber es solo esperar hasta que el {FFFF00}Presidente {FFFFFF}muera o sobreviva. Tu ícono no se muestra en el mapa\n");
			strcat(duty_vice,"para que nadie pueda encontrarte cuando el Presidente fallezca o se desconecte. Tú asumirás el puesto como Presidente.\n\n");
			strcat(duty_vice,"{FFFFFF}* Aliados: {FFFF00}Presidente, {41A323}Seguridad, {0066FF}Policía, {0066FF}SWAT\n");
			strcat(duty_vice,"{FFFFFF}* Enemigos: {8D1515}Terroristas\n\n");
			strcat(duty_vice, "{FFFFFF}- Armas que usa el {FFFF00}VicePresidente:\n");
			strcat(duty_vice, "{FFFFFF}1. Pistola silenciadora\n");
			strcat(duty_vice, "2. Flores\n");
			strcat(duty_vice, "3. Palo de golf");
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Deber: {FFFF00}VicePresidente", duty_vice, "Cerrar", "");
		}
		case SECURITY:
		{
 			new duty_secu[478+95];
			strcat(duty_secu,"{FFFFFF}Usted es el guardaespalda personal del {FFFF00}Presidente. {FFFFFF}Tu deber es simplemente proteger al presidente cueste lo que cueste.\n");
			strcat(duty_secu,"Debes mantenerte cerca de él, a menos que el presidente diga lo contrario o te dé otra tarea.\n");
			strcat(duty_secu,"La {0066FF}policía {FFFFFF}y {0066FF}SWAT {FFFFFF}te ayudará con tu tarea matando a los terroristas.\n\n");
			strcat(duty_secu,"{FFFFFF}* Aliados: {FFFF00}Presidente, {FFFF00}VicePresidente, {0066FF}Policía, {0066FF}SWAT\n");
			strcat(duty_secu,"{FFFFFF}* Enemigos: {8D1515}Terroristas\n\n");
			strcat(duty_secu, "{FFFFFF}- Armas que usan la {41A323}Seguridad:\n");
			strcat(duty_secu, "{FFFFFF}1. Sawnoff (recortada)\n");
			strcat(duty_secu, "2. MP5\n");
			strcat(duty_secu, "3. M4");
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Deber: {41A323}Seguridad", duty_secu, "Cerrar", "");
		}
		case POLI:
		{
		    new duty_poli[289+93];
			strcat(duty_poli, "{FFFFFF}Tu deber es ayudar al {FFFF00}presidente {FFFFFF}matando a los {8D1515}terroristas. {FFFFFF}Los {0066FF}SWAT {FFFFFF}te ayudará con tu tarea.\n\n");
			strcat(duty_poli,"{FFFFFF}* Aliados: {FFFF00}Presidente, {FFFF00}VicePresidente, {41A323}Seguridad, {0066FF}SWAT\n");
			strcat(duty_poli,"{FFFFFF}* Enemigos: {8D1515}Terroristas\n\n");
			strcat(duty_poli, "{FFFFFF}- Armas que usan los {0066FF}Policías:\n");
			strcat(duty_poli, "{FFFFFF}1. Desert Eagle\n");
			strcat(duty_poli, "2. Micro-Uzi\n");
			strcat(duty_poli, "3. M4");
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Deber: {0066FF}Policía", duty_poli, "Cerrar", "");
		}
		case SWAT:
		{
			new duty_swat[296+91];
			strcat(duty_swat, "{FFFFFF}Tu deber es ayudar al {FFFF00}presidente {FFFFFF}matando a los {8D1515}terroristas. {FFFFFF}Los {0066FF}policías {FFFFFF}te ayudará con tu tarea.\n\n");
			strcat(duty_swat,"{FFFFFF}* Aliados: {FFFF00}Presidente, {FFFF00}VicePresidente, {41A323}Seguridad, {0066FF}Policía\n");
			strcat(duty_swat,"{FFFFFF}* Enemigos: {8D1515}Terroristas\n\n");
			strcat(duty_swat, "{FFFFFF}- Armas que usan los {0066FF}SWAT:\n");
			strcat(duty_swat, "{FFFFFF}1. Desert Eagle\n");
			strcat(duty_swat, "2. Shotgun\n");
			strcat(duty_swat, "3. Tec-9");
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Deber: {0066FF}SWAT", duty_swat, "Cerrar", "");
		}
		case TERRO:
		{
		    new duty_terro[344+105];
			strcat(duty_terro,"{FFFFFF}Tu deber es matar al {FFFF00}Presidente. {FFFFFF}El Presidente estará protegido por la {41A323}seguridad.\n");
			strcat(duty_terro,"{FFFFFF}La {0066FF}policía {FFFFFF}y {0066FF}SWAT {FFFFFF}intentarán matarte también.\n\n");
			strcat(duty_terro,"{FFFFFF}* Aliados: Ninguno\n");
			strcat(duty_terro,"{FFFFFF}* Enemigos: {FFFF00}Presidente,{FFFF00} VicePresidente, {41A323}Seguridad, {0066FF}Policía, {0066FF}SWAT\n\n");
			strcat(duty_terro, "{FFFFFF}- Armas que usan los {8D1515}Terroristas:\n");
			strcat(duty_terro, "{FFFFFF}1. Tec-9\n");
			strcat(duty_terro, "2. Escopeta de combate\n");
			strcat(duty_terro, "3. Ak-47");
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Deber: {8D1515}Terrorista", duty_terro, "Cerrar", "");
		}
		case CIVIL:
		{
  			new duty_civil[275+94];
			strcat(duty_civil,"{FFFFFF}Los civiles no tienen un deber, pueden hacer lo que quieran.\n");
			strcat(duty_civil,"{FFFFFF}Eso significa que puedes matar al {FFFF00}presidente {FFFFFF}como protegerlo o no hacer nada y solo andar.\n");
			strcat(duty_civil,"Los civiles NO son un equipo, lo que significa que también puedes matar a otros civiles.\n\n");
			strcat(duty_civil, "{FFFFFF}- Armas que usan los {FF6600}Civiles:\n");
			strcat(duty_civil, "{FFFFFF}1. Desert Eagle\n");
			strcat(duty_civil, "2. Shotgun\n");
			strcat(duty_civil, "3. Dildo");
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, "Deber: {FF6600}Civil", duty_civil, "Cerrar", "");
		}
	}
	return 1;
}

SetPos(playerid, Float:x, Float:y, Float:z, Float:angle, interior, worldvirtual, vehicleid = 0)
{
	if(!(IsPlayerInAnyVehicle(playerid) == 1))
	{
		SetPlayerPos(playerid, x, y, z);
		SetPlayerInterior(playerid, interior);
		SetPlayerFacingAngle(playerid, angle);
		SetPlayerVirtualWorld(playerid, worldvirtual);
	}
	else
	{
		SetVehiclePos(vehicleid, x, y, z);
		SetVehicleZAngle(vehicleid, angle);
		LinkVehicleToInterior(vehicleid, interior);
		SetVehicleVirtualWorld(vehicleid, worldvirtual);
	}
	return 1;
}

GetPos(playerid, &Float:x, &Float:y, &Float:z, &Float:angle, &interior, &worldvirtual, vehicleid = 0)
{
	interior = GetPlayerInterior(playerid);
	if(!(IsPlayerInAnyVehicle(playerid) == 1))
	{
		GetPlayerPos(playerid, x, y, z);
		GetPlayerFacingAngle(playerid, angle);
		worldvirtual = GetPlayerVirtualWorld(playerid);
	}
	else
	{
		GetVehiclePos(vehicleid, x, y, z);
		GetVehicleZAngle(vehicleid, angle);
		worldvirtual = GetVehicleVirtualWorld(vehicleid);
	}
	return 1;
}

timer TimeLeft_President[60000]()/*  Esta función mide el tiempo restante (en minutos) que sobrevive el Presidente dentro de una ronda (15 minutos). */
{ //Cada 1 minuto se ejecutará la función "TimeLeft_President".
	//print("DEBUG: TimerLeft_President()\n");
	if(id_president != INVALID_PLAYER_ID)
	{
		if(Data[id_president][Teams] != PRESIDENT)
		{ //Restricción para evitar algún bug por un presidente que no "existe"
	    	id_president = INVALID_PLAYER_ID;
	    	stop Timer_President;
	    	return 1;
		}
		if((id_vicepresident != INVALID_PLAYER_ID) && (Data[id_vicepresident][Teams] != VICEPRESIDENT))
		    id_vicepresident = INVALID_PLAYER_ID;
	    new string[96];
		new text[25];
		new name[24];
		GetPlayerName(id_president, name, sizeof(name));
		if(--Minutes != 0)
		{
			format(string, sizeof(string), "(Central de Inteligencia) El Presidente %s estará seguro en %d %s.", name, Minutes, ((Minutes == 1) ? ("minuto") : ("minutos")));
			SendClientMessageToAll(Amarillo, string);
			format(text, sizeof(text), "~b~%d %s", Minutes, ((Minutes == 1) ? ("minuto restante") : ("minutos restantes")));
            TextDrawSetString(TextTL, text);
			foreach(new i : Player)
			{
				if((!IsPlayerRace(i)) && (EstarEnDuelo[i] != ESTA_EN_DUELO))
				//Si el jugador no está en una carrera y en un duelo, la condición se cumple.
				{
				    if(TimeLeft[i][GAME_TEXT])
						GameTextForPlayer(i, text, 5000, 3);
					if(TimeLeft[i][TEXT_DRAW])
					    TextDrawShowForPlayer(i, TextTL);
				}
			}
		}
		if(Minutes == 0)
		{
			++Data[id_president][PS];
			WriteData(id_president, PRESIDENT_SURVIVALS);
			Data[id_president][Teams] = PRESIDENTE_SOBREVIVIDO;
			//printf("DEBUG: Tiempo restante - %d", Minutes);
			SetPlayerMap(NO_HAY_PRESIDENTE);
		}
	}
	else
	    stop Timer_President;
	return 1;
}

timer LoadMap[10000]() /* Este prodecimiento va a cargar el nuevo mapa en 10 segundos. */
{
	new map[21];
	new newmap[49];
	IsMapLoading = false;
	id_president = INVALID_PLAYER_ID;
	id_vicepresident = INVALID_PLAYER_ID;
	City = ++CountMaps;
	if(CountMaps == 3)
		CountMaps = 0;
	format(map, sizeof(map), "mapname %s", GetMapName(City));
	SendRconCommand(map);
	format(newmap, sizeof(newmap), "* El mapa: %s se ha cargado con éxito.", GetMapName(City));
	SendClientMessageToAll(0x97FF2FFF, newmap);
	foreach(new i : Player)
	{
	    if((IsPlayerRace(i) == true) || (EstarEnDuelo[i] >= -1 && EstarEnDuelo[i] <= 9))
	    {
	        continue;
	    }
	    if(IsPlayerInClassSelection(i) != 0)
	    {
	        CallRemoteFunction("OnPlayerRequestClass", "dd", i, 0);
	        continue;
     	}
     	Data[i][ForcePlayer] = JUGADOR_SI_FORZADO;
     	Data[i][IsClassSelection] = true;
		TogglePlayerControllable(i, 1);
		ForceClassSelection(i);
		TogglePlayerSpectating(i, 1);
		TogglePlayerSpectating(i, 0);
	}
}

/*
Esta función va a crear el nuevo mapa a todos los jugadores.
Si el parámetro "type" recibe 0, quiere decir que se establecerá un nuevo mapa para todos los jugadores (razón: presidente muerto o sobrevivido).
Si el parámetro "type" recibe 1, quiere decir que se establecerá el mismo mapa para todos los jugadores (razón: hay un vicepresidente para tomar el cargo de presidente).
*/
SetPlayerMap(type)
{
    switch(Data[id_president][Teams])
	{
		case PRESIDENTE_SOBREVIVIDO:
		{
			SendClientMessageToAll(Amarillo, "(Central de Inteligencia) El Presidente está a salvo.");
			SendClientMessageToAll(Amarillo, "(Central de Inteligencia) El Presidente ha sobrevivido.");
		}
		case PRESIDENTE_MUERTO_1:
			SendClientMessageToAll(Amarillo, "(Central de Inteligencia) El Presidente fue asesinado por un Terrorista.");
		case PRESIDENTE_MUERTO_2:
        	SendClientMessageToAll(Amarillo, "(Central de Inteligencia) El Presidente fue asesinado por un Civil.");
		case PRESIDENTE_MUERTO_3:
    	    SendClientMessageToAll(Amarillo, "(Central de Inteligencia) El Presidente murió en un accidente.");
		case PRESIDENTE_DESAPARECIDO:
	    	SendClientMessageToAll(Amarillo, "(Central de Inteligencia) El Presidente desapareció misteriosamente.");
	}
	if(type == NO_HAY_PRESIDENTE)
	{
		new Float:X;
		new Float:Y;
		new Float:Z;
		stop Timer_President;
        IsMapLoading = true;
        if(MayorRacha > 1)
            TextDrawDestroy(TextSpree[0]);
		Data[id_president][President] = gettime() + (15 * 60); /* Tiempo de espera para volver a elegir la clase de presidente (15 minutos) */
		if(!(Data[id_president][Teams] == PRESIDENTE_SOBREVIVIDO))//Si el presidente no sobrevivió
		{
			SendClientMessageToAll(Amarillo, "(Central de Inteligencia) No hay un VicePresidente para tomar el cargo de Presidente.");
			SendClientMessageToAll(0x97FF2FFF, "* Los terroristas han tenido éxito.");
		}
		SendClientMessageToAll(0x97FF2FFF, "* El siguiente mapa comenzará en 10 segundos.");
		foreach(new i : Player)
		{
		    SetPlayerScore(i, 0);
		    if(IsPlayerInClassSelection(i) != 1)
				SetPlayerWorldBounds(i, 20000.0000, -20000.0000, 20000.0000, -20000.0000);//Borra el límite que hayas puesto antes
			if(TimeLeft[i][TEXT_DRAW] == true)
			{
			    TextDrawSetString(TextTL, ((Data[id_president][Teams] == PRESIDENTE_SOBREVIVIDO) ? ("~y~Ganador: Equipo Presidente") : ("~r~Ganador: Terroristas")));
				TextDrawShowForPlayer(i, TextTL);
			}
			if(EstarEnDuelo[i] == RETADOR_EN_ESPERA)
			{
				retadorid[ZonaDuelo[i]] = INVALID_PLAYER_ID;
				ZonaDuelo[i] = NO_DUELO;
				EstarEnDuelo[i] = NO_DUELO;
			}
			if((MayorRacha > 1) && (MostrarRachas[i] == true))
			{
			    TextDrawHideForPlayer(i, TextSpree[1]);
			    TextDrawHideForPlayer(i, TextSpree[2]);
			}
			RemoveASK(i);
			Data[i][KillingSpree] = 0;
			Data[i][IsKillingSpree] = false;
   			if((IsPlayerRace(i) == true) || (EstarEnDuelo[i] >= -1 && EstarEnDuelo[i] <= 9)) continue;
		    if((i == id_president) && (Data[id_president][Teams] >= -3 && Data[id_president][Teams] <= -5)) continue;
		    if(GetPlayerState(i) == PLAYER_STATE_SPECTATING)
		    {
				pc_cmd_noespiar(i);
		        continue;
		    }
			TogglePlayerControllable(i, 0);
			GetPlayerPos(i, X, Y, Z);
			SetPlayerPos(i, X+100, Y, Z);
			SetPlayerCameraPos(i, X, Y, Z);
			SetPlayerCameraLookAt(i, X, Y, Z-5);
		}
		MayorRacha = 1;
		defer LoadMap();
		switch(City)
		{
		    case VENTURAS:
		    {
				for(new i = 0; i <= 4; ++i)
					GangZoneHideForAll(i);
				DestroyVehicleForAll(MAX_VEHICLE_LV, VehicleID_LV[vehicleid])
				CreateVehicles(SANTOS);
			}
		    case SANTOS:
		    {
				for(new i = 5; i <= 8; ++i)
					GangZoneHideForAll(i);
                DestroyVehicleForAll(MAX_VEHICLE_LS, VehicleID_LS[vehicleid])
 				CreateVehicles(FIERRO);
			}
		    case FIERRO:
		    {
				for(new i = 9; i <= 12; ++i)
					GangZoneHideForAll(i);
                DestroyVehicleForAll(MAX_VEHICLE_SF, VehicleID_SF[vehicleid])
 				CreateVehicles(VENTURAS);
			}
		}
	}
	else
	{
		new str[105];
		new name[24];
		GetPlayerName(id_vicepresident, name, sizeof(name));
		stop Timer_President;
		Data[id_vicepresident][SkinClass] = 165;
		Data[id_vicepresident][Teams] = PRESIDENT;
		Data[id_vicepresident][VicePresident] = gettime() + (15 * 60); /* Tiempo de espera para volver a elegir la clase de vicepresidente (15 minutos) */
		Data[id_president][President] = gettime() + (15 * 60); /* Tiempo de espera para volver a elegir la clase de presidente (15 minutos) */
		format(str, sizeof(str), "(Central de Inteligencia) El VicePresidente %s ha tomado el cargo de Presidente.", name);
		SendClientMessageToAll(Amarillo, str);
	    id_vicepresident = INVALID_PLAYER_ID;
		id_president = INVALID_PLAYER_ID;
		foreach(new i : Player)
		{
		    if(IsPlayerStateClass(i)) continue;
			if(Data[i][Teams] == PRESIDENT || Data[i][Teams] == SECURITY)
			{
				SetPlayerWorldBounds(i, 20000.0000, -20000.0000, 20000.0000, -20000.0000);
				OnPlayerSpawn(i);
			}
		}
	}
	return 1;
}

Weapons(playerid, weapon1 = 0, ammo1 = 0, weapon2 = 0, ammo2 = 0, weapon3 = 0, ammo3 = 0, weapon4 = 0, ammo4 = 0)
{
	GivePlayerWeapon(playerid, weapon1, 0);
	GivePlayerWeapon(playerid, weapon2, 0);
	GivePlayerWeapon(playerid, weapon3, 0);
	GivePlayerWeapon(playerid, weapon4, 0);
	SetPlayerAmmo(playerid, weapon1, ammo1);
	SetPlayerAmmo(playerid, weapon2, ammo2);
	SetPlayerAmmo(playerid, weapon3, ammo3);
	SetPlayerAmmo(playerid, weapon4, ammo4);
}

RemoveASK(playerid) //Procedimiento que elimina la protección de spawn a un jugador, sí es que lo está realmente.
{
	if(Data[playerid][ASK] == true) //si esta protegido por la protección spawn
	{
		stop Data[playerid][SpawnKill];
		stop ID_EndSpawnProtection[playerid];
		Data[playerid][ASK] = false;
		Delete3DTextLabel(Data[playerid][LabelASK]);
	}
}

/*
Esta función va a detectar cada 1 segundo si el jugador ha salido de una área especifica.
Si la función devuelve 0, quiere decir que el jugador no salió de su zona de spawn.
Si la función devuelve 1, quiere decir que el jugador si salió de su zona de spawn y se detiene la función.
*/

IsPlayerExitAreaEx(playerid, Float:x_min, Float:x_max, Float:y_min, Float:y_max)
{
	static Float:x;
	static Float:y;
	static Float:z;
	GetPlayerPos(playerid, x, y, z);
	return (((x >= x_min && x <= x_max) && (y >= y_min && y <= y_max)) ? (0) : (1));
}

timer IsPlayerExitArea[1000](playerid)
{
	if(TimePositionSuccess[playerid] >= gettime()) return 0;
	static Float:x_min = 0.0;
	static Float:x_max = 0.0;
	static Float:y_min = 0.0;
	static Float:y_max = 0.0;
	new t = 0;
	t = Data[playerid][Teams];
	if(t >= 1 && t <= 3)
	    t = 0;
	if(City == SANTOS || City == FIERRO)
	{
		if(t == 4 || t == 5)
			t = 5;
	}
	switch(City)
	{
		case VENTURAS:
		{
			x_min = ((t == 0) ? (947.3376) : ((t == 4) ? (2232.032) : ((t == 5) ? (2263.271) : ((t == 6) ? (1228.487) : (2411.655)))));
			x_max = ((t == 0) ? (1166.009) : ((t == 4) ? (2368.702) : ((t == 5) ? (2376.511) : ((t == 6) ? (1415.919) : (2583.469)))));
			y_min = ((t == 0) ? (1001.35) :  ((t == 4) ? (2397.722) : ((t == 5) ? (2340.0857) : ((t == 6) ? (2645.278) : (1480.99)))));
			y_max = ((t == 0) ? (1171.545) : ((t == 4) ? (2471.215) : ((t == 5) ? (2467.347) :  ((t == 6) ? (2707.167) : (1612.505)))));
		}
		case SANTOS:
		{
			x_min = ((t == 0) ? (1105.779) : ((t == 5) ? (1535.499) : ((t == 6) ? (1607.753) : (2136.347))));
			x_max = ((t == 0) ? (1307.329) : ((t == 5) ? (1626.767) : ((t == 6) ? (1699.021) : (2174.376))));
			y_min = ((t == 0) ? (-2096.375) : ((t == 5) ? (-1741.078) : ((t == 6) ? (-1137.074) : (-1227.872))));
			y_max = ((t == 0) ? (-2001.629) : ((t == 5) ? (-1606.855) : ((t == 6) ? (-1022.589) : (-1125.23))));
		}
		case FIERRO:
		{
			x_min = ((t == 0) ? (-2699.835) : ((t == 5) ? (-1699.399) : ((t == 6) ? (-2309.704) : (-2313.566))));
			x_max = ((t == 0) ? (-2603.268) : ((t == 5) ? (-1560.343) : ((t == 6) ? (-2213.136) : (-2255.626))));
			y_min = ((t == 0) ? (-48.96741) : ((t == 5) ? (633.5114) : ((t == 6) ? (515.1625) : (69.38152))));
			y_max = ((t == 0) ? (49.6567) : ((t == 5) ? (751.8604) : ((t == 6) ? (578.2819) : (235.07))));
		}
	}
	if(IsPlayerExitAreaEx(playerid, x_min, x_max, y_min, y_max))
	{
		SendClientMessage(playerid, 0x97FF2FFF, "* Haz abandonado tu zona de spawn, tu protección de spawn se terminará en 5 segundos.");
    	stop Data[playerid][SpawnKill];
		ID_EndSpawnProtection[playerid] = defer EndSpawnProtection(playerid);
	}
	return 1;
}

timer EndSpawnProtection[5000](playerid)/* Esta no-función establecerá el final de protección de spawn al jugador que haya salido de su zona de spawn, pero lo hará en 5 segundos. */
{
	new teamid = Data[playerid][Teams];
	if(EstarEnDuelo[playerid] == OPONENTE_EN_ESPERA)
		SetPlayerHealth(playerid, 99999);
	else
	    SetPlayerHealth(playerid, 100);
	SetPlayerArmour(playerid, ((teamid == PRESIDENT) ? (100) : ((teamid == VICEPRESIDENT) ? (100) : ((teamid == SECURITY) ? (50) : (0)))));
	SendClientMessage(playerid, 0x97FF2FFF, "* Ya no estás bajo la protección de spawn.");
	Data[playerid][ASK] = false;
	Delete3DTextLabel(Data[playerid][LabelASK]);
}

timer FreezePlayer[5000](playerid)
{
	new Float:x;
	new Float:y;
	new Float:z;
	GetPlayerPos(playerid, x, y, z);
	SetPlayerPos(playerid, x, y, z+4.5);
	TogglePlayerControllable(playerid, true);
	SetPlayerVirtualWorld(playerid, 0);
	return 1;
}

/*
Esta función comprobará sí el jugador salió de la ciudad establecida por parte del servidor.
La función devuelve 0, sí el jugador todavía se encuentra dentro de la ciudad.
La función devolverá un 1, cuando el jugador haya salido de la ciudad.

timer IsPlayerExitCity[500](playerid)
{
	//print("DEBUG: IsPlayerExitCity\n");
	#define VALUE 20 //el valor para restarle o aumentar al vector x o y
	#define VALUE_Z 3 //el valor para aumentarle al vector z
	static Float:x;
	static Float:y;
	static Float:z;
	static invehicle;
	static vehicleid;
	invehicle = (IsPlayerInAnyVehicle(playerid) == 1);
	if(invehicle != 0) // si está en un vehículo.
	{
	    vehicleid = GetPlayerVehicleID(playerid);
		GetVehiclePos(vehicleid, x, y, z);
	}
	else // caso contrario.
	    GetPlayerPos(playerid, x, y, z);

	switch(City)
	{
		case VENTURAS:
		{
			if(!(x >= 642.2831 && x <= 2966.18)) // si el vector x del jugador no llega estar dentro del rango xmin e xmax, entonces la condición será verdadera.
			{
			    if(invehicle != 0)
			    	SetVehiclePos(vehicleid, ((x <= 642.2831) ? (x+VALUE) : (x-VALUE)), y, z+VALUE_Z);
				else
				    SetPlayerPos(playerid, ((x <= 642.2831) ? (x+VALUE) : (x-VALUE)), y, z+VALUE_Z);
			}
			else if(!(y >= 548.8602 && y <= 2954.502)) // si el vector y del jugador no llega estar dentro del rango ymin e ymax, entonces la condición será verdadera.
			{
			    if(invehicle != 0)
			    	SetVehiclePos(vehicleid, x, ((y <= 548.8602) ? (y+VALUE) : (y-VALUE)), z+VALUE_Z);
				else
				    SetPlayerPos(playerid, x, ((y <= 548.8602) ? (y+VALUE) : (y-VALUE)), z+VALUE_Z);
			}
			else return 0;
			GameTextForPlayer(playerid, "~w~DEBES PERMANECER DENTRO DE~n~~r~LA CIUDAD DE LAS VENTURAS", 2000, 5);
	    }
		case SANTOS:
		{
			if(!(x >= 93.423 && x <= 2977.858)) // si el vector x del jugador no llega estar dentro del rango xmin e xmax, entonces la condición será verdadera.
			{
			    if(invehicle != 0)
			    	SetVehiclePos(vehicleid, ((x <= 93.423) ? (x+VALUE) : (x-VALUE)), y, z+VALUE_Z);
				else
				    SetPlayerPos(playerid, ((x <= 93.423) ? (x+VALUE) : (x-VALUE)), y, z+VALUE_Z);
			}
			else if(!(y >= -2907.791 && y <= -688.9946)) // si el vector y del jugador no llega estar dentro del rango ymin e ymax, entonces la condición será verdadera.
			{
			    if(invehicle != 0)
			    	SetVehiclePos(vehicleid, x, ((y <= -2907.791) ? (y+VALUE) : (y-VALUE)), z+VALUE_Z);
				else
				    SetPlayerPos(playerid, x, ((y <= -2907.791) ? (y+VALUE) : (y-VALUE)), z+VALUE_Z);
			}
			else return 0;
			GameTextForPlayer(playerid, "~w~DEBES PERMANECER DENTRO DE~n~~r~LA CIUDAD DE LOS SANTOS", 2000, 5);
		}
  		case FIERRO:
  		{
 			if(!(x >= -2977.858 && x <= -934.23)) // si el vector x del jugador no llega estar dentro del rango xmin e xmax, entonces la condición será verdadera.
			{
			    if(invehicle != 0)
			    	SetVehiclePos(vehicleid, ((x <= -2977.858) ? (x+VALUE) : (x-VALUE)), y, z+VALUE_Z);
				else
				    SetPlayerPos(playerid, ((x <= -2977.858) ? (x+VALUE) : (x-VALUE)), y, z+VALUE_Z);
			}
			else if(!(y >= -1097.72 && y <= 1669.936)) // si el vector y del jugador no llega estar dentro del rango ymin e ymax, entonces la condición será verdadera.
			{
			    if(invehicle != 0)
			    	SetVehiclePos(vehicleid, x, ((y <= -1097.72) ? (y+VALUE) : (y-VALUE)), z+VALUE_Z);
				else
				    SetPlayerPos(playerid, x, ((y <= -1097.72) ? (y+VALUE) : (y-VALUE)), z+VALUE_Z);
			}
			else return 0;
			GameTextForPlayer(playerid, "~w~DEBES PERMANECER DENTRO DE~n~~r~LA CIUDAD DE SAN FIERRO", 2000, 5);
		}
	}
	return 1;
}
*/

timer Colors_Vehicle[100](playerid)
{
    ChangeVehicleColor(GetPlayerVehicleID(playerid), random(150), random(150));
	return 1;
}

task MessageAD[TIME_AD]()
{
	static Hour = 12;
	if(Hour == 23)
	    Hour = -1;
	SetWorldTime(++Hour);
	SendClientMessageToAll(0x33CCFFAA, MessageRandom[random(sizeof(MessageRandom))]);
}

timer SendMessage[100](playerid, string:reason[]) /* Este procedimiento envía un mensaje de expulsión o prohibición en 0.1 segundos (100 milisegundos). */
{
	if(Kicked == true)
		Kick(playerid);
	else
	{
		BanEx(playerid, reason);
		Kicked = true;
	}
}

SetPlayerMessage(playerid, reason[], color)
{
	new str[MAX_PLAYER_NAME + 30 + 20];
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(str, sizeof(str), "%s fue expulsado del servidor. [Razón: %s]", name, reason);
	SendClientMessageToAll(color, str);
	defer SendMessage(playerid, reason);
	return 1;
}

public Commands(name[], value[])
{
	INI_Bool("CMD_Holiday", HolidayOn);
	return 1;
}
#endif /* FIN _INC_fpap */
