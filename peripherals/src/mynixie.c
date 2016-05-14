/*
 * mynixie.c
 *
 * Created: 2013.07.25. 14:36:55
 *  Author: desLoges
 */
#include "mynixie.h"
#include <stdbool.h>
#include <avr/pgmspace.h>

void config_io_nixie() {
	SELECTOR_DDR |= (1 << GLOW_MINUS) | (1 << GLOW_DOT) | (1 << NIXIE_1)
			| (1 << NIXIE_2) | (1 << NIXIE_3);
	SELECTOR_DDR_SYM |= (1 << NIXIE_4);
	NUM_DDR |= (1 << BCD_A) | (1 << BCD_B) | (1 << BCD_D);
	DDRD |= (1 << BCD_C);
}

const uint16_t pwmtable[256] PROGMEM = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x02,
		0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x04, 0x04,
		0x04, 0x04, 0x04, 0x05, 0x05, 0x05, 0x05, 0x06, 0x06, 0x06, 0x07, 0x07,
		0x07, 0x08, 0x08, 0x08, 0x09, 0x09, 0x0A, 0x0A, 0x0B, 0x0B, 0x0C, 0x0C,
		0x0D, 0x0D, 0x0E, 0x0F, 0x0F, 0x10, 0x11, 0x11, 0x12, 0x13, 0x14, 0x15,
		0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1F, 0x20, 0x21, 0x23,
		0x24, 0x26, 0x27, 0x29, 0x2B, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38,
		0x3A, 0x3C, 0x3E, 0x40, 0x43, 0x45, 0x47, 0x4A, 0x4C, 0x4F, 0x51, 0x54,
		0x57, 0x59, 0x5C, 0x5F, 0x62, 0x64, 0x67, 0x6A, 0x6D, 0x70, 0x73, 0x76,
		0x79, 0x7C, 0x7F, 0x82, 0x85, 0x88, 0x8B, 0x8E, 0x91, 0x94, 0x97, 0x9A,
		0x9C, 0x9F, 0xA2, 0xA5, 0xA7, 0xAA, 0xAD, 0xAF, 0xB2, 0xB4, 0xB7, 0xB9,
		0xBB, 0xBE, 0xC0, 0xC2, 0xC4, 0xC6, 0xC8, 0xCA, 0xCC, 0xCE, 0xD0, 0xD2,
		0xD3, 0xD5, 0xD7, 0xD8, 0xDA, 0xDB, 0xDD, 0xDE, 0xDF, 0xE1, 0xE2, 0xE3,
		0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xED, 0xEE,
		0xEF, 0xEF, 0xF0, 0xF1, 0xF1, 0xF2, 0xF2, 0xF3, 0xF3, 0xF4, 0xF4, 0xF5,
		0xF5, 0xF6, 0xF6, 0xF6, 0xF7, 0xF7, 0xF7, 0xF8, 0xF8, 0xF8, 0xF9, 0xF9,
		0xF9, 0xF9, 0xFA, 0xFA, 0xFA, 0xFA, 0xFA, 0xFB, 0xFB, 0xFB, 0xFB, 0xFB,
		0xFB, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFC, 0xFD, 0xFD,
		0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD, 0xFD,
		0xFD, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF };

///------------nixie_led_driving------------------------------------------
void set_nixie_rgb(uint16_t r, uint16_t g, uint16_t b) {
	NIXIE_LED_RED_PWM = r;
	NIXIE_LED_GREEN_PWM = g;
	NIXIE_LED_BLUE_PWM = b;
}

