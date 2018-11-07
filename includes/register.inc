#if !defined _INC_register
	#define _INC_register

#include <YSI\y_hooks>

#define MAX_DATA    (16) //Cantidad máxima de datos a escribir

enum
{
	TL_GAMETEXT = 1, //Tiempo restante en GT
	TL_TEXTDRAW, //Tiempo restante en TD
	SKIN,
	LEVEL,
	DEATHS,
	KILLS,
	CARRERAS_PERDIDAS,
	CARRERAS_GANADAS,
	DUELOS_PERDIDOS,
	DUELOS_GANADOS,
	PRESIDENT_SURVIVALS,
	PRESIDENT_DEATHS,
	PRESIDENT_KILLS,
	PASSWORD
};

enum
{
	SI_REGISTRADO,
	NO_REGISTRADO,
	INICIO_SESION
};

forward LoadData(playerid, name[], value[]);

hook OnPlayerConnect(playerid)
{
	//printf("DEBUG: hook de OnPlayerConnect (register.inc)");
	new NamePath[45];
	Route(playerid, NamePath);
	Data[playerid][PasswordIncorrect] = 0;
	Data[playerid][SkinGuarded] = -1;
	Data[playerid][Password] = 0;
	Data[playerid][PK] = 0;//President Kills
	Data[playerid][PD] = 0;//President Deaths
	Data[playerid][PS] = 0;//President Survivals
	Data[playerid][DuelosGanados] = 0;
	Data[playerid][DuelosPerdidos] = 0;
	Data[playerid][CarrerasGanadas] = 0;
	Data[playerid][CarrerasPerdidas] = 0;
	Data[playerid][Kills] = 0;
	Data[playerid][Deaths] = 0;
	Data[playerid][Level] = 0;
	TimeLeft[playerid][TEXT_DRAW] = false;
	TimeLeft[playerid][GAME_TEXT] = true;
	/* Si la matriz toma el valor de 0, es porque està registrado y si guarda el valor de 1, es porque no està registrado. */
	Data[playerid][Register] = ((fexist(NamePath)) ? (SI_REGISTRADO) : (NO_REGISTRADO));
	if(Data[playerid][Register] == SI_REGISTRADO)
	{
	    INI_ParseFile(NamePath, "LoadData",.bExtra = true, .extra = playerid);
		ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_INPUT, "Cuenta SI registrada", "Ingrese su contraseña aquí:", "Aceptar", "Cerrar");
	}
	return 1;
}

