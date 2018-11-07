//Librerías PAP (l_pap.inc)

#if !defined _INC_lpap
	#define _INC_lpap
#else
	#endinput
#endif

#undef MAX_VEHICLES
#define MAX_VEHICLES (325)

#define SendErrorMessageLevel(%0) 			if(Data[playerid][Level] < (%0)) return SendClientMessage(playerid, Rojo, "ERROR: Usted no tiene suficiente nivel para usar este comando."),0
#define SetPlayerSkinRandom(%0,%1) 			Data[%0][SkinClass] = ((!((%1) != 1)) ? (Skins[0]) : ((!((%1) != 2)) ? (Skins[1]) : ((!((%1) != 3)) ? (Skins[random(4)+2]) : ((!((%1) != 4)) ? (Skins[random(4)+6]) : ((!((%1) != 5)) ? (Skins[random(2)+10]) : ((!((%1) != 6)) ? (Skins[random(8)+12]) : (Skins[random(11)+20])))))))
#define DialogSound%0; 						PlayerPlaySound(%0, ((!(response == 0)) ? (1083) : (1084)), 0, 0, 0);
#define IsVehicleUsed(%0)                   (VehicleUsed[(%0)-1])
#define IsPlayerInClassSelection(%0) 		((Data[(%0)][IsClassSelection] == true) ? (1) : (0))
#define IsPlayerStateClass(%0) 				(IsPlayerInClassSelection(%0) != 0 || GetPlayerState(%0) == PLAYER_STATE_SPECTATING)
#define GetPlayerKills(%0) 					Data[(%0)][Kills]
#define GetTeamName(%0) 					InfoTeam[Data[%0][Teams]-1][E_NAME_TEAM]
#define GetTeamColor(%0) 					InfoTeam[Data[%0][Teams]-1][E_COLOUR_TEAM]
#define GetRankName(%0)            			NameRank[Data[%0][Level]]
#define GetMapName(%0)              		NameMap[%0-1]

#define Rojo 								(0xFF0000FF)
#define Amarillo 							(0xFFFF00AA)

#define MAX_VEHICLE_NAME					(17)

//ID Ciudades
#define VENTURAS        					(1)
#define SANTOS          					(2)
#define FIERRO          					(3)

//ID Modelos
#define MODEL_9MM        					(346)
#define MODEL_SNIPER     					(358)
#define MODEL_RIFLE      					(357)
#define MODEL_KATANA     					(339)
#define MODEL_MOTOSIERRA 					(341)

#define JUGADOR_NO_FORZADO     				(0)
#define JUGADOR_SI_FORZADO     				(1)
#define JUGADOR_MUERTO_FORZADO 				(2)

#define GAME_TEXT                           (0)
#define TEXT_DRAW                           (1)

enum //ID diálogos
{
	DIALOG_REGISTER = 50,
	DIALOG_LOGIN,
	DIALOG_TEAM,
	DIALOG_TPMCOLOR,
	DIALOG_COLORS_PRIMARY,
	DIALOG_COLORS_SECUNDARY,
	DIALOG_CMDS_1,
	DIALOG_CMDS_2,
	ELEGIR_ARENAS,
	ELEGIR_ARMAS,
	ENVIAR_DUELO,
	ZONAS_VER_DUELO,
	DUELO_RECIBIDO,
	CREATE_RACE,
	DIALOG_TL,
	DIALOG_AUTOTUNE
};

enum //ID Team
{
	PRESIDENT = 1,
	VICEPRESIDENT,
	SECURITY,
	POLI,
	SWAT,
	TERRO,
	CIVIL,
};

enum E_SKINCLASS
{
	E_SECURITY,
	E_POLICE,
	E_SWAT,
	E_TERRO,
	E_CIVIL
};

enum E_CMD_VIP
{
	bool:Wheels,
	bool:Hydraulics,
	bool:Vset,
	bool:Nitro,
	bool:Autotune,
	bool:cc,
	Timer:Stop_cc,
	SaveWheels_ID,
	ColorPrimary,
	ColorSecundary,
	DialogPrimary
};

