#if !defined _INC_carreras
	#define _INC_carreras
	
#define COLOR_GRIS 				(0x808080FF)
#define MAX_RACERS				(8)

#define MAX_TIME_RACE			(5)
#define SIZE_CHECKPOINT			(12.0)

#define MINUTOS					(1)
#define SEGUNDOS				(0)

forward bool:IsPlayerRace(playerid);
forward HK_OnPlayerCommandReceived(playerid, cmd[], params[], flags);
forward HK_OnPlayerModelSelection(playerid, response, listid, modelid);

enum
{
	AIRPORT_LS,
	AIRPORT_LV,
	EAST_SANTOS,
	RED_COUNTY,
	MONTE_CHILLIAD,
	AA,
	BAYSIDE_SF,
	NO_RACE = -2
};

static racerid[MAX_RACERS];//En este arreglo guardaremos la id de cada corredor
static CountCheckPoint[MAX_RACERS];//En este arreglo guardaremos la cantidad de puntos de control que haga el corredor durante la carrera
static PlayerText:TextCP[MAX_RACERS];//En este arreglo se almacenará la id del "textdraw" de cada corredor
static Vehicles_Race[MAX_RACERS];
new Time[MAX_RACERS][2];//En esta matriz se guardará el tiempo (minutos y segundos) que tardo el corredor en terminar la carrera

static ConteoEnCarrera;
static Timer:ID_ConteoEnCarrera;
static Timer:ID_TiempoDeLlegada;

static Text:Box;
static Timer:ID_TimeWait;
static bool:StartedRace = false;//Esta variable es para chequear sí la carrera ha empezado o no
static SecondsX = 60;//En esta variable se guarda la cantidad de segundos de espera que tendrá la carrera antes de iniciar
static PositionInRace;//En esta variable se guarda la posición que terminó el corredor durante la carrera
static TotalCP;//Aquí se guarda el total de puntos de control que tendrá la carrera selecciona por el corredor
static CreateRace;//Esta variable es para comprobar en que lugar fue creada la carrera

static TypeVehicle;//Esta variable guardaremos la ID del modelo de un vehículo, especificada por el jugador quién creó la carrera
static Text3D:ID_Starting;
static vehicles_race = mS_INVALID_LISTID;

new CreateRacePlayer;//Esta variable es para verificar sí el jugador está recién eligiendo el modelo del vehículo que tendrá la carrera

#include "library/system_race/toptimes"
#include <YSI\y_hooks>

main()
{
	print("========================================================");
	print("|            Sistema de Carreras                       |");
	print("|                  Creado por MrDave                   |");
	print("|            Contacto: www.facebook.com/mrdavesamp     |");
	print("========================================================");
	print("\n");
	return HK_Main();
}

hook OnPlayerDisconnect(playerid, reason)
{
	//print("DEBUG: hook de OnPlayerDisconnect (carreras.inc)");
	if(IsPlayerRace(playerid) != false)
 	{
 	    new vehicleid = GetPlayerVehicleID(playerid);
 	    if(vehicleid != 0)
 			DestroyVehicle(vehicleid);
 	    RemovePlayerInRace(playerid);
		if(StartedRace == true)
 			SetEndRace();
 	}
 	if(CreateRacePlayer != INVALID_PLAYER_ID)
 	    CreateRacePlayer = INVALID_PLAYER_ID;
	return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    //print("DEBUG: hook de OnPlayerStateChange (carreras.inc)");
    if(newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER)
		VehiculoID[playerid] = GetPlayerVehicleID(playerid);
	if(newstate == PLAYER_STATE_DRIVER && oldstate == PLAYER_STATE_ONFOOT)
	    VehicleUsed[VehiculoID[playerid]-1] = true;
	if(newstate == PLAYER_STATE_ONFOOT && oldstate == PLAYER_STATE_DRIVER)
	    VehicleUsed[VehiculoID[playerid]-1] = false;
	if((oldstate == PLAYER_STATE_DRIVER) && (StartedRace == true) && (IsPlayerRace(playerid) != false))
	{
		new string[26 + 24];
		new name[24];
		GetPlayerName(playerid, name, sizeof(name));
		format(string, sizeof(string), "* %s ha abandonado la carrera", name);
		SendClientMessageToAll(COLOR_GRIS, string);
		DestroyVehicle(VehiculoID[playerid]);
		RemovePlayerInRace(playerid);
		SetEndRace();
		if(!(newstate == PLAYER_STATE_WASTED))
			OnPlayerSpawn(playerid);
		return -2;
	}
	return 1;
}

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
    //print("DEBUG: hook de OnPlayerCommandReceived (carreras.inc)");
	if(IsPlayerRace(playerid) == true)
	{
	    if(strcmp(cmd, "salir", true) != 0)
	    {
			SendClientMessage(playerid, Rojo, "ERROR: No puedes usar comandos estando en una carrera, usa /salir");
			PlayerPlaySound(playerid, 1140, 0, 0, 0);
			return 0;
	    }
	}
	if(CreateRacePlayer == playerid)
	{
	    SendClientMessage(playerid, Rojo, "ERROR: Usted no puedo usar comandos estando en un catálogo.");
	    return 0;
	}
	if(@flags_cmd:flags == CMD_RACE && Data[playerid][Teams] != CIVIL)
	{
		SendClientMessage(playerid, Rojo, "ERROR: Este comando sólo lo pueden usar los jugadores que sean de la clase civil.");
		return 0;
	}
	return HK_OnPlayerCommandReceived(playerid, cmd, params, flags);
}

