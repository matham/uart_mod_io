/*************************************************************************
 *
 *
 *    (c) Copyright Olimex 2011
 *
 *    File name   : DevAddress.h
 *    Description : gets/sets the Dev address of the device from/to EEPROM
 *
 *    History :
 *    1. Date        : 07 November 2011
 *       Author      : Aleksandar Mitev
 *       Description : Create
 *
 **************************************************************************/

#ifndef DEV_ADDRESS_H
#define DEV_ADDRESS_H

/******************************************************************************
* Description: Dev_Address_Initialize(..) - initializes EEPROM and reads the default address
* Input: 	none
* Output: 	none
* Return:	0 if successfully initialized, -1 if error occurred 
*******************************************************************************/
char Dev_Address_Initialize(void);

/******************************************************************************
* Description: Dev_Address_Get(..) - gets the current Dev address of the devide
* Input: 	none
* Output: 	none
* Return:	the current Dev address 
*******************************************************************************/
uint8_t Dev_Address_Get(void);

/******************************************************************************
* Description: Dev_Address_Set(..) - sets a new address of the Dev of the device
*		Stores it to internal EEPROM
* Input: 	none
* Output: 	none
* Return:	0 if successfully updated, -1 if error occurred 
*******************************************************************************/
char Dev_Address_Set(uint8_t addr);

/******************************************************************************
* Description: Dev_Address_SetDefault(..) - sets a the default Dev address
*		Stores it to internal EEPROM
* Input: 	none
* Output: 	none
* Return:	0 if successfully updated, -1 if error occurred 
*******************************************************************************/
char Dev_Address_SetDefault(void);

#endif // DEV_ADDRESS_H
