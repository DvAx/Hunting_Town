#include "AT_command.h"

extern UART_HandleTypeDef huart2;

int sizeTabChar(char * s){
	int lenght = 0;
	while (s[lenght] != '\0')
		lenght++;
	return lenght;
}

void sendAT(UART_HandleTypeDef* huart, char command[], int nbRep, int taille_max){
	char rxBuff[taille_max];
	char buff[taille_max];
	int s = sizeTabChar(command);

	if (nbRep < 0)
		return;
	//logiquement pas besoin -----
	//HAL_UART_Transmit(&huart2,(uint8_t*)command,sizeTabChar(command),10);
	// -----
	
	//vidage buffer
	HAL_UART_Receive(huart, (uint8_t*)buff,taille_max,1);
	HAL_Delay(50);
	
	HAL_UART_Transmit(huart,(uint8_t*)command,s,10);
	uartEndLine(&huart2);
	
	for(int nb_reponse = 0; nb_reponse < nbRep; nb_reponse++){
		HAL_UART_Receive(huart, (uint8_t*)rxBuff,taille_max,1000);
		HAL_UART_Transmit(&huart2,(uint8_t*)rxBuff,taille_max,10);
		rxBuff[0] = '.';
		
	}
	uartEndLine(&huart2);
}

void uartEndLine(UART_HandleTypeDef *huart){
	char n[] = {'\n'};
	
	HAL_UART_Transmit(huart,(uint8_t*)n,1,10);
	
	return;
}
void initLARA(UART_HandleTypeDef *huart){
	int nbCommand = 5;
	AT_command initsCommands[nbCommand];
	int num_commande;
	
	// Code Pin
	AT_command monAT = init_AT_command(1,"AT+CPIN=\"0264\"\r", 5);
	initsCommands[0] = monAT;
	
	// Mode full fonctionnality
	initsCommands[1] = init_AT_command(1,"AT+CFUN 1\r", 5);
	
	for(num_commande = 0; num_commande < nbCommand; num_commande++){
		AT_command currentAT = initsCommands[num_commande];
		sendAT(huart, currentAT.command, currentAT.nombre_reponses, currentAT.taille_max_reponses);
	}
}

void initConnectionHTTP(UART_HandleTypeDef *huart){
	int nbCommand = 6;
	AT_command initsCommands[nbCommand];
	
	/* Config r�seau */
	
	// Automatic network registration
	initsCommands[0] = init_AT_command(1, "AT+COPS=0\r", 5);
	
	// On active le contexte PDP
	initsCommands[1] = init_AT_command(1, "AT+UPSDA=0,3\r", 5);
	
	/* Gestion de l'HTTP */
	
	// Reset de l'environnement HTTP
	initsCommands[2] = init_AT_command(1, "AT+UHTTP=0\r", 5);
	
	// Renseignement du nom du serveur
	initsCommands[3] = init_AT_command(1, "UHTTP=0,1,\"nom_du_serveur.com\"\r", 5);
	
	// Renseignement du port de communication HTTP
	initsCommands[4] = init_AT_command(1, "UHTTP=0,5,80", 5);
	
	// R�solution DNS � partir du nom du serveur
	initsCommands[5] = init_AT_command(1, "UDNSRN=0,\"nom_du_serveur.com\"\r", 30);
	
	for(unsigned int num_commande = 0; num_commande < nbCommand; num_commande++){
		AT_command currentAT = initsCommands[num_commande];
		sendAT(huart, currentAT.command, currentAT.nombre_reponses, currentAT.taille_max_reponses);
	}
}

AT_command init_AT_command(int nombre_reponses, char * command, int taille_max_reponses){
	AT_command mon_AT;
	mon_AT.command = command;
	mon_AT.nombre_reponses=nombre_reponses;
	mon_AT.taille_max_reponses=taille_max_reponses;
	return mon_AT;
}