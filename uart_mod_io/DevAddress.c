/*************************************************************************
 *
 *
 *    (c) Copyright Olimex 2011
 *
 *    File name   : main.c
 *    Description : main file of the project
 *
 *    History :
 *    1. Date        : 07 November 2011
 *       Author      : Aleksandar Mitev
 *       Description : Create
 *
 **************************************************************************/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include "DevAddress.h"

/* DEFINE LOCAL TYPES HERE */

/* DEFINE LOCAL CONSTANTS HERE */
#define Dev_ADDRESS_EEADDRESS 0x00

// this is the value that has to be shifted to the left!
// this means that you can set addresses from 1 - 127
#define Dev_DEFAULT_ADDRESS 0x58

/* DECLARE EXTERNAL VARIABLES HERE */

/* DEFINE LOCAL MACROS HERE */

/* DEFINE LOCAL VARIABLES HERE */
static uint8_t localDevAddress;

uint8_t ee_DevAddress __attribute__((section(".eeprom"))) = Dev_DEFAULT_ADDRESS;

/* DECLARE EXTERNAL VARIABLES HERE */

/* DECLARE LOCAL FUNCTIONS HERE */

/* DEFINE FUNCTIONS HERE */

/******************************************************************************
* Description: Dev_Address_Initialize(..) - initializes EEPROM and reads the default address
* Input: 	none
* Output: 	none
* Return:	0 if successfully initialized, -1 if error occurred 
*******************************************************************************/
char Dev_Address_Initialize(void)
{
	char result = 0;
	
	localDevAddress = Dev_DEFAULT_ADDRESS;
	
	// read address stored in EEPROM
	eeprom_busy_wait();
	localDevAddress = eeprom_read_byte(Dev_ADDRESS_EEADDRESS);
	
	return result;
}

/******************************************************************************
* Description: Dev_Address_Get(..) - gets the current Dev address of the devide
* Input: 	none
* Output: 	none
* Return:	the current Dev address 
*******************************************************************************/
uint8_t Dev_Address_Get(void)
{
	return localDevAddress;
}

/******************************************************************************
* Description: Dev_Address_Set(..) - sets a new address of the Dev of the device
*		Stores it to internal EEPROM
* Input: 	none
* Output: 	none
* Return:	0 if successfully updated, -1 if error occurred 
*******************************************************************************/
char Dev_Address_Set(uint8_t addr)
{
	char result = 0;

	// update address here	
	eeprom_busy_wait();
	cli();
	eeprom_write_byte(Dev_ADDRESS_EEADDRESS, addr);
	sei();

	// verify that data is correct
	eeprom_busy_wait();
	result = (addr == eeprom_read_byte(Dev_ADDRESS_EEADDRESS)) ? 0 : -1;

	if(!result)
		localDevAddress = addr;
			
	return result;	
}

/******************************************************************************
* Description: Dev_Address_SetDefault(..) - sets a the default Dev address
*		Stores it to internal EEPROM
* Input: 	none
* Output: 	none
* Return:	0 if successfully updated, -1 if error occurred 
*******************************************************************************/
char Dev_Address_SetDefault(void)
{
	return Dev_Address_Set(Dev_DEFAULT_ADDRESS);
}
