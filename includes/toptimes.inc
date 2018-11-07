#if !defined _INC_toptimes
	#define _INC_toptimes
#else
	#endinput
#endif

#include <YSI\y_hooks>
#define Hide_TopTimes%0; for(new z; z < sizeof(Textdraw_TT); ++z)TextDrawHideForPlayer(%0, Textdraw_TT[z]);

forward Cargar_NombreCorredores(name[], value[]);
forward Cargar_Minutos(name[], value[]);
forward Cargar_Segundos(name[], value[]);

//Variables para el sistema de TOP-TIMES (TT)
new Text:Textdraw_TT[6];
static NombreCorredores_TT[5][24];
static Minutos_TT[5];
static Segundos_TT[5];

//===============================================================
//Funciones públicas que van a extraer los datos del archivo, a sus determinadas variables.

public Cargar_NombreCorredores(name[], value[])
{
	INI_String("Corredor A", NombreCorredores_TT[0]);
	INI_String("Corredor B", NombreCorredores_TT[1]);
	INI_String("Corredor C", NombreCorredores_TT[2]);
	INI_String("Corredor D", NombreCorredores_TT[3]);
	INI_String("Corredor E", NombreCorredores_TT[4]);
	return 1;
}

public Cargar_Minutos(name[], value[])
{
	INI_Int("Corredor A", Minutos_TT[0]);
	INI_Int("Corredor B", Minutos_TT[1]);
	INI_Int("Corredor C", Minutos_TT[2]);
	INI_Int("Corredor D", Minutos_TT[3]);
	INI_Int("Corredor E", Minutos_TT[4]);
	return 1;
}

public Cargar_Segundos(name[], value[])
{
	INI_Int("Corredor A", Segundos_TT[0]);
	INI_Int("Corredor B", Segundos_TT[1]);
	INI_Int("Corredor C", Segundos_TT[2]);
	INI_Int("Corredor D", Segundos_TT[3]);
	INI_Int("Corredor E", Segundos_TT[4]);
	return 1;
}

//===============================================================

hook OnGameModeInit()
{
    //printf("DEBUG: hook de OnGameModeInit (toptimes.inc)");
	Textdraw_TT[0] = TextDrawCreate(19.000000, 263.000000, "(00:01:02) - 1. David_Roman");
	TextDrawBackgroundColor(Textdraw_TT[0], 255);
	TextDrawFont(Textdraw_TT[0], 1);
	TextDrawLetterSize(Textdraw_TT[0], 0.270000, 1.100000);
	TextDrawColor(Textdraw_TT[0], -1);
	TextDrawSetOutline(Textdraw_TT[0], 1);
	TextDrawSetProportional(Textdraw_TT[0], 1);

	Textdraw_TT[1] = TextDrawCreate(19.000000, 276.000000, "(00:02:60) - 2. MrDave");
	TextDrawBackgroundColor(Textdraw_TT[1], 255);
	TextDrawFont(Textdraw_TT[1], 1);
	TextDrawLetterSize(Textdraw_TT[1], 0.270000, 1.100000);
	TextDrawColor(Textdraw_TT[1], -1);
	TextDrawSetOutline(Textdraw_TT[1], 1);
	TextDrawSetProportional(Textdraw_TT[1], 1);

	Textdraw_TT[2] = TextDrawCreate(19.000000, 289.000000, "(00:02:60) - 3. [DRE]K3NDO_[LT]");
	TextDrawBackgroundColor(Textdraw_TT[2], 255);
	TextDrawFont(Textdraw_TT[2], 1);
	TextDrawLetterSize(Textdraw_TT[2], 0.270000, 1.100000);
	TextDrawColor(Textdraw_TT[2], -1);
	TextDrawSetOutline(Textdraw_TT[2], 1);
	TextDrawSetProportional(Textdraw_TT[2], 1);

	Textdraw_TT[3] = TextDrawCreate(19.000000, 302.000000, "(00:02:43) - 4. El_Gato");
	TextDrawBackgroundColor(Textdraw_TT[3], 255);
	TextDrawFont(Textdraw_TT[3], 1);
	TextDrawLetterSize(Textdraw_TT[3], 0.270000, 1.100000);
	TextDrawColor(Textdraw_TT[3], -1);
	TextDrawSetOutline(Textdraw_TT[3], 1);
	TextDrawSetProportional(Textdraw_TT[3], 1);

	Textdraw_TT[4] = TextDrawCreate(19.000000, 316.000000, "(00:03:05) - 5. Nahuel_Martino");
	TextDrawBackgroundColor(Textdraw_TT[4], 255);
	TextDrawFont(Textdraw_TT[4], 1);
	TextDrawLetterSize(Textdraw_TT[4], 0.270000, 1.100000);
	TextDrawColor(Textdraw_TT[4], -1);
	TextDrawSetOutline(Textdraw_TT[4], 1);
	TextDrawSetProportional(Textdraw_TT[4], 1);

	Textdraw_TT[5] = TextDrawCreate(11.000000, 246.000000, "TopTimes:");
	TextDrawBackgroundColor(Textdraw_TT[5], 255);
	TextDrawFont(Textdraw_TT[5], 1);
	TextDrawLetterSize(Textdraw_TT[5], 0.310000, 1.400000);
	TextDrawColor(Textdraw_TT[5], -1);
	TextDrawSetOutline(Textdraw_TT[5], 1);
	TextDrawSetProportional(Textdraw_TT[5], 1);
	return 1;
}

