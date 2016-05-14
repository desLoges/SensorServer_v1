/*
 * hardware.c
 *
 * Created: 2013.06.25. 13:54:25
 *  Author: desLoges
 */

//#define F_CPU 16000000UL
#include "hardware.h"
#include "clock.h"
//#include <varargs.h>
#include <stdarg.h>
#include <stdio.h>

//static Usart_Message_deb_t uartInMessage;
char lcd_rollmsg_array[3][20];

bool process_ext1_data(void) {
	//nrf24l01_writeregister(NRF24L01_REG_STATUS, pipe);

	memcpy(&nrf_message, &bufferin, sizeof(bufferin));
	bool ret;

	//if (nrf_message.msgType == 1) {
	memcpy(&ext_measuredData.temp_minus, &nrf_message.temp_minus,
			sizeof(nrf_message.temp_minus));

	memcpy(&ext_measuredData.ext1_temp, &nrf_message.temperature,
			sizeof(nrf_message.temperature));

	memcpy(&ext_measuredData.ext1_hum, &nrf_message.humidity,
			sizeof(nrf_message.humidity));

	memcpy(&ext_measuredData.ext1_airpressure, &nrf_message.airpressure,
			sizeof(nrf_message.airpressure));
	ext_measuredData.ext1_airpressure[5] = ext_measuredData.ext1_airpressure[4];
	ext_measuredData.ext1_airpressure[4] = '.';
	ext_measuredData.ext1_airpressure[6] = '\0';

	memcpy(&ext_measuredData.ext1_lux, &nrf_message.lux,
			sizeof(nrf_message.lux));

	memcpy(&ext_measuredData.ext1_rain, &nrf_message.rain,
			sizeof(nrf_message.rain));

	memcpy(&ext_measuredData.ext1_batt, &nrf_message.outer_batt,
			sizeof(nrf_message.outer_batt));

	ret = true;
	//} else {
	//	ret = false;
	//}
	return ret;
}

void process_uartin(void) {
	cli();
	wasrx_uart_deb = 0;
	//rx_buffer_deb[deb_i + 1] = '\0';
	//memcpy(&uartInMessage, rx_buffer_deb, sizeof(uartInMessage));

	usart_printf(rx_buffer_deb); //send back received msg

//	switch (uartInMessage.supply_level) {
//	case '0':
//		nixie_suppy(0);
//		usart_printf("MIN");
//		break;
//	case '1':
//		nixie_suppy(1);
//		usart_printf("MID");
//		break;
//	case '2':
//		nixie_suppy(2);
//		usart_printf("HIGH");
//		break;
//	case '3':
//		nixie_suppy(3);
//		usart_printf("MAX");
//		break;
//	default:
//		usart_printf("err");
//		break;
//	}
//	set_nixie_rgb(str2int(uartInMessage.r_led, 3),
//			str2int(uartInMessage.g_led, 3), str2int(uartInMessage.b_led, 3));

	char lead_char = rx_buffer_deb[0];
	char temp[50];

	switch (lead_char) {
	case '#':
		strncpy(temp, rx_buffer_deb + 1, strlen(rx_buffer_deb + 1));
		usart_printf("\n%s", temp);
		USART_SendString_ETH(temp);
		waitForATAnswer("OK", 30);
		//atcom
		break;
	case '@':
		usart_printf("\nint: ");
		//internal
		break;
	default:
		//unknown
		usart_printf("\n???");
		break;
	}

	reset_rxbuffer_deb();
	LED_DEB_OFF;
	sei();
}

//-----------------IO------------------------------------------------
void init_IO() {
	DDRA |= (1 << LED_DEB); //set as output
	DDRE |= (1 << LED_STB); //| (1 << PANEL_LED);
	DDRB |= (1 << NIXIE_LED_RED) | (1 << NIXIE_LED_GREEN)
			| (1 << NIXIE_LED_BLUE) | (1 << DISP_BACKLIGHT);
	DDRD |= (1 << NIXIE_SUPPLY_A) | (1 << NIXIE_SUPPLY_B);
	DDRF |= (1 << WIFI_RESET_PIN) | (1 << LED_RED) | (1 << LED_YELLOW);
	//WIFI_DISABLE;
	//DDRE |= (1 << TOUCH_BUTTON_1_PIN) | (1 << TOUCH_BUTTON_2_PIN);
}