public OnPlayerModelSelection(playerid, response, listid, modelid)
{
    //print("DEBUG: hook de OnPlayerModelSelection (carreras.inc)");
    if(listid == vehicles_race)
    {
        if(response == 0)
        {
			CreateRace = NO_RACE;
			CreateRacePlayer = INVALID_PLAYER_ID;
		}
		else
		{
		    new string[57 + 24 + 22 + MAX_VEHICLE_NAME];
			new NameRace[22];
			new name[24];
			GetPlayerName(playerid, name, sizeof(name));
		    switch(CreateRace)
		    {
		        case AIRPORT_LS: NameRace = "Aeropuerto LS";
		        case AIRPORT_LV: NameRace = "Aeropuerto LV";
				case EAST_SANTOS: NameRace = "East Los Santos";
				case RED_COUNTY: NameRace = "Red County";
				case MONTE_CHILLIAD: NameRace = "Monte Chilliad";
				case AA: NameRace = "Aeropuerto Abandonado";
				case BAYSIDE_SF: NameRace = "Bayside SF";
		    }
			for(new i = 0; i < MAX_RACERS; ++i)
				Vehicles_Race[i] = 0;
		    CreateRacePlayer = INVALID_PLAYER_ID;
		    TypeVehicle = 1;
		    pc_cmd_entrar(playerid);
		    TypeVehicle = modelid;
		    format(string, sizeof(string), "* %s ha comenzado la carrera: %s. Vehículo: %s - (Hecho por MrDave)", name, NameRace, NamesVehicles[TypeVehicle - 400]);
		    SendClientMessageToAll(COLOR_GRIS, string);
			SendClientMessageToAll(COLOR_GRIS, "* Usa /entrar para ser parte de la carrera");
			ID_TimeWait = repeat TimeWait();
			//printf("DEBUG: racerid[0]: %d", racerid[0]);
		}
		return 1;//si no le colocas esta instrucción "return", tendrá fallo el jugador quien creó la carrera al momento de poner /salir
    }
	return HK_OnPlayerModelSelection(playerid, response, listid, modelid);
}

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    //print("DEBUG: hook de OnDialogResponse (carreras.inc)");
    DialogSound playerid;
    if(dialogid == CREATE_RACE)
    {
		if(response == 0)
	 	    return -2;
		else
		{
		    if(CreateRace >= 0)
		    {
		        SendClientMessage(playerid, Rojo, "ERROR: Ya se creó una carrera o ya se lo está creando.");
		        return -2;
			}
			CreateRace = listitem;
			switch(CreateRace)
			{
			    case RED_COUNTY:
		 			OnPlayerModelSelection(playerid, 1, vehicles_race, 493);
			    case MONTE_CHILLIAD:
					 OnPlayerModelSelection(playerid, 1, vehicles_race, 510);
			    case BAYSIDE_SF:
					 OnPlayerModelSelection(playerid, 1, vehicles_race, 539);
				default:
				{
 					CreateRacePlayer = playerid;
					ShowModelSelectionMenu(playerid, vehicles_race, "vehiculos");
					SendClientMessage(playerid, Rojo, "* Selecciona un vehículo para la carrera");
				}
			}
		}
    }
	return 1;
}

hook OnGameModeInit()
{
	//print("DEBUG: hook de OnGameModeInit (carreras.inc)");
	for(new i = 0; i < MAX_RACERS; ++i)
	    racerid[i] = INVALID_PLAYER_ID;
	CreateRace = NO_RACE;
	CreateRacePlayer = INVALID_PLAYER_ID;
	vehicles_race = LoadModelSelectionMenu("Modelos/vehicles_race.txt");
	
	Box = TextDrawCreate(110.000053, 296.433288, "usebox");
	TextDrawLetterSize(Box, 0.000000, 4.159999);
	TextDrawTextSize(Box, 1.999983, 0.000000);
	TextDrawAlignment(Box, 1);
	TextDrawColor(Box, 0);
	TextDrawUseBox(Box, true);
	TextDrawBoxColor(Box, 102);
	TextDrawSetShadow(Box, 0);
	TextDrawSetOutline(Box, 0);
	TextDrawFont(Box, 0);
	return 1;
}

