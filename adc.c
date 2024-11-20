#include <avr/io.h>

/************************************************************************
* USO DEL ADC CON INTERRUPCIONES
************************************************************************/

void ADC_WithInterrupt(uint8_t channel) {
	// Habilita el convertidor (encenderlo)
	ADCSRA |= 1 << ADEN;

	// Habilitar el prescaler
	ADCSRA |= 1 << ADPS2 | 1 << ADPS1 | 1 << ADPS0; // Factor de división = 128

	// Resultado justificado a la izquierda
	ADMUX |= 1 << ADLAR;

	// Seleccionar el canal ADC (0 o 1)
	ADMUX &= 0xF0; // Limpiar los bits de MUX
	ADMUX |= channel; // Establecer el canal

	// Seleccionar el voltaje de referencia interno (AVcc)
	ADMUX |= (1 << REFS0); // AVcc con capacitor externo en AREF

	// Habilitar interrupciones del ADC
	ADCSRA |= 1 << ADIE;

	// Comenzar la conversión
	ADCSRA |= 1 << ADSC;
}