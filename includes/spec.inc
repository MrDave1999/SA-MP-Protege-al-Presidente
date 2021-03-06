#if !defined _INC_spec
	#define _INC_spec

#include <YSI\y_hooks>

new SpecID[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...};

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	//printf("DEBUG: hook de OnPlayerKeyStateChange (spec.inc)");
    /*
	    - Sistema de Spec autom�tico.
	    S� el jugador hace click izquierdo, ir� espiando cada jugador de menor a mayor id (de manera ascendente).
	    S� el jugador hace click derecho, ir� espiando cada jugador de mayor a menor id (de manera descendente).
	    KEY_FIRE = bot�n izquierdo del mouse.
	    KEY_HANDBRAKE = bot�n derecho del mouse.
		La funci�n "IsPlayerStateClass" es para detectar s� el jugador est� en modo espectador o s� est� en la selecci�n de clases.
		-> Informaci�n importante:
		La variable "i", "primer_id", "ultimo_id" representa la id del jugador a quien vamos a espiar
		La variable "playerid" representa la id del espiador (el jugador que va a espiar)
	*/
	if((!(EstarEnDuelo[playerid] >= 0 && EstarEnDuelo[playerid] <= 9)) && (SpecID[playerid] != INVALID_PLAYER_ID) && (GetPlayerState(playerid) == PLAYER_STATE_SPECTATING))
	{ //Este c�digo s�lo se ejecutar� s� el jugador no est� mirando ning�n duelo y s� est� espiando alg�n jugador y s� est� en modo espectador, la condici�n se cumple.
	    new i = INVALID_PLAYER_ID;
	    if(IsPlayerConnected(SpecID[playerid]) != 1 || IsPlayerStateClass(SpecID[playerid]))
	    {
	        /*
				La condici�n de arriba es para detectar s� el jugador a quien est�bamos espiando, no est� conectado o s� est� en
				modo espectador o en la selecci�n de clases. Ya que esto podr�a afectar el algoritmo que est� abajo de esta condici�n.
				Por esa raz�n debemos cambiar la id del jugador de la variable del espiador, para as� ponerle otra y evitar problemas.
	        */
	        foreach(i : Player)
	        {
	            if(IsPlayerStateClass(i)) continue;
	            SpecID[playerid] = i;
	            SpecID[i] = playerid;
				pc_cmd_espiar(playerid, SpecID[playerid], true);
	            break;
	        }
	        return 1;
	    }
 		new bool:valuex = false;
		new primer_id = INVALID_PLAYER_ID;
		new ultima_id = GetPlayerPoolSize();
		new anterior_id = INVALID_PLAYER_ID;
	    if(newkeys == KEY_FIRE) //Si presion� el click izquierdo
	    {
			foreach(i : Player)
			{
		    	if(IsPlayerStateClass(i)) continue;
 				if(primer_id == INVALID_PLAYER_ID)
 		       		primer_id = i;
 				if(valuex == true)
 				{
 		       		SpecID[playerid] = i;
 		       		SpecID[i] = playerid;
 		       		valuex = false;
 		       		break;
   				}
				if(i == SpecID[playerid])
 					valuex = true;
			}
			if(valuex == true)
			{
			    /*
			        La condici�n de arriba es para que cuando el recorrido del ciclo foreach llegue a su fin y no encuentra una nueva id
			        se quede con la primera que se guard� en la variable "primer_id".
			        Ejemplo:
			        Supongamos que tengamos esta tabla
			        M�todo ascendente (ID):
			        ID:     ID del jugador a quien estaban espiando:   ID del espiador:
			        0                                                  X = 0; (esto quiere decir que est� en modo espectador)
			        1
			        3       X = 3;
			        4
			        7
					Aqu� se supone que la pr�xima id a espiar ser�a el ID: 4, pero como est� en la selecci�n de clase, pasar�a al 7, pero
					como la ID:7 es un jugador que est� en modo espectador, entonces nunca se cumplir�a el if(valuex == true) que tenemos
					en nuestro ciclo "foreach".
					La soluci�n era agregar un if(valuex == true) afuera del ciclo foreach y establecer el nuevo jugador a espiar
					que fuera lo que est� almacenado en la variable "primer_id", en este caso ser� el ID: 1, ya que es el �nico jugador
					que no est� en modo espectador o en la selecci�n de clases.
					Obviamente esto fue un sencillo ejemplo de algo que podr�a suceder en el juego.
			    */
				SpecID[playerid] = primer_id;
				SpecID[primer_id] = playerid;
			}
		}
		if(newkeys == KEY_HANDBRAKE) //Si presion� el click derecho
		{
			foreach(i : Player)
			{
		    	if(IsPlayerStateClass(i)) continue;
				if(primer_id == INVALID_PLAYER_ID)
 		 			primer_id = i;
		    	if(SpecID[playerid] != i)
					anterior_id = i;
				if(i == SpecID[playerid])
				{
			    	SpecID[playerid] = ((primer_id == SpecID[playerid]) ? (ultima_id) : (anterior_id));
			    	if(SpecID[playerid] == anterior_id)
			    	    SpecID[anterior_id] = playerid;
					if(SpecID[playerid] == ultima_id)
					    SpecID[ultima_id] = playerid;
			    	if(IsPlayerStateClass(SpecID[playerid])) //Si el �ltimo id llega a estar en la selecci�n de clase o en modo espectador
			    	{
			    	    SpecID[ultima_id] = INVALID_PLAYER_ID; //Quiere decir que ning�n jugador est� espiando a ese usuario
			    	    foreach(i : Player)
			    	    {
			    	        if(IsPlayerStateClass(i)) continue;
							SpecID[playerid] = i;
			    	    }
			    	    SpecID[SpecID[playerid]] = playerid;
			    	    /*
			    	        El siguiente ejemplo ser� este (bueno es casi el mismo de arriba):
     			        	ID:     ID del jugador a quien estaban espiando:   ID del espiador:
			        		0       X = 0;
			        		1
			        		3                                                  X = 3; (esto quiere decir que est� en modo espectador)
			        		4
			        		7       (�ltimo id del servidor)
			        		El m�todo descendete har� lo siguiente:
							Como el ID: 0 es el jugador a quien estamos espiando, el pr�ximo usuario a espiar ser�a el ID: 7;
							sin embargo, el ID: 7 est� en la selecci�n de clase, haciendo que el if(IsPlayerStateClass(SpecID[playerid])
							se cumpla y buscar�amos el ID anterior del ID: 7, en este caso ser� el 4 (porqu� suponemos que no est� en la
							selecci�n de clases o en modo de espectador) y fin del algoritmo.
			    	    */
					}
					break;
				}
			}
		}
		if(newkeys == KEY_FIRE || newkeys == KEY_HANDBRAKE)
			pc_cmd_espiar(playerid, SpecID[playerid], true);
  	}
	return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    //printf("DEBUG: hook de OnPlayerStateChange (spec.inc)");
	if((SpecID[playerid] != INVALID_PLAYER_ID) && (newstate == PLAYER_STATE_ONFOOT || newstate == PLAYER_STATE_DRIVER || newstate == PLAYER_STATE_PASSENGER) && (oldstate == PLAYER_STATE_ONFOOT || oldstate == PLAYER_STATE_DRIVER || oldstate == PLAYER_STATE_PASSENGER))
	{
	    new PlayerSpec = 0;
		foreach(new i : Player)
		{
		    if(SpecID[i] == playerid)
		    { //Esto es para encontrar al jugador quien lo est� espiando
		        ++PlayerSpec;
		        pc_cmd_espiar(i, SpecID[i], true);
			}
		}
		if(PlayerSpec == 0)
		    SpecID[playerid] = INVALID_PLAYER_ID;
	}
	return 1;
}

#endif /* FIN _INC_spec */