public OnPlayerEnterRaceCheckpoint(playerid)
{
	new i;//Aquí guardamos la posición del corredor que entró al punto de control (CP)
	new str[46];
	new racers = GetRacerCount();
	new position = racers;
    for(i = 0; i < MAX_RACERS; ++i)//Esto es para encontrar la posición del corredor
    {
        if(racerid[i] == playerid)
        {
			++CountCheckPoint[i];
			break;
		}
	}
	for(new k = 0; k < MAX_RACERS; ++k)//Esto es para actualizar los "textdraw" de cada corredor
	{
	    if(racerid[k] == INVALID_PLAYER_ID) continue;
	    position = racers;
		for(new j = 0; j < MAX_RACERS; ++j)//Esto es para comparar la cantidad de "CheckPoint" del corredor con los demás corredores
		{
	    	if(racerid[j] != INVALID_PLAYER_ID)
	    	{
	        	if((k != j) && (CountCheckPoint[k] >= CountCheckPoint[j]))
	            	--position;
	    	}
		}
		format(str, sizeof(str), "CheckPoint: %d/%d~n~Racers: %d/8~n~Position: %d", CountCheckPoint[k], TotalCP, racers, position);
		PlayerTextDrawSetString(racerid[k], TextCP[k], str);
		PlayerTextDrawShow(racerid[k], TextCP[k]);
	}
 	DisablePlayerRaceCheckpoint(playerid);
    switch(CreateRace)
    {
        case AIRPORT_LS:
        {
            switch(CountCheckPoint[i])
            { //Total CP: 16
				case 1: SetPlayerRaceCheckpoint(playerid, 0, 1930.0734,-2594.0278,13.2739, 1961.3419,-2593.6917,13.2740, SIZE_CHECKPOINT);
				case 2: SetPlayerRaceCheckpoint(playerid, 0, 2111.4348,-2549.9563,13.2662,2104.5017,-2524.7686,13.2730, SIZE_CHECKPOINT);
				case 3: SetPlayerRaceCheckpoint(playerid, 0, 2003.6057,-2493.1665,13.2662, 1969.1290,-2492.7397,13.2662, SIZE_CHECKPOINT);
				case 4: SetPlayerRaceCheckpoint(playerid, 0, 1768.2941,-2490.2622,13.2817, 1715.3293,-2489.6101,13.2818, SIZE_CHECKPOINT);
				case 5: SetPlayerRaceCheckpoint(playerid, 0, 1684.2783,-2593.5791,13.2740, 1706.9092,-2594.0938,13.2740, SIZE_CHECKPOINT);
				case 6: SetPlayerRaceCheckpoint(playerid, 0, 1930.0734,-2594.0278,13.2739, 1961.3419,-2593.6917,13.2740, SIZE_CHECKPOINT);
				case 7: SetPlayerRaceCheckpoint(playerid, 0, 2111.4348,-2549.9563,13.2662,2104.5017,-2524.7686,13.2730, SIZE_CHECKPOINT);
				case 8: SetPlayerRaceCheckpoint(playerid, 0, 2003.6057,-2493.1665,13.2662, 1969.1290,-2492.7397,13.2662, SIZE_CHECKPOINT);
				case 9: SetPlayerRaceCheckpoint(playerid, 0, 1768.2941,-2490.2622,13.2817, 1715.3293,-2489.6101,13.2818, SIZE_CHECKPOINT);
				case 10: SetPlayerRaceCheckpoint(playerid, 0, 1684.2783,-2593.5791,13.2740, 1706.9092,-2594.0938,13.2740, SIZE_CHECKPOINT);
				case 11: SetPlayerRaceCheckpoint(playerid, 0, 1930.0734,-2594.0278,13.2739, 1961.3419,-2593.6917,13.2740, SIZE_CHECKPOINT);
				case 12: SetPlayerRaceCheckpoint(playerid, 0, 2111.4348,-2549.9563,13.2662,2104.5017,-2524.7686,13.2730, SIZE_CHECKPOINT);
				case 13: SetPlayerRaceCheckpoint(playerid, 0, 2003.6057,-2493.1665,13.2662, 1969.1290,-2492.7397,13.2662, SIZE_CHECKPOINT);
				case 14: SetPlayerRaceCheckpoint(playerid, 0, 1768.2941,-2490.2622,13.2817, 1715.3293,-2489.6101,13.2818, SIZE_CHECKPOINT);
				case 15: SetPlayerRaceCheckpoint(playerid, 1, 1416.2501,-2492.0657,13.2818, 0, 0, 0, 20.0);
            }
        }
        case AIRPORT_LV:
        {
            switch(CountCheckPoint[i])
            { //Total CP: 19
                case 1: SetPlayerRaceCheckpoint(playerid, 0, 1477.9595,1376.2225,10.5474, 1478.1913,1349.7263,10.5513, SIZE_CHECKPOINT);
				case 2: SetPlayerRaceCheckpoint(playerid, 0, 1415.6279,1269.6392,10.5474, 1406.6318,1278.5808,10.5474, SIZE_CHECKPOINT);
				case 3: SetPlayerRaceCheckpoint(playerid, 0, 1389.5634,1447.4066,10.5474, 1388.8606,1496.5043,10.5473, SIZE_CHECKPOINT);
				case 4: SetPlayerRaceCheckpoint(playerid, 0, 1390.0499,1648.2261,10.5396, 1390.1683,1661.5491,10.5478, SIZE_CHECKPOINT);
				case 5: SetPlayerRaceCheckpoint(playerid, 0, 1433.6682,1722.4971,10.5474, 1451.5570,1715.5800,10.5474, SIZE_CHECKPOINT);
				case 6: SetPlayerRaceCheckpoint(playerid, 0, 1477.1224,1577.5353,10.5396, 1477.1854,1556.4949,10.5396, SIZE_CHECKPOINT);
    			case 7: SetPlayerRaceCheckpoint(playerid, 0, 1477.9595,1376.2225,10.5474, 1478.1913,1349.7263,10.5513, SIZE_CHECKPOINT);
				case 8: SetPlayerRaceCheckpoint(playerid, 0, 1415.6279,1269.6392,10.5474, 1406.6318,1278.5808,10.5474, SIZE_CHECKPOINT);
				case 9: SetPlayerRaceCheckpoint(playerid, 0, 1389.5634,1447.4066,10.5474, 1388.8606,1496.5043,10.5473, SIZE_CHECKPOINT);
				case 10: SetPlayerRaceCheckpoint(playerid, 0, 1390.0499,1648.2261,10.5396, 1390.1683,1661.5491,10.5478, SIZE_CHECKPOINT);
				case 11: SetPlayerRaceCheckpoint(playerid, 0, 1433.6682,1722.4971,10.5474, 1451.5570,1715.5800,10.5474, SIZE_CHECKPOINT);
				case 12: SetPlayerRaceCheckpoint(playerid, 0, 1477.1224,1577.5353,10.5396, 1477.1854,1556.4949,10.5396, SIZE_CHECKPOINT);
    			case 13: SetPlayerRaceCheckpoint(playerid, 0, 1477.9595,1376.2225,10.5474, 1478.1913,1349.7263,10.5513, SIZE_CHECKPOINT);
				case 14: SetPlayerRaceCheckpoint(playerid, 0, 1415.6279,1269.6392,10.5474, 1406.6318,1278.5808,10.5474, SIZE_CHECKPOINT);
				case 15: SetPlayerRaceCheckpoint(playerid, 0, 1389.5634,1447.4066,10.5474, 1388.8606,1496.5043,10.5473, SIZE_CHECKPOINT);
				case 16: SetPlayerRaceCheckpoint(playerid, 0, 1390.0499,1648.2261,10.5396, 1390.1683,1661.5491,10.5478, SIZE_CHECKPOINT);
				case 17: SetPlayerRaceCheckpoint(playerid, 0, 1468.4697,1725.0195,10.8125, 1468.9462,1731.8500,10.8125, SIZE_CHECKPOINT);
                case 18: SetPlayerRaceCheckpoint(playerid, 1, 1478.0845,1835.0878,10.5396, 0, 0, 0, 20.0);
					
            }
        }
        case EAST_SANTOS:
        {
            switch(CountCheckPoint[i])
            { //Total CP: 14
           		case 1: SetPlayerRaceCheckpoint(playerid, 0, 2564.4622,-1849.6777,3.5120, 2551.9890,-1850.6416,3.5324, SIZE_CHECKPOINT);
				case 2: SetPlayerRaceCheckpoint(playerid, 0, 2215.0979,-1850.8605,0.8780, 2192.7195,-1850.4603,2.2080, SIZE_CHECKPOINT);
				case 3: SetPlayerRaceCheckpoint(playerid, 0, 2141.8259,-1852.2335,3.7149, 2129.0620,-1852.0851,3.7115, SIZE_CHECKPOINT);
				case 4: SetPlayerRaceCheckpoint(playerid, 0, 1884.3607,-1826.0684,3.7114, 1873.0762,-1822.9357,3.7115, SIZE_CHECKPOINT);
				case 5: SetPlayerRaceCheckpoint(playerid, 0, 1643.9330,-1768.2417,3.6802, 1627.6384,-1764.4064,3.6802, SIZE_CHECKPOINT);
				case 6: SetPlayerRaceCheckpoint(playerid, 0, 1360.3640,-1711.8756,8.2483, 1360.0536,-1703.4209,8.3169, SIZE_CHECKPOINT);
				case 7: SetPlayerRaceCheckpoint(playerid, 0, 1389.6394,-1495.5392,8.3983, 1391.8931,-1488.0680,8.3990, SIZE_CHECKPOINT);
				case 8: SetPlayerRaceCheckpoint(playerid, 0, 1413.5067,-1354.3169,8.7641, 1414.3800,-1342.2837,11.1767, SIZE_CHECKPOINT);
				case 9: SetPlayerRaceCheckpoint(playerid, 0, 1454.6998,-1328.5887,13.1099, 1454.9023,-1321.1478,13.1099, SIZE_CHECKPOINT);
				case 10: SetPlayerRaceCheckpoint(playerid, 0, 1455.5493,-1192.8276,20.2488, 1455.4922,-1170.8077,23.4612, SIZE_CHECKPOINT);
				case 11: SetPlayerRaceCheckpoint(playerid, 0, 1510.4784,-1163.0211,23.6333, 1521.8689,-1162.9207,23.6333, SIZE_CHECKPOINT);
				case 12: SetPlayerRaceCheckpoint(playerid, 0, 1633.8917,-1161.9302,23.6224, 1637.4965,-1150.0260,23.6333, SIZE_CHECKPOINT);
				case 13: SetPlayerRaceCheckpoint(playerid, 1, 1638.9402,-1079.2927,23.6294, 0, 0, 0, 20.0);
            }
        }
        case RED_COUNTY:
        {
			switch(CountCheckPoint[i])
			{ //Total CP: 17
			    case 1: SetPlayerRaceCheckpoint(playerid, 0, 1601.7854,-47.2613,-0.0689, 1601.9169,-67.9399,-0.1588, SIZE_CHECKPOINT);
				case 2: SetPlayerRaceCheckpoint(playerid, 0, 1475.7603,-243.4765,-0.1226, 1461.2467,-247.2146,-0.1093, SIZE_CHECKPOINT);
				case 3: SetPlayerRaceCheckpoint(playerid, 0, 1311.1167,-238.3659,-0.1259, 1297.8207,-222.3972,-0.0954, SIZE_CHECKPOINT);
				case 4: SetPlayerRaceCheckpoint(playerid, 0, 1127.3284,-107.5887,-0.1436, 1107.5221,-106.3578,-0.1378, SIZE_CHECKPOINT);
				case 5: SetPlayerRaceCheckpoint(playerid, 0, 788.2062,-156.9054,-0.1402, 774.0362,-168.0102,-0.0440, SIZE_CHECKPOINT);
				case 6: SetPlayerRaceCheckpoint(playerid, 0, 610.5409,-228.0833,-0.1274, 576.4646,-234.2617,-0.1044, SIZE_CHECKPOINT);
				case 7: SetPlayerRaceCheckpoint(playerid, 0, 200.4537,-415.1142,-0.1062, 171.4522,-422.6388,-0.0863, SIZE_CHECKPOINT);
				case 8: SetPlayerRaceCheckpoint(playerid, 0, -38.1703,-614.9137,-0.1036, -54.9151,-626.7088,-0.1372, SIZE_CHECKPOINT);
				case 9: SetPlayerRaceCheckpoint(playerid, 0, -204.4428,-766.4868,-0.1191, -180.5345,-804.8074,-0.1626, SIZE_CHECKPOINT);
				case 10: SetPlayerRaceCheckpoint(playerid, 0, -28.9094,-914.4088,-0.0713, -1.4822,-912.6042,-0.1361, SIZE_CHECKPOINT);
				case 11: SetPlayerRaceCheckpoint(playerid, 0, 63.9451,-1221.2552,-0.1510, 48.8631,-1246.8671,-0.1060, SIZE_CHECKPOINT);
				case 12: SetPlayerRaceCheckpoint(playerid, 0, 9.8981,-1748.8392,-0.2761, 5.6051,-1776.4563,0.0312, SIZE_CHECKPOINT);
				case 13: SetPlayerRaceCheckpoint(playerid, 0, 111.8983,-2123.0388,-0.0872, 150.9472,-2141.8577,-0.2215, SIZE_CHECKPOINT);
				case 14: SetPlayerRaceCheckpoint(playerid, 0, 391.6814,-2136.3015,-0.1100, 415.6049,-2134.8960,-0.2664, SIZE_CHECKPOINT);
				case 15: SetPlayerRaceCheckpoint(playerid, 0, 724.8669,-1940.3585,-0.1888, 725.2842,-1908.7450,-0.3074, SIZE_CHECKPOINT);
				case 16: SetPlayerRaceCheckpoint(playerid, 1, 730.0796,-1515.2659,-0.0748, 0, 0, 0, 20.0);
			}
        }
		case MONTE_CHILLIAD:
		{
			switch(CountCheckPoint[i])
			{ //Total CP: 37
				case 1: SetPlayerRaceCheckpoint(playerid, 0, -2337.5188,-1816.0656,434.1120, -2341.0244,-1816.3048,433.7613, SIZE_CHECKPOINT);
				case 2: SetPlayerRaceCheckpoint(playerid, 0, -2430.7920,-1729.7212,433.5558, -2444.4756,-1719.1387,431.9053, SIZE_CHECKPOINT);
				case 3: SetPlayerRaceCheckpoint(playerid, 0, -2442.1667,-1781.8997,414.7874, -2440.6892,-1785.8302,413.7839, SIZE_CHECKPOINT);
				case 4: SetPlayerRaceCheckpoint(playerid, 0, -2476.4553,-1793.2950,404.9586, -2479.5928,-1790.2087,404.5122, SIZE_CHECKPOINT);
				case 5: SetPlayerRaceCheckpoint(playerid, 0, -2521.8684,-1707.6310,401.8838, -2522.1345,-1704.4321,401.9012, SIZE_CHECKPOINT);
				case 6: SetPlayerRaceCheckpoint(playerid, 0, -2526.6030,-1658.1919,400.6992, -2527.2556,-1653.3073,400.4085, SIZE_CHECKPOINT);
				case 7: SetPlayerRaceCheckpoint(playerid, 0, -2502.8628,-1567.2610,395.2555, -2500.5525,-1561.8931,394.9452, SIZE_CHECKPOINT);
				case 8: SetPlayerRaceCheckpoint(playerid, 0, -2480.6104,-1485.4412,390.7910, -2477.1814,-1476.8667,390.4528, SIZE_CHECKPOINT);
				case 9: SetPlayerRaceCheckpoint(playerid, 0, -2350.1135,-1447.3722,382.8891, -2344.3721,-1448.2501,382.5714, SIZE_CHECKPOINT);
				case 10: SetPlayerRaceCheckpoint(playerid, 0, -2244.9280,-1516.8763,378.5555, -2243.4292,-1522.0901,378.5903, SIZE_CHECKPOINT);
				case 11: SetPlayerRaceCheckpoint(playerid, 0, -2215.1782,-1680.5905,377.4406, -2215.8499,-1685.5673,377.3018, SIZE_CHECKPOINT);
				case 12: SetPlayerRaceCheckpoint(playerid, 0, -2189.2813,-1702.5417,374.7455, -2189.5828,-1693.7178,372.9881, SIZE_CHECKPOINT);
				case 13: SetPlayerRaceCheckpoint(playerid, 0, -2238.5830,-1470.7343,355.5596, -2241.1362,-1467.4478,355.1959, SIZE_CHECKPOINT);
				case 14: SetPlayerRaceCheckpoint(playerid, 0, -2337.9609,-1406.3744,359.2115, -2344.1389,-1405.0275,359.4434, SIZE_CHECKPOINT);
				case 15: SetPlayerRaceCheckpoint(playerid, 0, -2463.5493,-1423.4359,362.0656, -2467.7637,-1427.0757,362.3846, SIZE_CHECKPOINT);
				case 16: SetPlayerRaceCheckpoint(playerid, 0, -2556.9424,-1505.2494,358.6290, -2564.6230,-1505.8344,358.6436, SIZE_CHECKPOINT);
				case 17: SetPlayerRaceCheckpoint(playerid, 0, -2535.6880,-1434.5931,351.4910, -2529.6094,-1430.3879,350.4805, SIZE_CHECKPOINT);
				case 18: SetPlayerRaceCheckpoint(playerid, 0, -2345.6995,-1322.0068,311.3469, -2339.2307,-1315.9022,309.6611, SIZE_CHECKPOINT);
				case 19: SetPlayerRaceCheckpoint(playerid, 0, -2386.8538,-1282.3019,300.8168, -2394.5076,-1280.9384,299.1468, SIZE_CHECKPOINT);
				case 20: SetPlayerRaceCheckpoint(playerid, 0, -2557.4170,-1336.8441,266.5161, -2560.4771,-1341.8828,265.9873, SIZE_CHECKPOINT);
				case 21: SetPlayerRaceCheckpoint(playerid, 0, -2653.4294,-1387.6196,256.2105, -2665.6284,-1396.0565,254.1782, SIZE_CHECKPOINT);
				case 22: SetPlayerRaceCheckpoint(playerid, 0, -2621.8245,-1295.4215,225.5977, -2609.9131,-1284.5924,220.1760, SIZE_CHECKPOINT);
				case 23: SetPlayerRaceCheckpoint(playerid, 0, -2551.7712,-1157.9968,182.5557, -2545.9568,-1155.6968,181.5264, SIZE_CHECKPOINT);
				case 24: SetPlayerRaceCheckpoint(playerid, 0, -2550.0591,-1133.6758,174.1553, -2550.0591,-1133.6758,174.1553, SIZE_CHECKPOINT);
				case 25: SetPlayerRaceCheckpoint(playerid, 0, -2663.7839,-1205.8942,162.6365, -2667.4233,-1210.8408,162.0238, SIZE_CHECKPOINT);
				case 26: SetPlayerRaceCheckpoint(playerid, 0, -2748.4585,-1377.2163,143.7010, -2749.7961,-1385.1594,142.6885, SIZE_CHECKPOINT);
				case 27: SetPlayerRaceCheckpoint(playerid, 0, -2764.6067,-1574.8386,140.8287, -2765.0759,-1584.0613,140.8524, SIZE_CHECKPOINT);
				case 28: SetPlayerRaceCheckpoint(playerid, 0, -2771.2578,-1796.5760,141.3711, -2770.6428,-1801.7148,141.3809, SIZE_CHECKPOINT);
				case 29: SetPlayerRaceCheckpoint(playerid, 0, -2629.9663,-2001.0083,125.4385, -2627.4578,-2009.3073,125.2320, SIZE_CHECKPOINT);
				case 30: SetPlayerRaceCheckpoint(playerid, 0, -2535.0073,-2071.3413,127.4242, -2527.5664,-2072.1521,126.7523, SIZE_CHECKPOINT);
				case 31: SetPlayerRaceCheckpoint(playerid, 0, -2345.4941,-2101.1411,112.7104, -2334.3081,-2098.7241,113.7633, SIZE_CHECKPOINT);
				case 32: SetPlayerRaceCheckpoint(playerid, 0, -2146.3652,-1960.5237,117.7730, -2144.3364,-1955.8016,117.6728, SIZE_CHECKPOINT);
				case 33: SetPlayerRaceCheckpoint(playerid, 0, -2106.9346,-1886.8583,110.0812, -2102.9985,-1895.8900,108.9557, SIZE_CHECKPOINT);
				case 34: SetPlayerRaceCheckpoint(playerid, 0, -2185.7729,-2065.5198,84.3942,  -2189.8513,-2069.5007,83.1501, SIZE_CHECKPOINT);
				case 35: SetPlayerRaceCheckpoint(playerid, 0, -2339.4878,-2179.9399,34.3917, -2346.2590,-2182.9744,33.7299, SIZE_CHECKPOINT);
				case 36: SetPlayerRaceCheckpoint(playerid, 1, -2390.3889,-2202.7141,32.8926, 0, 0, 0, 20.0);
			}
		}
		case AA:
		{
		    switch(CountCheckPoint[i])
		    { //TotalCP: 24
				case 1: SetPlayerRaceCheckpoint(playerid, 0, -128.5921,2466.1804,14.5056, -135.0683,2468.6731,15.2468, SIZE_CHECKPOINT);
				case 2: SetPlayerRaceCheckpoint(playerid, 0, -274.2107,2518.7146,33.4656, -282.7391,2519.2070,33.8744, SIZE_CHECKPOINT);
				case 3: SetPlayerRaceCheckpoint(playerid, 0, -395.6831,2466.7068,41.5915, -401.7367,2462.2471,42.4293, SIZE_CHECKPOINT);
				case 4: SetPlayerRaceCheckpoint(playerid, 0, -577.8934,2428.0293,67.4398, -585.8781,2432.4631,68.8930, SIZE_CHECKPOINT);
				case 5: SetPlayerRaceCheckpoint(playerid, 0, -714.1791,2539.9973,73.1802, -717.1130,2547.4380,72.7987, SIZE_CHECKPOINT);
				case 6: SetPlayerRaceCheckpoint(playerid, 0, -739.4626,2642.2910,64.7831, -733.3566,2652.5791,62.7301, SIZE_CHECKPOINT);
				case 7: SetPlayerRaceCheckpoint(playerid, 0, -701.9642,2677.7776,57.1110, -706.9244,2692.5068,55.1437, SIZE_CHECKPOINT);
				case 8: SetPlayerRaceCheckpoint(playerid, 0, -758.0289,2691.8999,48.3277, -777.6136,2705.7852,45.9050, SIZE_CHECKPOINT);
				case 9: SetPlayerRaceCheckpoint(playerid, 0, -779.9521,2726.1924,45.0592, -795.6453,2730.2976,45.0661, SIZE_CHECKPOINT);
				case 10: SetPlayerRaceCheckpoint(playerid, 0, -969.8663,2716.7217,45.5943, -979.1757,2715.7881,45.5943, SIZE_CHECKPOINT);
				case 11: SetPlayerRaceCheckpoint(playerid, 0, -1190.9780,2690.8257,45.5943, -1206.6543,2688.3477,45.7001, SIZE_CHECKPOINT);
				case 12: SetPlayerRaceCheckpoint(playerid, 0, -1376.3251,2601.7791,54.5873, -1381.6262,2593.4534,55.5936, SIZE_CHECKPOINT);
				case 13: SetPlayerRaceCheckpoint(playerid, 0, -1428.6130,2352.0208,53.1893, -1424.6599,2340.7974,53.4693, SIZE_CHECKPOINT);
				case 14: SetPlayerRaceCheckpoint(playerid, 0, -1342.4091,2066.1865,51.8134, -1342.4292,2058.9861,52.3558, SIZE_CHECKPOINT);
				case 15: SetPlayerRaceCheckpoint(playerid, 0, -1179.9456,1801.3790,40.2545, -1196.0026,1798.9210,41.0770, SIZE_CHECKPOINT);
				case 16: SetPlayerRaceCheckpoint(playerid, 0, -1375.7280,1856.0029,36.8014, -1386.7966,1854.9155,36.4191, SIZE_CHECKPOINT);
				case 17: SetPlayerRaceCheckpoint(playerid, 0, -1621.0002,1834.0553,25.5708, -1632.0032,1832.4805,25.5708, SIZE_CHECKPOINT);
				case 18: SetPlayerRaceCheckpoint(playerid, 0, -1713.4462,1820.8271,24.3269, -1734.8479,1828.2357,23.2463, SIZE_CHECKPOINT);
				case 19: SetPlayerRaceCheckpoint(playerid, 0, -1772.4996,1881.5460,17.0187, -1778.0558,1898.1095,15.7643, SIZE_CHECKPOINT);
				case 20: SetPlayerRaceCheckpoint(playerid, 0, -1786.5884,2051.6694,8.4926,  -1791.6495,2063.9250,8.3660, SIZE_CHECKPOINT);
				case 21: SetPlayerRaceCheckpoint(playerid, 0, -1900.4399,2212.5437,5.9962, -1913.7865,2215.7397,6.8758, SIZE_CHECKPOINT);
				case 22: SetPlayerRaceCheckpoint(playerid, 0, -1984.7169,2363.1309,30.1646, -1987.0013,2374.9839,31.7994, SIZE_CHECKPOINT);
				case 23: SetPlayerRaceCheckpoint(playerid, 1, -1923.8564,2501.4092,42.3744, 0, 0, 0, 20.0);
		    }
		}
		case BAYSIDE_SF:
		{
		    switch(CountCheckPoint[i])
		    { //Total CP: 8
		        case 1: SetPlayerRaceCheckpoint(playerid, 0, -1920.8064,1993.5565,0.5000, -1900.6083,1970.8057,0.3408, SIZE_CHECKPOINT);
				case 2: SetPlayerRaceCheckpoint(playerid, 0, -1828.2794,1691.2156,0.3426, -1825.8458,1670.6361,0.2122, SIZE_CHECKPOINT);
				case 3: SetPlayerRaceCheckpoint(playerid, 0, -1812.3020,1431.1433,0.5971, -1811.3263,1420.1392,0.5305, SIZE_CHECKPOINT);
				case 4: SetPlayerRaceCheckpoint(playerid, 0, -1811.0118,1540.9359,0.0065, -1810.9268,1553.8740,0.4802, SIZE_CHECKPOINT);
				case 5: SetPlayerRaceCheckpoint(playerid, 0, -1645.1011,1475.6942,0.5925, -1635.7692,1452.1748,0.5421, SIZE_CHECKPOINT);
				case 6: SetPlayerRaceCheckpoint(playerid, 0, -1481.0823,1291.1047,0.4986, -1473.4254,1271.0222,0.2988, SIZE_CHECKPOINT);
				case 7: SetPlayerRaceCheckpoint(playerid, 1, -1566.8148,1249.6217,-0.2127, 0, 0, 0, 20.0);
		    }
		}
    }
	if(CountCheckPoint[i] == TotalCP)
	{
		new string[29 + 24 + 8 + 24];
		new lugar[8];
		new name[24];
		GetPlayerName(playerid, name, sizeof(name));
		PositionInRace++;
		switch(PositionInRace)
		{
			case 1: lugar = "primer";
			case 2: lugar = "segundo";
			case 3: lugar = "tercer";
			case 4: lugar = "cuarto";
			case 5: lugar = "quinto";
			case 6: lugar = "sexto";
			case 7: lugar = "séptimo";
			case 8: lugar = "octavo";
		}
		format(string, sizeof(string), "* %s terminó la carrera en %s lugar (Tiempo: 00:%02d:%02d)", name, lugar, Time[i][MINUTOS], Time[i][SEGUNDOS]);
		//printf("DEBUG: Time[0] - hour: %d | Time[1] - minutes: %d | Time[2] - seconds: %d\n", hour, minutes, seconds);
		SendClientMessageToAll(COLOR_GRIS, string);
		if(PositionInRace == 1) //Si ese ese corredor llega en primer lugar
		{
			EditFile_TopTimes(playerid, i);
			++Data[playerid][CarrerasGanadas];
 			WriteData(playerid, CARRERAS_GANADAS);
		}
		else
		{
			++Data[playerid][CarrerasPerdidas];
			WriteData(playerid, CARRERAS_PERDIDAS);
		}
		DestroyVehicle(VehiculoID[playerid]);
		racerid[i] = INVALID_PLAYER_ID;
  		DisablePlayerRaceCheckpoint(playerid);
  		CountCheckPoint[i] = 0;
  		PlayerTextDrawDestroy(playerid, TextCP[i]);
  		TextDrawHideForPlayer(playerid, Box);
		OnPlayerSpawn(playerid);
		SetEndRace();
	}
	return 1;
}