enum @flags_cmd
{
	CMD_LEVEL_1 = 1,
	CMD_VIP_VEHICLE,
	CMD_VIP,
	CMD_MOD,
	CMD_ADMIN,
	CMD_HIDDEN,
	CMD_RACE,
	CMD_DUEL
};

enum @ID_DATA
{
	Text3D:LabelASK,
	President,
	VicePresident,
	TPMColor,
	Sync,
	TimeHoli,
	TiempoApoyo,
	ForcePlayer,
	KillingSpree,
	Timer:ExitCity,
	Timer:ID_TimePause,
	Timer:SpawnKill,
	Timer:Stop_Painting,
	TimeSelectTeam,//tiempo para seleccionar otro equipo.
	TimeRegister, //tiempo para registrarse.
	bool:IsPlayerKick,
	bool:IsClassSelection,
	bool:IsKillingSpree,
	bool:ASK,
	bool:Godmode,
	bool:PM,
	Register,
	Warning,
	SkinClass,
	SkinGuarded,
	Teams,
	TeamPresident,
	PasswordIncorrect,
	Password,
	PK,//President Kills
	PD,//President Deaths
	PS,//President Survivals
	Kills,
	Deaths,
	Level,
	DuelosGanados,
	DuelosPerdidos,
	CarrerasGanadas,
	CarrerasPerdidas
};

enum E_INFOTEAM
{
	E_COLOUR_TEAM,
	E_NAME_TEAM[15]
};

enum E_TPM_COLOURS
{
	E_TPM_COLORID,
	E_NAME_TPM_COLOR[27]
};

new Data[MAX_PLAYERS][@ID_DATA];
new FechaRegistro[MAX_PLAYERS][19];
new HoraRegistro[MAX_PLAYERS][9];
new VehiculoID[MAX_PLAYERS];
new TimePositionSuccess[MAX_PLAYERS];

new	Timer:Timer_President;
new Timer:ID_EndSpawnProtection[MAX_PLAYERS];

new bool:Carjack[MAX_PLAYERS];
new bool:ClaseDisponible[MAX_PLAYERS];
new bool:MostrarRachas[MAX_PLAYERS];
new bool:TimeLeft[MAX_PLAYERS][2];
new bool:CommandName;

new	bool:Kicked;
new	bool:IsMapLoading;
new bool:HolidayOn;

new bool:VehicleUsed[MAX_VEHICLES];

new Text:TextSpree[3];
new Text:TextPAP[2];
new Text:TextTL;

new UltimoID_PM[MAX_PLAYERS];

new id_president;
new id_vicepresident;
new	Minutes;
new	City;
new	CountMaps;
new	MayorRacha;

new Skin_Class[E_SKINCLASS];

//Variables VIP
new armas_vip = mS_INVALID_LISTID;
new skins_vip = mS_INVALID_LISTID;
new ruedas_vip = mS_INVALID_LISTID;
new VIP[MAX_PLAYERS][E_CMD_VIP];

new const InfoTeam[7][E_INFOTEAM] =
{
	// Colours    Name Team
	{0xFFFF00AA, {"Presidente"}},
	{0xFFFF0000, {"VicePresidente"}},
	{0x33AA3333, {"Seguridad"}},
	{0x006BD733, {"Policía"}},
	{0x006BD733, {"SWAT"}},
	{0xAA333333, {"Terrorista"}},
	{0xFFA00033, {"Civil"}}
};

new const NameRank[6][10] =
{
	{"Normal"},
	{"Normal"},
	{"VIP"},
	{"Moderador"},
	{"Admin"},
	{"Dueño"}
};

new const NameMap[3][13] =
{
	{"Las Venturas"},
	{"Los Santos"},
	{"San Fierro"}
};