ShowPlayerTopTimes(playerid, bool:textdraw = false) //Esta función muestra el toptimes, ya sea mediante en "textdraw" o en diálogo
{
    INI_ParseFile("TopTimes.ini", "Cargar_%s",.bExtra = false);
    new i = 0;
	new j = 0;
	new aux = 0;
 	new SegTotal_TT[5];
	new PosID_TT[5]; //Aquí guardamos las posiciones de manera ordenada (las que estén disponible en el archivo .ini)
	for(i = 0; i < sizeof(PosID_TT); ++i)
	    PosID_TT[i] = i;
	for(i = 0; i < sizeof(SegTotal_TT); ++i)
		SegTotal_TT[i] = (Minutos_TT[i]*60) + Segundos_TT[i];
	for(i = 0; i < sizeof(SegTotal_TT); ++i)
	{ //Este ciclo es para comprobar sí el ordenamiento sigue siendo el correcto
 		for(j = 0; j < sizeof(SegTotal_TT); ++j)
 		{ //Esto ciclo es el que va a comparar cada elemento del array para ver cual es el mayor (valor_anterior > valor_siguiente)
 		    if(j == sizeof(SegTotal_TT)-1) break; //Esto es para evitar el desbordamiento del búfer
			if(SegTotal_TT[j] > SegTotal_TT[j+1])
 			{
 			    aux = PosID_TT[j];
 			    PosID_TT[j] = PosID_TT[j+1];
 			    PosID_TT[j+1] = aux;

 			    aux = SegTotal_TT[j];
 			    SegTotal_TT[j] = SegTotal_TT[j+1];
 			    SegTotal_TT[j+1] = aux;
 			}
		}
	}
	if(textdraw)
	{
	    new string[15+24];
		TextDrawShowForPlayer(playerid, Textdraw_TT[5]);
		for(i = 0; i < sizeof(SegTotal_TT); ++i)
		{
	    	format(string, sizeof(string), "(00:%02d:%02d) - %d. %s", Minutos_TT[PosID_TT[i]], Segundos_TT[PosID_TT[i]], (i+1), NombreCorredores_TT[PosID_TT[i]]);
	    	TextDrawSetString(Textdraw_TT[i], string);
	    	TextDrawShowForPlayer(playerid, Textdraw_TT[i]);
		}
	}
	else 
	{
	    new str_dest[(15+24)*5]; //cadena de destino
		new str_source[15+24]; //cadena fuente
		strcat(str_dest, "Lugar:\tCorredor:\tTiempo:\n");
		for(i = 0; i < sizeof(SegTotal_TT); ++i)
		{
	    	format(str_source, sizeof(str_source), "#%d\t%s\t00:%02d:%02d\n", (i+1), NombreCorredores_TT[PosID_TT[i]], Minutos_TT[PosID_TT[i]], Segundos_TT[PosID_TT[i]]);
	    	strcat(str_dest, str_source);
			ShowPlayerDialog(playerid, 5, DIALOG_STYLE_TABLIST_HEADERS, "TopTimes - Race", str_dest, "Cerrar", "");
		}
	}
	return 1;
}