hook OnPlayerRequestSpawn(playerid)
{
    //printf("DEBUG: hook de OnPlayerRequestSpawn (register.inc)");
   	if(Data[playerid][Register] == SI_REGISTRADO)
   	{
   	    SendClientMessage(playerid, Rojo, "ERROR: Primero debes iniciar sesión para poder jugar.");
		return -1;
	}
	return 1;
}

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    //printf("DEBUG: hook de OnDialogResponse (register.inc)");
    DialogSound playerid;
    if(dialogid == DIALOG_REGISTER)
    {
   		if(!((strlen(inputtext) != 0) && (response != 0)))
   		{
   		    ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_INPUT, "Cuenta no registrada", "Ingrese su contraseña aquí:", "Aceptar", "Cerrar");
			return -2;
		}
		if(strlen(inputtext) < 4 || strlen(inputtext) > 16)
		{
		    ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_INPUT, "Cuenta no registrada", "ERROR: La contraseña está demasiada corta o larga.\nMínimo: 4 caracteres - Máximo: 16 caracteres.\n", "Aceptar", "Cerrar");
			return -2;
		}
		new hour;
		new minutes;
		new seconds;
		new year;
		new month;
		new day;
		new NameMonth[11];
		new Fecha[19];
		new Hora[9];
		gettime(hour, minutes, seconds);
		getdate(year, month, day);
		switch(month)
		{
  			case 1: NameMonth = "Enero";
  			case 2: NameMonth = "Febrero";
  			case 3: NameMonth = "Marzo";
  			case 4: NameMonth = "Abril";
  			case 5: NameMonth = "Mayo";
  			case 6: NameMonth = "Junio";
  			case 7: NameMonth = "Julio";
  			case 8: NameMonth = "Agosto";
  			case 9: NameMonth = "Septiembre";
  			case 10: NameMonth = "Octubre";
  			case 11: NameMonth = "Noviembre";
  			case 12: NameMonth = "Diciembre";
		}
		format(Fecha, sizeof(Fecha), "%02d/%s/%d", day, NameMonth, year);
		format(FechaRegistro[playerid], sizeof(Fecha), "%s", Fecha);
		
		format(Hora, sizeof(Hora), "%02d:%02d:%02d", hour, minutes, seconds);
		format(HoraRegistro[playerid], sizeof(Hora), "%s", Hora);
		
		Data[playerid][Password] = udb_hash(inputtext);
		Data[playerid][Register] = INICIO_SESION;
		if(Data[playerid][Level] == 0)
		    Data[playerid][Level] = 1;
        WriteData(playerid, 1, MAX_DATA);
		GameTextForPlayer(playerid, "CUENTA REGISTRADA", 5000, 4);
		return -2;
    }

	if(dialogid == DIALOG_LOGIN)
	{
	    if(!((strlen(inputtext) != 0) && (response != 0)))
	    {
	        ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_INPUT, "Cuenta si registrada", "Ingrese su contraseña aquí:", "Aceptar", "Cerrar");
			return -2;
		}
		if(!(udb_hash(inputtext) == Data[playerid][Password]))
		{
		    ++Data[playerid][PasswordIncorrect];
			new pass[28];
			format(pass, sizeof(pass), "Contraseña Incorrecta [%d/4]", Data[playerid][PasswordIncorrect]);
			if(Data[playerid][PasswordIncorrect] == 4)
			{
			    SetPlayerMessage(playerid, "Contraseña Incorrecta", Rojo);
				return -2;
			}
			ShowPlayerDialog(playerid, DIALOG_LOGIN, DIALOG_STYLE_INPUT, "Cuenta si registrada", pass, "Aceptar", "Cerrar");
		}
		else
		{
			Data[playerid][Register] = INICIO_SESION;
			LoadAccount(playerid, playerid);
		}
	    return -2;
	}
	return 1;
}

/* Esta devolución de llamada va a ser invocado cuando se carguen los datos con su respectiva etiqueta "Data". */
public LoadData(playerid, name[], value[])
{
	INI_Int("Password", Data[playerid][Password]);
	INI_Int("President Kills", Data[playerid][PK]);
	INI_Int("President Deaths", Data[playerid][PD]);
	INI_Int("President Survivals", Data[playerid][PS]);
	INI_Int("Duelos Ganados", Data[playerid][DuelosGanados]);
	INI_Int("Duelos Perdidos", Data[playerid][DuelosPerdidos]);
	INI_Int("Carreras Ganadas", Data[playerid][CarrerasGanadas]);
	INI_Int("Carreras Perdidas", Data[playerid][CarrerasPerdidas]);
	INI_Int("Kills", Data[playerid][Kills]);
	INI_Int("Deaths", Data[playerid][Deaths]);
	INI_Int("Level", Data[playerid][Level]);
	INI_Int("Skin", Data[playerid][SkinGuarded]);
	INI_Bool("TL_GameText", TimeLeft[playerid][GAME_TEXT]);
	INI_Bool("TL_TextDraw", TimeLeft[playerid][TEXT_DRAW]);
	INI_String("Fecha Registro", FechaRegistro[playerid], sizeof(FechaRegistro[]));
	INI_String("Hora Registro", HoraRegistro[playerid], sizeof(HoraRegistro[]));
	return 1;
}

