/*************************************************************************
 *
 *
 *    (c) Copyright Olimex 2011
 *
 *    File name   : UART_FSM.h
 *    Description : implements the UART FSM
 *
 *    History :
 *    1. Date        : 07 November 2011
 *       Author      : Aleksandar Mitev
 *       Description : Create
 *    2. Date        : 13 July 2022
 *       Author      : Matt Einhorn
 *       Description : Updated
 *
 **************************************************************************/

#ifndef UART_FSM_H
#define UART_FSM_H

/******************************************************************************
* Description: UART_FSM_Initialize(..) - initializes UART interface and FSM
* Input: 	none
* Output: 	none
* Return:	0 if successfully initialized, -1 if error occurred 
*******************************************************************************/
char UART_FSM_Initialize(void);

/******************************************************************************
* Description: UART_FSM_Refresh(..) - call periodically to run FSM
* Input: 	none
* Output: 	none
* Return:	none
*******************************************************************************/
void UART_FSM_Refresh(void);

#endif // UART_FSM_H
