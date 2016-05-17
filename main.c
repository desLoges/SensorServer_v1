/*
 * main.c
 *
 * Created: 2013.07.15. 0:26:17
 *  Author: desLoges
 */
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "hardware.h"
#include "nrf24l01.h"
#include "nrf24l01registers.h"
#include "spi.h"
#include "dht.h"
#include "clock.h"

//static Usart_Message_eth_t Usart_Message_eth;
//static Asdu_Message_t Asdu_Message;
mainStates_t mainStateNext = MEASURE; //default state
mainStates_t mainStatePrev = DO_NOTHING;
periodicTasks_t periodicTasks = MEASURE;
periodicTasks_t periodicTasksPrev = MEASURE;
nonperiodicTasks_t nonperiodicTasks = SYS_CTRL;
nixieDisplayStates_t nixieDisplayStates = HOURS;

//char x[5];
char temp_ch[5];

uint8_t bufferin[NRF24L01_PAYLOAD];
//volatile char rx_buffer_eth[RX_BUFFER_SIZE_ETH];

uint8_t nixie_enable_mask = 0b0001111;
uint16_t temp, nixie_display_next = 2;
volatile uint16_t tim2, nix = 0;
volatile bool was_butt1_int = false;
volatile bool was_butt2_int = false;
volatile bool was_pir_int = false;
volatile bool was_sysctrl_int = false;
volatile bool was_nrf_int = false;
volatile bool servoce_mode = false;
volatile uint8_t LCD_light_level = 255;