void nixie_digit(uint8_t nixie, uint8_t value) {
	switch (nixie) {
	case 0:
		if (nixie_enable_mask & (1 << 3)) {
			NIXIE_1_ENABLE;
			NIXIE_2_DISABLE;
			NIXIE_3_DISABLE;
			NIXIE_4_DISABLE;
		} else {
			NIXIE_1_DISABLE;
			NIXIE_2_DISABLE;
			NIXIE_3_DISABLE;
			NIXIE_3_DISABLE;
		}
		break;

	case 1:
		if (nixie_enable_mask & (1 << 2)) {
			NIXIE_1_DISABLE;
			NIXIE_2_ENABLE;
			NIXIE_3_DISABLE;
			NIXIE_4_DISABLE;
		} else {
			NIXIE_1_DISABLE;
			NIXIE_2_DISABLE;
			NIXIE_3_DISABLE;
			NIXIE_3_DISABLE;
		}
		break;

	case 2:
		if (nixie_enable_mask & (1 << 1)) {
			NIXIE_1_DISABLE;
			NIXIE_2_DISABLE;
			NIXIE_3_ENABLE;
			NIXIE_4_DISABLE;
		} else {
			NIXIE_1_DISABLE;
			NIXIE_2_DISABLE;
			NIXIE_3_DISABLE;
			NIXIE_3_DISABLE;
		}
		break;

	case 3:
		if (nixie_enable_mask & (1 << 0)) {
			NIXIE_1_DISABLE;
			NIXIE_2_DISABLE;
			NIXIE_3_DISABLE;
			NIXIE_4_ENABLE;
		} else {
			NIXIE_1_DISABLE;
			NIXIE_2_DISABLE;
			NIXIE_3_DISABLE;
			NIXIE_3_DISABLE;
		}
		break;

	case 4:
		NIXIE_1_DISABLE;
		NIXIE_2_DISABLE;
		NIXIE_3_DISABLE;
		NIXIE_3_DISABLE;
		break;
	}

	if (nixie != 4) {
		switch (value) {
		case 0:
			NUM_PORT &= ~((1 << BCD_A) | (1 << BCD_B) | (1 << BCD_D));
			PORTD &= ~(1 << BCD_C);
			break;
		case 1:
			NUM_PORT &= ~((1 << BCD_B) | (1 << BCD_D));
			NUM_PORT |= (1 << BCD_A);
			PORTD &= ~(1 << BCD_C);
			break;
		case 2:
			NUM_PORT &= ~((1 << BCD_A) | (1 << BCD_D));
			NUM_PORT |= (1 << BCD_B);
			PORTD &= ~(1 << BCD_C);
			break;
		case 3:
			NUM_PORT &= ~(1 << BCD_D);
			NUM_PORT |= (1 << BCD_A) | (1 << BCD_B);
			PORTD &= ~(1 << BCD_C);
			break;
		case 4:
			NUM_PORT &= ~((1 << BCD_A) | (1 << BCD_B) | (1 << BCD_D));
			PORTD |= (1 << BCD_C);
			break;
		case 5:
			NUM_PORT &= ~((1 << BCD_B) | (1 << BCD_D));
			NUM_PORT |= (1 << BCD_A);
			PORTD |= (1 << BCD_C);
			break;
		case 6:
			NUM_PORT &= ~((1 << BCD_A) | (1 << BCD_D));
			NUM_PORT |= (1 << BCD_B);
			PORTD |= (1 << BCD_C);
			break;
		case 7:
			NUM_PORT &= ~(1 << BCD_D);
			NUM_PORT |= (1 << BCD_A) | (1 << BCD_B);
			PORTD |= (1 << BCD_C);
			break;
		case 8:
			NUM_PORT &= ~((1 << BCD_B) | (1 << BCD_A));
			NUM_PORT |= (1 << BCD_D);
			PORTD &= ~(1 << BCD_C);
			break;
		case 9:
			NUM_PORT &= ~(1 << BCD_B);
			NUM_PORT |= (1 << BCD_A) | (1 << BCD_D);
			PORTD &= ~(1 << BCD_C);
			break;
		}
	}
}

void all_nixies_on(void) {
	NIXIE_1_ENABLE;
	NIXIE_2_ENABLE;
	NIXIE_3_ENABLE;
	NIXIE_4_ENABLE;
	NIXIES_ENABLE;
	GLOW_DOT_ON;
	GLOW_MINUS_ON;
}

void all_nixies_off(void) {
	NIXIE_1_DISABLE;
	NIXIE_2_DISABLE;
	NIXIE_3_DISABLE;
	NIXIE_4_DISABLE;
	NIXIES_DISABLE;
	GLOW_DOT_OFF;
	GLOW_MINUS_OFF;
}

void nixie_print(char* value, bool dot, bool minus) {
	digit[0] = (uint8_t) value[0] - '0';
	digit[1] = (uint8_t) value[1] - '0';
	digit[2] = (uint8_t) value[2] - '0';
	digit[3] = (uint8_t) value[3] - '0';

	if (dot)
		GLOW_DOT_ON;
	else
		GLOW_DOT_OFF;

	if (minus)
		GLOW_MINUS_ON;
	else
		GLOW_MINUS_OFF;
}

