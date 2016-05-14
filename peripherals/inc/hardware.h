/*
 * hardware.h
 *
 * Created: 2013.06.25. 13:58:28
 *  Author: desLoges
 */

#ifndef HARDWARE_H_
#define HARDWARE_H_

#include <avr/io.h>
#include <string.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <stdbool.h>

#include "mynixie.h"
//#include "SHTxx.h"
#include "nrf24l01.h"
//#include "nrf24l01registers.h"
#include "lcd.h"
#include "uart.h"
//#include "rtc.h"

//The main events
typedef enum {
	DO_NOTHING, PERIODIC, NONPERIODIC
} mainStates_t;
//extern mainStates_t mainState;

typedef enum {
	MEASURE, DISPLAY_NIXIE, DISPLAY_LCD, REFRESHRATE_WAIT, CLOCK, WEB_UPDATE
} periodicTasks_t;
//extern maintasks_t mainState;

typedef enum {
	SYS_CTRL
} nonperiodicTasks_t;
//extern maintasks_t mainState;

typedef enum {
	HOURS, LOC_TEMP, LOC_HUM, EXT_TEMP, EXT_HUM, NIXIE_END
} nixieDisplayStates_t;
extern nixieDisplayStates_t nixieDisplayStates;

extern uint8_t bufferin[NRF24L01_PAYLOAD];


#define THINK_IP 				"184.106.153.149" // thingspeak.com
#define THINK_GET	 			"GET /update?key=PPDYFSJG0DCWXJV7"

#define ITIME_IP				"146.227.57.195"

#define REFRESH_RATE_SYSTEM				3 //s
#define REFRESH_RATE_CTRL				100 //ms
#define MAX_EXT1_INACTIVE_CNT			10

#define NIXIE_CNT_LIMIT					50
#define WEB_UPDATE_LIMIT				40//20 - cca 1 min
#define LCD_SHUTDOWN_LIMIT				20
//define debug print enable or disable on uart
#define DEBUGENABLED 1

#define STROUT_PR_NAME 			"\r\n\nNixieSensors_v1"
#define STROUT_NEW_LINE 		"\n"
//#define STROUT_ERROR_DHT 		"\nDHT_ERROR!"
#define LCDOUT_CLRROW 			"                    "
#define RAIN_LIMIT						(900)

#define MIN_BATT_LEVEL					(3400)//mv

#define RGB_MAX_DIV						(1)
#define RGB_MID_DIV						(1)
#define RGB_MIN_DIV						(6)

//////////////////////////////////////////////////////////////////////////
#define WIFI_RESET_PORT				PORTF
#define WIFI_RESET_PIN				PF1
#define WIFI_RESET_DIS 				WIFI_RESET_PORT |= (1<<WIFI_RESET_PIN)
#define WIFI_RESET_ACT				WIFI_RESET_PORT &= ~(1<<WIFI_RESET_PIN)

//
// LEDs
//
#define LED_STB 					PINE4
#define LED_STB_LVL					OCR3B

#define LED_DEB 					PINA7
#define LED_DEB_ON 					PORTA |= (1<<LED_DEB)
#define LED_DEB_OFF 				PORTA &= ~(1<<LED_DEB)
#define LED_DEB_TOGGLE 				PORTA ^= (1<<LED_DEB)

#define	LED_YELLOW					PINF0
#define LED_YELLOW_ON 				PORTF |= (1<<LED_YELLOW)
#define LED_YELLOW_OFF 				PORTF &= ~(1<<LED_YELLOW)
#define LED_YELLOW_TOGGLE 			PORTF ^= (1<<LED_YELLOW)

#define LED_RED						PINF4
#define LED_RED_ON 					PORTF |= (1<<LED_RED)
#define LED_RED_OFF 				PORTF &= ~(1<<LED_RED)
#define LED_RED_TOGGLE 				PORTF ^= (1<<LED_RED)

#define DISP_BACKLIGHT 				PINB4
#define DISP_BACKLIGHT_ON 			PORTB |= (1<<DISP_BACKLIGHT)
#define DISP_BACKLIGHT_OFF 			PORTB &= ~(1<<DISP_BACKLIGHT)

#define PANEL_LED 					PINE3
#define PANEL_LED_ON 				PORTE |= (1<<PANEL_LED)
#define PANEL_LED_OFF 				PORTE &= ~(1<<PANEL_LED)

#define PIR_PIN						PF2
#define PIR_PORT					PORTF
#define PIR_PINNAME					PINF

#define LED_DELAY 300