//char d[5] = { '1', '2', '3', '4', '\0' };
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
int main(void) {
	//uint8_t ethpack = 0;
	//uint8_t i = 0;
	uint8_t timeout = 0;
	//uint8_t at_answer = 0;
	//bool minus = false;
	bool ext1_is_active = false;
	uint8_t ext1_inactive_cnt = 0;
	bool motion_enable_nixies = true;
	bool button_enable_show_nixie = false;
	bool button_enable_show_maintanance = false;
	bool PIR_active = true;
	uint32_t motion_enable_nixies_cnt = NIXIE_CNT_LIMIT + 1;
	bool motion_enable_lcd = true;
	uint32_t motion_enable_lcd_cnt = 0;
	float temperature;
	float humidity;
	uint8_t web_refresh_cnt = 0;
	bool stb_led_up = false;
	uint32_t butt_1_cnt = 0;
	uint32_t butt_2_cnt = 0;
	loc_measuredData.wifiErr = 0;
	loc_measuredData.NRFErr = 0;
	uint8_t RGB_light_correction = RGB_MAX_DIV;

	//struct rtc_time rtc;

	//IO
	init_IO();

	WIFI_RESET_ACT;
	_delay_ms(100);
	WIFI_RESET_DIS;
	_delay_ms(100);

	LED_DEB_ON;

//	PORTB |= (1<<NIXIE_LED_RED);
//	PORTB |= (1<<NIXIE_LED_GREEN);
//	PORTB |= (1<<NIXIE_LED_BLUE);
//
//	while(1);

	//USART
	init_USART_ETH(MYUBRR_ETH);

	//wifiEnable(false);
#if DEBUGENABLED==1

//	if(PIND & (1<<PD2)){
//
//	}

	init_USART_DEB(MYUBRR_DEB);
	usart_printf("\nMCUCSR:%d ", MCUCSR);
	if (MCUCSR & (1 << PORF))
	usart_printf("POR");
	if (MCUCSR & (1 << EXTRF))
	usart_printf("ERST");
	if (MCUCSR & (1 << BORF))
	usart_printf("BOD");
	if (MCUCSR & (1 << WDRF))
	usart_printf("WDOG");
	if (MCUCSR & (1 << JTRF))
	usart_printf("JTAG");

	MCUCSR = 0;

#endif
	usart_printf(STROUT_PR_NAME);

	//LCD
	lcd_init(LCD_DISP_ON); //LCD initialization
	custom_chars();
	lcd_clrscr(); //clear LCD screen
	lcd_home(); //set LCD cursor to default position
	lcd_gotoxy(3, 0);
	lcd_printf("System Start:");
	timers_lcdlight_init();
	DISP_BACKLIGHT = LCD_light_level;
	usart_printf("\nLCD...");
	LCD_verticalScroll("LCD done");

	sei();
	usart_printf("\nsei()...");
	LCD_verticalScroll("IRQ enabled");

	LCD_verticalScroll("LED Test");
	LED_RED_ON;
	_delay_ms(300);
	LED_YELLOW_ON;
	_delay_ms(300);
	LED_RED_OFF;
	LED_YELLOW_OFF;

	timers_rgb_init();
	set_nixie_rgb(255, 0, 0);
	_delay_ms(200);
	set_nixie_rgb(0, 255, 0);
	_delay_ms(200);
	set_nixie_rgb(0, 0, 255);
	_delay_ms(200);
	set_nixie_rgb(0, 0, 0);

	//
	// RTC
	//
	if (ClockInit()) {
		usart_printf("\nClock running!");
		LCD_verticalScroll("Clock running!");

		usart_printf("\nT before sync %d:%d:%d %d %d %d", GetHour(),
				GetMinute(), GetSecond(), GetDate(), GetMonth(), GetYear());

	} else {
		usart_printf("\nClock ERR!");
		LCD_verticalScroll("Clock ERR!");
	}

	//
	// WIFI
	//
	usart_printf("\ninit wifi");
	lcd_gotoxy(2, 2);
	LCD_verticalScroll("WiFi connect...");

	if (wifi_connect()) {
		LCD_verticalScroll("...OK");
		LCD_verticalScroll("Time Syncing...");
		if (get_internetTime(true)) {
			usart_printf("\nTime Synced> ");
			LCD_verticalScroll("...OK"); //lcd_printf("%s", "OK");
		} else {
			usart_printf("\nTime Sync ERROR!");
			LCD_verticalScroll("...ERROR"); //lcd_printf("%s", "ERROR");
			LED_RED_ON;
		}
	} else {
		LCD_verticalScroll("...Error");
		loc_measuredData.wifiErr++;
		LED_RED_ON;
	}

//	while (1) {
//
//		LED_RED_ON;
//		WIFI_RESET_ACT;
//		_delay_ms(100);
//		WIFI_RESET_DIS;
//		_delay_ms(3000);
//
//		LED_RED_OFF;
//		_delay_ms(10000);
//
//	}

	//
	//timer
	//
	timers_init();
	LCD_verticalScroll("Timer GO");

	LED_STB_LVL = 255;

	//
	//NIXIE
	//
	config_io_nixie();

	//
	// NRF radio
	//
	nrf24l01_init();
	spi_init();
	memset(bufferin, 0, sizeof(bufferin));
	usart_printf("\nNRF inited...");
	LCD_verticalScroll("Radio inited");

	//
	//INTERRUPT
	//
	EIMSK |= (1 << INT7) | (1 << INT5) | (1 << INT6);
	EICRB |= (1 << ISC71) | (1 << ISC50) | (1 << ISC51) | (1 << ISC60)
			| (1 << ISC61); //button rising edge

	set_nixie_rgb(0, 0, 0);
	nixie_suppy(0);
	usart_printf("\nNixie Supply enabled...");
	LCD_verticalScroll("Nixies ON");

	//
	// ADC
	//
	init_ADC();
	usart_printf("\nADC inited...");
	LCD_verticalScroll("ADC");

	dht_gettemperaturehumidity(&temperature, &humidity);
	usart_printf("\nDHT inited...");
	LCD_verticalScroll("DHT inited");

	if (BUTTON1_PRESS) {
		//maintanance mode
		lcd_clrscr();
		lcd_printf("%s", "  SERVICE MODE");
		usart_printf("\nSERVICE MODE");
		servoce_mode = true;

		init_USART_DEB(MYUBRR_DEB);
		usart_printf("\nMCUCSR:%d ", MCUCSR);
//		lcd_gotoxy(0, 1);
//		lcd_printf("%s", "Time Syncing...");
//		if (get_internetTime(true)) {
//			usart_printf("\nTime Synced> ");
//			lcd_printf("%s", "OK");
//		} else {
//			usart_printf("\nTime Sync ERROR!");
//			lcd_printf("%s", "ERROR");
//			LED_RED_ON;
//		}

		while (1) {
			if (wasrx_uart_deb) {
				process_uartin();
			}
		}
	}

	memcpy(&ext_measuredData.ext1_temp, " ? ?", 4);
	memcpy(&ext_measuredData.ext1_hum, " ? ?", 4);
	memcpy(&ext_measuredData.ext1_airpressure, " ? ? ?", 6);
	memcpy(&ext_measuredData.ext1_lux, "? ? ?", 5);
	memcpy(&ext_measuredData.ext1_batt, "? ? ?", 5);
	memcpy(&ext_measuredData.ext1_rain, "1000", 4);
	LED_YELLOW_ON;

	//while (1)
	//	;

	button_enable_show_maintanance = false;

	LCD_verticalScroll("-- ALL DONE --");
//////////////////////////////////////////////////////////////////////////
	while (1) {
		LED_DEB_TOGGLE; //we are alive

		if (wasrx_uart_eth) {
			usart_printf("\n %s", &rx_buffer_eth);
			wasrx_uart_eth = 0;
			reset_rxbuffer_eth();
		}

		switch (mainStateNext) {

		case DO_NOTHING:
#if PRINT_STATES==1
			usart_printf("\nDO_NOTHING");
#endif
			mainStateNext = PERIODIC;
			break;

		case PERIODIC:
#if PRINT_STATES==1
			usart_printf("\nPERIODIC");
#endif
			switch (periodicTasks) {

			case REFRESHRATE_WAIT:
#if PRINT_STATES==1
				usart_printf("\nREFRESHRATE_WAIT");
#endif
				if (timeout >= (REFRESH_RATE_SYSTEM * 10))
					timeout = 0;
				//while (timeout != 0 && !was_sysctrl_int) {
				_delay_ms(REFRESH_RATE_CTRL);
				timeout++;

				if (was_sysctrl_int) {
					CLR_SYSCTRL_INT;
					mainStateNext = NONPERIODIC;
					break;
				}

				if (!motion_enable_lcd) {
					if (stb_led_up) {
						if (LED_STB_LVL < 200)
							LED_STB_LVL += 5;
						else
							stb_led_up = false;
					} else {
						if (LED_STB_LVL >= 6)
							LED_STB_LVL -= 5;
						else
							stb_led_up = true;
					}
				} else {
					LED_STB_LVL = 255;
				}

				if (timeout == 2) {
					periodicTasks = CLOCK;
					//usart_printf("\nmeas");
				}

				if (timeout == 5) {
					periodicTasks = MEASURE;
					//usart_printf("\nmeas");
				}

				if (timeout == 10) {
					periodicTasks = DISPLAY_LCD;
					if (!PIR_active)
						motion_enable_nixies_cnt++;
				}

				if (timeout == 15) {
					web_refresh_cnt++;
					if (web_refresh_cnt == WEB_UPDATE_LIMIT) {
						periodicTasks = WEB_UPDATE;
						web_refresh_cnt = 0;
					}
				}

				if (timeout == 20) {

					//
					// if motion detected
					//
					if (PIR_PINNAME & (1 << PIR_PIN)) {
						PIR_active = true;
						usart_printf("\nPIR act");
						DISP_BACKLIGHT = LCD_light_level;
						motion_enable_lcd = true;
						//
						//	No actual motion detected
						//
					} else {
						PIR_active = false;
						if (motion_enable_lcd || button_enable_show_nixie) {
							if (button_enable_show_nixie) {
								motion_enable_lcd_cnt = 0;
							}
							//if no motion was detected in defined time
							if (motion_enable_lcd_cnt > LCD_SHUTDOWN_LIMIT) {
								motion_enable_lcd = false;
								DISP_BACKLIGHT = 0;
							}

							// if no motion detected
							// but is still in time
							// increment timeout counter
							motion_enable_lcd_cnt++;
							usart_printf("\nPIR cnt %d", motion_enable_lcd_cnt);
						} else {
							// if motion timeout, and no activity
							motion_enable_lcd_cnt = 0;
						}
					}

					// if no timeout, display
					if (((motion_enable_nixies_cnt > NIXIE_CNT_LIMIT)
							&& PIR_active) || button_enable_show_nixie) {
						button_enable_show_nixie = false;
						motion_enable_nixies_cnt = 0;
						motion_enable_nixies = true;
						all_nixies_on();
						nixie_display_next = HOURS;
						periodicTasks = DISPLAY_NIXIE;
					}

					if (motion_enable_nixies) {
						periodicTasks = DISPLAY_NIXIE;
					}

				}
				break;

			case CLOCK:
#if PRINT_STATES==1
				usart_printf("\nCLOCK");
#endif
				lcd_gotoxy(0, 0);
				lcd_printf(LCDOUT_CLRROW);
				lcd_gotoxy(3, 0);
				ds1307_update(&actTime);
				lcd_printf("%02d %s %02d:%02d", actTime.day,
						get_nameOfTheMonth(actTime.mounth), actTime.hours,
						actTime.mins);
				periodicTasks = REFRESHRATE_WAIT;
				break;

			case WEB_UPDATE:
#if PRINT_STATES==1
				usart_printf("\nWEB");
#endif
				web_update(ext1_is_active);
				periodicTasks = REFRESHRATE_WAIT;

				break;

			case MEASURE:
#if PRINT_STATES==1
				usart_printf("\nMEASURE");
#endif
				if (was_nrf_int) {
					was_nrf_int = false;

					process_ext1_data();

					memset(bufferin, 0, sizeof(bufferin));

					ext1_is_active = true;
					ext1_inactive_cnt = 0;
					LED_YELLOW_OFF;

				} else {
					if (ext1_is_active) {
						if (ext1_inactive_cnt > MAX_EXT1_INACTIVE_CNT) {
							ext1_is_active = false;
							memcpy(&ext_measuredData.ext1_temp, " ? ?", 4);
							memcpy(&ext_measuredData.ext1_hum, " ? ?", 4);
							memcpy(&ext_measuredData.ext1_airpressure, " ? ? ?",
									6);
							memcpy(&ext_measuredData.ext1_lux, "? ? ?", 5);
							memcpy(&ext_measuredData.ext1_batt, "? ? ?", 5);
							memcpy(&ext_measuredData.ext1_rain, "1000", 4);
							LED_YELLOW_ON;
							loc_measuredData.NRFErr++;
						}
						ext1_inactive_cnt++;
					}

				}

				dht_gettemperaturehumidity(&temperature, &humidity);
				dtostrf(temperature, 3, 1, loc_measuredData.loc_temp);
				dtostrf(humidity, 3, 1, loc_measuredData.loc_hum);

				loc_measuredData.ldr = Read_ADC(ADC_LDR);

				if (loc_measuredData.ldr <= NIXIE_SUP_THR_0) {
					nixie_suppy(0);
					RGB_light_correction = RGB_MIN_DIV;
					LCD_light_level = 100;
				} else if (loc_measuredData.ldr < NIXIE_SUP_THR_1) {
					nixie_suppy(1);
					RGB_light_correction = RGB_MAX_DIV;
					LCD_light_level = 190;
				} else if (loc_measuredData.ldr < NIXIE_SUP_THR_2) {
					nixie_suppy(2);
					RGB_light_correction = RGB_MAX_DIV;
					LCD_light_level = 220;
				} else {
					nixie_suppy(3);
					RGB_light_correction = RGB_MAX_DIV;
					LCD_light_level = 255;
				}

				usart_printf("\nRGB corr: %d", RGB_light_correction);

				if (motion_enable_lcd)
					DISP_BACKLIGHT = LCD_light_level;

				if (atoi(ext_measuredData.ext1_batt) < MIN_BATT_LEVEL
						&& atoi(ext_measuredData.ext1_batt) > 1) {
					set_nixie_rgb(180, 0, 0);
					usart_printf("\nLOW BATT");
				}

				periodicTasks = REFRESHRATE_WAIT;
				break; //end of MEASURE

			case DISPLAY_LCD:
#if PRINT_STATES==1
				usart_printf("\nDISPLAY_LCD");
#endif
				if (ext1_is_active) {
					ext_measuredData.ext1_temp[2] = '.'; //workaround
					ext_measuredData.ext1_hum[2] = '.'; //workaround
				}

				if (!button_enable_show_maintanance) {
					display_data_LCD(&loc_measuredData, &ext_measuredData);
				} else {
					display_service_LCD(&loc_measuredData, &ext_measuredData);
					button_enable_show_maintanance = false;
				}

				usart_sendValues();

				periodicTasks = REFRESHRATE_WAIT;
				break;

			case DISPLAY_NIXIE:
#if PRINT_STATES==1
				usart_printf("\nDISPLAY_NIXIE");
#endif
				NIXIES_ENABLE;

				switch (nixie_display_next) {

				case HOURS:
					nixie_dislpay_fadeout();
					set_nixie_rgb(0, 255 / RGB_light_correction, 0);

					char htmp[3];
					char mtmp[3];
					char stmp[3];

					ds1307_update(&actTime);

					itoa(actTime.hours, htmp, 10);
					if (actTime.hours < 10) {
						htmp[1] = htmp[0];
						htmp[0] = '0';
					}

					itoa(actTime.mins, mtmp, 10);
					if (actTime.mins < 10) {
						mtmp[1] = mtmp[0];
						mtmp[0] = '0';
					}

					itoa(actTime.secs, stmp, 10);
					if (actTime.secs < 10) {
						stmp[1] = stmp[0];
						stmp[0] = '0';
					}

					//22:30:35 01/20/2016
					char time[] = { 'x', 'x', 'x', htmp[0], htmp[1], 'x',
							mtmp[0], mtmp[1], 'x', stmp[0], stmp[1], 'x', 'x',
							'x', };

					//for (uint8_t k = 0; k < 2; k++) {
					for (uint8_t n = 0; n < 12; n++) {

						nixie_enable_mask = 0b0001110;

						if (time[n] != 'x')
							temp_ch[0] = time[n];
						else {
							//temp_ch[0] = '0';
							nixie_enable_mask &= ~(1 << 3);
						}

						if (time[n + 1] != 'x')
							temp_ch[1] = time[n + 1];
						else {
							//temp_ch[1] = '0';
							nixie_enable_mask &= ~(1 << 2);
						}

						if (time[n + 2] != 'x')
							temp_ch[2] = time[n + 2];
						else {
							//temp_ch[2] = '0';
							nixie_enable_mask &= ~(1 << 1);
						}

						//temp_ch[1] = time[n + 1];
						//temp_ch[2] = time[n + 2];
						temp_ch[3] = '\0';
						temp_ch[4] = '\0';

						//nixie_enable_mask = 0b0001110;
						nixie_print(temp_ch, false, false);

						_delay_ms(300);

					}
					//}

					nixie_display_next = LOC_TEMP;
					continue;

				case LOC_TEMP:
					nixie_dislpay_fadeout();
					set_nixie_rgb(0, 0, 255 / RGB_light_correction);
					nixie_print_value(loc_measuredData.loc_temp, NIXIE_CELSIUS,
							0);

					nixie_display_next = LOC_HUM;

					break;

				case LOC_HUM:
					nixie_dislpay_fadeout();
					set_nixie_rgb(0, 0, 255 / RGB_light_correction);
					nixie_print_value(loc_measuredData.loc_hum, NIXIE_PERCENT,
							0);
					if (ext1_is_active) {
						nixie_display_next = EXT_TEMP;
					} else {
						nixie_display_next = NIXIE_END;
					}

					//nixie_dislpay_fadeout();
					break;
					//
				case EXT_TEMP:
					nixie_dislpay_fadeout();
					set_nixie_rgb(0, 255 / RGB_light_correction,
							200 / RGB_light_correction);
					nixie_print_value(ext_measuredData.ext1_temp, NIXIE_CELSIUS,
							ext_measuredData.temp_minus);

					nixie_display_next = EXT_HUM;
					//nixie_dislpay_fadeout();
					break;

				case EXT_HUM:
					nixie_dislpay_fadeout();
					set_nixie_rgb(0, 255 / RGB_light_correction,
							200 / RGB_light_correction);
					nixie_print_value(ext_measuredData.ext1_hum, NIXIE_PERCENT,
							0);

					nixie_display_next = NIXIE_END;
					//nixie_dislpay_fadeout();
					break;

				case NIXIE_END:
					motion_enable_nixies = false;
					motion_enable_nixies_cnt = 0;
					nixie_dislpay_fadeout();
					if (atoi(ext_measuredData.ext1_batt) < MIN_BATT_LEVEL
							&& atoi(ext_measuredData.ext1_batt) > 1) {
						set_nixie_rgb(180 / RGB_light_correction, 0, 0);
					} else {
						if (RGB_light_correction == RGB_MIN_DIV) {
							set_nixie_rgb(0, 0, 0);
						} else {
							set_nixie_rgb(65, 12, 0);
						}
					}

					all_nixies_off();
					break;

				}

				//mainStatePrev = NIXIE_DISPLAY;
				periodicTasks = REFRESHRATE_WAIT;
				break;
			}

			break;

		case NONPERIODIC:
#if PRINT_STATES==1
			usart_printf("\nNON_PERIODIC");
#endif
			switch (nonperiodicTasks) {
			case SYS_CTRL:
				if (was_butt1_int) {
					was_butt1_int = false;
					butt_1_cnt++;
					//lcd_gotoxy(0, 1);
					//lcd_printf("butt1 %d", butt_1_cnt);
					button_enable_show_maintanance = true;
					TOUCH_BUTTON_1_FC;
					TOUCH_BUTTON_1_IE;
				}
				if (was_butt2_int) {
					was_butt2_int = false;
					butt_2_cnt++;
					//lcd_gotoxy(0, 1);
					//lcd_printf("butt2 %d", butt_2_cnt);
					button_enable_show_nixie = true;
					TOUCH_BUTTON_2_FC;
					TOUCH_BUTTON_2_IE;
				}
				//mainStatePrev = SYS_CTRL;
				mainStateNext = PERIODIC;
				break;

			}

		} //end of switch mainStates
		  //////////////////////////////////////////////////////////////////////////
	} //end of while(1)
} //end of main
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
ISR(INT7_vect) {
//rx
	cli();
	was_nrf_int = true;
	pipe = 0;
	if (nrf24l01_readready(&pipe)) { //if data is ready
		nrf24l01_read(bufferin);
	}
	sei();
}

