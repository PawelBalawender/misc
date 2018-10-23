/* This header contains abstract utilities' headers for the kisaburo project
 * author: Pawel Balawender
 * contact: voilagrange@gmail.com
 */
#include<stdint.h>  // uint8_t, uint32_t
#include<stm32f0xx.h>  // GPIOA

#define LED_PIN (5U)
#define LED_ON() ((GPIOA->BSRR |= (1U << LED_PIN)))
#define LED_OFF() ((GPIOA->BRR |= (1U << LED_PIN)))
#define LED_TOG() ((GPIOA->ODR ^= (1U << LED_PIN)))


void blink(void);

void delay(unsigned int);

void send_char(char);


/** Send an integer value through USART
 *
 */
void send_int(int);
