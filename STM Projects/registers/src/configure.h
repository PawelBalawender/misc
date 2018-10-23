/* This header contains configuring functions' headers for the kisaburo project
 * author: Pawel Balawender
 * contact: voilagrange@gmail.com
 */
#ifndef KISABURO_CONFIGURE_H
#define KISABURO_CONFIGURE_H

#include<stdint.h>  /* uint8_t */

#define SYSCLK_FREQ (8000000U)
#define AHB_FREQ (SYSCLK_FREQ)
#define APB_FREQ (AHB_FREQ)
#define USART1_BAUD_RATE (9600U)


/** Configure GPIO pins for LD2
 * 
 * 
 */
void configure_GPIO_LD2(void);


/** Configure GPIO pins for USART1
 *
 */
void configure_GPIO_USART1(void);


/** Configure USART
 * DMA: TRANSMITTER & RECEIVER
 * FULL-DUPLEX
 * 1 START BIT
 * 8 DATA BITS
 * 1 STOP BIT
 * NO PARITY BIT
 * BAUD RATE: 9600
 * OVERSAMPLE x16
 */
void configure_USART1(void);


/** Enable interrupts from USART1
 *
 */
void configure_IRQ_USART1(void);


/** Configure DMAT for USART
 * DMA1, channel: 2
 * Look at the Table 31, p. 197 RM for DMA1 requests mapping
 * @param address_mem Memory address from which to transmit data to UART
 */
void configure_DMAT(char* address_mem);


/** Configure DMAR for USART
 * DMA1, channel: 3
 * Look at the Table 31, p. 197 RM for DMA1 requests mapping
 * @param address_mem Memory addres under which to store data from UART
 */
void configure_DMAR(char* address_mem);


/** Configure system, AHB and APB clocks
 *
 */
void configure_RCC_FLASH(void);


#endif  // KISABURO_CONFIGURE_H
