#include <avr/io.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <util/delay.h>
#include "uart.h"

//static const char * const g_pcHex = "0123456789abcdef";

char rx_buffer_deb[RX_BUFFER_SIZE_DEB];
volatile char rx_buffer_eth[RX_BUFFER_SIZE_ETH];

volatile uint16_t deb_i = 0;
volatile uint16_t eth_i = 0;
volatile uint8_t wasrx_uart_deb = 0;
volatile uint8_t wasrx_uart_eth = 0;
volatile uint8_t wastx_uart_deb = 0;

//---------------UART------------------------------------------------
void init_USART_ETH(uint16_t ubrr) {
	/* Set baud rate */
	UBRR0H = (uint8_t) (ubrr >> 8);
	UBRR0L = (uint8_t) ubrr;

	/* Enable receiver and transmitter */UCSR0B = (1 << TXEN0) | (1 << RXEN0)
			| (1 << RXCIE0);

	/* Set frame format: Async, Parity-Disabled, 1 Stop bit, 8 data bits */
	UCSR0C = (1 << UCSZ00) | (1 << UCSZ01);
}

void init_USART_DEB(uint16_t ubrr) {
	/* Set baud rate */
	UBRR1H = (uint8_t) (ubrr >> 8);
	UBRR1L = (uint8_t) ubrr;

	/* Enable receiver and transmitter */UCSR1B =  (1 << RXEN1)
			| (1 << RXCIE1);// | (1 << TXEN1); //|(1<<TXCIE1); (1 << TXEN1) |

	/* Set frame format: Async, Parity-Disabled, 1 Stop bit, 8 data bits */
	UCSR1C = (1 << UCSZ10) | (1 << UCSZ11);
}

char USART_receive_ETH(void) {
	while (!(UCSR0A & (1 << RXC0)))
		;
	return UDR0 ;
}

char USART_receive_DEB(void) {
	while (!(UCSR1A & (1 << RXC1)))
		;
	return UDR1 ;
}

void USART_Transmit_ETH(uint8_t data) {
	/* Wait for empty transmit buffer */
	while (!(UCSR0A & (1 << UDRE0)))
		;
	/* Put data into buffer, sends the data */
	UDR0 = data;
}

void USART_Transmit_DEB(uint8_t data) {
	/* Wait for empty transmit buffer */
	while (!(UCSR1A & (1 << UDRE1)))
		;
	/* Put data into buffer, sends the data */
	UDR1 = data;
}

void USART_SendString_ETH(char* str) {
	int lng = 0;
	lng = strlen(str);

	for (int i = 0; i < lng; i++) {
		USART_Transmit_ETH(str[i]);
	}

	//USART_Transmit_ETH(' ');
}

void USART_SendString_DEB(char* str) {
	int len = strlen(str);

	for (int i = 0; i < len; i++) {
		USART_Transmit_DEB(str[i]);
	}

	USART_Transmit_DEB(' ');
}

void USART_SendString_DEB_num(const char* str, uint8_t len) {
	for (int i = 0; i < len; i++) {
		USART_Transmit_DEB(str[i]);
	}

	USART_Transmit_DEB(' ');
}

void reset_rxbuffer_eth() {
	for (int k = 0; k < RX_BUFFER_SIZE_ETH; k++) {
		rx_buffer_eth[k] = 0;
	}
	//memset(rx_buffer_eth, 0, RX_BUFFER_SIZE_ETH);
	eth_i = 0;
}

void reset_rxbuffer_deb() {
	//for (deb_i = 0; deb_i < RX_BUFFER_SIZE_DEB; deb_i++) {
	//	rx_buffer_deb[deb_i] = 0;
	//}
	memset(rx_buffer_deb, 0, RX_BUFFER_SIZE_DEB);
	deb_i = 0;
}

//debugMSG
void usart_printf(const char *fmt, ...) // <a href="http://en.wikipedia.org/wiki/Stdarg.h">variadic function </a>
{
//#ifdef DEBUGENABLED
	int num_chars;
	char *uart_buff;
	num_chars = strlen(fmt) + 1; //get size of our string
	uart_buff = (char *) malloc(sizeof(char) * num_chars); //allocate num_chars of character

	va_list args;
	va_start(args, fmt);
	vsprintf(uart_buff, fmt, args);
	USART_Transmit_DEB('\r'); //send CR
	USART_SendString_DEB(uart_buff); //send msg
	va_end(args);
	free(uart_buff); //deallocation array
//#endif
}

