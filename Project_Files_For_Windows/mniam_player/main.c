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

uint8_t first_iter_food = 1;


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
		static AMCOM_NewGameRequestPayload newgameRequestPayload;

		memcpy(amcomBuf, packet->payload, sizeof(packet->payload));
		newgameRequestPayload = *(AMCOM_NewGameRequestPayload*)amcomBuf;

		bytesToSend = AMCOM_Serialize(AMCOM_NEW_GAME_RESPONSE, NULL, 0, amcomBuf);

	    break;
	case AMCOM_PLAYER_UPDATE_REQUEST:
		printf("Got PLAYER_UPDATE.request.\n");
		// TODO: use the received information
		static AMCOM_PlayerUpdateRequestPayload playerupdateRequestPayload;

		memcpy(amcomBuf, packet->payload, sizeof(packet->payload));
		playerupdateRequestPayload = *(AMCOM_PlayerUpdateRequestPayload*)amcomBuf;

	    break;
	case AMCOM_FOOD_UPDATE_REQUEST:
		printf("Got FOOD_UPDATE.request.\n");
		// TODO: use the received information

		static AMCOM_FoodUpdateRequestPayload foodupdateRequestPayload;
		static int number_of_foods = 0;
		
		if(first_iter_food == 1){ // saving all foods
			memcpy(amcomBuf, packet->payload, sizeof(packet->payload));
			foodupdateRequestPayload = *(AMCOM_FoodUpdateRequestPayload*)amcomBuf;
			number_of_foods = packet->header.length/11;
			first_iter_food = 0;
		}else{ // update only incoming changes
			memcpy(amcomBuf, packet->payload, sizeof(packet->payload));
			AMCOM_FoodUpdateRequestPayload *buf = (AMCOM_FoodUpdateRequestPayload*)amcomBuf;
			for(int i = 0; i < packet->header.length/11; i++){ //the number of iterations is the number of incoming packets
				for(int j = 0; j < number_of_foods; j++){ // the number of iterations is the number of the current amount of food
					if(foodupdateRequestPayload.foodState[j].foodNo == buf->foodState[i].foodNo){
						foodupdateRequestPayload.foodState[j].foodNo = buf->foodState[i].foodNo;
						foodupdateRequestPayload.foodState[j].state = buf->foodState[i].state;
						foodupdateRequestPayload.foodState[j].x = buf->foodState[i].x;
						foodupdateRequestPayload.foodState[j].y = buf->foodState[i].y;
					}
				}
			}
		}

		// for(int i = 0; i < number_of_foods; i++){
		// 	printf("foodNO = %d, state = %d, x = %f, y = %f\n", foodupdateRequestPayload.foodState[i].foodNo,
		// 	foodupdateRequestPayload.foodState[i].state, foodupdateRequestPayload.foodState[i].x, foodupdateRequestPayload.foodState[i].y);
		// }

		break;
	case AMCOM_MOVE_REQUEST:
		printf("Got MOVE.request.\n");
		// TODO: respond with MOVE.confirmation

		static AMCOM_MoveRequestPayload moveRequestPayload;
		static AMCOM_MoveResponsePayload moveResponsePayload;

		memcpy(amcomBuf, packet->payload, sizeof(packet->payload));
		moveRequestPayload = *(AMCOM_MoveRequestPayload*)amcomBuf;

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
				temp_distance_between_food = sqrtf(powf(foodupdateRequestPayload.foodState[i].x - moveRequestPayload.x,2) 
										+ powf(foodupdateRequestPayload.foodState[i].y - moveRequestPayload.y,2));
				if( (temp_distance_between_food < min_distance_between_food) && (temp_distance_between_food > 0) ){
					min_distance_between_food = temp_distance_between_food;
					closest_point_x_food = foodupdateRequestPayload.foodState[i].x;
					closest_point_y_food = foodupdateRequestPayload.foodState[i].y;
				}
			}
		}

		/* Looking for the nearest distance between player and other player */
		for(int i = 0; i<newgameRequestPayload.numberOfPlayers; i++){
			if( (playerupdateRequestPayload.playerState[newgameRequestPayload.playerNumber].hp - playerupdateRequestPayload.playerState[i].hp) >= 2 ){
				temp_distance_between_player = sqrtf(powf(playerupdateRequestPayload.playerState[i].x - moveRequestPayload.x,2) 
										+ powf(playerupdateRequestPayload.playerState[i].y - moveRequestPayload.y,2));
				if( (temp_distance_between_player < min_distance_between_player) && (temp_distance_between_player > 0) ){
					min_distance_between_player = temp_distance_between_player;
					closest_point_x_player = playerupdateRequestPayload.playerState[i].x;
					closest_point_y_player = playerupdateRequestPayload.playerState[i].y;
				}
			}
		}

		/*Check what is closer (food or player(with 2hp less than my player) )*/
		if(min_distance_between_food < min_distance_between_player){
			closest_point_x = closest_point_x_food;
			closest_point_y = closest_point_y_food;
		}
		else{
			closest_point_x = closest_point_x_player;
			closest_point_y = closest_point_y_player;
		}

		angle = atan2f(closest_point_y - moveRequestPayload.y, closest_point_x - moveRequestPayload.x);
		moveResponsePayload.angle = angle;
		printf("angle:%f\n", angle);
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