ISR(USART1_RX_vect) {
	cli();
	LED_DEB_ON;

	//USART_Transmit_ETH(USART_receive_DEB());

	char data_deb = USART_receive_DEB();

	if (data_deb == 0xA)
		wasrx_uart_deb = 1;

	rx_buffer_deb[deb_i] = data_deb;
	deb_i++;

	sei();
}

ISR(USART0_RX_vect) {
	cli();
//////	DEB_LED_ON;
//	char data_eth = USART_receive_ETH();
////if(data==USART_EOS_CHAR_DEB) data = '\0';
//	rx_buffer_eth[eth_i] = data_eth;
//	if (data_eth == USART_EOF_CHAR_ETH)
//		wasrx_uart_eth = 1;
//	eth_i++;

	char last_rx_byte = USART_receive_ETH();
	//if (last_rx_byte) {
	if (last_rx_byte == 0)
		last_rx_byte = '_';

	if (last_rx_byte == 0xA) {
		last_rx_byte = '_';
		wasrx_uart_eth = 1;
	}

	if (last_rx_byte == 0xD)
		last_rx_byte = '_';

	rx_buffer_eth[eth_i] = last_rx_byte;
	eth_i++;
	//}

	//if (servoce_mode) {
	//	USART_Transmit_DEB(USART_receive_ETH());
	//}
	sei();
}

