#if !defined _INC_spec
	#define _INC_spec

#include <YSI\y_hooks>

new SpecID[MAX_PLAYERS] = {INVALID_PLAYER_ID, ...};

hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	//printf("DEBUG: hook de OnPlayerKeyStateChange (spec.inc)");
    /*
	    - Sistema de Spec automático.
	    Sí el jugador hace click izquierdo, irá espiando cada jugador de menor a mayor id (de manera ascendente).
	    Sí el jugador hace click derecho, irá espiando cada jugador de mayor a menor id (de manera descendente).
	    KEY_FIRE = botón izquierdo del mouse.
	    KEY_HANDBRAKE = botón derecho del mouse.
		La función "IsPlayerStateClass" es para detectar sí el jugador está en modo espectador o sí está en la selección de clases.
		-> Información importante:
		La variable "i", "primer_id", "ultimo_id" representa la id del jugador a quien vamos a espiar
		La variable "playerid" representa la id del espiador (el jugador que va a espiar)
	*/
	if((!(EstarEnDuelo[playerid] >= 0 && EstarEnDuelo[playerid] <= 9)) && (SpecID[playerid] != INVALID_PLAYER_ID) && (GetPlayerState(playerid) == PLAYER_STATE_SPECTATING))
	{ //Este código sólo se ejecutará sí el jugador no está mirando ningún duelo y sí está espiando algún jugador y sí está en modo espectador, la condición se cumple.
	    new i = INVALID_PLAYER_ID;
	    if(IsPlayerConnected(SpecID[playerid]) != 1 || IsPlayerStateClass(SpecID[playerid]))
	    {
	        /*
				La condición de arriba es para detectar sí el jugador a quien estábamos espiando, no está conectado o sí está en
				modo espectador o en la selección de clases. Ya que esto podría afectar el algoritmo que esté abajo de esta condición.
				Por esa razón debemos cambiar la id del jugador de la variable del espiador, para así ponerle otra y evitar problemas.
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
	    if(newkeys == KEY_FIRE) //Si presionó el click izquierdo
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
			        La condición de arriba es para que cuando el recorrido del ciclo foreach llegue a su fin y no encuentra una nueva id
			        se quede con la primera que se guardó en la variable "primer_id".
			        Ejemplo:
			        Supongamos que tengamos esta tabla
			        Método ascendente (ID):
			        ID:     ID del jugador a quien estaban espiando:   ID del espiador:
			        0                                                  X = 0; (esto quiere decir que está en modo espectador)
			        1
			        3       X = 3;
			        4
			        7
					Aquí se supone que la próxima id a espiar sería el ID: 4, pero como está en la selección de clase, pasaría al 7, pero
					como la ID:7 es un jugador que está en modo espectador, entonces nunca se cumpliría el if(valuex == true) que tenemos
					en nuestro ciclo "foreach".
					La solución era agregar un if(valuex == true) afuera del ciclo foreach y establecer el nuevo jugador a espiar
					que fuera lo que esté almacenado en la variable "primer_id", en este caso será el ID: 1, ya que es el único jugador
					que no está en modo espectador o en la selección de clases.
					Obviamente esto fue un sencillo ejemplo de algo que podría suceder en el juego.
			    */
				SpecID[playerid] = primer_id;
				SpecID[primer_id] = playerid;
			}
		}
		if(newkeys == KEY_HANDBRAKE) //Si presionó el click derecho
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
			    	if(IsPlayerStateClass(SpecID[playerid])) //Si el último id llega a estar en la selección de clase o en modo espectador
			    	{
			    	    SpecID[ultima_id] = INVALID_PLAYER_ID; //Quiere decir que ningún jugador está espiando a ese usuario
			    	    foreach(i : Player)
			    	    {
			    	        if(IsPlayerStateClass(i)) continue;
							SpecID[playerid] = i;
			    	    }
			    	    SpecID[SpecID[playerid]] = playerid;
			    	    /*
			    	        El siguiente ejemplo será este (bueno es casi el mismo de arriba):
     			        	ID:     ID del jugador a quien estaban espiando:   ID del espiador:
			        		0       X = 0;
			        		1
			        		3                                                  X = 3; (esto quiere decir que está en modo espectador)
			        		4
			        		7       (Último id del servidor)
			        		El método descendete hará lo siguiente:
							Como el ID: 0 es el jugador a quien estamos espiando, el próximo usuario a espiar sería el ID: 7;
							sin embargo, el ID: 7 está en la selección de clase, haciendo que el if(IsPlayerStateClass(SpecID[playerid])
							se cumpla y buscaríamos el ID anterior del ID: 7, en este caso será el 4 (porqué suponemos que no está en la
							selección de clases o en modo de espectador) y fin del algoritmo.
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
		    { //Esto es para encontrar al jugador quien lo está espiando
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
