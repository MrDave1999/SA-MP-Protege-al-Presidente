#if !defined _INC_OnPlayerPause
	#define _INC_OnPlayerPause
	
#include <YSI\y_hooks>

#define TIME_PAUSE_PLAYER 		(60*5*1000)// 5 minutos
#define TIME_PAUSE_PRESIDENT 	(60*2*1000) //2 minutos
#define IsPlayerPaused(%0) 		((PlayerPaused[(%0)] == true) ? (1) : (0))
#define NEW_TIME_PAUSE(%0)      ((Data[(%0)][Teams] == PRESIDENT) ? (TIME_PAUSE_PRESIDENT) : (TIME_PAUSE_PLAYER))

forward OnPlayerPause(playerid);
forward OnPlayerUnpause(playerid);

new TimeOfLastUpdate[MAX_PLAYERS];
new bool:PlayerPaused[MAX_PLAYERS];

hook OnPlayerConnect(playerid)
{
	//printf("DEBUG: hook de OnPlayerConnect (OnPlayerPause.inc)");
    PlayerPaused[playerid] = false;
	return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
    //printf("DEBUG: hook de OnPlayerConnect (OnPlayerPause.inc)");
	if(IsPlayerPaused(playerid) != 0)
	    stop Data[playerid][ID_TimePause];
 	PlayerPaused[playerid] = false;
	return 1;
}

public OnPlayerUpdate(playerid)
{
    TimeOfLastUpdate[playerid] = gettime();
    if(IsPlayerPaused(playerid) == 1)
        OnPlayerUnpause(playerid);
    return 1;
}

public OnPlayerPause(playerid)
{
    //print("DEBUG: OnPlayerPause");
	PlayerPaused[playerid] = true;
	Data[playerid][ID_TimePause] = defer TimePause_Player[NEW_TIME_PAUSE(playerid)](playerid);
    return 1;
}

public OnPlayerUnpause(playerid)
{
	//printf("DEBUG: OnPlayerUnpause");
	if(IsPlayerPaused(playerid) == 1)
	{
		stop Data[playerid][ID_TimePause];
		PlayerPaused[playerid] = false;
	}
    return 1;
}

task BuscarPausados[500]()
{
    static PlayerState;
    foreach(new i : Player)
    {
        if((IsPlayerPaused(i) != 1) && (IsPlayerInClassSelection(i) != 1) && ((gettime()-TimeOfLastUpdate[i]) >= 2))
        {
      		PlayerState = GetPlayerState(i);
       		if((PlayerState == PLAYER_STATE_ONFOOT) || (PlayerState == PLAYER_STATE_DRIVER) || (PlayerState == PLAYER_STATE_PASSENGER))
      			OnPlayerPause(i);
        }
    }
}

timer TimePause_Player[TIME_PAUSE_PLAYER](playerid)/* Este procedimiento tipo temporizador se ejecutará en 5 minutos si un jugador no llega salir de la pausa antes de ese tiempo */
{ //El tiempo pre-determinado va ser 5 minutos (5*60*1000)
	new message_kick[MAX_PLAYER_NAME + 92];
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	if(Data[playerid][Teams] != PRESIDENT)
		format(message_kick, sizeof(message_kick), "%s (ID: %d) fue etiquetado fuera del servidor por estar inactivo durante 5 minutos.", name, playerid);
	else
		format(message_kick, sizeof(message_kick), "%s (ID: %d) fue etiquetado fuera del servidor por estar inactivo durante 2 minutos como líder.", name, playerid);
	SendClientMessageToAll(0xFF8000FF, message_kick);
	defer SendMessage(playerid, "_");
}

#endif /* FIN _INC_OnPlayerPause */
