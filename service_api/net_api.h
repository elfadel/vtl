/**
* @file : 	net-api.h
* @author :	El-Fadel Bonfoh
* @date :	03/2019 - 
* @version :	0.1
* @brief : 	The API used by vtl to transmit and receive data to and from the network.
*/

#ifndef NET_API_H
#define NET_API_H

// Includes

/**
* @brief :	Retrieve packet from the network.
* @return :
*/
void from_net(struct vtl_pkt *pkt);

/**
* @brief :	Send packet to the network.
* @return :
*/
void to_net(struct vtl_pkt *pkt);

#endif // end of "define NET_API_H