void nixie_suppy(uint8_t value) {
	switch (value) {
	case 0:
		PORTD |= (1 << NIXIE_SUPPLY_A) | (1 << NIXIE_SUPPLY_B);
		break;

	case 1:
		PORTD |= (1 << NIXIE_SUPPLY_A);
		PORTD &= ~(1 << NIXIE_SUPPLY_B);
		break;

	case 2:
		PORTD &= ~(1 << NIXIE_SUPPLY_A);
		PORTD |= (1 << NIXIE_SUPPLY_B);
		break;

	case 3:
		PORTD &= ~((1 << NIXIE_SUPPLY_A) | (1 << NIXIE_SUPPLY_B));
		break;

	default:
		break;
	}
}

void nixie_print_value(char* data, char sign, uint8_t minus) {
	bool isminus;

	float tmp = atof(data);

	if (tmp > 10) {

		//if (data[0] == 32)
		//	data[0] = '0';
		data[2] = data[3];
		data[3] = sign;
		data[4] = '\0';
		nixie_enable_mask = 0b0001111;
	} else {
		if (data[0] == 32)
			data[0] = '0';
		//data[1] = data[0];
		//data[0] = '0';
		data[2] = data[3];
		//data[3] = sign;
		data[3] = sign;
		data[4] = '\0';
		nixie_enable_mask = 0b0000111;
	}

	if (minus > 0)
		isminus = true;
	else
		isminus = false;

	nixie_print(data, true, isminus);
}

void nixie_display_change(uint8_t val) {
	GLOW_MINUS_OFF;
	GLOW_DOT_OFF;
	uint8_t r = NIXIE_LED_RED_PWM;
	uint8_t g = NIXIE_LED_GREEN_PWM;
	uint8_t b = NIXIE_LED_BLUE_PWM;

}

void nixie_dislpay_fadeout(void) {
	GLOW_MINUS_OFF;
	GLOW_DOT_OFF;
	uint8_t r = NIXIE_LED_RED_PWM;
	uint8_t g = NIXIE_LED_GREEN_PWM;
	uint8_t b = NIXIE_LED_BLUE_PWM;
//	uint8_t a[] = { 1, 2, 3, 6, 7, 9, 0, 1, 2, 3, 6, 7, 9, 0, 1, 2, 3, 6, 7, 9,
//			0, 1, 2, 3, 6, 7, 9 };

	nixie_enable_mask = 0b00001111;

	for (uint8_t i = 255; i > 0; i--) {
		if (r > 0)
			r--;
		if (g > 0)
			g--;
		if (b > 0)
			b--;
		set_nixie_rgb(r, g, b);
		_delay_ms(4);

		if (i % 25 == 0) {
			digit[0] = rand() % 9;
			digit[1] = rand() % 9;
			digit[2] = rand() % 9;
			digit[3] = rand() % 9;
			//GLOW_MINUS_TOGGLE;
			//GLOW_DOT_TOGGLE;
		}
	}

	set_nixie_rgb(0, 0, 0);
	nixie_enable_mask = 0b00000000;
	GLOW_MINUS_OFF;
	GLOW_DOT_OFF;

	_delay_ms(10);
}
/*
 void nixie_disp(uint16_t value)
 {
 uint8_t digit[3];
 for(uint8_t i=3; i>0; i--){
 digit[i] = value%10;
 value = value/10;
 }

 for (uint8_t i=1; i<4; i++)
 {
 nixie_digit(i,digit[i]);
 _delay_ms(MULTIPLEX_DELAY);
 }
 }
 */
/*
 void nixie_welcome(void){
 uint16_t r_num = rand()%999;
 uint8_t k = 0;
 GLOW_MINUS_ON;
 for(uint8_t i = 0; i<=WELCOME_COUNT; i++)
 {
 if(k == WELCOME_DELAY)
 {
 r_num = rand()%999;
 k=0;
 GLOW_MINUS_TOGGLE;
 GLOW_DOT_TOGGLE;
 }
 nixie_disp(r_num);
 k++;
 }

 }
 */

//for(int k=1; k<4; k++)
//{
//for(int l=0; l<10; l++)
//{
//nixie_digit(k,l);
//_delay_ms(300);
//}
//}
