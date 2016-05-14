/*
 * uart.h
 *
 *  Created on: 18.10.2014
 *      Author: desLoges
 */

#ifndef UART_H_
#define UART_H_

//uart
#define BAUD_ETH 38400//38400
#define MYUBRR_ETH (F_CPU/16/BAUD_ETH-1)

#define BAUD_DEB 38400
#define MYUBRR_DEB (F_CPU/16/BAUD_DEB-1)


#define RX_BUFFER_SIZE_DEB 30
#define RX_BUFFER_SIZE_ETH 450
#define USART_EOS_CHAR_DEB 'x'
#define USART_EOF_CHAR_DEB 'w'
#define USART_EOF_CHAR_ETH '\n'

extern char rx_buffer_deb[RX_BUFFER_SIZE_DEB];
extern volatile char rx_buffer_eth[RX_BUFFER_SIZE_ETH];
//extern volatile int i;
extern volatile uint8_t wasrx_uart_deb;
extern volatile uint8_t wasrx_uart_eth;
extern volatile uint8_t wastx_uart_deb;

extern volatile uint16_t deb_i;
extern volatile uint16_t eth_i;


extern void init_USART_ETH(uint16_t ubrr);
extern void init_USART_DEB(uint16_t ubrr);
extern char USART_receive_ETH(void);
extern char USART_receive_DEB(void);
extern void USART_Transmit_ETH(uint8_t data);
extern void USART_Transmit_DEB(uint8_t data);
extern void USART_SendString_ETH(char* str);
extern void USART_SendString_DEB(char* str);
extern void reset_rxbuffer_deb();
extern void reset_rxbuffer_eth();
uint8_t waitForATAnswer(char* what, int16_t timer);
void sendCRLF(void);
void sendAT(void);
void sendATCOM(char* atcmd);
void usart_printf(const char *fmt, ...);
//void UARTvprintf(const char *pcString, va_list vaArgP);


#endif /* UART_H_ */