WriteData(playerid, const begin, end = -1)
{
	if(Data[playerid][Register] == INICIO_SESION)
	{
    	new NamePath[45];
		Route(playerid,NamePath);
		new INI:File = INI_Open(NamePath);
		if(end != -1) //Si no escribo varios datos...
		{
			for(new i = begin; i <= end; ++i)
				SaveData(playerid,i,File);
		}
		else
		    SaveData(playerid,begin,File);
		INI_Close(File);
	}
}

						
/* Este procedimiento guardará los datos sí el jugador se encuentra registrado dentro del servidor. */
SaveData(playerid, const typedata, const INI:File)
{
	switch(typedata)
	{
		case 1: INI_WriteBool(File, "TL_GameText", TimeLeft[playerid][GAME_TEXT]);
		case 2: INI_WriteBool(File, "TL_TextDraw", TimeLeft[playerid][TEXT_DRAW]);
		case 3: INI_WriteInt(File, "Skin", Data[playerid][SkinGuarded]);
		case 4: INI_WriteInt(File, "Level", Data[playerid][Level]);
		case 5: INI_WriteInt(File, "Deaths", Data[playerid][Deaths]);
		case 6: INI_WriteInt(File, "Kills", Data[playerid][Kills]);
		case 7: INI_WriteInt(File, "Carreras Perdidas", Data[playerid][CarrerasPerdidas]);
		case 8: INI_WriteInt(File, "Carreras Ganadas", Data[playerid][CarrerasGanadas]);
		case 9: INI_WriteInt(File, "Duelos Perdidos", Data[playerid][DuelosPerdidos]);
		case 10: INI_WriteInt(File, "Duelos Ganados", Data[playerid][DuelosGanados]);
		case 11: INI_WriteInt(File, "President Survivals", Data[playerid][PS]);
		case 12: INI_WriteInt(File, "President Deaths", Data[playerid][PD]);
		case 13: INI_WriteInt(File, "President Kills", Data[playerid][PK]);
		case 14: INI_WriteInt(File, "Password", Data[playerid][Password]);
		case 15: INI_WriteString(File, "Hora Registro", HoraRegistro[playerid]);
		case 16: INI_WriteString(File, "Fecha Registro", FechaRegistro[playerid]);
	}
}

/* Esta función extrae la localización del archivo. */
Route(playerid, namepath[])
{
	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	format(namepath, 45, "Dadmin/Usuarios/%s.ini", name);
	return 1;
}

/* Este procedimiento va a ser llamado cuando se cargue la cuenta de un jugador (estadísticas). */
LoadAccount(playerid, userid)
{
	new title[33];
	new content[463];
	new Float:PKills;
	new Float:PDeaths;
	new name[24];
	GetPlayerName(userid, name, sizeof(name));
	PDeaths = Data[userid][Deaths], PKills = Data[userid][Kills];
	format(title, sizeof(title), "Cuenta: %s", name);
	format(content, sizeof(content),
	"Usuario Registrado: %s\n\n\
	Fecha de Registro: %s\n\n\
	Hora de Registro: %s\n\n\
	Score: %d\n\n\
	Asesinatos: %d\n\n\
	Muertes: %d\n\n\
	Nivel: %d\n\n\
	Rango: %s\n\n\
	Dinero: $%d\n\n\
	Duelos Ganados: %d\n\n\
	Duelos Perdidos: %d\n\n\
	Carreras Ganadas: %d\n\n\
	Carreras Perdidas: %d\n\n\
	Asesinatos como Presidente: %d\n\n\
	Muertes como Presidente: %d\n\n\
	Supervivencias como Presidente: %d\n\n\
	Ratio: %.2f",
	((Data[userid][Register] == NO_REGISTRADO) ? ("No") : ("Si")),
	((Data[userid][Register] == NO_REGISTRADO) ? ("No tiene") : (FechaRegistro[playerid])),
	((Data[userid][Register] == NO_REGISTRADO) ? ("No tiene") : (HoraRegistro[playerid])),
	GetPlayerScore(userid),
	Data[userid][Kills],
	Data[userid][Deaths],
	Data[userid][Level],
	GetRankName(userid),
	GetPlayerMoney(userid),
	Data[userid][DuelosGanados],
	Data[userid][DuelosPerdidos],
	Data[userid][CarrerasGanadas],
	Data[userid][CarrerasPerdidas],
	Data[userid][PK],
	Data[userid][PD],
	Data[userid][PS],
	((Data[userid][Kills] == 0 || Data[userid][Deaths] == 0) ? (0.00) : (PKills / PDeaths)));
	ShowPlayerDialog(playerid, 5, DIALOG_STYLE_MSGBOX, title, content, "Aceptar", "");
	PlayerPlaySound(playerid, 1139,0,0,0);
}