timer TimeWait[1000]()
{
    new string[35];
	if(SecondsX == 0)
	{
		new racers = GetRacerCount();
	    Delete3DTextLabel(ID_Starting);
		SecondsX = 60;
		stop ID_TimeWait;
		if(racers < 2) //si hay menos de 2 corredores
		{
			for(new i = 0; i < MAX_RACERS; ++i)
			{
				if(racerid[i] != INVALID_PLAYER_ID)
				{
					//printf("DEBUG: racerid: %d", racerid[i]);
					OnPlayerSpawn(racerid[i]);
					racerid[i] = INVALID_PLAYER_ID;
				}
			}
			CreateRace = NO_RACE;
			TypeVehicle = 0;
			SendClientMessageToAll(COLOR_GRIS, "* La carrera ha sido cancelada por falta de corredores (mínimo: 2)");
		}
		else //caso contrario, si hay 3 corredores o más
		{
		    new x[4] = {-4, -5, -12, -9};
		    new i = 0;
		    new pausados = 0;
		    StartedRace = true;
			do
			{
				//printf("DEBUG: %d", i);
				if(racerid[i] == INVALID_PLAYER_ID) continue;
				if(IsPlayerPaused(racerid[i]) != 0)
				    ++pausados;
			}while(++i < MAX_RACERS);
			if(racers-pausados < 2)
			{
			    i = -1;
			    while(++i < MAX_RACERS)
			    {
			        //printf("DEBUG: %d", i);
			        if(racerid[i] != INVALID_PLAYER_ID)
			        {
			            OnPlayerSpawn(racerid[i]);
			            racerid[i] = INVALID_PLAYER_ID;
			        }
			    }
				new string2[113];
				StartedRace = false;
    			CreateRace = NO_RACE;
				TypeVehicle = 0;
				format(string2, sizeof(string2),
				"* La carrera ha sido cancelada porqué hay %d %s y %s.",
				pausados,
				((pausados == 1) ? ("jugador en pausa") : ("jugadores en pausa")),
				(((racers-pausados) == 1) ? ("sólo hay un usuario disponible para la carrera") : ("no hay usuarios disponible para la carrera")));
			    SendClientMessageToAll(COLOR_GRIS, string2);
				return 1;
			}
			for(i = 0; i < MAX_RACERS; ++i)
			{
			    if(racerid[i] != INVALID_PLAYER_ID)
			    {
			        if(IsPlayerPaused(racerid[i]) != 0)
			        {
			            OnPlayerSpawn(racerid[i]);
			            SendClientMessage(racerid[i], Rojo, "* Fuiste eliminado de la carrera por estar en pausa.");
			            racerid[i] = INVALID_PLAYER_ID;
			            continue;
					}
			    	switch(CreateRace)
			    	{
						case AIRPORT_LS:
						{
						    x[0] = x[0] + 4;
						    TotalCP = 16;
				    		VehiculoID[racerid[i]] = CreateVehicle(TypeVehicle, 1431.2506,-2606.3176+x[0],13.2948,268.9387,-1,-1,-1);
				    		SetPlayerRaceCheckpoint(racerid[i], 0, 1684.2783,-2593.5791,13.2740, 1706.9092,-2594.0938,13.2740, SIZE_CHECKPOINT);
						}
						case AIRPORT_LV:
						{
						    x[1] = x[1] + 5;
						    TotalCP = 19;
    						VehiculoID[racerid[i]] = CreateVehicle(TypeVehicle, 1495.9607-x[1],1840.4764,10.5396,179.5449,-1,-1,-1);
    						SetPlayerRaceCheckpoint(racerid[i], 0, 1477.1224,1577.5353,10.5396, 1477.1854,1556.4949,10.5396, SIZE_CHECKPOINT);
						}
						case EAST_SANTOS:
						{
	    					x[0] = x[0] + 4;
	    					TotalCP = 14;
							VehiculoID[racerid[i]] = CreateVehicle(TypeVehicle, 2587.7405+x[0],-1472.7476,16.4552, 178.7851, -1, -1, -1);
							SetPlayerRaceCheckpoint(racerid[i], 0, 2583.4597,-1645.3906,2.1757, 2583.1765,-1660.1772,1.8289, SIZE_CHECKPOINT);
						}
						case RED_COUNTY:
						{
						    x[2] = x[2] + 12;
	    					TotalCP = 17;
							VehiculoID[racerid[i]] = CreateVehicle(TypeVehicle, 2207.9309,-150.8276-x[2],-0.1594,75.5859, -1, -1, -1);
							SetPlayerRaceCheckpoint(racerid[i], 0, 1815.6139,-30.4420,-0.1370, 1793.6884,-25.9330,-0.1929, SIZE_CHECKPOINT);
						}
						case MONTE_CHILLIAD:
						{
						    x[0] = x[0] + 4;
						    TotalCP = 37;
						    VehiculoID[racerid[i]] = CreateVehicle(TypeVehicle, -2309.2380-x[0],-1611.6111,483.4824,189.5177, -1, -1, -1);
						    SetPlayerRaceCheckpoint(racerid[i], 0, -2286.1147,-1707.8040,476.2592, -2283.9067,-1714.8105,473.8723, SIZE_CHECKPOINT);
						}
						case AA:
						{
							x[1] = x[1] + 5;
							TotalCP = 24;
							VehiculoID[racerid[i]] = CreateVehicle(TypeVehicle, 427.8650,2521.4700-x[1],16.2115,90.0806, -1, -1, -1);
							SetPlayerRaceCheckpoint(racerid[i], 0, 189.4212,2505.6987,16.2115, 176.6179,2505.8787,16.2115, SIZE_CHECKPOINT);
						}
						case BAYSIDE_SF:
						{
							x[3] = x[3] + 9;
							TotalCP = 8;
							VehiculoID[racerid[i]] = CreateVehicle(TypeVehicle, -2422.6970,2233.9543+x[3],0.5303,269.1426, -1, -1, -1);
							SetPlayerRaceCheckpoint(racerid[i], 0, -2325.9712,2196.6157,0.4970, -2323.7432,2191.5322,0.4366, SIZE_CHECKPOINT);
						}
			    	}
			    	Vehicles_Race[i] = VehiculoID[racerid[i]];
					Hide_TopTimes racerid[i]; //Esconde el "textdraw" del toptimes
			    	SetPlayerInterior(racerid[i], 0);
			    	SetVehicleVirtualWorld(VehiculoID[racerid[i]], 200);
			    	LinkVehicleToInterior(VehiculoID[racerid[i]], 0);
			    	PutPlayerInVehicle(racerid[i], VehiculoID[racerid[i]], 0);
			    	TogglePlayerControllable(racerid[i], 0);
			    	new str[46];
			    	format(str, sizeof(str), "CheckPoint: 0/%d~n~Racers: %d/8~n~Position: 0", TotalCP, racers);
					TextCP[i] = CreatePlayerTextDraw(racerid[i], 8.000000, 298.000000, str);
					PlayerTextDrawBackgroundColor(racerid[i], TextCP[i], 255);
					PlayerTextDrawFont(racerid[i], TextCP[i], 1);
					PlayerTextDrawLetterSize(racerid[i], TextCP[i], 0.280000, 1.000000);
					PlayerTextDrawColor(racerid[i], TextCP[i], -1);
					PlayerTextDrawSetOutline(racerid[i], TextCP[i], 0);
					PlayerTextDrawSetProportional(racerid[i], TextCP[i], 1);
					PlayerTextDrawSetShadow(racerid[i], TextCP[i], 1);
					PlayerTextDrawShow(racerid[i], TextCP[i]);
					TextDrawShowForPlayer(racerid[i], Box);
					Time[i][MINUTOS] = 0;
					Time[i][SEGUNDOS] = 0;
			    } //cierre del if(racerid[i] != INVALID_PLAYER_ID)
			} //cierre del for
			ConteoEnCarrera = 11;
			ID_ConteoEnCarrera = repeat TimerConteoEnCarrera();
		}//cierre del else
		return 1;
	} //cierre del if(SecondsX == 0)
	format(string, sizeof(string), "La carrera comienza en %d %s", SecondsX, ((SecondsX == 1) ? ("segundo") : ("segundos")));
	if(--SecondsX != 59)
	    Delete3DTextLabel(ID_Starting);
	ID_Starting = Create3DTextLabel(string, 0x0080FFFF, 829.1848,4.9253,1004.1870, 40.0, 200, 0);
	return 1;
}

