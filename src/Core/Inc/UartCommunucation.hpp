#ifndef __UART_COMMUNICATION_PARAMETERS_HPP__
#define __UART_COMMUNICATION_PARAMETERS_HPP__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

#ifdef __cplusplus
}
#endif


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


//////////////////////////////////////
// Collaborator define

#define BUFFER_SIZE			((uint16_t)50)

#define FIRST_SYNC				0x03
#define SECOND_SYNC				0x11

// Collaborator define
//////////////////////////////////////

//////////////////////////////////////
// Satte Machine Define

#define CAPTURE_FIRST_SYNC					0x01
#define CAPTURE_SECOND_SYNC					0x02
#define CAPTURE_PAYLOAD_TYPE				0x03
#define CAPTURE_PAYLOAD_SIZE				0x04
#define CAPTURE_PAYLOAD						0x05
#define CAPTURE_CHECKSUM					0x06

// Satte Machine Define
//////////////////////////////////////

class UART_communucation_parameters_c {

private:


	/////////////////////////////////////
	// Uart Parameters

	UART_HandleTypeDef *rxUart;
	UART_HandleTypeDef *txuart;

	// Uart Parameters
	////////////////////////////////////

	///////////////////////////////////
	// Transmit Message Parameters

	char tranmist_message_buffer_u8[BUFFER_SIZE];

	// Transmit Message Parameters
	///////////////////////////////////

	///////////////////////////////////
	// Recevived Message Parameters

	uint8_t recevice_message_buffer_u8[BUFFER_SIZE];

	// Recevived Message Parameters
	///////////////////////////////////

	//////////////////////////////////
	// State Machine Parameters

	uint8_t package_array_sm_u8[BUFFER_SIZE];
	uint8_t payload_array_sm_u8[BUFFER_SIZE];

	uint8_t data_counter_sm_u8;
	uint8_t capture_status_u8;
	uint8_t calculated_CK_u8;
	uint8_t captured_CK_u8;

	uint8_t payload_type_sm_u8;
	uint8_t payload_size_sm_u8;

	enum package_type : uint8_t {

		Package_A = 0x0A, Package_B = 0x0B, Package_C = 0x0C

	};

	// State Machine Parameters
	/////////////////////////////////

public:

	UART_communucation_parameters_c(UART_HandleTypeDef *__rxUart, UART_HandleTypeDef *__txUart) {

		////////////////////////////////////
		// Set Uart Parameters

		this->rxUart = __rxUart;
		this->txuart = __txUart;

		// Set Uart Parameters
		///////////////////////////////////

		/////////////////////////////////////
		// Clear buffers

		memset(this->recevice_message_buffer_u8, 0, BUFFER_SIZE);
		memset(this->tranmist_message_buffer_u8, 0, BUFFER_SIZE);

		// Clear buffers
		/////////////////////////////////////


		////////////////////////////////////////////////////
		// State Machine Parameters Init

		memset(this->package_array_sm_u8, 0, BUFFER_SIZE);
		memset(this->payload_array_sm_u8, 0, BUFFER_SIZE);

		this->data_counter_sm_u8 = 0;
		this->calculated_CK_u8 = 0;
		this->captured_CK_u8 = 0;
		this->capture_status_u8 = CAPTURE_FIRST_SYNC;
		this->payload_type_sm_u8 = 0;
		this->payload_size_sm_u8 = 0;

		// State Machine Parameters Init
		///////////////////////////////////////////////////

	}

	///////////////////////////////////////////////////////
	// HAL UART Receive via DMA

	void start_recieve_message_via_DMA() {

		HAL_UART_Receive_DMA(this->rxUart, this->recevice_message_buffer_u8,
		BUFFER_SIZE);

	}

	// HAL UART Receive via DMA
	////////////////////////////////////////////////////////

	////////////////////////////////////////////////////////
	// Parse Received Message with State Machine Algorithm

