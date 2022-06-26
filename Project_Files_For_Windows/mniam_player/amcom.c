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
//wskaźnik do funkcji, która będzie wywołana w przypadku gdy nasz odbiornik odbierze cały i prawidłowy paket
	receiver->packetHandler = packetHandlerCallback;    
//wskaźnik ogólnego przeznaczenia, który może nam posłużyć do odróżniania instancji odbiorników
	receiver->userContext = userContext;
}

 // @param packetType type of packet
 // @param payload pointer to the payload data or NULL if the packet has no payload
 // @param payloadSize number of bytes in the payload or 0 if the packet has no payload
 // @param destinationBuffer place to store the packet bytes (must be large enough!)
 // @return number of bytes written to the destinationBuffer
 
// Uzupełniamy pakiet AMCOM do wysłania, czyli 3 pola + obliczanie crc
size_t AMCOM_Serialize(uint8_t packetType, const void* payload, size_t payloadSize, uint8_t* destinationBuffer) {

    //CRC - W naszym protokole wartość CRC będzie obliczana kolejno z pól: TYPE, LENGTH i PAYLOAD (o ile pole to występuje).
	uint16_t crc = 0xFFFF;                   // Wartość startowa 
    	crc = AMCOM_UpdateCRC(packetType, crc);  // TYPE
    	crc = AMCOM_UpdateCRC(payloadSize, crc); // LENGTH
             
    // NAGŁÓWEK
	destinationBuffer[0] = 0xa1;          // Pole SOP = 0xA1
	destinationBuffer[1] = packetType;    // TYPE
	destinationBuffer[2] = payloadSize;   // LENGHT
    
    if(payloadSize != 0){ // Jeśli = 0 to funkcja zwraca samą wielkość nagłówka = 5
        // Dane z payload
        uint8_t* dane = (uint8_t*)payload;
        
        for(int i = 0; i<payloadSize; i++){
            crc = AMCOM_UpdateCRC(dane[i], crc); // CRC z payload
            destinationBuffer[i+5] = dane[i]; // Wypełnia bufor danymi
        }
    }
    
    // NAGŁÓWEK CRC - little endian
    destinationBuffer[3] = (crc)&0xff;     // Najpierw wysyłany młodszy bajt    
    destinationBuffer[4] = (crc>>8)&0xff;  // Potem starszy

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
             if(*((uint8_t*)data+i) == 0xA1){    // 1 bajt == A1, sprawdza po koleji całe dane
                receiver -> receivedPacket.header.sop = 0xA1;
                receiver -> receivedPacketState = AMCOM_PACKET_STATE_GOT_SOP;
                continue;
             }
        }

        if(receiver -> receivedPacketState == AMCOM_PACKET_STATE_GOT_SOP){
            // Jeśli natrafił wcześniej na A1 to napewno kolejnym bajtem będzie TYP
            receiver -> receivedPacket.header.type =*((uint8_t*)data+i);            
            receiver -> receivedPacketState = AMCOM_PACKET_STATE_GOT_TYPE;
            continue;
        }

        if(receiver -> receivedPacketState == AMCOM_PACKET_STATE_GOT_TYPE){
            if(*((uint8_t*)data+i) <= 200 && *((uint8_t*)data+i)>= 0){     // Jeśli zawiera sie w przedziale 0-200
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
             receiver -> receivedPacket.header.crc =*((uint8_t*)data+i);    // 1 Bajt CRC           
             receiver -> receivedPacketState = AMCOM_PACKET_STATE_GOT_CRC_LO;
             continue;
        }

        if(receiver -> receivedPacketState == AMCOM_PACKET_STATE_GOT_CRC_LO){
            receiver -> receivedPacket.header.crc |= *((uint8_t*)data+i) << 8;   // przesunięcie o 8 bitów (1 Bajt)
            if(receiver -> receivedPacket.header.length == 0){   // Jeśli nie ma payloadu to idzie dalej
                receiver -> receivedPacketState = AMCOM_PACKET_STATE_GOT_WHOLE_PACKET;
            }
            if(receiver -> receivedPacket.header.length > 0){    // Jeśli są jakies dane to idzie je odczytywać w petli
                receiver -> receivedPacketState = AMCOM_PACKET_STATE_GETTING_PAYLOAD;
                receiver -> payloadCounter = 0;
                continue;
            }
        }

        if(receiver->receivedPacketState == AMCOM_PACKET_STATE_GETTING_PAYLOAD){
            if(receiver->payloadCounter < receiver->receivedPacket.header.length - 1){
		receiver->receivedPacket.payload[receiver->payloadCounter] = *((uint8_t*)data+i);  // Zapis danych
                receiver->payloadCounter++;
                continue;
            }          
	    receiver->receivedPacket.payload[receiver->payloadCounter] = *((uint8_t*)data+i);
            receiver->receivedPacketState = AMCOM_PACKET_STATE_GOT_WHOLE_PACKET;
        }

        if(receiver->receivedPacketState == AMCOM_PACKET_STATE_GOT_WHOLE_PACKET){
            // Porównanie CRC z nagłówka z CRC obliczonym z danych i nagłówka
             uint16_t crc = 0xFFFF;
             crc = AMCOM_UpdateCRC(receiver->receivedPacket.header.type, crc);
             crc = AMCOM_UpdateCRC(receiver->receivedPacket.header.length, crc);

             if(receiver->receivedPacket.header.length > 0){
                 for(int j = 0; j < receiver->receivedPacket.header.length; j++){
                     crc = AMCOM_UpdateCRC(receiver->receivedPacket.payload[j], crc);
                 }
             }
                
                // Jeśli CRC się zgadza - odpalana funkcja użytkownika
             if(crc == receiver->receivedPacket.header.crc){
                 receiver->receivedPacketState = AMCOM_PACKET_STATE_EMPTY;                 
		 (*receiver->packetHandler)(&receiver->receivedPacket,receiver->userContext);
             }
        }
     }
}