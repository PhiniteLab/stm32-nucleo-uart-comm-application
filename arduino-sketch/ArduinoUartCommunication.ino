
/*
 * Packate Structure : [First Syncron Byte][Second Syncron Byte][Payload Type][Payload Length][Payload][Checksum]
 * 
 * First Syncron Byte : 0x03
 * Second Syncron Byte : 0x11
 * 
 * Payload Type : -> for package A = 0x0A -> length payload for package A = 2 bytes 
 *                -> for package B = 0x0B -> length payload for package B = 3 bytes
 *                -> for package C = 0x0C -> length payload for package C = 4 bytes
 *                 
 *  Checksum : 8 bit integer checksum value
 */


#define BUFFER_SIZE 100

uint8_t transmit_message_buffer_u8[BUFFER_SIZE];
uint8_t transmit_array_length_u8;
String char_from_PC = "";

uint8_t indix_u8 = 0;


void setup(){
  
  Serial1.begin(115200);
  Serial.begin(115200);
  
}

void loop(){

  if (Serial.available() > 0 ) {
    char_from_PC = Serial.readStringUntil('\n');

    if (char_from_PC == "a") {
      
      transmit_array_length_u8 = create_package_A();
      send_package();
      
    } else if (char_from_PC == "b") {
      
      transmit_array_length_u8 = create_package_B();
      send_package();
      
    } else if(char_from_PC == "c"){
      
      transmit_array_length_u8 = create_package_C();
      send_package();
      
    }
    else {
      
      Serial.println("Wrong Command");
      
    }
  }
}


void send_package(){

   Serial1.write(transmit_message_buffer_u8, transmit_array_length_u8 / sizeof(uint8_t));
  for (int i = 0; i < transmit_array_length_u8; i++) {
    Serial.print("0x");
    Serial.print(transmit_message_buffer_u8[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
  
}


uint8_t create_package_C(){
  // Package Length
  uint8_t package_length_u8 = 9;

  // Syncron Bits
  uint8_t first_char_u8 = 0x03;
  uint8_t second_char_u8 = 0x11;

  // Payload Information Bits
  uint8_t payload_type_u8 = 0x0C;
  uint8_t payload_length = 0x04;

  // Assign Syncron Bits
  transmit_message_buffer_u8[0] = first_char_u8;
  transmit_message_buffer_u8[1] = second_char_u8;

  // Assign Payload Information Bits
  transmit_message_buffer_u8[2] = payload_type_u8;
  transmit_message_buffer_u8[3] = payload_length;

  // Assign Payload
  transmit_message_buffer_u8[4] = 0x01;
  transmit_message_buffer_u8[5] = 0x02;
  transmit_message_buffer_u8[6] = 0x03;
  transmit_message_buffer_u8[7] = 0x04;
  
  // Caculate Checksum Value
  uint8_t check_sum_value_u8 = calculeted_checksum(transmit_message_buffer_u8, 8);

  // Assign Checksum Value
  transmit_message_buffer_u8[8] = check_sum_value_u8;

  // Return total package length
  return package_length_u8;
  
}

uint8_t create_package_B(){
  // Package Length
  uint8_t package_length_u8 = 8;

  // Syncron Bits
  uint8_t first_char_u8 = 0x03;
  uint8_t second_char_u8 = 0x11;

  // Payload Information Bits
  uint8_t payload_type_u8 = 0x0B;
  uint8_t payload_length = 0x03;

  // Assign Syncron Bits
  transmit_message_buffer_u8[0] = first_char_u8;
  transmit_message_buffer_u8[1] = second_char_u8;

  // Assign Payload Information Bits
  transmit_message_buffer_u8[2] = payload_type_u8;
  transmit_message_buffer_u8[3] = payload_length;

  // Assign Payload
  transmit_message_buffer_u8[4] = 0x01;
  transmit_message_buffer_u8[5] = 0x02;
  transmit_message_buffer_u8[6] = 0x03;
  
  // Caculate Checksum Value
  uint8_t check_sum_value_u8 = calculeted_checksum(transmit_message_buffer_u8, 7);

  // Assign Checksum Value
  transmit_message_buffer_u8[7] = check_sum_value_u8;

  // Return total package length
  return package_length_u8;
  
}
uint8_t create_package_A(){

  // Package Length
  uint8_t package_length_u8 = 7;

  // Syncron Bits
  uint8_t first_char_u8 = 0x03;
  uint8_t second_char_u8 = 0x11;

  // Payload Information Bits
  uint8_t payload_type_u8 = 0x0A;
  uint8_t payload_length = 0x02;

  // Assign Syncron Bits
  transmit_message_buffer_u8[0] = first_char_u8;
  transmit_message_buffer_u8[1] = second_char_u8;

  // Assign Payload Information Bits
  transmit_message_buffer_u8[2] = payload_type_u8;
  transmit_message_buffer_u8[3] = payload_length;

  // Assign Payload
  transmit_message_buffer_u8[4] = 0x01;
  transmit_message_buffer_u8[5] = 0x02;

  // Caculate Checksum Value
  uint8_t check_sum_value_u8 = calculeted_checksum(transmit_message_buffer_u8, 6);

  // Assign Checksum Value
  transmit_message_buffer_u8[6] = check_sum_value_u8;

  // Return total package length
  return package_length_u8;
  
}

uint8_t calculeted_checksum(uint8_t *data_array , uint8_t size_ar){

    uint8_t calculated_CK_u8 = 0;
  
    for (int i = 2; i < size_ar; i++) {

      calculated_CK_u8 += data_array[i];
      calculated_CK_u8 = calculated_CK_u8 & 0xFF;
     
    }
    return calculated_CK_u8;
}
