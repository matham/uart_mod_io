/*************************************************************************
 *
 *
 *    File name   : UART_FSM.h
 *    Description : runs the UART
 *
 **************************************************************************/
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include "BSP.h"
#include "DigitalINs.h"
#include "AnalogINs.h"
#include "DigitalOUTs.h"
#include "DevAddress.h"
#include "UART_FSM.h"

/* DEFINE LOCAL TYPES HERE */
typedef enum _UART_FSM_STATES {
	UART_FSM_IDLE = 0,
	UART_FSM_WAIT_COMMAND,
	UART_FSM_WAIT_DOUTS,
	UART_FSM_WAIT_ADDRUPDATE,
	UART_FSM_WRITING,
	UART_FSM_READING,
} UART_FSM_STATES;

typedef enum _UART_COMMAND_CODES {
	UART_NO_COMMAND = 0x00,
	UART_SET_OUTPUTS = 0x10,
	UART_GET_DINPUTS = 0x20,
	UART_GET_AIN_0 = 0x30,
	UART_GET_AIN_1,
	UART_GET_AIN_2,
	UART_GET_AIN_3,
	UART_SET_SLAVE_ADDR = 0x40
} UART_COMMAND_CODES;

/* DEFINE LOCAL CONSTANTS HERE */
#define TX_BUF_LENGTH 2
#define UCSRB_VAL ((1 << RXCIE) | (1 << RXEN))
#define ADDRESS_FLAG 0b10000000

/* DECLARE EXTERNAL VARIABLES HERE */

/* DEFINE LOCAL MACROS HERE */

/* DEFINE LOCAL VARIABLES HERE */
static UART_FSM_STATES uart_state;
static UART_COMMAND_CODES uart_command;
static uint32_t addressUpdatedIndTimeout;
static uint8_t txBuf[TX_BUF_LENGTH];
static uint8_t devAddress;
static uint8_t bufIndex;

/* DECLARE EXTERNAL VARIABLES HERE */

/* DECLARE LOCAL FUNCTIONS HERE */

/* DEFINE FUNCTIONS HERE */

/******************************************************************************
* Description: UART_FSM_Initialize(..) - initializes UART interface and FSM
* Input: 	none
* Output: 	none
* Return:	0 if successfully initialized, -1 if error occurred 
*******************************************************************************/
char UART_FSM_Initialize(void)
{
	// configure the UART module
	UCSRA = 0;
	// 8MHz crystal, 19.2k baud
	UBRRL = 25;
	// async, no parity, 1 stop bit, 8 bit
	UCSRC = (1 << URSEL) | (1 << UCSZ0) | (1 << UCSZ1);
	// keep transmitter disabled for now. Disable transmit empty interrupt
	UCSRB = UCSRB_VAL;
	devAddress = Dev_Address_Get();
	
	uart_command = UART_NO_COMMAND;
	uart_state = UART_FSM_IDLE;

	addressUpdatedIndTimeout = 0;
	
	return 0;
}

/******************************************************************************
* Description: UART_FSM_Refresh(..) - call periodically to run FSM
* Input: 	none
* Output: 	none
* Return:	none
*******************************************************************************/
void UART_FSM_Refresh(void)
{
	uint16_t temp;
	// do this in main "thread"
	if(uart_state == UART_FSM_READING) {
		// all incoming data are ignored while we are reading
		// read data
		switch(uart_command) {
			case UART_GET_DINPUTS:
				txBuf[0] = DINs_Get();
				break;
			case UART_GET_AIN_0:
			case UART_GET_AIN_1:
			case UART_GET_AIN_2:
			case UART_GET_AIN_3:
				temp = AINs_Get(uart_command - UART_GET_AIN_0);
				txBuf[0] = temp & 0xFF;
				txBuf[1] = (temp >> 8) & 0xFF;
				bufIndex = 1;
				break;
			default:
				break;
		}
		// write the first data
		switch(uart_command) {
			case UART_GET_DINPUTS:
			case UART_GET_AIN_0:
			case UART_GET_AIN_1:
			case UART_GET_AIN_2:
			case UART_GET_AIN_3:
				uart_state = UART_FSM_WRITING;
				// enable, write, and enable interrupt
				UCSRB = UCSRB_VAL | (1 << TXEN);
				UDR = txBuf[0];
				UCSRB = UCSRB_VAL | (1 << TXEN) | (1 << UDRIE);
				break;
			default:
				break;
		}
	}

	// lit LED constantly for the period of timeout
	if(addressUpdatedIndTimeout) {
		addressUpdatedIndTimeout--;
		SetLED(1);
	}
}


ISR(USART_RXC_vect)
{
	uint8_t data = UDR;

	switch(uart_state) {
		case UART_FSM_IDLE:
			if (data & ADDRESS_FLAG && ((data & ~ADDRESS_FLAG) == devAddress)) {
				uart_state = UART_FSM_WAIT_COMMAND;
			}
			break;
		case UART_FSM_WAIT_COMMAND:
			// save command
			uart_command = data;
			switch(uart_command) {
				case UART_SET_OUTPUTS:
					// wait to read data to write
					uart_state = UART_FSM_WAIT_DOUTS;
					break;
				case UART_SET_SLAVE_ADDR:
					// wait to get new address
					uart_state = UART_FSM_WAIT_ADDRUPDATE;
					break;
				case UART_GET_DINPUTS:
				case UART_GET_AIN_0:
				case UART_GET_AIN_1:
				case UART_GET_AIN_2:
				case UART_GET_AIN_3:
					// read data in main thread
					uart_state = UART_FSM_READING;
					break;
				default:
					// dunno what happened, just go back to waiting
					uart_state = UART_FSM_IDLE;
					break;
			}

			break;
		case UART_FSM_WAIT_DOUTS:
			DOUTs_Set(data);
			// back to normal
			uart_state = UART_FSM_IDLE;
			break;
		case UART_FSM_WAIT_ADDRUPDATE:
			// only update address if button is pressed
			if( BtnPressed() ) {
				if( Dev_Address_Set(data) == 0) {
					// immediately update own address
					devAddress = Dev_Address_Get();
					addressUpdatedIndTimeout = 500000;
				}
			}
			// back to normal
			uart_state = UART_FSM_IDLE;
			break;
		case UART_FSM_READING:
		case UART_FSM_WRITING:
			// ignore any incoming data while handling read request in main "thread"
		default:
			break;
	}
	
}


ISR(USART_UDRE_vect){
	switch(uart_state){
		case UART_FSM_WRITING:
			switch(uart_command) {
				case UART_GET_DINPUTS:
					// done so disable tx and back to waiting
					UCSRB = UCSRB_VAL;
					uart_state = UART_FSM_IDLE;
					break;
				case UART_GET_AIN_0:
				case UART_GET_AIN_1:
				case UART_GET_AIN_2:
				case UART_GET_AIN_3:
					if (bufIndex < TX_BUF_LENGTH) {
						// write next byte
						UDR = txBuf[bufIndex];
						bufIndex++;
					} else {
						// done so disable tx and back to waiting
						UCSRB = UCSRB_VAL;
						uart_state = UART_FSM_IDLE;
					}
					break;
				default:
					break;
			}
			break;
		default:
			// should not happen but disable tx anyway in case
			UCSRB = UCSRB_VAL;
			// back to waiting
			uart_state = UART_FSM_IDLE;
			break;
	}

}