void timers_init(void) {

	TCCR2 |= (1 << CS22);

	ICR3 = 0x0155;
	TCCR3A |= (1 << WGM31) | (1 << WGM30) | (1 << COM3B1);
	TCCR3B |= (1 << WGM32) | (1 << CS32); //|(1<<CS31);

	TCCR1A |= (1 << WGM10) | (1 << COM1A1) | (1 << COM1B1) | (1 << COM1C1);
	TCCR1B |= (1 << WGM12) | (1 << CS11); //|(1<<CS31);

	TCCR0 |= (1 << CS00) | (1 << CS02);
	// initialize counter
	TCNT0 = 0;
}

void lcd_printf(const char *fmt, ...) {

	int num_chars;
	char *lcd_buff;
	num_chars = strlen(fmt) + 1;
	lcd_buff = (char *) malloc(sizeof(char) * num_chars);

	va_list args;
	va_start(args, fmt);
	vsprintf(lcd_buff, fmt, args);
	lcd_puts(lcd_buff);
	va_end(args);
	free(lcd_buff);
}

uint8_t str2int(const char* str, uint8_t len) {
	uint8_t i;
	uint8_t ret = 0;
	for (i = 0; i < len; ++i) {
		ret = ret * 10 + (str[i] - '0');
	}
	return ret;
}

void RemoveSpaces(char* source) {
	char* i = source;
	char* j = source;
	while (*j != 0) {
		*i = *j++;
		if (*i != ' ')
			i++;
	}
	*i = 0;
}

int strinsert(char *dest, const char *src, int offset) {

	char temp[50];

	strncpy(temp, dest + offset, strlen(dest + offset));

	strncpy(dest + offset, src, strlen(src));

	strcat(dest + offset + strlen(src), temp);

	return 0;

}

void display_data_LCD(loc_measuredData_t* l_data, ext_measuredData_t* e_data) {

	lcd_gotoxy(0, 1);
	lcd_printf(LCDOUT_CLRROW);
	lcd_gotoxy(0, 2);
	lcd_printf(LCDOUT_CLRROW);
	lcd_gotoxy(0, 3);
	lcd_printf(LCDOUT_CLRROW);
	//
	// lcd temp
	//
	lcd_gotoxy(0, 1);
	lcd_data(LCD_CH_TEMP_INT);
	lcd_printf(" %s", l_data->loc_temp);
	lcd_data(LCD_CH_CELSIUS);
	lcd_putc('C');

	lcd_gotoxy(0, 2);
	lcd_data(LCD_CH_TEMP_EXT);
	lcd_printf(" %s", e_data->ext1_temp);
	lcd_data(LCD_CH_CELSIUS);
	lcd_putc('C');

	//
	// lcd temp
	//
	lcd_gotoxy(10, 1);
	lcd_data(LCD_CH_HUM_INT);
	lcd_printf(" %s%%", l_data->loc_hum);

	lcd_gotoxy(10, 2);
	lcd_data(LCD_CH_HUM_EXT);
	lcd_printf(" %s%%", e_data->ext1_hum);

	//
	// lcd airpressure
	//
	lcd_gotoxy(0, 3);
	lcd_data(LCD_CH_AIRPRS);
	lcd_printf(" %s", e_data->ext1_airpressure);
	lcd_gotoxy(8, 3);
	lcd_data(LCD_CH_HPA);

	//
	// lcd lux
	//
	lcd_gotoxy(10, 3);
	lcd_data(LCD_CH_LUX);
	if (e_data->ext1_lux[0] == '?')
		lcd_printf(" %s lx", e_data->ext1_lux);
	else
		lcd_printf(" %d lx", atoi(e_data->ext1_lux));

	if (atoi(e_data->ext1_rain) < RAIN_LIMIT) {
		lcd_gotoxy(16, 0);
		lcd_printf("Rain");
	} else {
		lcd_gotoxy(16, 0);
		lcd_printf("   ");
	}

}

