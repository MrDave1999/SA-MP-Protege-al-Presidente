#if !defined _INC_duelos
	#define _INC_duelos

#include <YSI\y_hooks>

//Valores que tendrá guardada la variable "EstarEnDuelo"
#define ESTA_EN_DUELO      		(-1)
#define RETADOR_EN_ESPERA  		(-2)
#define OPONENTE_EN_ESPERA 		(-3)
#define NO_DUELO           		(-4)

#define NINGUNO  				(0)
#define RETADOR  				(1)
#define OPONENTE 				(2)

#define ABANDONO 				(1)
#define SUICIDO  				(2)

#define COLOR_NARANJA 		   	(0xFF8000FF)
#define Dialog(%0,%1) 			ShowPlayerDialog(%0, ENVIAR_DUELO, DIALOG_STYLE_INPUT, "Ingresa una id válida", %1, "Seleccionar", "Atrás"),-2
#define GetWeaponNameDuel(%0) 	weapons[ArmasDuelo[%0]]

//forward's
forward OnPlayerCommandReceivedEx(playerid, cmd[], params[], flags);
forward pc_cmd_rc(playerid);

enum
{
	ARMAS_LENTAS,
	ARMAS_PESADAS,
	ARMAS_RAPIDAS,
	DESERT_EAGLE,
	SNIPER,
	SHOTGUN,
	SAWNOFF,
	TEC_9,
	M4,
	COMBAT_SHOTGUN,
	NO_ARMAS = -1
};

//Variables
new Espiar[MAX_PLAYERS];//Esta variable es para saber a quien está espiando, sí al retador o al oponente
new ZonaDuelo[MAX_PLAYERS]; //Esta variable guardará la id de la arena que eliga el jugador
static ArmasDuelo[MAX_PLAYERS];//Esta variable guardará la id de la arma que eliga el jugador
new retadorid[10];//Esta variable guardará la id del retador con respecto al lugar que haya establecido él
static bool:Arenas[10];//Esta variable guardará dos valores: "true", sí está ocupado el lugar, "false", sí está desocupado
new Text:Texto;
static ID_AsesinoAnterior = INVALID_PLAYER_ID;
static bool:ActivarDuelo[MAX_PLAYERS];//Esta variable guardará dos valores: "true", sí el jugador activó para que le envíen los duelos, "false", sí el jugador lo desactivó
static Timer:IDConteoEnDuelo[MAX_PLAYERS];//Esta variable guardará la id del temporizador proporcionado por la función "ComenzarDuelo"
static Timer:IDTimer_GanadorDuelo;
static ConteoEnDuelo[MAX_PLAYERS];
new EstarEnDuelo[MAX_PLAYERS];
/*
Valores que toma la variable "EstarEnDuelo":
-> Recibe 0 hasta 9, sí el jugador está como espectador dentro de un duelo
-> Recibe -1, sí el jugador está como retador o oponente de un duelo
-> Recibe -2, sí el jugador está como retador en espera (aún no le aceptan el duelo)
-> Recibe -3, sí el jugador está como oponente pero aún no acepta o rechaza el duelo
-> Recibe -4, sí el jugador no está en ningún duelo
*/

static const weapons[10][16] =
{
	{"Armas Lentas"},
	{"Armas Pesadas"},
	{"Armas Rápidas"},
	{"Desert Eagle"},
	{"Sniper"},
	{"Shotgun"},
	{"Sawnoff"},
	{"Tec-9"},
	{"M4"},
	{"Combat Shotgun"}
};

main()
{
	print("======================================================");
	print("|            Sistema de Duelos                       |");
	print("|                  Creado por MrDave                 |");
	print("|            Contacto: www.facebook.com/mrdavesamp   |");
	print("======================================================");
	print("\n");
	return MainEx();
}

hook OnPlayerConnect(playerid)
{
	//print("DEBUG: hook de OnPlayerConnect (duelos.inc)\n");
	ZonaDuelo[playerid] = NO_DUELO;
	ArmasDuelo[playerid] = NO_ARMAS;
	EstarEnDuelo[playerid] = NO_DUELO;
	Espiar[playerid] = NINGUNO;
	ConteoEnDuelo[playerid] = 11;
	ActivarDuelo[playerid] = true;
	return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    //print("DEBUG: hook de OnPlayerDisconnect (duelos.inc)\n");
    if(EstarEnDuelo[playerid] == OPONENTE_EN_ESPERA)//cuando el jugador le enviaron el duelo, pero aún no lo aceptó o rechazó
    {
        if(retadorid[ZonaDuelo[playerid]] != INVALID_PLAYER_ID) //cuando el retador se desconecta o cancela el duelo 
        {
			ZonaDuelo[retadorid[ZonaDuelo[playerid]]] = NO_DUELO;
			ArmasDuelo[retadorid[ZonaDuelo[playerid]]] = NO_ARMAS;
			EstarEnDuelo[retadorid[ZonaDuelo[playerid]]] = NO_DUELO;
			retadorid[ZonaDuelo[playerid]] = INVALID_PLAYER_ID;
		}
    }
	if(EstarEnDuelo[playerid] == RETADOR_EN_ESPERA) //cuando al retador aún no le aceptan el duelo, pero se desconecta por algún motivo
		retadorid[ZonaDuelo[playerid]] = INVALID_PLAYER_ID;

    if(EstarEnDuelo[playerid] == ESTA_EN_DUELO)//cuando el jugador ya aceptó el duelo y se desconectó
    {
		SetPlayerMessageDuel(playerid, ABANDONO);
    }
	return 1;
}

