/*
 * mynixie.h
 *
 * Created: 2013.07.25. 14:36:41
 *  Author: desLoges
 */ 


#ifndef MYNIXIE_H_
#define MYNIXIE_H_

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <stdbool.h>

extern uint8_t nixie_enable_mask;

#define SELECTOR_PORT 				PORTC
#define SELECTOR_PORT_SYM 			PORTF
#define SELECTOR_DDR 				DDRC
#define SELECTOR_DDR_SYM 			DDRF
#define NUM_PORT 					PORTC
#define NUM_DDR 					DDRC

#define GLOW_DOT 					PINC7
#define GLOW_MINUS 					PINC6
#define NIXIE_1 					PINC3
#define NIXIE_2 					PINC4
#define NIXIE_3 					PINC5
#define NIXIE_4 					PINF6
#define BCD_A 						PINC2
#define BCD_B 						PINC0
#define BCD_C 						PIND7 //!!!!BCD_C is in the PORTD
#define BCD_D 						PINC1

#define GLOW_MINUS_ON 				SELECTOR_PORT |= (1<<GLOW_MINUS)
#define GLOW_MINUS_OFF 				SELECTOR_PORT &= ~ (1<<GLOW_MINUS)
#define GLOW_MINUS_TOGGLE 			SELECTOR_PORT ^= (1<<GLOW_MINUS)

#define GLOW_DOT_ON 				SELECTOR_PORT |= (1<<GLOW_DOT)
#define GLOW_DOT_OFF 				SELECTOR_PORT &= ~ (1<<GLOW_DOT)
#define GLOW_DOT_TOGGLE 			SELECTOR_PORT ^= (1<<GLOW_DOT)

#define NIXIE_1_ENABLE 				SELECTOR_PORT |= (1<<NIXIE_1)
#define NIXIE_1_DISABLE 			SELECTOR_PORT &= ~(1<<NIXIE_1)

#define NIXIE_2_ENABLE 				SELECTOR_PORT |= (1<<NIXIE_2)
#define NIXIE_2_DISABLE 			SELECTOR_PORT &= ~(1<<NIXIE_2)

#define NIXIE_3_ENABLE 				SELECTOR_PORT |= (1<<NIXIE_3)
#define NIXIE_3_DISABLE 			SELECTOR_PORT &= ~(1<<NIXIE_3)

#define NIXIE_4_ENABLE 				SELECTOR_PORT_SYM |= (1<<NIXIE_4)
#define NIXIE_4_DISABLE 			SELECTOR_PORT_SYM &= ~(1<<NIXIE_4)

#define NIXIE_1_ENABLE_FORCED 		SELECTOR_DDR |= (1 << NIXIE_1)
#define NIXIE_1_DISABLE_FORCED		SELECTOR_DDR &= ~ (1 << NIXIE_1)

#define NIXIE_2_ENABLE_FORCED		SELECTOR_DDR |= (1 << NIXIE_2)
#define NIXIE_2_DISABLE_FORCED		SELECTOR_DDR &= ~ (1 << NIXIE_1)

#define NIXIE_3_ENABLE_FORCED		SELECTOR_DDR |= (1 << NIXIE_3)
#define NIXIE_3_DISABLE_FORCED		SELECTOR_DDR &= ~ (1 << NIXIE_1)

#define NIXIE_4_ENABLE_FORCED		SELECTOR_DDR_SYM |= (1 << NIXIE_4)
#define NIXIE_4_DISABLE_FORCED		SELECTOR_DDR_SYM &= ~ (1 << NIXIE_4)


#define NIXIES_ENABLE 				TIMSK |=(1<<TOIE2)
#define NIXIES_DISABLE 				TIMSK &= ~(1<<TOIE2)

#define NIXIE_SUPPLY_A 				PIND5
#define NIXIE_SUPPLY_B 				PIND6

#define NIXIE_CELSIUS 				'7'
#define NIXIE_PERCENT 				'6'

#define NIXIE_LED_RED 				PINB7
#define NIXIE_LED_GREEN 			PINB5
#define NIXIE_LED_BLUE 				PINB6
#define NIXIE_LED_RED_PWM 			OCR1C
#define NIXIE_LED_GREEN_PWM 		OCR1A
#define NIXIE_LED_BLUE_PWM 			OCR1B


volatile uint8_t digit[4];

extern void config_io_nixie(void);
extern void set_nixie_rgb(uint16_t r, uint16_t g, uint16_t b);
extern void nixie_digit(uint8_t nixie, uint8_t value);
extern void all_nixies_on(void);
extern void all_nixies_off(void);
void nixie_print(char* value, bool dot, bool minus);
void nixie_print_value(char* data, char sign, uint8_t minus);
extern void nixie_suppy(uint8_t value);
void nixie_dislpay_fadeout(void);
#endif /* MYNIXIE_H_ */
