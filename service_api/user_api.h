/**
* @file : 	user-api.h
* @author :	El-Fadel Bonfoh
* @date :	03/2019 - 
* @version :	0.1
* @brief : 	The API applications have to used 
*			to transmit and receive data to and from vtl.
*/

#ifndef USER_API_H
#define USER_API_H

/**
* @brief :	Used by applications to send data through vtl.
* @return :	
*
* Implemented in :
* injector/injector.c
*/
void vtl_send(void *msg, int msg_len);

/**
* @brief :	Retrieve user data from vtl.
* @return :	
*/
void vtl_rcv(void *msg);

#endif // end of "define USER_API_H"
