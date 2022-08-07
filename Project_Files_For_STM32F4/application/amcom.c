#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "amcom.h"

/// Start of packet character
const uint8_t  AMCOM_SOP         = 0xA1;
const uint16_t AMCOM_INITIAL_CRC = 0xFFFF;

static uint16_t AMCOM_UpdateCRC(uint8_t byte, uint16_t crc)       // Oblicza CRC
{
	byte ^= (uint8_t)(crc & 0x00ff);
	byte ^= (uint8_t)(byte << 4);
	return ((((uint16_t)byte << 8) | (uint8_t)(crc >> 8)) ^ (uint8_t)(byte >> 4) ^ ((uint16_t)byte << 3));
}

void myPacketHandler(const AMCOM_Packet* packet, void* userContext) {
    printf("Otrzymano pakiety AMCOM !\n");
}

 // @param receiver pointer to the AMCOM receiver structure
 // @param packetHandlerCallback callback function that will be called each time a packet is received
 // @param userContext user defined, general purpose context, that will be fed back to the callback function

void AMCOM_InitReceiver(AMCOM_Receiver* receiver, AMCOM_PacketHandler packetHandlerCallback, void* userContext) {
	receiver->packetHandler = packetHandlerCallback;    
	receiver->userContext = userContext;
}

 // @param packetType type of packet
 // @param payload pointer to the payload data or NULL if the packet has no payload
 // @param payloadSize number of bytes in the payload or 0 if the packet has no payload
 // @param destinationBuffer place to store the packet bytes (must be large enough!)
 // @return number of bytes written to the destinationBuffer
 

size_t AMCOM_Serialize(uint8_t packetType, const void* payload, size_t payloadSize, uint8_t* destinationBuffer) {

	uint16_t crc = 0xFFFF;                   // Wartosc startowa 
    crc = AMCOM_UpdateCRC(packetType, crc);  // TYPE
    crc = AMCOM_UpdateCRC(payloadSize, crc); // LENGTH
             
	destinationBuffer[0] = 0xa1;          // Pole SOP = 0xA1
	destinationBuffer[1] = packetType;    // TYPE
	destinationBuffer[2] = payloadSize;   // LENGHT
    
    if(payloadSize != 0){ 
        uint8_t* dane = (uint8_t*)payload;
        
        for(int i = 0; i<payloadSize; i++){
            crc = AMCOM_UpdateCRC(dane[i], crc); 
            destinationBuffer[i+5] = dane[i]; 
        }
    }
    
    destinationBuffer[3] = (crc)&0xff;    
    destinationBuffer[4] = (crc>>8)&0xff;

    if(payloadSize == 0){
	    return 5;
    }
    else{
	    return payloadSize + 5;
    }
}

 // @param receiver pointer to the AMCOM receiver structure
 // @param data incoming data
 // @param dataSize number of bytes in the incoming data
 
void AMCOM_Deserialize(AMCOM_Receiver* receiver, const void* data, size_t dataSize) {
    for(int i=0; i < dataSize; i++){

        if(receiver -> receivedPacketState == AMCOM_PACKET_STATE_EMPTY){
            if(*((uint8_t*)data+i) == 0xA1){
                receiver -> receivedPacket.header.sop = 0xA1;
                receiver -> receivedPacketState = AMCOM_PACKET_STATE_GOT_SOP;
                continue;
            }
        }

        if(receiver -> receivedPacketState == AMCOM_PACKET_STATE_GOT_SOP){
            receiver -> receivedPacket.header.type =*((uint8_t*)data+i);            
            receiver -> receivedPacketState = AMCOM_PACKET_STATE_GOT_TYPE;
            continue;
        }

        if(receiver -> receivedPacketState == AMCOM_PACKET_STATE_GOT_TYPE){
            if(*((uint8_t*)data+i) <= 200 && *((uint8_t*)data+i)>= 0){ 
                receiver -> receivedPacket.header.length = *((uint8_t*)data+i);              
                receiver -> receivedPacketState = AMCOM_PACKET_STATE_GOT_LENGTH;
                continue;
            }
            else{
                receiver -> receivedPacketState = AMCOM_PACKET_STATE_EMPTY;
                continue;
            }
        }

        if(receiver -> receivedPacketState == AMCOM_PACKET_STATE_GOT_LENGTH){
            receiver -> receivedPacket.header.crc =*((uint8_t*)data+i);   
            receiver -> receivedPacketState = AMCOM_PACKET_STATE_GOT_CRC_LO;
            continue;
        }

        if(receiver -> receivedPacketState == AMCOM_PACKET_STATE_GOT_CRC_LO){
            receiver -> receivedPacket.header.crc |= *((uint8_t*)data+i) << 8;   
            if(receiver -> receivedPacket.header.length == 0){ 
                receiver -> receivedPacketState = AMCOM_PACKET_STATE_GOT_WHOLE_PACKET;
            }
            if(receiver -> receivedPacket.header.length > 0){  
                receiver -> receivedPacketState = AMCOM_PACKET_STATE_GETTING_PAYLOAD;
                receiver -> payloadCounter = 0;
                continue;
            }
        }

        if(receiver->receivedPacketState == AMCOM_PACKET_STATE_GETTING_PAYLOAD){
            if(receiver->payloadCounter < receiver->receivedPacket.header.length - 1){
		        receiver->receivedPacket.payload[receiver->payloadCounter] = *((uint8_t*)data+i); 
                receiver->payloadCounter++;
                continue;
            }          
	        receiver->receivedPacket.payload[receiver->payloadCounter] = *((uint8_t*)data+i);
            receiver->receivedPacketState = AMCOM_PACKET_STATE_GOT_WHOLE_PACKET;
        }

        if(receiver->receivedPacketState == AMCOM_PACKET_STATE_GOT_WHOLE_PACKET){
            uint16_t crc = 0xFFFF;
            crc = AMCOM_UpdateCRC(receiver->receivedPacket.header.type, crc);
            crc = AMCOM_UpdateCRC(receiver->receivedPacket.header.length, crc);

            if(receiver->receivedPacket.header.length > 0){
                for(int j = 0; j < receiver->receivedPacket.header.length; j++){
                    crc = AMCOM_UpdateCRC(receiver->receivedPacket.payload[j], crc);
                }
            }
                
            if(crc == receiver->receivedPacket.header.crc){
                receiver->receivedPacketState = AMCOM_PACKET_STATE_EMPTY;                 
		        (*receiver->packetHandler)(&receiver->receivedPacket,receiver->userContext);
            }
        }
    }
}