void display_service_LCD(loc_measuredData_t* l_data, ext_measuredData_t* e_data) {

	lcd_gotoxy(0, 1);
	lcd_printf(LCDOUT_CLRROW);
	lcd_gotoxy(0, 2);
	lcd_printf(LCDOUT_CLRROW);
	lcd_gotoxy(0, 3);
	lcd_printf(LCDOUT_CLRROW);
	//
	// batt
	//
	lcd_gotoxy(0, 1);
	lcd_printf("Ext Batt:");
	lcd_printf(" %s", e_data->ext1_batt);
	lcd_printf("mV");

	//
	// Wifi error
	//
	lcd_gotoxy(0, 2);
	lcd_printf("WiFi ERR:");
	lcd_printf(" %d", l_data->wifiErr);

	//
	// NRF error
	//
	lcd_gotoxy(0, 3);
	lcd_printf("NRF ERR:");
	lcd_printf(" %d", l_data->NRFErr);

}

void LCD_verticalScroll(char* str) {

	memset(lcd_rollmsg_array[2], 0, 19);
	memcpy(lcd_rollmsg_array[2], lcd_rollmsg_array[1], 20);
	memset(lcd_rollmsg_array[1], 0, 19);
	memcpy(lcd_rollmsg_array[1], lcd_rollmsg_array[0], 20);
	memset(lcd_rollmsg_array[0], 0, 19);
	memcpy(lcd_rollmsg_array[0], str, strlen(str));

	lcd_gotoxy(3, 1);
	lcd_puts(LCDOUT_CLRROW);
	lcd_gotoxy(3, 1);
	lcd_printf("%s", lcd_rollmsg_array[2]);
	lcd_gotoxy(3, 2);
	lcd_puts(LCDOUT_CLRROW);
	lcd_gotoxy(3, 2);
	lcd_printf("%s", lcd_rollmsg_array[1]);
	lcd_gotoxy(3, 3);
	lcd_puts(LCDOUT_CLRROW);
	lcd_gotoxy(3, 3);
	lcd_printf("%s", lcd_rollmsg_array[0]);

	_delay_ms(500);

}

void usart_sendValues(void) {
	usart_printf("\n");
	usart_printf("\nseq: %d", nrf_message.seqNum);
	usart_printf("\ntemp:%s", loc_measuredData.loc_temp);
	usart_printf("\nhum:%s", loc_measuredData.loc_hum);
	usart_printf("\netemp:%s", ext_measuredData.ext1_temp);
	usart_printf("\nehum:%s", ext_measuredData.ext1_hum);
	usart_printf("\nepres:%s", ext_measuredData.ext1_airpressure);
	usart_printf("\nelux:%s", ext_measuredData.ext1_lux);
	usart_printf("\nerain:%d", atoi(ext_measuredData.ext1_rain));
	usart_printf("\nExt_batt: %d", atoi(ext_measuredData.ext1_batt));
	usart_printf("\nLDR: %d", loc_measuredData.ldr);
	//				usart_printf("%s %02d %02d:%02d:%02d",
	//						get_nameOfTheMonth(rtc.month), rtc.date, rtc.hour,
	//						rtc.minute, rtc.second);
}

//---------------ADC------------------------------------------------
void init_ADC(void) {
	// For Aref=5V
	ADMUX |= (1 << REFS0) | (1 << ADLAR);
	//Prescaler div factor =64  8MHZ/64 = 125KHz
	ADCSRA |= (1 << ADPS0) | (1 << ADPS1);
	// ADC Enable
	ADCSRA |= (1 << ADEN);
}

uint16_t Read_ADC(uint8_t channel) {
	ADMUX = (ADMUX & 0b11110000) | channel; // ADC csatorna kivalasztasa
	ADCSRA |= (1 << ADSC); // ADC konverzio elinditasa
	while (ADCSRA & (1 << ADSC))
		; // varas az atalakitasra
	ADCSRA |= (1 << ADSC); // konverzió elindítás
	while (ADCSRA & (1 << ADSC))
		;
	return (ADCH);
}