uint8_t waitForATAnswer(char* what, int16_t timer) {
	//uint16_t timer = howmany;
	volatile uint8_t ret = 0;
	while (!strstr((const char*) rx_buffer_eth, what) && timer != 0) {
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
	reset_rxbuffer_eth();

	if (timer > 0) {
		ret = 1;
	} else {
		ret = 0;
	}
	return ret;

}

void sendCRLF(void) {
	USART_Transmit_ETH('\r');
	USART_Transmit_ETH('\n');
	//USART_Transmit_ETH('\0');
}

void sendAT(void) {
	USART_Transmit_ETH('A');
	USART_Transmit_ETH('T');
	sendCRLF();
}

void sendATCOM(char* atcmd) {
	USART_SendString_ETH(atcmd);
	sendCRLF();
}

//
//void UARTvprintf(const char *pcString, va_list vaArgP) {
//	uint32_t ui32Idx, ui32Value, ui32Pos, ui32Count, ui32Base, ui32Neg;
//	char *pcStr, pcBuf[16], cFill;
//
//	//
//	// Loop while there are more characters in the string.
//	//
//	while (*pcString) {
//		//
//		// Find the first non-% character, or the end of the string.
//		//
//		for (ui32Idx = 0;
//				(pcString[ui32Idx] != '%') && (pcString[ui32Idx] != '\0');
//				ui32Idx++) {
//		}
//
//		//
//		// Write this portion of the string.
//		//
//		USART_SendString_DEB_num(pcString, ui32Idx);
//
//		//
//		// Skip the portion of the string that was written.
//		//
//		pcString += ui32Idx;
//
//		//
//		// See if the next character is a %.
//		//
//		if (*pcString == '%') {
//			//
//			// Skip the %.
//			//
//			pcString++;
//
//			//
//			// Set the digit count to zero, and the fill character to space
//			// (in other words, to the defaults).
//			//
//			ui32Count = 0;
//			cFill = ' ';
//
//			//
//			// It may be necessary to get back here to process more characters.
//			// Goto's aren't pretty, but effective.  I feel extremely dirty for
//			// using not one but two of the beasts.
//			//
//			again:
//
//			//
//			// Determine how to handle the next character.
//			//
//			switch (*pcString++) {
//			//
//			// Handle the digit characters.
//			//
//			case '0':
//			case '1':
//			case '2':
//			case '3':
//			case '4':
//			case '5':
//			case '6':
//			case '7':
//			case '8':
//			case '9': {
//				//
//				// If this is a zero, and it is the first digit, then the
//				// fill character is a zero instead of a space.
//				//
//				if ((pcString[-1] == '0') && (ui32Count == 0)) {
//					cFill = '0';
//				}
//
//				//
//				// Update the digit count.
//				//
//				ui32Count *= 10;
//				ui32Count += pcString[-1] - '0';
//
//				//
//				// Get the next character.
//				//
//				goto again;
//			}
//
//				//
//				// Handle the %c command.
//				//
//			case 'c': {
//				//
//				// Get the value from the varargs.
//				//
//				ui32Value = va_arg(vaArgP, uint32_t);
//
//				//
//				// Print out the character.
//				//
//				USART_SendString_DEB_num((char *) &ui32Value, 1);
//
//				//
//				// This command has been handled.
//				//
//				break;
//			}
//
//				//
//				// Handle the %d and %i commands.
//				//
//			case 'd':
//			case 'i': {
//				//
//				// Get the value from the varargs.
//				//
//				ui32Value = va_arg(vaArgP, uint32_t);
//
//				//
//				// Reset the buffer position.
//				//
//				ui32Pos = 0;
//
//				//
//				// If the value is negative, make it positive and indicate
//				// that a minus sign is needed.
//				//
//				if ((int32_t) ui32Value < 0) {
//					//
//					// Make the value positive.
//					//
//					ui32Value = -(int32_t) ui32Value;
//
//					//
//					// Indicate that the value is negative.
//					//
//					ui32Neg = 1;
//				} else {
//					//
//					// Indicate that the value is positive so that a minus
//					// sign isn't inserted.
//					//
//					ui32Neg = 0;
//				}
//
//				//
//				// Set the base to 10.
//				//
//				ui32Base = 10;
//
//				//
//				// Convert the value to ASCII.
//				//
//				goto convert;
//			}
//
//				//
//				// Handle the %s command.
//				//
//			case 's': {
//				//
//				// Get the string pointer from the varargs.
//				//
//				pcStr = va_arg(vaArgP, char *);
//
//				//
//				// Determine the length of the string.
//				//
//				for (ui32Idx = 0; pcStr[ui32Idx] != '\0'; ui32Idx++) {
//				}
//
//				//
//				// Write the string.
//				//
//				USART_SendString_DEB_num(pcStr, ui32Idx);
//
//				//
//				// Write any required padding spaces
//				//
//				if (ui32Count > ui32Idx) {
//					ui32Count -= ui32Idx;
//					while (ui32Count--) {
//						USART_SendString_DEB_num(" ", 1);
//					}
//				}
//
//				//
//				// This command has been handled.
//				//
//				break;
//			}
//
//				//
//				// Handle the %u command.
//				//
//			case 'u': {
//				//
//				// Get the value from the varargs.
//				//
//				ui32Value = va_arg(vaArgP, uint32_t);
//
//				//
//				// Reset the buffer position.
//				//
//				ui32Pos = 0;
//
//				//
//				// Set the base to 10.
//				//
//				ui32Base = 10;
//
//				//
//				// Indicate that the value is positive so that a minus sign
//				// isn't inserted.
//				//
//				ui32Neg = 0;
//
//				//
//				// Convert the value to ASCII.
//				//
//				goto convert;
//			}
//
//				//
//				// Handle the %x and %X commands.  Note that they are treated
//				// identically; in other words, %X will use lower case letters
//				// for a-f instead of the upper case letters it should use.  We
//				// also alias %p to %x.
//				//
//			case 'x':
//			case 'X':
//			case 'p': {
//				//
//				// Get the value from the varargs.
//				//
//				ui32Value = va_arg(vaArgP, uint32_t);
//
//				//
//				// Reset the buffer position.
//				//
//				ui32Pos = 0;
//
//				//
//				// Set the base to 16.
//				//
//				ui32Base = 16;
//
//				//
//				// Indicate that the value is positive so that a minus sign
//				// isn't inserted.
//				//
//				ui32Neg = 0;
//
//				//
//				// Determine the number of digits in the string version of
//				// the value.
//				//
//				convert: for (ui32Idx = 1;
//						(((ui32Idx * ui32Base) <= ui32Value)
//								&& (((ui32Idx * ui32Base) / ui32Base) == ui32Idx));
//						ui32Idx *= ui32Base, ui32Count--) {
//				}
//
//				//
//				// If the value is negative, reduce the count of padding
//				// characters needed.
//				//
//				if (ui32Neg) {
//					ui32Count--;
//				}
//
//				//
//				// If the value is negative and the value is padded with
//				// zeros, then place the minus sign before the padding.
//				//
//				if (ui32Neg && (cFill == '0')) {
//					//
//					// Place the minus sign in the output buffer.
//					//
//					pcBuf[ui32Pos++] = '-';
//
//					//
//					// The minus sign has been placed, so turn off the
//					// negative flag.
//					//
//					ui32Neg = 0;
//				}
//
//				//
//				// Provide additional padding at the beginning of the
//				// string conversion if needed.
//				//
//				if ((ui32Count > 1) && (ui32Count < 16)) {
//					for (ui32Count--; ui32Count; ui32Count--) {
//						pcBuf[ui32Pos++] = cFill;
//					}
//				}
//
//				//
//				// If the value is negative, then place the minus sign
//				// before the number.
//				//
//				if (ui32Neg) {
//					//
//					// Place the minus sign in the output buffer.
//					//
//					pcBuf[ui32Pos++] = '-';
//				}
//
//				//
//				// Convert the value into a string.
//				//
//				for (; ui32Idx; ui32Idx /= ui32Base) {
//					pcBuf[ui32Pos++] =
//							g_pcHex[(ui32Value / ui32Idx) % ui32Base];
//				}
//
//				//
//				// Write the string.
//				//
//				USART_SendString_DEB_num(pcBuf, ui32Pos);
//
//				//
//				// This command has been handled.
//				//
//				break;
//			}
//
//				//
//				// Handle the %% command.
//				//
//			case '%': {
//				//
//				// Simply write a single %.
//				//
//				USART_SendString_DEB_num(pcString - 1, 1);
//
//				//
//				// This command has been handled.
//				//
//				break;
//			}
//
//				//
//				// Handle all other commands.
//				//
//			default: {
//				//
//				// Indicate an error.
//				//
//				USART_SendString_DEB_num("ERROR", 5);
//
//				//
//				// This command has been handled.
//				//
//				break;
//			}
//			}
//		}
//	}
//}

