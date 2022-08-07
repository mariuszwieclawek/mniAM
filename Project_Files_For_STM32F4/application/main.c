#undef UNICODE
#define WIN32_LEAN_AND_MEAN
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "stm32f4xx_hal.h"
#include "usart.h"
#include "amcom.h"
#include "amcom_packets.h"

uint8_t first_iter_food = 1;
uint8_t first_iter_player = 1;

void SysTick_Handler(void){}

/**
 * This function will be called each time a valid AMCOM packet is received
 */
void amcomPacketHandler(const AMCOM_Packet* packet, void* userContext) {
	uint8_t amcomBuf[AMCOM_MAX_PACKET_SIZE];	// buffer used to serialize outgoing packets
	size_t bytesToSend = 0;						// size of the outgoing packet
	static int playerCounter;					// just a counter to distinguish player instances
	

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
		static AMCOM_NewGameRequestPayload newgameRequestPayload;

    newgameRequestPayload.playerNumber = ptr1->playerNumber;
    newgameRequestPayload.numberOfPlayers = ptr1->numberOfPlayers;
    newgameRequestPayload.mapWidth = ptr1->mapWidth;
    newgameRequestPayload.mapHeight = ptr1->mapHeight;

		bytesToSend = AMCOM_Serialize(AMCOM_NEW_GAME_RESPONSE, NULL, 0, amcomBuf);

	  break;
	case AMCOM_PLAYER_UPDATE_REQUEST:
		printf("Got PLAYER_UPDATE.request.\n");
		// TODO: use the received information

		AMCOM_PlayerUpdateRequestPayload * ptr2 = packet->payload;
		static AMCOM_PlayerUpdateRequestPayload playerupdateRequestPayload;

		if(first_iter_player == 1){ // saving all players
			for(int i = 0; i < newgameRequestPayload.numberOfPlayers; i++){
				playerupdateRequestPayload.playerState[i].playerNo = ptr2->playerState[i].playerNo;
				playerupdateRequestPayload.playerState[i].hp = ptr2->playerState[i].hp;
				playerupdateRequestPayload.playerState[i].x = ptr2->playerState[i].x;
				playerupdateRequestPayload.playerState[i].y = ptr2->playerState[i].y;
			}
			first_iter_player = 0;
		}else{ // here only update incoming changes
			for(int i = 0; i < packet->header.length/11; i++){ //the number of iterations is the number of incoming packets
				for(int j = 0; j < newgameRequestPayload.numberOfPlayers; j++){ // the number of iterations is the number of the current amount of players
					if(playerupdateRequestPayload.playerState[j].playerNo == ptr2->playerState[i].playerNo){
						playerupdateRequestPayload.playerState[j].playerNo = ptr2->playerState[i].playerNo;
						playerupdateRequestPayload.playerState[j].hp = ptr2->playerState[i].hp;
						playerupdateRequestPayload.playerState[j].x = ptr2->playerState[i].x;
						playerupdateRequestPayload.playerState[j].y = ptr2->playerState[i].y;
					}
				}
			}
		}
	
	  break;
	case AMCOM_FOOD_UPDATE_REQUEST:
		printf("Got FOOD_UPDATE.request.\n");
		// TODO: use the received information

		AMCOM_FoodUpdateRequestPayload * ptr3 = packet->payload;
		static AMCOM_FoodUpdateRequestPayload foodupdateRequestPayload;
		static int number_of_foods = 0;
		
		if(first_iter_food == 1){ // saving all foods
			number_of_foods = packet->header.length/11;
			for(int i = 0; i < number_of_foods; i++){
				foodupdateRequestPayload.foodState[i].foodNo = ptr3->foodState[i].foodNo;
				foodupdateRequestPayload.foodState[i].state = ptr3->foodState[i].state;
				foodupdateRequestPayload.foodState[i].x = ptr3->foodState[i].x;
				foodupdateRequestPayload.foodState[i].y = ptr3->foodState[i].y;
			}
			first_iter_food = 0;
		}else{ // here only update incoming changes
			for(int i = 0; i < packet->header.length/11; i++){ //the number of iterations is the number of incoming packets
				for(int j = 0; j < number_of_foods; j++){ // the number of iterations is the number of the current amount of food
					if(foodupdateRequestPayload.foodState[j].foodNo == ptr3->foodState[i].foodNo){
						foodupdateRequestPayload.foodState[j].foodNo = ptr3->foodState[i].foodNo;
						foodupdateRequestPayload.foodState[j].state = ptr3->foodState[i].state;
						foodupdateRequestPayload.foodState[j].x = ptr3->foodState[i].x;
						foodupdateRequestPayload.foodState[j].y = ptr3->foodState[i].y;
					}
				}
			}
		}

		break;
	case AMCOM_MOVE_REQUEST:
		printf("Got MOVE.request.\n");
		// TODO: respond with MOVE.confirmation

		AMCOM_MoveRequestPayload * ptr4 = packet->payload;
		static AMCOM_MoveRequestPayload moveRequestPayload;
		static AMCOM_MoveResponsePayload moveResponsePayload;

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
		bytesToSend = AMCOM_Serialize(AMCOM_MOVE_RESPONSE, &moveResponsePayload, sizeof(moveResponsePayload), amcomBuf);

		break;
	}

	if (bytesToSend > 0) {
		size_t bytesSent = USART_WriteData((const char*)amcomBuf,bytesToSend);
	}
}


int main(int argc, char **argv) {
	HAL_Init();
	USART_Init();

	static char buf[512];						// buffer for temporary data
	size_t receivedBytesCount;				// holds the number of bytes received via socket

	static AMCOM_Receiver amcomReceiver;		// AMCOM receiver structure
	// Initialize AMCOM receiver
	AMCOM_InitReceiver(&amcomReceiver, amcomPacketHandler, NULL);

	// Receive data from socket until the peer shuts down the connection
	while(1){
		receivedBytesCount = USART_ReadData(buf, sizeof(buf));
		if (receivedBytesCount > 0) {
			// Try to deserialize the incoming data
			AMCOM_Deserialize(&amcomReceiver, (const void*)buf, receivedBytesCount);

		}
	}
	
	return 0;
}