hook OnPlayerDeath(playerid, killerid, reason)
{
    //print("DEBUG: hook de OnPlayerDeath (duelos.inc)\n");
    if((killerid != INVALID_PLAYER_ID) && ((EstarEnDuelo[killerid] == ESTA_EN_DUELO) || (ID_AsesinoAnterior == playerid)))
    {
		if(ID_AsesinoAnterior == INVALID_PLAYER_ID) 
		{
		    SetRespawnToAll(killerid);
        	SetPlayerVirtualWorld(killerid, 0);
        	Arenas[ZonaDuelo[killerid]] = false;
        	retadorid[ZonaDuelo[killerid]] = INVALID_PLAYER_ID;
       		EstarEnDuelo[killerid] = NO_DUELO;
			ZonaDuelo[killerid] = NO_DUELO;
			ArmasDuelo[killerid] = NO_ARMAS;
        	SetPlayerVirtualWorld(playerid, 0);
        	EstarEnDuelo[playerid] = NO_DUELO;
        	ZonaDuelo[playerid] = NO_DUELO;
			ArmasDuelo[playerid] = NO_ARMAS;
			ID_AsesinoAnterior = killerid;
		}
		IDTimer_GanadorDuelo = defer GanadorDuelo(playerid, killerid);
    }
	else if((killerid == INVALID_PLAYER_ID) && (EstarEnDuelo[playerid] == ESTA_EN_DUELO))
		SetPlayerMessageDuel(playerid, SUICIDO);
	return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    //print("DEBUG: hook de OnPlayerStateChange (duelos.inc)\n");
	if((newstate == PLAYER_STATE_WASTED) && (ID_AsesinoAnterior == playerid))
	    stop IDTimer_GanadorDuelo;
	return 1;
}

