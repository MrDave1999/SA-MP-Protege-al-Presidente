#if !defined _INC_anti
	#define _INC_anti
	
#include <YSI\y_hooks>
#undef COMMAND
#define IsPlayerBOT(%0) (PlayerBOT[(%0)] == true)

forward OnPlayerCommandReceivedEx_Hook(playerid, cmd[], params[], flags);
forward bool:AntiSpam(playerid, string[]);
forward pc_cmd_antibot(playerid);

const MESSAGE = 0;
const COMMAND = 1;
const MAX_IP = 2;
const TIME_USED = 5;

static bool:PlayerBOT[MAX_PLAYERS] = {false, ...};
new bool:BOT = true;
static CountFlood[MAX_PLAYERS][2];
static TimeFlood[MAX_PLAYERS][2];

hook OnPlayerConnect(playerid)
{
	//printf("DEBUG: hook de OnPlayerConnect (anticheat.inc)");
	CountFlood[playerid][MESSAGE] = 0;
	CountFlood[playerid][COMMAND] = 0;
	TimeFlood[playerid][MESSAGE] = 0;
	TimeFlood[playerid][COMMAND] = 0;
	PlayerBOT[playerid] = false;
	if(BOT == true)
	{
		new ip[16];
		new ip_player[16];
		new ContarIPs = 0;
		GetPlayerIp(playerid, ip, sizeof(ip));
		foreach(new i : Player)
		{
	    	GetPlayerIp(i, ip_player, sizeof(ip_player));
	    	if(strcmp(ip, ip_player, true) == 0)
		    	++ContarIPs;
			else
		    	continue;
		}
		if(ContarIPs > MAX_IP)
		{
	    	PlayerBOT[playerid] = true;
			defer SendMessage(playerid, "_");
			return -2;
		}
	}
	return 1;
}

hook OnPlayerDisconnect(playerid, reason)
{
	//printf("DEBUG: hook de OnPlayerDisconnect (anticheat.inc)");
	if(IsPlayerBOT(playerid)) return -2;
	return 1;
}

hook OnPlayerText(playerid, text[])
{
    //printf("DEBUG: hook de OnPlayerText (anticheat.inc)");
   	if(IsPlayerInClassSelection(playerid) != 0)
   	{
		SendClientMessage(playerid, Rojo, "ERROR: Usted no puede escribir estando en la selección de clases.");
		return -1;
	}
	if(AntiSpam(playerid, text))
		return -1;
	if(Data[playerid][Level] < 3)
	{
	    new time = TimeFlood[playerid][MESSAGE];
   		if(time < gettime())
	    	CountFlood[playerid][MESSAGE] = 0;
		if((time-gettime()) > 0)
		{
	    	new string[73];
	    	format(string, sizeof(string), "ERROR: Te falta %d %s para volver a escribir un mensaje en el chat.", (time-gettime()), (((time-gettime()) == 1) ? ("segundo") : ("segundos")));
	    	SendClientMessage(playerid, Rojo, string);
			//printf("%d", CountFlood[playerid][MESSAGE]);
			if(++CountFlood[playerid][MESSAGE] == 3)
				SetPlayerMessage(playerid, "Flood", Rojo);
			return -1;
		}
		TimeFlood[playerid][MESSAGE] = gettime()+TIME_USED;
	}
	return 1;
}

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
    //printf("DEBUG: hook de OnPlayerCommandReceived (anticheat.inc)");
   	if(IsMapLoading == true)
	{
		SendClientMessage(playerid, Rojo, "ERROR: No puedes usar comandos al momento que se esté cargando el mapa.");
		return 0;
	}
	if(IsPlayerInClassSelection(playerid) != 0)
	{
		SendClientMessage(playerid, Rojo, "ERROR: Usted no puede usar ningún comando estando en la selección de clases.");
		return 0;
	}
	if(Data[playerid][Level] < 3)
	{
	    new time = TimeFlood[playerid][COMMAND];
		if(time < gettime())
	    	CountFlood[playerid][COMMAND] = 0;
		if((time-gettime()) > 0)
		{
	    	new string[62];
	    	format(string, sizeof(string), "ERROR: Te falta %d %s para volver a usar un comando.", (time-gettime()), (((time-gettime()) == 1) ? ("segundo") : ("segundos")));
	    	SendClientMessage(playerid, Rojo, string);
			//printf("%d", CountFlood[playerid][COMMAND]);
			if(++CountFlood[playerid][COMMAND] == 3)
				SetPlayerMessage(playerid, "Flood", Rojo);
			return 0;
		}
		TimeFlood[playerid][COMMAND] = gettime()+TIME_USED;
	}
	return OnPlayerCommandReceivedEx_Hook(playerid, cmd, params, flags);
}


flags:antibot(CMD_ADMIN);
public pc_cmd_antibot(playerid)
{
	BOT = !BOT;
	new string[38];
	format(string, sizeof(string), "* %s el sistema de Anti-Bot.", ((BOT == false) ? ("Desactivaste") : ("Activaste")));
	SendClientMessage(playerid, Amarillo, string);
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

bool:AntiSpam(playerid, string[])
{
	if(Data[playerid][Level] < 3)
	{
	    new const TextSpam[9][7] =
		{
			{".com"},
			{".org"},
			{".net"},
			{".me"},
			{".cl"},
			{".es"},
			{".biz"},
			{".info"},
			{".co"}
		};
		new ContarNumeros = 0;
		new i;
		for(i = 0; i < sizeof(TextSpam); ++i)
		{
		    if(strfind(string, TextSpam[i], true) >= 0)
		    {
		        SendClientMessage(playerid, Rojo, "ERROR: Está prohíbido ingresar direcciones web.");
		        return true;
		    }
		}
		for(i = 0; string[i] != '\0'; ++i)
		{
	    	if((ContarNumeros < 3) && (string[i] >= '0' && string[i] <= '9'))
	        	++ContarNumeros;
	    	if(ContarNumeros >= 3)
	    	{
	        	if(string[i] >= '0' && string[i] <= '9')
					string[i] = '*';
	    	}
		}
	}
	return false;
}
//==============================================================
#if !defined _ALS_OnPlayerCommandReceived
	#define _ALS_OnPlayerCommandReceived
#else
	#undef OnPlayerCommandReceived
#endif
#define OnPlayerCommandReceived OnPlayerCommandReceivedEx_Hook
//==============================================================
#endif /* FIN _INC_anti */