timer TimerConteoEnCarrera[1000]()
{
	if(--ConteoEnCarrera == 0)
	{
	    ID_TiempoDeLlegada = repeat TiempoDeLlegada();
		ConteoEnCarrera = 11;
		stop ID_ConteoEnCarrera;
	    for(new i = 0; i < MAX_RACERS; ++i)
	    {
	        if(racerid[i] == INVALID_PLAYER_ID) continue;
			TogglePlayerControllable(racerid[i], 1);
 			GameTextForPlayer(racerid[i], "GO!", 1000, 3);
			SetPlayerHealth(racerid[i], 100);
 			PlayerPlaySound(racerid[i], 1057, 0, 0, 0);
 		}
	}
	else
	{
	    for(new i = 0; i < MAX_RACERS; ++i)
	    {
	        if(racerid[i] == INVALID_PLAYER_ID) continue;
	    	new str[3];
	    	format(str, sizeof(str), "%d", ConteoEnCarrera);
			GameTextForPlayer(racerid[i], str, 1000, 3);
			PlayerPlaySound(racerid[i], 1056, 0, 0, 0);
		}
	}
	return 1;
}

timer TiempoDeLlegada[1000]()
{
	for(new position = 0; position < MAX_RACERS; ++position)
	{
	    if(racerid[position] == INVALID_PLAYER_ID) continue;
		if(++Time[position][SEGUNDOS] == 60)
		{
	    	Time[position][SEGUNDOS] = 0;
	    	if(++Time[position][MINUTOS] == 4)
				SendClientMessage(racerid[position], Amarillo, "* En 1 minuto finalizará la carrera");
	    	if(Time[position][MINUTOS] == MAX_TIME_RACE)
	    	{
	        	DestroyVehicle(VehiculoID[racerid[position]]);
	        	OnPlayerSpawn(racerid[position]);
  				RemovePlayerInRace(racerid[position]);
				SetEndRace();
	    	}
		}
	}
}

