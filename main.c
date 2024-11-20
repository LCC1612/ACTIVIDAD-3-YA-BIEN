// Este codigo simplemente le sirve al compilador, es para que sepa a que
// frecuencia funciona el microcontrolador. Asi, funciones como "sleep()"
// sepan cuanto tiempo esperar. Dependiendo de la frecuencia, cambia la
// definicion de segundo, minutos u horas.
#define F_CPU 16000000UL // 16 MHz. Frecuencia del atmega328p (arduino)

// libreria de entradas y salidas. Vienen definiciones de estas. Para que
// ya no excribas 0x1B23 y mejor escribas UDR0 o ADCL. Digamos que le da
// apodos a los registros
#include <avr/io.h>

// libreria que importa la funcion _delay_ms() que usamos mas abajo, esta
// libreria la usamos para esperar y que no suceda nada por unos segundos
#include <util/delay.h>

// libreria para habilitar las interrupciones. Importa las funciones ISR() y
// sei()
#include <avr/interrupt.h>

// librerias hechas en clase. Importan, por ejemplo, las funciones
// usar_transmit(), etc. Una funciones son las que tienen () al final. Por
// ejemplo: usar_transmit_string().
// Para saber cuales estas importando, puedes ver los archivos.h
#include "usart.h"
#include "adc.h"

// De aqui solo ocupamos la funcion dtostrf(), que es para comvertir un numero
// en texto
#include <stdlib.h>	//Para utilizar las rutinas de conversión a ASCII

// varibles globales. Se pueden usar en cualquier parte del documento
volatile char usart_received_char;

// solo tenemos dos canales, que estan conectados a dos potenciometros
// el canal 0 (pin A0 en el arduino) y el canal 1 (pin A1 en el arduino)
volatile uint8_t adc_channel = 0; // Canal ADC inicial 0

// 0 significa que no esta listo y 1 que si
volatile uint8_t adc_ready = 0; // Flag para indicar que el ADC está listo

// Esta es una interrupcion. Se activa cada vez que revibamos un dato por el
// bluetooth. Lo unico que hace es cambiar el canal (potenciometro), 
// si le enviamos un 0, entonces usa el pin A0 en el arduino. Esto lo hace
// modificando al variable global "adc_channel"
ISR (USART_RX_vect) {
	
	// UDR0 es un registro. En este registro se guardan los datos recividos.
	// lo que hay en UDR0 lo guardamos en la variable global "usar_received_char"
	usart_received_char = UDR0;

	// usamos un "if" para comparar. Si recivimos un 0, avismos al telefono que
	// recibimos un 0 y modificamos la variable global "adc_channel" a 0
	if (usart_received_char == '0') {
		usart_transmit_string("Canal 0:\n");
		adc_channel = 0; // Cambiar a ADC0

		// si recivimos un 1, avisamos al telefono y cambiamos al canal 1, justo
		// como arriba
		} else if (usart_received_char == '1') {
		usart_transmit_string("Canal 1:\n");
		adc_channel = 1; // Cambiar a ADC1
	}

}

// Esta tambien es una interrupcion. Esta se activa cada vez que existe una
// conversion de analogico a digital. Esta se activa muchas veces. Lo unico
// que hace es enviar al telefono los valores una vez que se ha hecho la
// conversion
ISR (ADC_vect) {

	// como tenemos una resolucion de 10 bits, por eso tenemos ADC en dos partes
	// ADCL (L para low) y ADCH (h para high)
	uint8_t LowPart = ADCL;
	uint16_t TenBitResult = ADCH << 2 | LowPart >> 6; // 2^10 = 1024 -> 0 - 1023

	char ascii_value[10]; // variable para almacenar el valor. Se usa mas abajo

	// ejemplo
	// usando una regla de para convertir: TenBitResult = 511
	// 5v            = 1023
	// TenBitResult  = 511
	// tenemos que tenbitresult -> 511 * 5 / 1023 = 2.4975
	float voltage = (TenBitResult * 5.0 / 1023.0); // Calcular el voltaje correspondiente

	// Usamos la funcion dtostrf para convertir de float a string
	// https://www.programmingelectronics.com/dtostrf/
	dtostrf(voltage, 2, 2 , ascii_value);

	// Transmitir el valor de voltaje
	usart_transmit_string(ascii_value);
	// luego se transmite una V
	usart_transmit('V');
	// y al final un salto de linea o como si oprimieras la tecla "enter"
	usart_transmit('\n');

	adc_ready = 1; // Indicar que la conversión ha terminado
}

int main(void) {
	// Esta funcion es de la libreria de las clases. Configura el USART
	init_usart(207); // Baudrate 9600

	// Esta tambien es una libreria de la clase. Esta iniciazilza el ADC en el
	// canal indicado. Como se puede ver entre los parentesis, recibe
	// "adc_channel" y esta es una variable global que esta definida hasta arriba,
	// se modifica dependiendo de la interrupcion ISR(usart_rx_vect) que esta
	// arriba
	// cada vez que se use, va a hacer una conversion
	ADC_WithInterrupt(adc_channel); // Inicializar ADC

	// permite invocar la funciones ISR que estan arriba desde cualquier parte
	sei(); // Habilitar interrupciones globales

	// while(1) es un bucle infinito. todo lo que este dentro {} se va a ejecutar
	// para siempre
	while (1) {
		
		// Condicional "if". Si "adc_ready" es un 1, se ejecuta lo que esta dentro de {}
		if (adc_ready) {

			
			// esto es para detenerlo y que no haga conversiones y espere 2000 milisegundos
			// o 2 segundos
			adc_ready = 0; // Reiniciar el flag. 0 significa que no esta listo
			_delay_ms(2000); // Esperar 2 segundos antes de la próxima lectura
			
			// Acabados esos dos segundo, volvemos a hacer una conversion con el
			// canal especificado en la variable global "adc_channel"
			// Esto se va a repetir desde el while(1), una y otra vez para siempre
			ADC_WithInterrupt(adc_channel); // Iniciar nueva conversión
		}
	}
}
