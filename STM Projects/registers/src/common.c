/* This header contains math abstract utilities for the kisaburo project
 * author: Pawel Balawender
 * contact: voilagrange@gmail.com
 */
#include<stm32f0xx.h>
#include"common.h"


void delay(unsigned int time) {
    while (time--) __ASM volatile ("nop");
}


void send_char(char c) {
    while ((USART1->ISR & USART_ISR_TXE) == 0) {}
    USART1->TDR = c;
}

void blink(void) {
    LED_ON();
    delay(48000);
    LED_OFF();
    delay(48000);
}


/** Send an integer value using send_char function
 *
 * @param num: the integer value to send
 */
void send_int(int num) {
    int i;
    uint32_t mask = 0xFF000000;
    for (i = sizeof(int); i >= 0; i--) {
        send_char((unsigned char)((num & mask) >> (i * 8)));
        mask >>= 8;
    }
}
