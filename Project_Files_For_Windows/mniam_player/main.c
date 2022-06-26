#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "amcom.h"
#include "amcom_packets.h"

#define DEFAULT_TCP_PORT 	"2001"

int number_of_foods = 0;
int number_of_players = 0;
int my_player = 0;

/*
//testy
uint16_t foodnumber = 6;
uint8_t statyw = 0;
float x = 340;
float y = 240;
*/

uint8_t first_iter_food = 1;
uint8_t first_iter_player = 1;
AMCOM_NewGameRequestPayload newgameRequestPayload;
AMCOM_PlayerUpdateRequestPayload playerupdateRequestPayload;
AMCOM_FoodUpdateRequestPayload foodupdateRequestPayload;
AMCOM_MoveRequestPayload moveRequestPayload;
AMCOM_MoveResponsePayload moveResponsePayload;

/**
 * This function will be called each time a valid AMCOM packet is received
 */
void amcomPacketHandler(const AMCOM_Packet* packet, void* userContext) {
	uint8_t amcomBuf[AMCOM_MAX_PACKET_SIZE];	// buffer used to serialize outgoing packets
	size_t bytesToSend = 0;						// size of the outgoing packet
	static int playerCounter;					// just a counter to distinguish player instances
	SOCKET sock = (SOCKET)userContext;			// socket used for communication with the game client

	switch (packet->header.type) {
	case AMCOM_IDENTIFY_REQUEST:
		printf("Got IDENTIFY.request. Responding with IDENTIFY.response\n");
		AMCOM_IdentifyResponsePayload identifyResponse;
		sprintf(identifyResponse.playerName, "Mariusz%d", playerCounter++);
		bytesToSend = AMCOM_Serialize(AMCOM_IDENTIFY_RESPONSE, &identifyResponse, sizeof(identifyResponse), amcomBuf);
		break;
	case AMCOM_NEW_GAME_REQUEST:
		printf("Got NEW_GAME.request.\n");
		// TODO: respond with NEW_GAME.confirmation

		AMCOM_NewGameRequestPayload * ptr1 = packet->payload;
        newgameRequestPayload.playerNumber = ptr1->playerNumber;
        newgameRequestPayload.numberOfPlayers = ptr1->numberOfPlayers;
        newgameRequestPayload.mapWidth = ptr1->mapWidth;
        newgameRequestPayload.mapHeight = ptr1->mapHeight;
		my_player = newgameRequestPayload.playerNumber;
		
		/*
		printf("\nnumer gracza : %d\n",ptr1->playerNumber);
		printf("liczba graczy : %d\n",ptr1->numberOfPlayers);
		printf("szerokosc mapy : %f\n",ptr1->mapWidth);
		printf("wysokosc mapy : %f\n",ptr1->mapHeight);
		*/

		bytesToSend = AMCOM_Serialize(AMCOM_NEW_GAME_RESPONSE, NULL, 0, amcomBuf);

	    break;
	case AMCOM_PLAYER_UPDATE_REQUEST:
		printf("Got PLAYER_UPDATE.request.\n");
		// TODO: use the received information

		AMCOM_PlayerUpdateRequestPayload * ptr2 = packet->payload;

		/*testy
		if(first_iter_player == 0 ){
			printf("\ndodajemy gracza, rozmiar ramki co przyszla: %d\n",packet->header.length);
			ptr2->playerState[1].playerNo = 1;
			ptr2->playerState[1].hp = 2;
			ptr2->playerState[1].x = x;
			ptr2->playerState[1].y = y;
			//hp++;
			x--;
			y--;
		}
		*/

		if(first_iter_player == 1){
			number_of_players = packet->header.length/11;
			for(int i = 0; i < number_of_players; i++){
				playerupdateRequestPayload.playerState[i].playerNo = ptr2->playerState[i].playerNo;
				playerupdateRequestPayload.playerState[i].hp = ptr2->playerState[i].hp;
				playerupdateRequestPayload.playerState[i].x = ptr2->playerState[i].x;
				playerupdateRequestPayload.playerState[i].y = ptr2->playerState[i].y;
			}
			first_iter_player = 0;
		}else{
			for(int i = 0; i < packet->header.length/11; i++){ //the number of iterations is the number of incoming packets
				for(int j = 0; j < number_of_players; j++){ // the number of iterations is the number of the current amount of players
					if(playerupdateRequestPayload.playerState[j].playerNo == ptr2->playerState[i].playerNo){
						//printf("\naktualizacja iteracja  i:%d j:%d\n", i,j);
						playerupdateRequestPayload.playerState[j].playerNo = ptr2->playerState[i].playerNo;
						playerupdateRequestPayload.playerState[j].hp = ptr2->playerState[i].hp;
						playerupdateRequestPayload.playerState[j].x = ptr2->playerState[i].x;
						playerupdateRequestPayload.playerState[j].y = ptr2->playerState[i].y;
					}
				}
			}
		}

		for(int i = 0; i < number_of_players+1; i++){
			printf("\nnumer gracza : %d\n",ptr2->playerState[i].playerNo);
			printf("hp gracza : %d\n",ptr2->playerState[i].hp);
			printf("pozycja X : %f\n",ptr2->playerState[i].x);
			printf("pozycja Y : %f\n",ptr2->playerState[i].y);
		}
	
	    break;
	case AMCOM_FOOD_UPDATE_REQUEST:
		printf("Got FOOD_UPDATE.request.\n");
		// TODO: use the received information

		AMCOM_FoodUpdateRequestPayload * ptr3 = packet->payload;

		int actualization_food_counter = 0;
		
		/* testy
		if(first_iter == 0 ){
			printf("\ndodajemy jedzenie, rozmiar ramki co przyszla: %d\n",packet->header.length);
			ptr3->foodState[1].foodNo = foodnumber;
			ptr3->foodState[1].state = statyw;
			ptr3->foodState[1].x = x;
			ptr3->foodState[1].y = y;
			foodnumber++;
			x -= 50;
			y -= 50;
		}
		*/
		
		if(first_iter_food == 1){
			number_of_foods = packet->header.length/11;
			for(int i = 0; i < number_of_foods; i++){
				foodupdateRequestPayload.foodState[i].foodNo = ptr3->foodState[i].foodNo;
				foodupdateRequestPayload.foodState[i].state = ptr3->foodState[i].state;
				foodupdateRequestPayload.foodState[i].x = ptr3->foodState[i].x;
				foodupdateRequestPayload.foodState[i].y = ptr3->foodState[i].y;
			}
			first_iter_food = 0;
		}else{
			for(int i = 0; i < packet->header.length/11; i++){ //the number of iterations is the number of incoming packets
				actualization_food_counter = 0;
				for(int j = 0; j < number_of_foods; j++){ // the number of iterations is the number of the current amount of food
					if(foodupdateRequestPayload.foodState[j].foodNo == ptr3->foodState[i].foodNo){
						//printf("\naktualizacja iteracja  i:%d j:%d\n", i,j);
						foodupdateRequestPayload.foodState[j].foodNo = ptr3->foodState[i].foodNo;
						foodupdateRequestPayload.foodState[j].state = ptr3->foodState[i].state;
						foodupdateRequestPayload.foodState[j].x = ptr3->foodState[i].x;
						foodupdateRequestPayload.foodState[j].y = ptr3->foodState[i].y;
						actualization_food_counter++; // when we update food, it means that it is not a new object and we will not have to add it in the next steps
					}
				}
				if(actualization_food_counter == 0){ // when the package is not update we need to add it to the structure
					//printf("\nudalo sie dodac jedzenie iteracja nr: %d\n", i);
					foodupdateRequestPayload.foodState[number_of_foods].foodNo = ptr3->foodState[i].foodNo;
					foodupdateRequestPayload.foodState[number_of_foods].state = ptr3->foodState[i].state;
					foodupdateRequestPayload.foodState[number_of_foods].x = ptr3->foodState[i].x;
					foodupdateRequestPayload.foodState[number_of_foods].y = ptr3->foodState[i].y;
					number_of_foods++;
				}
			}
		}

		//printf("\nactualization_food_counter: %d\n",actualization_food_counter);
		printf("ilosc jedzenia: %d\n",number_of_foods);

		for(int i = 0; i<number_of_foods;i++){
			printf("\nnumer jedzenia : %d\n",foodupdateRequestPayload.foodState[i].foodNo);
			printf("stan jedzenia : %d\n",foodupdateRequestPayload.foodState[i].state);
			printf("pozycja X : %f\n",foodupdateRequestPayload.foodState[i].x);
			printf("pozycja Y : %f\n",foodupdateRequestPayload.foodState[i].y);
		}

		break;
	case AMCOM_MOVE_REQUEST:
		printf("Got MOVE.request.\n");
		// TODO: respond with MOVE.confirmation

		AMCOM_MoveRequestPayload * ptr4 = packet->payload;

		moveRequestPayload.x = ptr4->x;
		moveRequestPayload.y = ptr4->y;

		/* Angle calculation and send AMCOM_MOVE_RESPONSE */
		float temp_distance_between_food, min_distance_between_food = 1000.0; // start value 1000 because this is max distance of map
		float closest_point_x_food, closest_point_y_food;

		float temp_distance_between_player, min_distance_between_player = 1000.0; // start value 1000 because this is max distance of map
		float closest_point_x_player, closest_point_y_player;

		float closest_point_x, closest_point_y;

		float angle;

		/* Looking for the nearest distance between player and food */
		for(int i = 0; i<number_of_foods; i++){
			if(foodupdateRequestPayload.foodState[i].state != 0){
				//printf("Weszlo:%d\n",i);
				temp_distance_between_food = sqrtf(powf(foodupdateRequestPayload.foodState[i].x - moveRequestPayload.x,2) 
										+ powf(foodupdateRequestPayload.foodState[i].y - moveRequestPayload.y,2));
				//printf("	Iteracja nr:%d Odleglosc:%f\n",i,temp_distance_between_food);
				if( (temp_distance_between_food < min_distance_between_food) && (temp_distance_between_food > 0) ){
					//printf("		Nowy najmniejszy dystans w iteracji nr:%d Odleglosc:%f\n",i,temp_distance_between_food);
					min_distance_between_food = temp_distance_between_food;
					closest_point_x_food = foodupdateRequestPayload.foodState[i].x;
					closest_point_y_food = foodupdateRequestPayload.foodState[i].y;
				}else{
					//printf("		Brak nowego najmniejszego dystansu w iteracji nr:%d\n",i);
				}
			}else{
				//printf("Nie weszlo:%d\n",i);
			}
		}

		/* Looking for the nearest distance between player and other player */
		for(int i = 0; i<number_of_players; i++){
			if( (playerupdateRequestPayload.playerState[my_player].hp - playerupdateRequestPayload.playerState[i].hp) >= 2 ){
				//printf("Weszlo:%d\n",i);
				temp_distance_between_player = sqrtf(powf(playerupdateRequestPayload.playerState[i].x - moveRequestPayload.x,2) 
										+ powf(playerupdateRequestPayload.playerState[i].y - moveRequestPayload.y,2));
				//printf("	Iteracja nr:%d Odleglosc:%f\n",i,temp_distance_between_food);
				if( (temp_distance_between_player < min_distance_between_player) && (temp_distance_between_player > 0) ){
					//printf("		Nowy najmniejszy dystans w iteracji nr:%d Odleglosc:%f\n",i,temp_distance_between_food);
					min_distance_between_player = temp_distance_between_player;
					closest_point_x_player = playerupdateRequestPayload.playerState[i].x;
					closest_point_y_player = playerupdateRequestPayload.playerState[i].y;
				}else{
					//printf("		Brak nowego najmniejszego dystansu w iteracji nr:%d\n",i);
				}
			}else{
				//printf("Nie weszlo:%d\n",i);
			}
		}

		/*Check what is closer (food or player(with 2hp less than my_player) )*/
		if(min_distance_between_food < min_distance_between_player){
			closest_point_x = closest_point_x_food;
			closest_point_y = closest_point_y_food;
		}
		else{
			closest_point_x = closest_point_x_player;
			closest_point_y = closest_point_y_player;
		}
		

		printf("\nMoje kordynaty: X:%f Y:%f\n", moveRequestPayload.x, moveRequestPayload.y);
		printf("Min_Odl do jedzenia: %f X:%f Y:%f\n", min_distance_between_food, closest_point_x_food, closest_point_y_food);
		printf("Min_Odl do gracza: %f X:%f Y:%f\n", min_distance_between_player, closest_point_x_player, closest_point_y_player);
		printf("Najblizszy punkt do gracza/jedzenia: X:%f Y:%f\n", closest_point_x, closest_point_y);

		angle = atan2f(closest_point_y - moveRequestPayload.y, closest_point_x - moveRequestPayload.x);

		moveResponsePayload.angle = angle;
		printf("Otrzymany kat: %f\n",angle);
		Sleep(5);

		bytesToSend = AMCOM_Serialize(AMCOM_MOVE_RESPONSE, &moveResponsePayload, sizeof(moveResponsePayload), amcomBuf);

		break;
	}

	if (bytesToSend > 0) {
		int bytesSent = send(sock, (const char*)amcomBuf, bytesToSend, 0 );
		if (bytesSent == SOCKET_ERROR) {
			printf("Socket send failed with error: %d\n", WSAGetLastError());
			closesocket(sock);
			return;
		} else {
			printf("Sent %d bytes through socket.\n", bytesSent);
		}
	}
}