new const Colours[10][E_TPM_COLOURS] =
{
	// Colours    Name Colours
	{0xFFFF00FF, {"{FFFF00}Amarillo (defecto)"}},
	{0xFF0000FF, {"{FF0000}Rojo"}},
	{0x00FF00FF, {"{40EF0C}Verde"}},
	{0xFF8000FF, {"{FF6600}Naranja"}},
	{0x0080FFFF, {"{0066FF}Azul"}},
	{0x800080FF, {"{990099}Morado"}},
	{0x804000FF, {"{993300}Café"}},
	{0xFF00FFFF, {"{FF33FF}Rosado"}},
	{0xFFFFFFFF, {"{FFFFFF}Blanco"}},
	{0x808080FF, {"{CCCCFF}Gris"}}
};

new const Skins[31] =
{
	165, //id skin presidente (índice: 0)
	147, //id skin vicepresidente (índice: 1)
	141, 164, 219, 150, //id skins seguridad (índice: 2..5)
	280, 284, 282, 288, //id skins policía (índice: 6..5)
	285, 286, //id skins swat (índice: 10..11)
	30, 46, 142, 192, 112, 223, 124, 127, //id skins terrorista (índice: 12..19)
	199, 140, 167, 264, 26, 114, 90, 29, 82, 260, 35 //id skins civiles (índice: 20..30)
};


/* Aquí están las posiciones del vector X, donde establecerá el sitio del textdraw dependiendo de la cantidad de letras que tenga el nombre del jugador con la mayor racha. */
new const Float:Position[18] =
{
	454.000000,
	446.000000,
	438.000000,
	430.000000,
	423.000000,
	415.000000,
	408.000000,
	400.000000,
	393.000000,
	385.000000,
	377.000000,
	369.000000,
	361.000000,
	354.000000,
	346.000000,
	338.000000,
	330.000000,
	324.000000
};

new const NamesVehicles[212][MAX_VEHICLE_NAME] =
{
	{"Landstalker"},{"Bravura"},{"Buffalo"},{"Linerunner"},{"Perrenial"},{"Sentinel"},{"Dumper"},{"Firetruck"},{"Trashmaster"},{"Stretch"},{"Manana"},{"Infernus"},{"Voodoo"},{"Pony"},{"Mule"},
	{"Cheetah"},{"Ambulance"},{"Leviathan"},{"Moonbeam"},{"Esperanto"},{"Taxi"},{"Washington"},{"Bobcat"},{"MrWhoopee"},{"BF Injection"},{"Hunter"},{"Premier"},{"Enforcer"},{"Securicar"},{"Banshee"},{"Predator"},
	{"Bus"},{"Rhino"},{"Barracks"},{"Hotknife"},{"Trailer1"},{"Previon"},{"Coach"},{"Cabbie"},{"Stallion"},{"Rumpo"},{"RCBandit"},{"Romero"},{"Packer"},{"Monster"},{"Admiral"},{"Squalo"},{"Seasparrow"},{"Pizzaboy"},
	{"Tram"},{"Trailer2"},{"Turismo"},{"Speeder"},{"Reefer"},{"Tropic"},{"Flatbed"},{"Yankee"},{"Caddy"},{"Solair"},{"BerkleyRCVan"},{"Skimmer"},{"PCJ-600"},{"Faggio"},{"Freeway"},{"RCBaron"},{"RCRaider"},{"Glendale"},
	{"Oceanic"},{"Sanchez"},{"Sparrow"},{"Patriot"},{"Quad"},{"Coastguard"},{"Dinghy"},{"Hermes"},{"Sabre"},{"Rustler"},{"ZR-350"},{"Walton"},{"Regina"},{"Comet"},{"BMX"},{"Burrito"},{"Camper"},{"Marquis"},{"Baggage"},
	{"Dozer"},{"Maverick"},{"NewsChopper"},{"Rancher"},{"FBI Rancher"},{"Virgo"},{"Greenwood"},{"Jetmax"},{"Hotring"},{"Sandking"},{"Blista Compact"},{"Police Maverick"},{"Boxville"},{"Benson"},{"Mesa"},{"RCGoblin"},
	{"HotringRacer A"},{"HotringRacer B"},{"BloodringBanger"},{"Rancher"},{"SuperGT"},{"Elegant"},{"Journey"},{"Bike"},{"MountainBike"},{"Beagle"},{"Cropdust"},{"Stunt"},{"Tanker"},{"Roadtrain"},{"Nebula"},{"Majestic"},
	{"Buccaneer"},{"Shamal"},{"Hydra"},{"FCR-900"},{"NRG-500"},{"HPV1000"},{"CementTruck"},{"TowTruck"},{"Fortune"},{"Cadrona"},{"FBITruck"},{"Willard"},{"Forklift"},{"Tractor"},{"Combine"},{"Feltzer"},{"Remington"},
	{"Slamvan"},{"Blade"},{"Freight"},{"Streak"},{"Vortex"},{"Vincent"},{"Bullet"},{"Clover"},{"Sadler"},{"FiretruckLA"},{"Hustler"},{"Intruder"},{"Primo"},{"Cargobob"},{"Tampa"},{"Sunrise"},{"Merit"},{"Utility"},
	{"Nevada"},{"Yosemite"},{"Windsor"},{"MonsterA"},{"MonsterB"},{"Uranus"},{"Jester"},{"Sultan"},{"Stratum"},{"Elegy"},{"Raindance"},{"RC Tiger"},{"Flash"},{"Tahoma"},{"Savanna"},{"Bandito"},{"FreightFlat"},
	{"StreakCarriage"},{"Kart"},{"Mower"},{"Duneride"},{"Sweeper"},{"Broadway"},{"Tornado"},{"AT-400"},{"DFT-30"},{"Huntley"},{"Stafford"},{"BF-400"},{"Newsvan"},{"Tug"},{"Trailer 3"},{"Emperor"},{"Wayfarer"},{"Euros"},
	{"Hotdog"},{"Club"},{"FreightCarriage"},{"Trailer3"},{"Andromada"},{"Dodo"},{"RCCam"},{"Launch"},{"PoliceCar(LSPD)"},{"PoliceCar(SFPD)"},{"PoliceCar(LVPD)"},{"PoliceRanger"},{"Picador"},{"S.W.A.T.Van"},{"Alpha"},
	{"Phoenix"},{"Glendale"},{"Sadler"},{"LuggageTrailerA"},{"LuggageTrailerB"},{"StairTrailer"},{"Boxville"},{"FarmPlow"},{"UtilityTrailer"}
};