	void parse_received_message() {

		for (int i = 0; i < BUFFER_SIZE; i++) {

			switch (this->capture_status_u8) {

			case CAPTURE_FIRST_SYNC: {

				if (this->recevice_message_buffer_u8[i] == FIRST_SYNC) {

					this->package_array_sm_u8[this->data_counter_sm_u8] =
							this->recevice_message_buffer_u8[i];

					this->data_counter_sm_u8++;
					this->capture_status_u8 = CAPTURE_SECOND_SYNC;

				}

				else {

					this->data_counter_sm_u8 = 0;
					this->capture_status_u8 = CAPTURE_FIRST_SYNC;

				}

				break;
			}

			case CAPTURE_SECOND_SYNC: {

				if (this->recevice_message_buffer_u8[i] == SECOND_SYNC) {

					this->package_array_sm_u8[this->data_counter_sm_u8] =
							this->recevice_message_buffer_u8[i];

					this->data_counter_sm_u8++;
					this->capture_status_u8 = CAPTURE_PAYLOAD_TYPE;

				}

				else {

					this->data_counter_sm_u8 = 0;
					this->capture_status_u8 = CAPTURE_FIRST_SYNC;

				}

				break;
			}

			case CAPTURE_PAYLOAD_TYPE: {

				this->package_array_sm_u8[this->data_counter_sm_u8] =
						this->recevice_message_buffer_u8[i];

				this->payload_type_sm_u8 =
						this->package_array_sm_u8[this->data_counter_sm_u8];
				this->data_counter_sm_u8++;
				this->capture_status_u8 = CAPTURE_PAYLOAD_SIZE;

				break;
			}

			case CAPTURE_PAYLOAD_SIZE: {

				this->package_array_sm_u8[this->data_counter_sm_u8] =
						this->recevice_message_buffer_u8[i];

				this->payload_size_sm_u8 =
						this->package_array_sm_u8[this->data_counter_sm_u8];
				this->data_counter_sm_u8++;
				this->capture_status_u8 = CAPTURE_PAYLOAD;

				break;
			}

			case CAPTURE_PAYLOAD: {

				this->package_array_sm_u8[this->data_counter_sm_u8] =
						this->recevice_message_buffer_u8[i];

				this->data_counter_sm_u8++;

				if (this->data_counter_sm_u8
						>= (this->payload_size_sm_u8 + 4)) {

					this->data_counter_sm_u8 = 0;
					this->capture_status_u8 = CAPTURE_CHECKSUM;
				}

				break;
			}

			case CAPTURE_CHECKSUM: {

				this->captured_CK_u8 = this->recevice_message_buffer_u8[i];
				this->calculate_check_sum(this->package_array_sm_u8,
						(this->payload_size_sm_u8 + 4));

				if (this->captured_CK_u8 == this->calculated_CK_u8) {

					this->recevice_message_buffer_u8[i] = 0;
					memcpy(this->payload_array_sm_u8,
							(this->package_array_sm_u8 + 4),
							this->payload_size_sm_u8);

					this->execute_package();

				}

				else {

					this->payload_size_sm_u8 = 0;
					this->payload_type_sm_u8 = 0;

				}

				memset(this->package_array_sm_u8, 0, BUFFER_SIZE);
				memset(this->payload_array_sm_u8,0,BUFFER_SIZE);
				memset(this->tranmist_message_buffer_u8,0,BUFFER_SIZE);

				this->capture_status_u8 = CAPTURE_FIRST_SYNC;
				break;
			}

			default:
				this->capture_status_u8 = CAPTURE_FIRST_SYNC;
				break;

			}

		}

	}

	// Parse Received Message with State Machine Algorithm
	//////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////
	// Execute Package

	void execute_package() {

		switch (this->payload_type_sm_u8) {

		case Package_A:
			sprintf(this->tranmist_message_buffer_u8,
					"Received Package A, Payload: %u %u\r\n",
					this->payload_array_sm_u8[0], this->payload_array_sm_u8[1]);
			break;

		case Package_B:
			sprintf(this->tranmist_message_buffer_u8,
					"Received Package B, Payload: %u %u %u\r\n",
					this->payload_array_sm_u8[0], this->payload_array_sm_u8[1],
					this->payload_array_sm_u8[2]);
			break;

		case Package_C:
			sprintf(this->tranmist_message_buffer_u8,
					"Received Package C, Payload: %u %u %u %u\r\n",
					this->payload_array_sm_u8[0], this->payload_array_sm_u8[1],
					this->payload_array_sm_u8[2], this->payload_array_sm_u8[3]);
			break;

		default:
			break;

		}

		HAL_UART_Transmit(this->txuart, (uint8_t*) this->tranmist_message_buffer_u8,
				strlen(this->tranmist_message_buffer_u8), HAL_MAX_DELAY);

	}

	// Execute Package
	///////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////
	// Other collaborator function

	void calculate_check_sum(uint8_t *data_array, uint8_t size) {

		this->calculated_CK_u8 = 0;

		for (int i = 2; i < size; i++) {

			this->calculated_CK_u8 += data_array[i];

			this->calculated_CK_u8 = this->calculated_CK_u8 & 0xFF;

		}

	}

	// Other collaborator function
	//////////////////////////////////////////////////////////

};

typedef UART_communucation_parameters_c *UART_communucation_parameters_c_ptr;

// classes
/////////////////////////////////////////////////////////////////

#endif