DWORD WINAPI playerThread( LPVOID lpParam )
{
	AMCOM_Receiver amcomReceiver;		// AMCOM receiver structure
	SOCKET sock = (SOCKET)(lpParam);	// socket used for communication with game client
	char buf[512];						// buffer for temporary data
	int receivedBytesCount;				// holds the number of bytes received via socket

	printf("Got new TCP connection.\n");

	// Initialize AMCOM receiver
	AMCOM_InitReceiver(&amcomReceiver, amcomPacketHandler, (void*)sock);

	// Receive data from socket until the peer shuts down the connection
	do {
		// Fetch the bytes from socket into buf
		receivedBytesCount = recv(sock, buf, sizeof(buf), 0);
		if (receivedBytesCount > 0) {
			printf("Received %d bytes in socket.\n", receivedBytesCount);
			// Try to deserialize the incoming data
			AMCOM_Deserialize(&amcomReceiver, buf, receivedBytesCount);
		} else if (receivedBytesCount < 0) {
			// Negative result indicates that there was socket communication error
			printf("Socket recv failed with error: %d\n", WSAGetLastError());
			closesocket(sock);
			break;
		}
	} while (receivedBytesCount > 0);

	printf("Closing connection.\n");

	// shutdown the connection since we're done
	receivedBytesCount = shutdown(sock, SD_SEND);
	// cleanup
	closesocket(sock);

	return 0;
}