bool:IsPlayerRace(playerid) //Esta función comprueba sí el jugador está en una carrera o no.
{
	for(new i = 0; i < MAX_RACERS; ++i)
	{
			if(playerid == racerid[i])
			    return true;
	}
	return false;
}
	
static RemovePlayerInRace(playerid)//Este procedimiento retira a un jugador de una determinada carrera.
{
	for(new i = 0; i < MAX_RACERS; ++i)
	{
		if(playerid == racerid[i])
		{
			racerid[i] = INVALID_PLAYER_ID;
			if(StartedRace == true)//Si la carrera ya comenzó
			{
			    DisablePlayerRaceCheckpoint(playerid);
			    CountCheckPoint[i] = 0;
			    PlayerTextDrawDestroy(playerid, TextCP[i]);
			    TextDrawHideForPlayer(playerid, Box);
		    	Data[playerid][CarrerasPerdidas]++;
				WriteData(playerid, CARRERAS_PERDIDAS);
				if(ConteoEnCarrera > 0)
   					TogglePlayerControllable(playerid, 1);
			}
			break;
		}
	}
}

static SetEndRace()//Este procedimiento establecerá el final de la carrera sí hay 0 corredores en la carrera
{
	if(GetRacerCount() == 0)
	{
	    stop ID_TiempoDeLlegada;
	    CreateRace = NO_RACE;
	    StartedRace = false;
	    TypeVehicle = 0;
	    PositionInRace = 0;
	    SendClientMessageToAll(COLOR_GRIS, "* La carrera ha finalizado");
	    for(new i = 0; i < MAX_RACERS; i++) 
		{ //Comprobar si hay vehículos en la carrera
		    if(Vehicles_Race[i] == 0) break;
			DestroyVehicle(Vehicles_Race[i]);
		}
	    if(ConteoEnCarrera > 0)
			stop ID_ConteoEnCarrera;
	}
}