ISR(USART1_TX_vect) {
	wastx_uart_deb = 1;
}

ISR(TIMER2_OVF_vect) {
	cli();
	LED_DEB_TOGGLE;
	if (nix < 3)
		nix++;
	else
		nix = 0;
	nixie_digit(nix, digit[nix]);
	sei();
//usart_printf("\nx");
}

ISR(INT1_vect) {
	cli();
	was_pir_int = true;
	usart_printf("\nPIR INT");
	DISP_BACKLIGHT = LCD_light_level;
	sei();
}

//button 1
ISR(INT5_vect) {
	TOUCH_BUTTON_1_ID;
	cli();
	was_butt1_int = true;
	SET_SYSCTRL_INT;
//lcd_printf("b1");
//mainStateNext = NONPERIODIC;
	nonperiodicTasks = SYS_CTRL;
	sei();
}

//button 2
ISR(INT6_vect) {
	TOUCH_BUTTON_2_ID;
	cli();
	was_butt2_int = true;
	SET_SYSCTRL_INT;
//lcd_printf("b2");
//mainStateNext = NONPERIODIC;
	nonperiodicTasks = SYS_CTRL;
	sei();
}

ISR(BADISR_vect) {

//for (;;) UDR0='!';
	usart_printf("\n!!!!");
	LED_DEB_TOGGLE;
//DEB_LED_TOGGLE;
}