hook OnPlayerRequestClass(playerid, classid)
{
    //print("DEBUG: hook de OnPlayerRequestClass (duelos.inc)\n");
	if(EstarEnDuelo[playerid] == RETADOR_EN_ESPERA)
	{
	    SetPlayerWorldBounds(playerid, 20000.0000, -20000.0000, 20000.0000, -20000.0000);
		GameTextForPlayer(playerid, "~w~DEBES CANCELAR TU DUELO~n~   ~r~USA /CDUELO", 5000, 3);
	    SetSpawnInfo(playerid, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		SpawnPlayer(playerid);
	    return -2;
	}
	return 1;
}

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    //print("DEBUG: hook de OnPlayerKeyStateChange (duelos.inc)\n");
    if(EstarEnDuelo[playerid] >= 0)
    {
    	if((newkeys == KEY_FIRE) || (newkeys == KEY_HANDBRAKE))
    	{
    	    new i;
			if(Espiar[playerid] == OPONENTE)
				Espiar[playerid] = NINGUNO;
  			Espiar[playerid]++;
			if(Espiar[playerid] == OPONENTE)
			{
				foreach(i : Player)
				{
					if((i != retadorid[ZonaDuelo[playerid]]) && (EstarEnDuelo[i] == ESTA_EN_DUELO))
					{
						if(ZonaDuelo[i] == ZonaDuelo[retadorid[ZonaDuelo[playerid]]])
						{
							break;
						}
					}
				}
			}
			if(Espiar[playerid] == RETADOR)
			{
			    TogglePlayerSpectating(playerid, true);
			    PlayerSpectatePlayer(playerid, retadorid[ZonaDuelo[playerid]]);
			}
			else
			{
			    TogglePlayerSpectating(playerid, true);
			    PlayerSpectatePlayer(playerid, i);
			}
		}
    }
	return 1;
}

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    //print("DEBUG: hook de OnDialogResponse (duelos.inc)\n");
    DialogSound playerid;
	switch(dialogid)
	{
	    case ELEGIR_ARENAS: //cuando el jugador selecciona una arena
	    {
	        if(response == 0)
	        {
				ZonaDuelo[playerid] = NO_DUELO;
				ArmasDuelo[playerid] = NO_ARMAS;
				return -2;
			}
			switch(listitem)
			{
			    case 0..9:
			    {
			        if(Arenas[listitem] == true) // si la arena llega estar ocupada
			        {
			            SendClientMessage(playerid, Rojo, "ERROR: Esa arena se encuentra ocupada actualmente.");
			            pc_cmd_duelo(playerid);
			            return -2;
			        }
					ZonaDuelo[playerid] = listitem;
					ShowPlayerDialog(playerid, ELEGIR_ARMAS, DIALOG_STYLE_LIST,
					"Menú de Armas", "Armas Lentas - WW2\nArmas Pesadas - WW\nArmas Rápidas - RW\nDesert Eagle\nFranco Tirador\nShotgun\nSawnoff\nTec-9\nM4\nCombat Shotgun", "Seleccionar", "Atrás");
				}
			}
	    }
	    case ELEGIR_ARMAS: //cuando el jugador selecciona una arma
	    {
			if(response == 0)
			{
			    pc_cmd_duelo(playerid);
			    return -2;
			}
	        switch(listitem)
	        {
	            case 0..9:
	            {
	                ArmasDuelo[playerid] = listitem;
					ShowPlayerDialog(playerid, ENVIAR_DUELO, DIALOG_STYLE_INPUT, "Invitación a un duelo", "Ingresa la ID de ese jugador:", "Seleccionar", "Atrás");
				}
	        }
	    }
		case ENVIAR_DUELO: //cuando un jugador envía un duelo a otro jugador
		{
			if(response == 0)
			{
				ShowPlayerDialog(playerid, ELEGIR_ARMAS, DIALOG_STYLE_LIST,
				"Menú de Armas", "Armas Lentas - WW2\nArmas Pesadas - WW\nArmas Rápidas - RW\nDesert Eagle\nFranco Tirador\nShotgun\nSawnoff\nTec-9\nM4\nCombat Shotgun", "Seleccionar", "Atrás");
			    return -2;
			}
			else
			{
				new userid = strval(inputtext);
				new len = strlen(inputtext);
				if(IsPlayerConnected(userid) != 1) //si el jugador no está conectado (id inválida)
			    	return Dialog(playerid, "ERROR: Ese jugador no se encuentra conectado");
				if(len == 0) //si la longitud del texto es nula (vacía)
                    return Dialog(playerid, "ERROR: El texto está vacío, debes escribir algo");
				if(len > 3)//si la longitud del texto es mayor a 3
				    return Dialog(playerid, "ERROR: El texto está demasiado grande (máximo: 3 caracteres)");
				for(new i = 0; i < len; ++i)
				{
					if(!(inputtext[i] >= '0' && inputtext[i] <= '9'))
						return Dialog(playerid, "ERROR: El texto debe ser únicamente numérico");
				}
				if(IsPlayerInClassSelection(userid) != 0)//si el jugador llega a estar en la selección de clases
					return Dialog(playerid, "ERROR: Ese jugador se encuentra en la selección de clases");
				if(Data[userid][Teams] != CIVIL)//si el jugador no llega a ser de la clase civil
			    	return Dialog(playerid, "ERROR: Ese jugador no se encuentra en la clase de civil");
				if(ActivarDuelo[userid] == false)//si el jugador tiene desactivado los duelos
			    	return Dialog(playerid, "ERROR: Ese jugador tiene desactivado los duelos\nDile que coloque /siduelo");
				if(playerid == userid)//si la id del jugador quién presionó el botón coincide con la id que haya puesto el jugador en el diálogo
				    return Dialog(playerid, "ERROR: Usted mismo no se puede enviar un duelo");
        		if(IsPlayerRace(userid) == true)//si el jugador llega a estar en una carrera
        			return Dialog(playerid, "ERROR: Ese jugador se encuentra en una carrera actualmente.");
				if(CreateRacePlayer == userid)
				    return Dialog(playerid, "ERROR: Ese jugador se encuentra seleccionando una carrera");
				if(EstarEnDuelo[userid] == ESTA_EN_DUELO)//si el retador o el oponente se encuentra en algún duelo
			    	return Dialog(playerid, "ERROR: Ese jugador se encuentra actualmente en un duelo");
				if(EstarEnDuelo[userid] == OPONENTE_EN_ESPERA)
					return Dialog(playerid, "ERROR: A ese jugador ya le enviaron una invitación a un duelo");
				if(EstarEnDuelo[userid] == RETADOR_EN_ESPERA)
					return Dialog(playerid, "ERROR: Ese jugador ya envío una invitación a otro jugador para establecer un duelo");
				if(retadorid[ZonaDuelo[playerid]] != INVALID_PLAYER_ID)//si ya hubo algún retador que escogió esa arena
				{
				    SendClientMessage(playerid, Rojo, "ERROR: Esta arena ya fue elegida por otro jugador, selecciona otra.");
				    pc_cmd_duelo(playerid);
					return 1;
				}
				if(EstarEnDuelo[userid] >= 0 && EstarEnDuelo[userid] <= 9)//si el jugador está como espectador en algún duelo
				    return Dialog(playerid, "ERROR: Ese jugador se encuentra como espectador en algún duelo");
				if(IsPlayerPaused(userid) != 0) //si el jugador llega a estar en pausa (AFK)
				    return Dialog(playerid, "ERROR: Ese jugador se encuentra en pausa (AFK)");
				if(GetPlayerState(userid) == PLAYER_STATE_WASTED)//si el jugador se murió
				    return Dialog(playerid, "ERROR: Ese jugador acabó de morir");
				if(GetPlayerState(userid) == PLAYER_STATE_SPECTATING)//si el jugador está de modo espectador
    				return Dialog(playerid, "ERROR: Ese jugador se encuentra en modo espectador");
 				new str[103 + 24 + 15];
 				new name[24];
 				GetPlayerName(playerid, name, sizeof(name));
				retadorid[ZonaDuelo[playerid]] = playerid;
				EstarEnDuelo[playerid] = RETADOR_EN_ESPERA;
				ZonaDuelo[userid] = ZonaDuelo[playerid];
				ArmasDuelo[userid] = ArmasDuelo[playerid];
				EstarEnDuelo[userid] = OPONENTE_EN_ESPERA;
				if(Data[userid][ASK] == false)
					SetPlayerHealth(userid, 99999);
				format(str, sizeof(str), "{FFFFFF}Tienes una invitación para un duelo.\n%s {FFFF00}(ID: %d) {FFFFFF}te retó a un duelo de {66FF00}%s", name, playerid, GetWeaponNameDuel(playerid));
				ShowPlayerDialog(userid, DUELO_RECIBIDO, DIALOG_STYLE_MSGBOX, "Desafío", str, "Aceptar", "Rechazar");
				GameTextForPlayer(playerid, "DUELO ENVIADO", 2000, 4);
			}
		}
		case DUELO_RECIBIDO: //cuando el usuario recibe el duelo por parte de otro jugador
		{
		    new dueloid = ZonaDuelo[playerid];
    		if(retadorid[dueloid] == INVALID_PLAYER_ID)
			{
				SendClientMessage(playerid, Rojo, "ERROR: El retador canceló el duelo.");
				ZonaDuelo[playerid] = NO_DUELO;
				ArmasDuelo[playerid] = NO_ARMAS;
				EstarEnDuelo[playerid] = NO_DUELO;
				if(Data[playerid][ASK] == false)
                	SetPlayerHealth(playerid, 100);
				return -2;
			}
			if(GetPlayerState(retadorid[dueloid]) == PLAYER_STATE_WASTED)
			{
   				EstarEnDuelo[playerid] = NO_DUELO;
		        ZonaDuelo[playerid] = NO_DUELO;
		        EstarEnDuelo[retadorid[dueloid]] = NO_DUELO;
		        ZonaDuelo[retadorid[dueloid]] = NO_DUELO;
		        ArmasDuelo[retadorid[dueloid]] = NO_ARMAS;
		        ArmasDuelo[playerid] = NO_ARMAS;
		        retadorid[dueloid] = INVALID_PLAYER_ID;
		        SendClientMessage(playerid, Rojo, "ERROR: Tu retador se acabó de morir.");
   				if(Data[playerid][ASK] == false)
                	SetPlayerHealth(playerid, 100);
			    return -2;
			}
		    if(response == 1) // si aceptó el duelo
		    {
		        Arenas[dueloid] = true;
				RemoveASK(retadorid[dueloid]);
				RemoveASK(playerid);
		        SetPlayerWorldBounds(retadorid[dueloid], 20000.0000, -20000.0000, 20000.0000, -20000.0000);
		        EstarEnDuelo[retadorid[dueloid]] = ESTA_EN_DUELO;
				ResetPlayerWeapons(retadorid[dueloid]);
				SetCameraBehindPlayer(retadorid[dueloid]);
				SetPlayerPos(retadorid[dueloid], 1360.5299, -24.5914, 1000.9219);
				SetPlayerFacingAngle(retadorid[dueloid], 267.8792);
				SetPlayerInterior(retadorid[dueloid], 1);
				SetPlayerVirtualWorld(retadorid[dueloid], ((dueloid * 10) + 10));
				SetPlayerHealth(retadorid[dueloid], 100);
				SetPlayerArmour(retadorid[dueloid], 100);
				TogglePlayerControllable(retadorid[dueloid], 0);
                IDConteoEnDuelo[retadorid[dueloid]] = repeat ComenzarDuelo(retadorid[dueloid]);

		        SetPlayerWorldBounds(playerid, 20000.0000, -20000.0000, 20000.0000, -20000.0000);
				EstarEnDuelo[playerid] = ESTA_EN_DUELO;
				ResetPlayerWeapons(playerid);
				SetCameraBehindPlayer(playerid);
				SetPlayerPos(playerid, 1417.8213, -25.6164, 1000.9269);
				SetPlayerFacingAngle(playerid, 83.9741);
				SetPlayerInterior(playerid, 1);
				SetPlayerVirtualWorld(playerid, ((dueloid * 10) + 10));
				SetPlayerHealth(playerid, 100);
				SetPlayerArmour(playerid, 100);
				TogglePlayerControllable(playerid, 0);
				IDConteoEnDuelo[playerid] = repeat ComenzarDuelo(playerid);
				
				switch(ArmasDuelo[playerid])
				{
					case ARMAS_LENTAS: 
					{
					    GivePlayerWeapon(playerid, WEAPON_DEAGLE, 9000);
					    GivePlayerWeapon(playerid, WEAPON_SHOTGUN, 9000);
					    GivePlayerWeapon(playerid, WEAPON_SNIPER, 9000);
					    GivePlayerWeapon(retadorid[dueloid], WEAPON_DEAGLE, 9000);
					    GivePlayerWeapon(retadorid[dueloid], WEAPON_SHOTGUN, 9000);
					    GivePlayerWeapon(retadorid[dueloid], WEAPON_SNIPER, 9000);
					}
					case ARMAS_PESADAS: 
					{
						GivePlayerWeapon(playerid, WEAPON_SILENCED, 9000);
						GivePlayerWeapon(playerid, WEAPON_M4, 9000);
						GivePlayerWeapon(playerid, WEAPON_MP5, 9000);
						GivePlayerWeapon(playerid, WEAPON_SHOTGSPA, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_SILENCED, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_M4, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_MP5, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_SHOTGSPA, 9000);
					}
					case ARMAS_RAPIDAS: 
					{
						GivePlayerWeapon(playerid, WEAPON_COLT45, 9000);
						GivePlayerWeapon(playerid, WEAPON_TEC9, 9000);
						GivePlayerWeapon(playerid, WEAPON_SAWEDOFF, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_COLT45, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_TEC9, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_SAWEDOFF, 9000);
					}
					case DESERT_EAGLE:
					{
						GivePlayerWeapon(playerid, WEAPON_DEAGLE, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_DEAGLE, 9000);
					}
					case SNIPER:
					{
						GivePlayerWeapon(playerid, WEAPON_SNIPER, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_SNIPER, 9000);
					}
					case SHOTGUN: 
					{
						GivePlayerWeapon(playerid, WEAPON_SHOTGUN, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_SHOTGUN, 9000);
					}
					case SAWNOFF:
					{
						GivePlayerWeapon(playerid, WEAPON_SAWEDOFF, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_SAWEDOFF, 9000);
					}
					case TEC_9: 
					{
						GivePlayerWeapon(playerid, WEAPON_TEC9, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_TEC9, 9000);
					}
					case M4: 
					{
						GivePlayerWeapon(playerid, WEAPON_M4, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_M4, 9000);
					}
					case COMBAT_SHOTGUN: 
					{
						GivePlayerWeapon(playerid, WEAPON_SHOTGSPA, 9000);
						GivePlayerWeapon(retadorid[dueloid], WEAPON_SHOTGSPA, 9000);
					}
				}
				new str2[62 + (2 * 24)];
				new name1[24];
				new name2[24];
				GetPlayerName(playerid, name1, sizeof(name1));
				GetPlayerName(retadorid[dueloid], name2, sizeof(name2));
				format(str2, sizeof(str2), "(DUELO) Un duelo entre %s y %s comenzará en 10 segundos. (Arena: %d)", name1, name2, (dueloid + 1));
				SendClientMessageToAll(COLOR_NARANJA, str2);
		    }
		    else if(response == 0) //si rechazó el duelo
		    {
		        new str3[40 + 24 + 15];
		        new name[24];
		        GetPlayerName(playerid, name, sizeof(name));
		        EstarEnDuelo[playerid] = NO_DUELO;
		        ZonaDuelo[playerid] = NO_DUELO;
		        EstarEnDuelo[retadorid[dueloid]] = NO_DUELO;
		        ZonaDuelo[retadorid[dueloid]] = NO_DUELO;
		        ArmasDuelo[retadorid[dueloid]] = NO_ARMAS;
		        format(str3, sizeof(str3), "(DUELO) %s (ID: %d) te rechazó el duelo de %s", name, playerid, GetWeaponNameDuel(playerid));
		        SendClientMessage(retadorid[dueloid], COLOR_NARANJA, str3);
		        ArmasDuelo[playerid] = NO_ARMAS;
		        retadorid[dueloid] = INVALID_PLAYER_ID;
		        GameTextForPlayer(playerid, "DUELO RECHAZADO", 3000, 3);
  				if(Data[playerid][ASK] == false)
                	SetPlayerHealth(playerid, 100);
		    }
		}
		case ZONAS_VER_DUELO://cuando el usuario abre el diálogo para seleccionar alguna arena y así poder ver el duelo entre dos usuarios
		{
		    if(response == 0) return 1;
		    switch(listitem)
		    {
		        case 0..9:
		        {
		            if(Arenas[listitem] != true)
					{
					    SendClientMessage(playerid, Rojo, "ERROR: No hay ningún duelo en esa arena.");
						pc_cmd_verduelo(playerid);
					    return -2;
					}
					new i;
					foreach(i : Player)
					{
					    if((i != retadorid[listitem]) && (EstarEnDuelo[i] == ESTA_EN_DUELO))
					    {
							if(ZonaDuelo[i] == ZonaDuelo[retadorid[listitem]])
							{
						    	break;
							}
						}
					}
					RemoveASK(playerid);
					SetPlayerWorldBounds(playerid, 20000.0000, -20000.0000, 20000.0000, -20000.0000);
					TextDrawShowForPlayer(playerid, Texto);
					if(Data[playerid][ForcePlayer] == JUGADOR_SI_FORZADO)
						Data[playerid][ForcePlayer] = JUGADOR_MUERTO_FORZADO;
					Espiar[playerid] = OPONENTE;
		        	EstarEnDuelo[playerid] = listitem;
					ZonaDuelo[playerid] = listitem;
					SetPlayerInterior(playerid, 1);
					SetPlayerVirtualWorld(playerid, ((listitem * 10) + 10));
					TogglePlayerSpectating(playerid, true);
					PlayerSpectatePlayer(playerid, i);
					SetPlayerHealth(playerid, 99999);
					new string[51 + (3 * 24)];
					new name1[24];
					new name2[24];
					new name3[24];
					GetPlayerName(playerid, name1, sizeof(name1));
					GetPlayerName(retadorid[listitem], name2, sizeof(name2));
					GetPlayerName(i, name3, sizeof(name3));
					format(string, sizeof(string), "(DUELO) %s entró a ver el duelo entre %s y %s (Arena: %d)", name1, name2, name3, (listitem + 1));
					SendClientMessageToAll(COLOR_NARANJA, string);
				}
		    }
		}
	}
	return 1;
}

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
	//print("DEBUG: hook de OnPlayerCommandReceived (duelos.inc)\n");
	if(EstarEnDuelo[playerid] == OPONENTE_EN_ESPERA)
	{
 		SendClientMessage(playerid, Rojo, "ERROR: Debes aceptar o rechazar la invitación para volver a usar algún comando.");
	    PlayerPlaySound(playerid, 1140, 0, 0, 0);
		return 0;
	}

	if(EstarEnDuelo[playerid] >= -1 && EstarEnDuelo[playerid] <= 9)
	{
	    if(strcmp(cmd, "sduelo", true) != 0)
	    {
			SendClientMessage(playerid, Rojo, "ERROR: No puedes usar comandos estando en un duelo, usa /sduelo para salir del lugar.");
			PlayerPlaySound(playerid, 1140, 0, 0, 0);
			return 0;
	    }
	}
	if(@flags_cmd:flags == CMD_DUEL)
	{
		if(Data[playerid][Teams] != CIVIL)
		{
	    	SendClientMessage(playerid, Rojo, "ERROR: Este comando sólo lo pueden usar los jugadores que sean de la clase civil.");
			return 0;
	    }
		if((strcmp(cmd, "siduelo", true) == 0) || (strcmp(cmd, "noduelo", true) == 0))
		{
			if(Data[playerid][Level] == 0)
			{
				SendClientMessage(playerid, Rojo, "ERROR: Usted debe estar registrado para poder usar este comando.");
 				return 0;
			}
		}
	}
	return OnPlayerCommandReceivedEx(playerid, cmd, params, flags);
}