static GetRacerCount()//Esta función obtiene la cantidad de corredores que hay en una carrera.
{
	new Count = 0;
	for(new i = 0; i < MAX_RACERS; ++i)
	{
		if(racerid[i] == INVALID_PLAYER_ID) continue;
		++Count;
	}
	return Count;
}

flags:carrera(CMD_RACE);
flags:entrar(CMD_RACE);
flags:salir(CMD_RACE);

cmd:toptimes(playerid) return ShowPlayerTopTimes(playerid);

cmd:carrera(playerid)
{
	if(TypeVehicle != 0)
	    return SendClientMessage(playerid, Rojo, "ERROR: Un jugador ya creó una carrera.");
	ShowPlayerDialog(playerid, CREATE_RACE, DIALOG_STYLE_LIST, "Selecciona una carrera",
	"1: Aeropuerto LS\n2: Aeropuerto LV\n3: East Los Santos\n4: Red County - JetMax\n5: Monte Chilliad - Mountain Bike\n6: Aeropuerto Abandonado\n7: Bayside SF - Vortex",
	"Seleccionar", "Cerrar");
	PlayerPlaySound(playerid, 1139, 0, 0, 0);
	return 1;
}

cmd:entrar(playerid)
{
	if(StartedRace == true)
	    return SendClientMessage(playerid, Rojo, "ERROR: La carrera ya comenzó.");
	if(TypeVehicle == 0)
		return SendClientMessage(playerid, Rojo, "ERROR: No hay ninguna carrera creada por el momento.");
	for(new i = 0; i < MAX_RACERS; ++i)
	{
	    if(racerid[i] == INVALID_PLAYER_ID)
	    {
	        racerid[i] = playerid;
	        break;
	    }
	}
	RemoveASK(playerid);
	ShowPlayerTopTimes(playerid, true);
    SetPlayerWorldBounds(playerid, 20000.0000, -20000.0000, 20000.0000, -20000.0000);
	SetPlayerPos(playerid, 833.2700,10.5884,1004.1797);
	SetPlayerFacingAngle(playerid, 164.1883);
	SetPlayerInterior(playerid, 3);
	SetPlayerVirtualWorld(playerid, 200);
	SetPlayerHealth(playerid, 99999);
	ResetPlayerWeapons(playerid);
	if(TypeVehicle != 1)
	{
		new string[43 + 24];
		new name[24];
		GetPlayerName(playerid, name, sizeof(name));
		format(string, sizeof(string), "* %s ha entrado a la carrera (Corredores: %d/%d)", name, GetRacerCount(), MAX_RACERS);
		SendClientMessageToAll(COLOR_GRIS, string);
	}
	if(racerid[7] == playerid)
	{
		SecondsX = 0;
		StartedRace = true;
	}
	return 1;
}

