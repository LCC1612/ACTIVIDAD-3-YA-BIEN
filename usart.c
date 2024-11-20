
#include <avr/io.h>

//Global variables
volatile char usart_received_char;

/*
* Lee el byte que llega por el receptor
*/
unsigned char usart_receive( void )
{
	while ((UCSR0A & 0x80) == 0x00); // Espera por un dato (RXC=1)
	return UDR0;
}

/*
* Transmite el dato que se pasa como argumento
*/
void usart_transmit( unsigned int data )
{
	while ((UCSR0A & 0x20) == 0x00);  // Espera hasta que el buffer del transmisor este vacio (UDRE=1)
	UDR0 = data; // Cuando el buffer este vacio, coloca el dato en el registro UDR
}

/*
Transmite una cadena de caracteres
*/
void usart_transmit_string( char s[] )
{
	int i = 0;
	while (i < 64)
	{
		if (s[i] == '\0') break;
		usart_transmit(s[i++]);
	}
}

/*
* Inicializa USART
* Entrada: valor de UBRR de las tablas(datasheet)de BAUDRATE
*/
void init_usart(unsigned int baudrate)
{
	//Escoge modo de reloj: UMSEL=0 asicrono, UMSEL=1 sincrono
	UCSR0C &= (~(1<<UMSEL00) & ~(1<<UMSEL01)); // bit UMSEL = 0 asincrono

	//En modo asincrono escoge la velocidad: U2X=0 normal, U2X=1 doble
	UCSR0A = (1<<U2X0); // bit U2X = 1 doble

	//Baudrate ejemplo: fosc = 16 Mhz, U2Xn= 1, BaudRate = 9600, entonces UBRR= 207
	//Ver DataSheet pag.153
	UBRR0H = (unsigned char) (baudrate>>8); // Escribe (MSB) en la parte alta del registro
	UBRR0L = (unsigned char) (baudrate); //Escribe (LSB) el valor en la parte baja

	//Tamano de los datos
	UCSR0C = ((1<<UCSZ00) | (1<<UCSZ01)); //Selecciona registro,8 bits para longitud de datos

	//Habilitar el transmisor y receptor
	UCSR0B = (1<<RXEN0) | (1<<TXEN0);

	//Habilita 2 bits de paro
	//UCSRC = (1<<USBS);


	//Para activar/desactivar las interrupcines, solo quitar/poner el simbolo de comentarios
	UCSR0B |= (1<<RXCIE0);	//Activa interrupciones de "recepcion completada" (RXCIE=1)
	//y "registro de datos vacio" (UDRIE=1)
}