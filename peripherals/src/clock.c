/******************************************************************************

 eXtreme Electronics xAPI(TM)
 ----------------------------
 xAPI is a Powerful but easy to use C library to program the xBoard(TM)
 series of AVR development board. The library has support for commonly use tasks
 like:-

 *LCD interfacing
 *MUXED 7 segment displays.
 *Remote Control
 *Serial Communication
 *DC Motor Controls with Speed Control
 *Analog Measurement for Sensor Interface.
 *Temperature Measurement.
 *I2C Communication.
 *EEPROM Interface
 *Real Time Clock (RTC Interface)

 The APIs are highly documented and easy to use even by a beginner.

 For More Info Log On to
 www.eXtremeElectronics.co.in

 Copyright 2008-2014 eXtreme Electronics India

 Clock Core
 ----------
 This module is used for interfacing with DS1307 based clock module.
 The library provide easy to use to use functions to get and set time.

 For More information please see supplied tutorials and videos.

 NOTICE
 --------
 NO PART OF THIS WORK CAN BE COPIED, DISTRIBUTED OR PUBLISHED WITHOUT A
 WRITTEN PERMISSION FROM EXTREME ELECTRONICS INDIA. THE LIBRARY, NOR ANY PART
 OF IT CAN BE USED IN COMMERCIAL APPLICATIONS. IT IS INTENDED TO BE USED FOR
 HOBBY, LEARNING AND EDUCATIONAL PURPOSE ONLY. IF YOU WANT TO USE THEM IN
 COMMERCIAL APPLICATION PLEASE WRITE TO THE AUTHOR.


 WRITTEN BY:
 AVINASH GUPTA
 me@avinashgupta.com

 *******************************************************************************/

#include <avr/io.h>
#include <util/delay.h>

#include "I2C.h"
#include "ds1307.h"
#include "clock.h"
#include "uart.h"

/***************************************

 Setup the DS1307 Chip. Start it and set
 12 Hr Mode.
 Must be called before any other clock
 related functions.

 Parameters
 ----------
 NONE

 Return Value
 ------------
 0=Failed
 1=Success

 ****************************************/

bool ClockInit() {
	//Initialize I2C Bus
	I2CInit();
	usart_printf("\nI2C done, ");
	//Clear CH bit of RTC
#define CH 7

	uint8_t temp;
	if (!DS1307Read(0x00, &temp))
		return 0;
	usart_printf("read, ");
	//Clear CH Bit
	temp &= (~(1 << CH));

	if (!DS1307Write(0x00, temp))
		return 0;
	usart_printf("write");
	//Set 12 Hour Mode
	if (!DS1307Read(0x02, &temp))
		return 0;

	//Set 12Hour BIT
	temp |= (0b00000000);

	//Write Back to DS1307
	if (!DS1307Write(0x02, temp))
		return 0;

	return 1;

}
/***************************************

 Get second

 Parameters
 ----------
 NONE

 Return Value
 ------------
 the "second" part of time.

 ****************************************/
uint8_t GetSecond() {
	uint8_t sec, temp;

	//Read the Second Register
	DS1307Read(0x00, &temp);
	sec = ((temp & 0x0F) + ((temp & 0x70) >> 4) * 10); //(((temp & 0b01110000) >> 4) * 10) + (temp & 0b00001111);

	return sec;

}
/***************************************

 Get minute

 Parameters
 ----------
 NONE

 Return Value
 ------------
 the "minute" part of time.

 ****************************************/

uint8_t GetMinute() {
	uint8_t min, temp;

	//Read the Minute Register
	DS1307Read(0x01, &temp);
	min = ((temp & 0x0F) + ((temp & 0x70) >> 4) * 10); //(((temp & 0b01110000) >> 4) * 10) + (temp & 0b00001111);

	return min;

}

/***************************************

 Get hour

 Parameters
 ----------
 NONE

 Return Value
 ------------
 the "hour" part of time.

 ****************************************/