cmd:salir(playerid)
{
	if(IsPlayerRace(playerid) != true)
	    return SendClientMessage(playerid, Rojo, "ERROR: Usted debe estar en una carrera para usar este comando.");
	new vehicleid = GetPlayerVehicleID(playerid);
	if(vehicleid != 0)
	    DestroyVehicle(vehicleid);
	RemovePlayerInRace(playerid);
	if(StartedRace == true)//Si la carrera ha empezado
	{
 		new string[26 + 24];
		new name[24];
		GetPlayerName(playerid, name, sizeof(name));
		format(string, sizeof(string), "* %s ha abandonado la carrera", name);
		SendClientMessageToAll(COLOR_GRIS, string);
		SetEndRace();
	}
	OnPlayerSpawn(playerid);
	return 1;
}

//==============================================================
#if !defined _ALS_main
	#define _ALS_main
#else
	#undef main
#endif
#define main HK_Main
//==============================================================
#if !defined _ALS_OnPlayerCommandReceived
	#define _ALS_OnPlayerCommandReceived
#else
	#undef OnPlayerCommandReceived
#endif
#define OnPlayerCommandReceived HK_OnPlayerCommandReceived
//==============================================================
#if !defined _ALS_OnPlayerModelSelection
	#define _ALS_OnPlayerModelSelection
#else
	#undef OnPlayerModelSelection
#endif
#define OnPlayerModelSelection HK_OnPlayerModelSelection
//==============================================================
#endif /* FIN _INC_carreras */