alias:register("registrar");
alias:nombre("nick", "name", "cambiarnombre");
alias:cambiarpass("changepass");
alias:stats("cuenta");

flags:nombre(CMD_LEVEL_1);
flags:cambiarpass(CMD_LEVEL_1);

/*
Si la matriz Data[playerid][Register] guarda:
- El valor 0 es porque esta registrado.
- El valor 1 es porque no esta registrado.
- El valor 2 es porque ya se registró.
*/

/*
Si la matriz Data[playerid][Register] guarda:
- El valor 0 es porque esta registrado y no se necesita usar /register.
- El valor necesario que son 600 segundos (10 minutos) + el tiempo unix (gettime()), para que no pueda registrarse sin haber pasado ese determinado tiempo.
*/

cmd:register(playerid)
{
	if(Data[playerid][Register] == SI_REGISTRADO)
		return SendClientMessage(playerid, Rojo, "ERROR: Su cuenta ya se encuentra registrada.");
	if(Data[playerid][Register] == INICIO_SESION)
		return SendClientMessage(playerid, Rojo, "ERROR: Usted ya se registró.");
	if((Data[playerid][Level] != 5) && (gettime() < Data[playerid][TimeRegister]))
	    return SetPlayerTimeCMD(playerid, "para poder registrarte", Data[playerid][TimeRegister], 10);
	ShowPlayerDialog(playerid, DIALOG_REGISTER, DIALOG_STYLE_INPUT, "Cuenta NO registrada", "Ingrese su contraseña aquí:", "Aceptar", "Cerrar");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:nombre(playerid, params[])
{
	if(CommandName == false)
		return SendClientMessage(playerid, Rojo, "ERROR: Este comando fue desactivado por un Administrador.");
	if(!(params[0] != '\0'))
		return SendClientMessage(playerid,Rojo,"USO: /nombre [nuevo-nombre]");
	if(strlen(params) < 3 || strlen(params) > 20)
		return SendClientMessage(playerid,Rojo,"ERROR: El nombre está demasiado corto o largo (mínimo: 3 - máximo: 20).");
 	new name[24];
	GetPlayerName(playerid, name, sizeof(name));
	if(strcmp(params, name, true) == 0)
	    return SendClientMessage(playerid, Rojo, "ERROR: El nuevo nombre coincide con el tuyo.");
	new File_Player[29];
	format(File_Player, sizeof(File_Player), "Dadmin/Usuarios/%s.ini", params);
	if(fexist(File_Player) == 1)
		return SendClientMessage(playerid, Rojo, "ERROR: Ese nombre ya está registrado dentro del servidor.");
	new OldName[24];
	new string[80];
	GetPlayerName(playerid, OldName, 24);
	SetPlayerName(playerid, params);
    WriteData(playerid, 1, MAX_DATA);
	format(string, sizeof(string), "** %s se cambió el nombre por %s", OldName, params);
	SendClientMessageToAll(Amarillo, string);
	return 1;
}

cmd:cambiarpass(playerid, params[])
{
	if(!(params[0] != '\0'))
		return SendClientMessage(playerid,Rojo,"USO: /cambiarpass [nueva-contraseña]");
	if(strlen(params) < 4 || strlen(params) > 16)
		return SendClientMessage(playerid,Rojo,"ERROR: La contraseña está demasiada corta o larga (mínimo: 4 caracteres - máximo: 16 caracteres).");
	Data[playerid][Password] = udb_hash(params);
	WriteData(playerid, PASSWORD);
	new string[60];
	format(string, sizeof(string), "** La nueva contraseña de su cuenta es: %s", params);
	SendClientMessage(playerid, Amarillo, string);
	return 1;
}

cmd:stats(playerid, params[])
{
	if(sscanf(params, "d", params[0]))
	{
		SendClientMessage(playerid,Rojo,"* También puedes usar /stats [playerid]");
		LoadAccount(playerid, playerid);
		return 1;
	}
	if(!(IsPlayerConnected(params[0]) != 0))
		return SendClientMessage(playerid,Rojo,"ERROR: Ese jugador no se encuentra conectado.");
	LoadAccount(playerid, params[0]);
	return 1;
}

#endif /* FIN _INC_register */