uint8_t GetHour() {
	uint8_t hr, temp;

	//Read the Hour Register
	DS1307Read(0x02, &temp);
	//hr = (((temp & 0b01000000) >> 4) * 10) + (temp & 0b00011111);
	hr = ((temp & 0x0F) + ((temp & 0x20) >> 4) * 10);

	return hr;

}

/***************************************

 Get date

 Parameters
 ----------
 NONE

 Return Value
 ------------
 the "date" part of time.

 ****************************************/

uint8_t GetDate() {
	uint8_t dt, temp;

	//Read the date Register
	DS1307Read(0x04, &temp);
	dt = ((temp & 0x0F) + ((temp & 0x30) >> 4) * 10); //(((temp & 0b01000000) >> 4) * 10) + (temp & 0b00111111);

	return dt;

}

/***************************************

 Get Month

 Parameters
 ----------
 NONE

 Return Value
 ------------
 the "month" part of time.

 ****************************************/

uint8_t GetMonth() {
	uint8_t mt, temp;

	//Read the Hour Register
	DS1307Read(0x05, &temp);
	mt = ((temp & 0x0F) + ((temp & 0x10) >> 4) * 10); //(((temp & 0b01100000) >> 4) * 10) + (temp & 0b00011111);

	return mt;

}

/***************************************

 Get Year

 Parameters
 ----------
 NONE

 Return Value
 ------------
 the "year" part of time.

 ****************************************/

uint8_t GetYear() {
	uint8_t yr, temp;

	//Read the Hour Register
	DS1307Read(0x06, &temp);
	yr = (((temp & 0b00000000) >> 4) * 10) + (temp & 0b11111111);//((temp & 0x0F) + ((temp & 0xF0) >> 4) * 10); //(((temp & 0b00000000) >> 4) * 10) + (temp & 0b11111111);

	return yr;

}

/***************************************

 Get am/pm

 Parameters
 ----------
 NONE

 Return Value
 ------------
 0=am
 1=pm

 ****************************************/

uint8_t GetAmPm() {
	uint8_t am_pm, temp;

	//Read the Hour Register
	DS1307Read(0x02, &temp);

	am_pm = (temp & 0b00100000) >> 4;

	return am_pm;

}
/***************************************

 Set the second

 Parameters
 ----------
 seconds

 Return Value
 ------------
 0=failure
 1=success

 ****************************************/

bool SetSecond(uint8_t sec) {
	uint8_t temp, result;

	temp = (((sec / 10) << 4) & 0x70) | (sec % 10);
	result = DS1307Write(0x00, temp);

	return result;
}

/***************************************

 Set the minutes

 Parameters
 ----------
 minutes

 Return Value
 ------------
 0=failure
 1=success

 ****************************************/

bool SetMinute(uint8_t min) {
	uint8_t temp, result;

	temp = (((min / 10) << 4) & 0x70) | (min % 10);
	result = DS1307Write(0x01, temp);

	return result;

}

/***************************************

 Set the hour

 Parameters
 ----------
 hour

 Return Value
 ------------
 0=failure
 1=success

 ****************************************/

bool SetHour(uint8_t hr) {
	//uint8_t temp, result, am_pm;

//	am_pm = GetAmPm();
//
//	temp = ((hr / 10) << 4) | (hr % 10);
//	temp |= 0b01000000; //12 Hr Mode
//
//	if (am_pm) {
//		temp |= 0b00100000;
//	}
//	result = DS1307Write(0x02, temp);
//
//	return result;

	uint8_t temp, result;

	temp = ((((hr / 10) << 4) & 0x20) | (hr % 10));
	result = DS1307Write(0x02, temp);

	return result;

}

/***************************************

 Set the Date

 Parameters
 ----------
 date

 Return Value
 ------------
 0=failure
 1=success

 ****************************************/

bool SetDate(uint8_t dt) {
	uint8_t temp, result;

	temp = (((dt / 10) << 4) & 0x30) | (dt % 10);
	result = DS1307Write(0x04, temp);

	return result;

}