new const MessageRandom[15][107] =
{
	{"* >> Únete al grupo de facebook: www.facebook.com/groups/ProtegeAlPresidente/"},
	{"* >> Para hablar por el chat de equipo use ![texto] por ejemplo: !Hola o también /tpm [mensaje]"},
	{"* >> Si tienes algún bug en el juego usa /sync"},
	{"* >> Usa /racha para mirar cuantos asesinatos sin morir tienes"},
	{"* >> ¿Quieres cambiarte de equipo? Usa /rc o /clase"},
	{"* >> Si vez algún hacker usa /reportar [id] [razón]"},
	{"* >> ¿Tienes alguna inquietud sobre el servidor? Usa /duda y los administradores te responderán"},
	{"* >> Usa /presidente para saber quien es el nuevo Gobernador"},
	{"* >> Usa /tpmcolor para cambiar la apariencia del Team Chat"},
	{"* >> ¿No quieres que nadie esté en tu vehículo? Usa /pka para sacar a todos del coche"},
	{"* >> ¿Quieres sacar alguna persona de tu vehículo? Pues usa /pk para retirar a ese jugador del coche"},
	{"* >> Usa /vicepresidente para descubrir quien es el que va a reemplazar al Presidente cuando muera"},
	{"* >> Para enviar mensajes privados a un determinado jugador, podéis usar /pm [playerid] [mensaje]"},
	{"* >> Si no quieres que los jugadores te envién mensajes privados, podéis usar /nopm"},
	{"* >> ¿No encuentras a donde está el Presidente situado? Pues usa /intel para obtener más información de él"}
};

#include "library/missing_time"
#include "library/OnPlayerPause"
#include "library/register"
#include "library/system_race/carreras"
#include "library/duelos"
#include "library/GetVehicleColor"
#include "library/vehicles"
#include "library/f_pap"
#include "library/spec"
#include "library/anti"