//--------------wellcome----------------------------------------------
void wellcome(void) {
	NIXIE_1_DISABLE;
	NIXIE_2_DISABLE;
	NIXIE_3_DISABLE;
	NIXIE_4_DISABLE;

	_delay_ms(1000);

	//for(uint16_t k=0; k<256; k++)
	//{
	//nixie_rgb(pgm_read_word( &pwmtable[k]),pgm_read_word( &pwmtable[k]),pgm_read_word( &pwmtable[k]));
	//_delay_ms(10);
	//}
	//for(uint16_t k=255; k>0; k--)
	//{
	//nixie_rgb(255,255,pgm_read_word( &pwmtable[k]));
	//_delay_ms(7);
	//}

	for (uint16_t k = 0; k < 250; k++) {
		if (k < 80)
			set_nixie_rgb(k, k, 0);
		else
			set_nixie_rgb(k, 80, 0);
		_delay_ms(11);
	}

	_delay_ms(1000);
	GLOW_MINUS_ON;
	_delay_ms(300);
	NIXIE_1_ENABLE;
	_delay_ms(300);
	NIXIE_2_ENABLE;
	_delay_ms(300);
	GLOW_DOT_ON;
	_delay_ms(300);
	NIXIE_3_ENABLE;
	_delay_ms(300);
	NIXIE_4_ENABLE;
	_delay_ms(300);

	NIXIES_ENABLE;

	_delay_ms(1000);

	/*nixie_suppy(0);
	 _delay_ms(500);
	 nixie_suppy(1);
	 _delay_ms(500);
	 nixie_suppy(2);
	 _delay_ms(500);
	 nixie_suppy(3);
	 _delay_ms(500);
	 */

	uint8_t del = 51;
//	uint8_t a[] = { 1, 2, 3, 6, 7, 9, 0, 1, 2, 3, 6, 7, 9, 0, 1, 2, 3, 6, 7, 9,
//			0, 1, 2, 3, 6, 7, 9 };

	for (uint8_t k = 0; k < 25; k++) {
		GLOW_MINUS_OFF;
		GLOW_DOT_TOGGLE;
		GLOW_MINUS_TOGGLE;

		digit[0] = rand() % 9;
		digit[1] = rand() % 9;
		digit[2] = rand() % 9;
		digit[3] = rand() % 9;

		set_nixie_rgb(rand() % 255, rand() % 255, rand() % 255);

		for (uint8_t n = del; n > 0; n--) {
			_delay_ms(8);
		}
		del = del - 2;
	}

	NIXIES_DISABLE;
	NIXIE_1_DISABLE;
	NIXIE_2_DISABLE;
	NIXIE_3_DISABLE;
	NIXIE_4_DISABLE;
	GLOW_DOT_OFF;
	GLOW_MINUS_OFF;

	set_nixie_rgb(0, 0, 0);
	_delay_ms(1000);

	//nixie_suppy(2);

}

void testNixies(void) {
	nixie_enable_mask = 0b0001111;
	all_nixies_on();
	NIXIES_ENABLE;
	while (1) {

		for (uint8_t i = 0; i < 10; i++) {
			digit[0] = i;
			digit[1] = i;
			digit[2] = i;
			digit[3] = i;
			usart_printf("\n%d", i);
			_delay_ms(500);
		}

	}
}

