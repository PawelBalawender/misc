#include <stm32f0xx.h>
#define LED_PIN 5
#define LED_ON() (GPIOA->BSRR |= (1 << LED_PIN))
#define LED_OFF() (GPIOA->BRR |= (1 << LED_PIN))
volatile unsigned int time = 4 * 72000;
unsigned int i;

void delay(void) {
    for (i=0; i < time; i++) {}
}

int main(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    while (1) {
        LED_ON();
        delay();
        LED_OFF();
        delay();
    }
    return 0;
}