/***************************************

 Set the Month

 Parameters
 ----------
 month

 Return Value
 ------------
 0=failure
 1=success

 ****************************************/

bool SetMonth(uint8_t mt) {
	uint8_t temp, result;

	temp = (((mt / 10) << 4) & 0x10) | (mt % 10);
	result = DS1307Write(0x05, temp);

	return result;

}

/***************************************

 Set the Year

 Parameters
 ----------
 year

 Return Value
 ------------
 0=failure
 1=success

 ****************************************/

bool SetYear(uint8_t yr) {
	uint8_t temp, result;

	temp = (((yr / 10) << 4) & 0xF0) | (yr % 10);
	result = DS1307Write(0x06, temp);

	return result;

}

/***************************************

 Set the second

 Parameters
 ----------
 0=am
 1=pm

 Return Value
 ------------
 0=failure
 1=success

 ****************************************/

bool SetAmPm(bool pm) {
	uint8_t temp, result;

	DS1307Read(0x02, &temp);

	if (pm)
		temp |= 0b00100000; //SET
	else
		temp &= 0b11011111; //CLEAR

	result = DS1307Write(0x02, temp);

	return result;

}

/***************************************

 Gets the current time as a ascii/text string.

 Example 12:49:22 PM (HH:MM:SS: PM)


 Parameters
 ----------
 Pointer to a string.

 Return Value
 ------------
 0=failure
 1=success

 Example Usage
 -------------

 char time[12];			//The string must be at least 12bytes long
 GetTimeString(time);	//Now the string time contains the current time


 ****************************************/

bool GetTimeString(char *Time) {
	uint8_t data;

	if (!DS1307Read(0x00, &data)) {
		//I/O Error
		return 0;
	}

	Time[11] = '\0';
	Time[10] = 'M';
	Time[8] = ' ';

	Time[7] = 48 + (data & 0b00001111);
	Time[6] = 48 + ((data & 0b01110000) >> 4);
	Time[5] = ':';

	if (!DS1307Read(0x01, &data)) {
		//I/O Error
		return 0;
	}

	Time[4] = 48 + (data & 0b00001111);
	Time[3] = 48 + ((data & 0b01110000) >> 4);
	Time[2] = ':';

	if (!DS1307Read(0x02, &data)) {
		//I/O Error
		return 0;
	}

	Time[1] = 48 + (data & 0b00001111);
	Time[0] = 48 + ((data & 0b00010000) >> 4);

	//AM/PM
	if (data & 0b00100000) {
		Time[9] = 'P';
	} else {
		Time[9] = 'A';
	}

	return 1; //Success

}


const char* get_nameOfTheDay(uint8_t day) {
	const char* ret = 0;
	switch (day) {
	case 1:
		ret = "Mon";
		break;
	case 2:
		ret = "Tue";
		break;
	case 3:
		ret = "Wed";
		break;
	case 4:
		ret = "Thu";
		break;
	case 5:
		ret = "Fri";
		break;
	case 6:
		ret = "Sat";
		break;
	case 7:
		ret = "Sun";
		break;
	}

	return ret;
}

const char* get_nameOfTheMonth(uint8_t month) {
	const char* ret = 0;
	switch (month) {
	case 1:
		ret = "Jan";
		break;
	case 2:
		ret = "Feb";
		break;
	case 3:
		ret = "Mar";
		break;
	case 4:
		ret = "Apr";
		break;
	case 5:
		ret = "May";
		break;
	case 6:
		ret = "June";
		break;
	case 7:
		ret = "July";
		break;
	case 8:
		ret = "Aug";
		break;
	case 9:
		ret = "Sept";
		break;
	case 10:
		ret = "Oct";
		break;
	case 11:
		ret = "Nov";
		break;
	case 12:
		ret = "Dec";
		break;
	}

	return ret;
}


uint8_t PREV_PINC = 0xFF;