//////////////////////////////////////////////////////////////////////////
void custom_chars(void) {
	/* custom character */
	lcd_clrscr();
//int temp
	lcd_command(0x40); //CGRAM
	lcd_data(0b00100);
	lcd_data(0b01100);
	lcd_data(0b00100);
	lcd_data(0b01100);
	lcd_data(0b00100);
	lcd_data(0b01110);
	lcd_data(0b01110);
	lcd_data(0b00100);
//ext temp
	lcd_data(0b11011);
	lcd_data(0b10011);
	lcd_data(0b11011);
	lcd_data(0b10011);
	lcd_data(0b11011);
	lcd_data(0b10001);
	lcd_data(0b10001);
	lcd_data(0b11011);
//int hum
	lcd_data(0b00100);
	lcd_data(0b01110);
	lcd_data(0b01110);
	lcd_data(0b11111);
	lcd_data(0b11111);
	lcd_data(0b11111);
	lcd_data(0b01110);
	lcd_data(0b00100);
//ext hum
	lcd_data(0b11011);
	lcd_data(0b10001);
	lcd_data(0b10001);
	lcd_data(0b00000);
	lcd_data(0b00000);
	lcd_data(0b00000);
	lcd_data(0b10001);
	lcd_data(0b11011);
//celsius
	lcd_data(0b00110);
	lcd_data(0b01001);
	lcd_data(0b01001);
	lcd_data(0b00110);
	lcd_data(0b00000);
	lcd_data(0b00000);
	lcd_data(0b00000);
	lcd_data(0b00000);
//lux
	lcd_data(0b00100);
	lcd_data(0b10101);
	lcd_data(0b01110);
	lcd_data(0b11111);
	lcd_data(0b01110);
	lcd_data(0b10101);
	lcd_data(0b00100);
	lcd_data(0b00000);
//PRS
	lcd_data(0b01110);
	lcd_data(0b00100);
	lcd_data(0b00100);
	lcd_data(0b10101);
	lcd_data(0b11111);
	lcd_data(0b11111);
	lcd_data(0b10001);
	lcd_data(0b11111);

//hpa
	lcd_data(0b10000);
	lcd_data(0b11100);
	lcd_data(0b10100);
	lcd_data(0b00000);
	lcd_data(0b11010);
	lcd_data(0b10101);
	lcd_data(0b11111);
	lcd_data(0b10101);

////lx
//	lcd_data(0b00000);
//	lcd_data(0b10000);
//	lcd_data(0b10000);
//	lcd_data(0b10000);
//	lcd_data(0b11001);
//	lcd_data(0b10110);
//	lcd_data(0b10110);
//	lcd_data(0b11001);

	lcd_command(0x80); //DDRAM
}
//////////////////////////////////////////////////////////////////////////
//
//void wifiEnable(bool e) {
//	if (e) {
//		WIFI_RESET_DIS;
//		PORTE |= (1 << PE0);
//	} else {
//		WIFI_RESET_ACT;
//		PORTE &= ~(1 << PE0);
//	}
//}

bool web_update(bool nrf_act) {
	bool ret = false;

	if (nrf_act) {
		ext_measuredData.ext1_temp[2] = '.'; //workaround
		ext_measuredData.ext1_hum[2] = '.'; //workaround
	}

	RemoveSpaces(ext_measuredData.ext1_temp);
	RemoveSpaces(ext_measuredData.ext1_hum);
	RemoveSpaces(ext_measuredData.ext1_airpressure);
	RemoveSpaces(ext_measuredData.ext1_lux);
	RemoveSpaces(ext_measuredData.ext1_rain);
	RemoveSpaces(ext_measuredData.ext1_batt);

	char cmd[150];
	strcpy(cmd, "AT+CIPSTART=\"TCP\",\"");
	strcat(cmd, THINK_IP);
	strcat(cmd, "\",80");
	sendATCOM(cmd);

	if (waitForATAnswer("OK", 20)) {
		usart_printf("\nWEB CONN");
		strcpy(cmd, THINK_GET);
		strcat(cmd, "&field1=");
		strcat(cmd, loc_measuredData.loc_temp);
		strcat(cmd, "&field2=");
		strcat(cmd, loc_measuredData.loc_hum);
		strcat(cmd, "&field3=");
		strcat(cmd, ext_measuredData.ext1_temp);
		strcat(cmd, "&field4=");
		strcat(cmd, ext_measuredData.ext1_hum);
		strcat(cmd, "&field5=");
		strcat(cmd, ext_measuredData.ext1_airpressure);
		strcat(cmd, "&field6=");
		strcat(cmd, ext_measuredData.ext1_lux);
		strcat(cmd, "&field7=");
		strcat(cmd, ext_measuredData.ext1_rain);
		strcat(cmd, "&field8=");
		strcat(cmd, ext_measuredData.ext1_batt);
		strcat(cmd, "\r\n");

		char lens[5];
		itoa(strlen(cmd), lens, 10);
		char cmd2[20];
		strcpy(cmd2, "AT+CIPSEND=");
		strcat(cmd2, lens);

		sendATCOM(cmd2);

		if (waitForATAnswer(">", 20)) {
			sendATCOM(cmd);
			if (!waitForATAnswer("CLOSED", 50)) {
				sendATCOM("AT+CIPCLOSE");
			}
			LED_RED_OFF;
			ret = true;
		} else {
			LED_RED_ON;
			WIFI_RESET_ACT;
			_delay_ms(100);
			WIFI_RESET_DIS;
			_delay_ms(3000);
			loc_measuredData.wifiErr++;
			ret = false;
		}
	} else {
		//usart_printf("\n%s", "Error");
		LED_RED_ON;
		WIFI_RESET_ACT;
		_delay_ms(100);
		WIFI_RESET_DIS;
		_delay_ms(3000);
		loc_measuredData.wifiErr++;
		ret = false;
	}

	return ret;
}

