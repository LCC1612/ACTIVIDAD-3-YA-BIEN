/*
 * usart.h
 *
 * Created: 05/11/2024 06:12:46 p. m.
 *  Author: lilia
 */ 


#ifndef USART_H_
#define USART_H_


void init_usart(unsigned int baudrate);
unsigned char usart_receive( void );
void usart_transmit( unsigned int data );
void usart_transmit_string( char s[] );



#endif /* USART_H_ */