#define NIXIE_SUP_THR_0				5
#define NIXIE_SUP_THR_1				80
#define NIXIE_SUP_THR_2				150

#define TOUCH_BUTTON_1_PIN 			PE5
#define TOUCH_BUTTON_2_PIN 			PE6
#define TOUCH_BUTTON_1_FC			EIFR |= (1 << INTF5)
#define TOUCH_BUTTON_2_FC			EIFR |= (1 << INTF6)
#define TOUCH_BUTTON_1_IE			EIMSK |= (1 << INT5)
#define TOUCH_BUTTON_1_ID			EIMSK &= ~(1 << INT5)
#define TOUCH_BUTTON_2_IE			EIMSK |= (1 << INT6)
#define TOUCH_BUTTON_2_ID			EIMSK &= ~(1 << INT6)

#define BUTTON1_PRESS				PINE & (1<<TOUCH_BUTTON_1_PIN)

#define SET_SYSCTRL_INT				was_sysctrl_int = true
#define CLR_SYSCTRL_INT				was_sysctrl_int = false

#define ADC_LDR						7

#define LCD_CH_TEMP_INT				0x00
#define LCD_CH_TEMP_EXT				0x01
#define LCD_CH_HUM_INT				0x02
#define LCD_CH_HUM_EXT				0x03
#define LCD_CH_CELSIUS				0x04
#define LCD_CH_LUX					0x05
#define LCD_CH_AIRPRS				0x06
#define LCD_CH_HPA					0x07
#define LCD_CH_LX					0x08

#define REG_NUM 0x1C
//////////////////////////////////////////////////////////////////////////
char printbuff[3];

typedef struct {
	uint8_t temp_minus;
	char ext1_temp[5];
	char ext1_hum[5];
	char ext1_airpressure[7];
	char ext1_lux[6];
	char ext1_rain[5];
	char ext1_batt[6];
} ext_measuredData_t;
ext_measuredData_t ext_measuredData;

typedef struct {
	char loc_temp[5];
	char loc_hum[5];
	uint16_t ldr;
	uint16_t wifiErr;
	uint16_t NRFErr;

} loc_measuredData_t;
loc_measuredData_t loc_measuredData;

//volatile uint8_t adcvalue;

uint8_t pipe;
//////////////////////////////////////////////////////////////////////////
typedef struct {
	//char ID; //ID workstation - 1B
	char supply_level;
	char r_led[3];
	char g_led[3];
	char b_led[3];
	char endchar;
//char humidity[5]; //humidity value - 3B
//uint8_t seq; //packet number - 1B

} Usart_Message_deb_t;

typedef struct {
	//char devID; //ID workstation - 1B
	//char msgType;
	char seqNum; //packet number - 1B
	char temp_minus;
	char temperature[4]; //temperature value - 3B
	char humidity[4]; //humidity value - 3B
	char airpressure[6];
	char lux[5];
	char rain[4];
	char outer_batt[5];
} nrf_message_t;
nrf_message_t nrf_message;


typedef struct {
	uint8_t hours;
	uint8_t mins;
	uint8_t secs;
	uint8_t day;
	uint8_t mounth;
	//uint8_t datetime[5];
	uint16_t year;
	//char actTch[21];

} actTime_t;
actTime_t actTime;
/*
 typedef struct
 {
 char ID; //ID workstation - 1B
 char temp_home_out[5];
 //char temp_home_myroom[4];
 //float temp_floor_0;
 //float temp_rehearsal;
 char hum_home_out[5];
 //float hum_home_myroom;
 //float hum_floor_0;
 //float hum_rehearsal;
 //char gas_home;
 }Usart_Message_eth_t;
 */
//////////////////////////////////////////////////////////////////////////
void process_uartin(void);
extern void init_IO(void);
void timers_init(void);
extern void init_ADC(void);
extern uint16_t Read_ADC(uint8_t channel);
void lcd_printf(const char *fmt, ...);
extern void wellcome(void);
uint8_t str2int(const char* str, uint8_t len);
double get_local_temp(void);
double get_local_hum(void);
bool process_ext1_data(void);
void custom_chars(void);
void display_data_LCD(loc_measuredData_t* l_data, ext_measuredData_t* e_data);
void display_service_LCD(loc_measuredData_t* l_data, ext_measuredData_t* e_data);
void LCD_verticalScroll(char* str);
void usart_sendValues(void);
void wifiEnable(bool e);
bool web_update(bool nrf_act);
bool get_internetTime(bool gmt);
bool wifi_connect(void);
void ds1307_update(actTime_t* t);
void freeRam ();

#endif /* HARDWARE_H_ */