int main(int argc, char **argv) {
	(void)argc; (void)argv; // Mark as unused
	WSADATA wsaData;						// socket library data
	SOCKET listenSocket = INVALID_SOCKET;	// socket on which we will listen for incoming connections
	SOCKET clientSocket = INVALID_SOCKET;	// socket for actual communication with the game client
	struct addrinfo *addrResult = NULL;
	struct addrinfo hints;
	int result;

	// Say hello
	printf("mniAM player listening on port %s\nPress CTRL+x to quit\n", DEFAULT_TCP_PORT);

	// Initialize Winsock
	result = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (result != 0) {
		printf("WSAStartup failed with error: %d\n", result);
		return -1;
	}

	// Prepare hints structure
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	result = getaddrinfo(NULL, DEFAULT_TCP_PORT, &hints, &addrResult);
	if ( result != 0 ) {
		printf("Function 'getaddrinfo' failed with error: %d\n", result);
		WSACleanup();
		return -2;
	}

	// Create a socket for connecting to server
	listenSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);
	if (listenSocket == INVALID_SOCKET) {
		printf("Function 'socket' failed with error: %d\n", (int)(WSAGetLastError()));
		freeaddrinfo(addrResult);
		WSACleanup();
		return -3;
	}
	// Setup the TCP listening socket
	result = bind(listenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR) {
		printf("Function 'bind' failed with error: %d\n", (int)(WSAGetLastError()));
		freeaddrinfo(addrResult);
		closesocket(listenSocket);
		WSACleanup();
		return -4;
	}
	freeaddrinfo(addrResult);

	// Listen for connections
	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR) {
		printf("Function 'listen' failed with error: %d\n", (int)(WSAGetLastError()));
		closesocket(listenSocket);
		WSACleanup();
		return -5;
	}

	while (1) {
		// Accept client socket
		clientSocket = accept(listenSocket, NULL, NULL);
		if (clientSocket == INVALID_SOCKET) {
			printf("Function 'accept' failed with error: %d\n", (int)(WSAGetLastError()));
			closesocket(listenSocket);
			WSACleanup();
			return -6;
		} else {
			// Run a separate thread to handle the actual game communication
			CreateThread(NULL, 0, playerThread, (void*)clientSocket, 0, NULL);
		}
		Sleep(10);
	}

	// No longer need server socket
	closesocket(listenSocket);
	// Deinitialize socket library
	WSACleanup();
	// We're done
	return 0;
}