hook OnGameModeInit()
{
    //print("DEBUG: hook de OnGameModeInit (duelos.inc)\n");
	for(new i = 0; i < sizeof(retadorid); ++i)
	    retadorid[i] = INVALID_PLAYER_ID;
	Texto = TextDrawCreate(14.000000, 390.000000, "Presiona click Izquierdo/Derecho para cambiar de jugador");
	TextDrawBackgroundColor(Texto, 255);
	TextDrawFont(Texto, 1);
	TextDrawLetterSize(Texto, 0.319999, 1.799999);
	TextDrawColor(Texto, -1);
	TextDrawSetOutline(Texto, 1);
	TextDrawSetProportional(Texto, 1);
	return 1;
}

public pc_cmd_rc(playerid)
{
	//print("DEBUG: hook de pc_cmd_rc (duelos.inc)\n");
	if(EstarEnDuelo[playerid] == RETADOR_EN_ESPERA)
	{
	    SendClientMessage(playerid, Rojo, "ERROR: Usted para cambiarse de equipo, debe primero cancelar la invitación (/cduelo)");
	    return 1;
	}
	return pc_cmd_1234(playerid);
}

flags:duelo(CMD_DUEL);
flags:verduelo(CMD_DUEL);
flags:cduelo(CMD_DUEL);
flags:siduelo(CMD_DUEL);
flags:noduelo(CMD_DUEL);
flags:sduelo(CMD_DUEL);