bool get_internetTime(bool gmt) {

	bool ret = false;
	uint32_t timer = 0;
	timer = 90;

	usart_printf("\nItime");

	if (gmt) {
		sendATCOM("AT+CIPNTP=2");
		_delay_ms(5000);
	}

	reset_rxbuffer_eth();
	sendATCOM("AT+CIPNTP?");

	//Time: 22:30:35 01/20/2016 GMT+01___OK
	while (!strstr((const char*) rx_buffer_eth, "GMT") && timer != 0) {
		_delay_ms(100);
		//USART_Transmit_DEB(timer);
		usart_printf("%d", timer);
		timer--;
	}

	//	if (strstr(where, what) != NULL )
	//		usart_printf("\nfound");
	//	else
	//		usart_printf("\nnot found");

	usart_printf("\n %s", rx_buffer_eth);

	if (timer > 0) {
		ret = 1;

		char* pos_ch = strstr((const char*) rx_buffer_eth, "Time:");
		//uint8_t pos_d = pos_ch - rx_buffer_eth + 1;

		char actTch[21];

		memcpy(actTch, pos_ch + 6, 20);

		usart_printf("\nTime got! %d %d %d %d %d %d", actTime.hours,
				actTime.mins, actTime.secs, actTime.mounth, actTime.day,
				actTime.year);

		// 22:30:35 01/20/2016
		char dtmp[3];
		memcpy(dtmp, actTch, 2);
		actTime.hours = atoi(dtmp);
		SetHour(actTime.hours);

		memcpy(dtmp, actTch + 3, 2);
		actTime.mins = atoi(dtmp);
		SetMinute(actTime.mins);

		memcpy(dtmp, actTch + 6, 2);
		actTime.secs = atoi(dtmp);
		SetSecond(actTime.secs);

		memcpy(dtmp, actTch + 9, 2);
		actTime.mounth = atoi(dtmp);
		SetMonth(actTime.mounth);

		memcpy(dtmp, actTch + 12, 2);
		actTime.day = atoi(dtmp);
		SetDate(actTime.day);

		char ytmp[5];
		memcpy(ytmp, actTch + 15, 4);
		actTime.year = atoi(ytmp);
		SetYear(actTime.year);

		usart_printf("\nT after sync %d:%d:%d %d %d %d", GetHour(), GetMinute(),
				GetSecond(), GetDate(), GetMonth(), GetYear());
		//usart_printf(actTime.actTch);

	} else {
		ret = 0;
	}

	reset_rxbuffer_eth();

	return ret;
}

bool wifi_connect(void) {

	bool ret = false;
	char cmd[50];
	strcpy(cmd, "AT+CWJAP=\"PalHome_1\",\"gerbicic\""); //make this command: AT+CPISTART="TCP","146.227.57.195",80
	sendATCOM(cmd);
	//usart_printf("\n%s", cmd);

	if (waitForATAnswer("OK", 90)) {
		usart_printf("\nRouter OK");
		ret = true;
	} else {
		ret = false;
		usart_printf("\nRouter Error");
	}

	return ret;
}

void ds1307_update(actTime_t* t) {
	t->secs = GetSecond();
	t->mins = GetMinute();
	t->hours = GetHour();
	t->day = GetDate();
	t->mounth = GetMonth();
	t->year = 20 + GetYear();
}

void freeRam() {
	extern int __heap_start, *__brkval;
	int v;
	usart_printf("\nRAM %d",
			(int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval));
}