EditFile_TopTimes(playerid, position) //Este procedimiento modifica el archivo TopTimes.ini (ya sea para agregar un nuevo corredor en la lista o no)
{
	new ID_Corredor = 0;
	new bool:NombreExistente = false;
	new bool:EscribirEnArchivo = true;
	new NombreCorredor_Actual[MAX_PLAYER_NAME];
	GetPlayerName(playerid, NombreCorredor_Actual, 24);
	new const RacersFile[5][11] =
	{
		{"Corredor A"},
		{"Corredor B"},
		{"Corredor C"},
		{"Corredor D"},
		{"Corredor E"}
	};
	INI_ParseFile("TopTimes.ini", "Cargar_%s",.bExtra = false);
	/*
		Este es el primer caso:
		Todo este algoritmo es para detectar si un corredor se encuentra en el archivo, pero sólo lo vamos a escribir si el tiempo
		es mejor al que tenía en el archivo
	*/
	for(new i = 0; i < sizeof(NombreCorredores_TT); ++i)
	{ //Este ciclo es para detectar si el nombre de ese corredor se encuentra en el archivo (TopTimes.ini)
		if(strcmp(NombreCorredores_TT[i], NombreCorredor_Actual, true) == 0)
		{
			new SegTotal_Archivo = (Minutos_TT[i]*60) + Segundos_TT[i];
			new SegTotal_ActualCorredor = (Time[position][MINUTOS]*60) + Time[position][SEGUNDOS];
			if(!(SegTotal_ActualCorredor < SegTotal_Archivo))
			//Esto es para que el usuario conserve su mejor tiempo en el archivo
				EscribirEnArchivo = false;
			else
			{
				NombreExistente = true; //Esto quiere decir, que ese nombre se encontró en el archivo (TopTimes.ini)
				ID_Corredor = i+1; //Aquí se suma +1 porqué estoy contando las posiciones de cada corredor en el archivo desde 1
			}
			break;
		}
	}
	if(!NombreExistente && EscribirEnArchivo)
	{
   		new PosID_CorredorTT = 0;//Aquí guardamos la ID del corredor que tenga un tiempo muy alto en la lista TopTimes.ini
   		new mayor_segundos = 0; //Aquí guardamos la cantidad de segundos más alto que tenga algún corredor de la lista TopTimes.ini
		new SegTotal_TT[5]; //Para guardar un tiempo total en SEGUNDOS (minutos*60 + los segundos adicionales)
		new SegTotal_CorredorActual = 0;
		for(new i = 0; i < sizeof(SegTotal_TT); ++i)
   		//Este ciclo es para convertir el tiempo de cada corredor que se encuentre en el archivo a SEGUNDOS
			SegTotal_TT[i] = (Minutos_TT[i]*60) + Segundos_TT[i];
		for(new i = 0; i < sizeof(SegTotal_TT); ++i)
		{ //Este ciclo es para determinar cual es el mayor tiempo de todos los corredores que estén en la lista TopTimes.ini
   			if(SegTotal_TT[i] > mayor_segundos)
   			{
   				mayor_segundos = SegTotal_TT[i];
   				PosID_CorredorTT = i+1;
			}
		}
		SegTotal_CorredorActual = (Time[position][MINUTOS]*60) + Time[position][SEGUNDOS];
		/*
			Luego debemos comparar los tiempos en segundos, de esa forma vamos hallar al nuevo corredor que vamos a escribir en el archivo.
			Vamos a suponer que el tiempo que llegó el corredor actual fue de 2:30 (en segundos sería: 120s + 30s = 150s)
			Ahora el tiempo más alto de la lista TopTimes.ini fue de 3:20 (en segundos sería: 180s + 20s = 190s) y ese corredor se encuentra
			en el archivo como en segunda posición, esto quiere decir que la variable PosID_CorredorTT tendrá guardado un dos.
			La comparación sería así:
			if(150s < 190s) Si la cantidad de segundos llega a ser menor a la de ese corredor, pues la condición se cumple y reemplazamos
			este nuevo corredor por el que había antes.
		*/
		if(SegTotal_CorredorActual < mayor_segundos)
  			ID_Corredor = PosID_CorredorTT; //Para escribir en el archivo los nuevos datos que había en el anterior corredor
		else
    		EscribirEnArchivo = false;
	}
 	if(EscribirEnArchivo)
 	{
		new INI:File = INI_Open("TopTimes.ini");
		INI_SetTag(File, "NombreCorredores");
		INI_WriteString(File, RacersFile[ID_Corredor-1], NombreCorredor_Actual);

		INI_SetTag(File, "Minutos");
		INI_WriteInt(File, RacersFile[ID_Corredor-1], Time[position][MINUTOS]);
		
		INI_SetTag(File, "Segundos");
		INI_WriteInt(File, RacersFile[ID_Corredor-1], Time[position][SEGUNDOS]);
		INI_Close(File);
	}
}