cmd:duelo(playerid)
{
	if(EstarEnDuelo[playerid] == RETADOR_EN_ESPERA)//si el retador envió algún duelo
	{
		SendClientMessage(playerid, Rojo, "ERROR: Usted ya envió una invitación, usa /cduelo para cancelar el duelo.");
		return 1;
	}
	new string[330];
	new dest[30];
	new worldid = 0;
	strcat(string, "Arena\tMundo Virtual\tDisponibilidad\n");
	for(new i = 0; i < 10; ++i)
	{
		worldid += 10;
	    format(dest, sizeof(dest), "Desmadre #%d\t%d\t%s\n", (i+1), worldid, ((Arenas[i] == true) ? ("Ocupado") : ("Desocupado")));
	    strcat(string, dest);
	}
	ShowPlayerDialog(playerid, ELEGIR_ARENAS, DIALOG_STYLE_TABLIST_HEADERS, "Selecciona una Arena", string, "Seleccionar", "Cerrar");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:verduelo(playerid)
{
	if(EstarEnDuelo[playerid] == RETADOR_EN_ESPERA)
	{
		SendClientMessage(playerid, Rojo, "ERROR: Usted no puede ver ningún duelo, primero debes cancelar tu invitación (/cduelo)");
		return 1;
	}
	new string[300];
	new spec[10];
	new dest[30];
	new worldid = 0;
	new i;
	foreach(i : Player)
	{
	    switch(EstarEnDuelo[i])
	    {
	        case 0..9:
	            ++spec[EstarEnDuelo[i]];
			default: continue;
	    }
	}
	strcat(string, "Arena\tMundo Virtual\tDisponible\tEspectadores\n");
	for(i = 0; i < 10; ++i)
	{
	    worldid += 10;
	    format(dest, sizeof(dest), "Desmadre #%d\t%d\t%s\t%d\n", (i+1), worldid, ((Arenas[i] == true) ? ("Si") : ("No")), spec[i]);
	    strcat(string, dest);
	}
	ShowPlayerDialog(playerid, ZONAS_VER_DUELO, DIALOG_STYLE_TABLIST_HEADERS, "Selecciona una Arena", string, "Entrar", "Cerrar");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:cduelo(playerid)
{
	if(!(EstarEnDuelo[playerid] == RETADOR_EN_ESPERA))
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no ha enviado ningún duelo a otro jugador.");
	retadorid[ZonaDuelo[playerid]] = INVALID_PLAYER_ID;
	EstarEnDuelo[playerid] = NO_DUELO;
	ArmasDuelo[playerid] = NO_ARMAS;
	ZonaDuelo[playerid] = NO_DUELO;
	GameTextForPlayer(playerid, "DUELO CANCELADO", 5000, 4);
	PlayerPlaySound(playerid, 1009, 0, 0, 0);
	return 1;
}

cmd:siduelo(playerid)
{
	if(ActivarDuelo[playerid] == true)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted ya tiene activado para que te envíen un duelo, usa /noduelo");
	ActivarDuelo[playerid] = true;
	GameTextForPlayer(playerid, "DUELOS ACTIVADOS", 5000, 4);
	SendClientMessage(playerid, Amarillo, "INFO: Ahora los jugadores sí podrán enviarte una invitación para un duelo.");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:noduelo(playerid)
{
	if(ActivarDuelo[playerid] == false)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted ya tiene desactivado para que no te envíen un duelo, usa /siduelo");
	ActivarDuelo[playerid] = false;
	GameTextForPlayer(playerid, "DUELOS DESACTIVADOS", 5000, 4);
	SendClientMessage(playerid, Amarillo, "INFO: Ahora los jugadores no podrán enviarte una invitación para un duelo.");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:sduelo(playerid)
{
	if(!(EstarEnDuelo[playerid] >= -1 && EstarEnDuelo[playerid] <= 9))
		return SendClientMessage(playerid, Rojo, "ERROR: Usted no está en un duelo.");
	if(EstarEnDuelo[playerid] >= 0)
	{
		EstarEnDuelo[playerid] = NO_DUELO;
		ZonaDuelo[playerid] = NO_DUELO;
		TogglePlayerSpectating(playerid, false);
	}
	else
	{
		OnPlayerSpawn(playerid);
		SetPlayerMessageDuel(playerid, ABANDONO);
	}
	return 1;
}

timer ComenzarDuelo[1000](playerid)
{
	if(--ConteoEnDuelo[playerid] == 0)
	{
		GameTextForPlayer(playerid,"Asesinalo YA!",1000,3);
		PlayerPlaySound(playerid, 1057, 0, 0, 0);
		ConteoEnDuelo[playerid] = 11;
		TogglePlayerControllable(playerid, 1);
		stop IDConteoEnDuelo[playerid];
	}
	else
	{
		new str[3];
		format(str, sizeof(str), "%d", ConteoEnDuelo[playerid]);
		GameTextForPlayer(playerid, str, 1000, 3);
		PlayerPlaySound(playerid, 1056, 0, 0, 0);
	}
	return 1;
}

timer GanadorDuelo[1000](playerid, killerid)
{
	new name1[24];
	new name2[24];
	GetPlayerName(playerid, name1, sizeof(name1));
	GetPlayerName(killerid, name2, sizeof(name2));
	if(ID_AsesinoAnterior == playerid)//Si la id del asesino anterior llega a ser igual al id del jugador que murió en ese instante
	{
		new string2[65 + (2 * 24)];
		ID_AsesinoAnterior = INVALID_PLAYER_ID;
		format(string2, sizeof(string2), "(DUELO) El duelo entre %s y %s fue un empate (ninguno de los dos ganó)", name2, name1);
		SendClientMessageToAll(COLOR_NARANJA, string2);
	}
	else
	{
		new string[83 + (2 * 24)];
		new Float:health;
		new Float:armour;
		GetPlayerHealth(killerid, health);
		GetPlayerArmour(killerid, armour);
		++Data[killerid][DuelosGanados];
		WriteData(killerid, DUELOS_GANADOS);
		OnPlayerSpawn(killerid);
		++Data[playerid][DuelosPerdidos];
		WriteData(playerid, DUELOS_PERDIDOS);
		ID_AsesinoAnterior = INVALID_PLAYER_ID;
		format(string, sizeof(string), "(DUELO) %s ha ganado el duelo contra %s con %.2f de salud y %.2f de armadura restante", name2, name1, health, armour);
		SendClientMessageToAll(COLOR_NARANJA, string);
	}
	return 1;
}

static SetPlayerMessageDuel(playerid, typemessage)
{
	new i;
	new name1[24];
	new name2[24];
	foreach(i : Player)
	{
		if(playerid == i) continue;
		if((EstarEnDuelo[i] == ESTA_EN_DUELO) && (ZonaDuelo[playerid] == ZonaDuelo[i]))
		{
			SetRespawnToAll(i);
			SetPlayerVirtualWorld(i, 0);
			Arenas[ZonaDuelo[i]] = false;
			retadorid[ZonaDuelo[i]] = INVALID_PLAYER_ID;
			++Data[i][DuelosGanados];
			WriteData(i, DUELOS_GANADOS);
			EstarEnDuelo[i] = NO_DUELO;
			ZonaDuelo[i] = NO_DUELO;
			ArmasDuelo[i] = NO_ARMAS;
			OnPlayerSpawn(i);
			++Data[playerid][DuelosPerdidos];
			WriteData(playerid, DUELOS_PERDIDOS);
			SetPlayerVirtualWorld(playerid, 0);
			EstarEnDuelo[playerid] = NO_DUELO;
			ZonaDuelo[playerid] = NO_DUELO;
			ArmasDuelo[playerid] = NO_ARMAS;
			break;
		}
	}
	GetPlayerName(playerid, name1, sizeof(name1));
	GetPlayerName(i, name2, sizeof(name2));
	if(typemessage == SUICIDO)
	{
	    new string[63 + (2 * 24)];
		format(string, sizeof(string), "(DUELO) %s se suicidó, por lo tanto %s ha ganado el duelo por default", name1, name2);
		SendClientMessageToAll(0xFF8000FF, string);
	}
	else if(typemessage == ABANDONO)
	{
	    new string[54 + (2 * 24)];
		format(string, sizeof(string), "(DUELO) %s abandonó el duelo contra %s (ganador por default)", name1, name2);
		SendClientMessageToAll(0xFF8000FF, string);
	}
	return 1;
}
	
static SetRespawnToAll(playerid)
{
	foreach(new i : Player)
	{
		if((EstarEnDuelo[i] >= 0) && (ZonaDuelo[playerid] == ZonaDuelo[i]))
		{
		    EstarEnDuelo[i] = NO_DUELO;
		    ZonaDuelo[i] = NO_DUELO;
		    TogglePlayerSpectating(i, false);
		}
	}
}

//==========================================================
#if !defined _ALS_main
	#define _ALS_main
#else
	#undef main
#endif
#define main MainEx
//==========================================================
#if !defined _ALS_OnPlayerCommandReceived
	#define _ALS_OnPlayerCommandReceived
#else
	#undef OnPlayerCommandReceived
#endif
#define OnPlayerCommandReceived OnPlayerCommandReceivedEx
//==========================================================
#define pc_cmd_rc pc_cmd_1234
//==========================================================
#endif /* FIN _INC_duelos*/
