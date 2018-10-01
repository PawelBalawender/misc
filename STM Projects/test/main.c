#include <stm32f0xx.h>
#define LED_PIN 5
#define LED_ON() (GPIOA->BSRR |= (1 << LED_PIN))
#define LED_OFF() (GPIOA->BRR |= (1 << LED_PIN))

int main(void) {
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= GPIO_MODER_MODER5_0;
	
	while(1) {
		LED_ON();
		for(int i=0; i<100000; i++) __NOP();
		LED_OFF();
		for(int i=0; i<100000; i++) __NOP();
	}
}


