/* This example implements some kind of 3-bits binary counter
 * This means that it iterates over binary representations of numbers
 * between 0 and 7, inclusively, and outputs these 3 bits to pins
 * [7:5] of GPIOA port
 */
#include <stm32f0xx.h>

#define RED_PIN 5
#define GREEN_PIN 6
#define BLUE_PIN 7
#define BTN_PIN 13

unsigned int led_counter = 0;
volatile unsigned int n;  // for delays


void delay(unsigned int time) {
    n = time;
    while (n) {n--;}
}


void update_leds(void) {
    // update and perform modulo
    led_counter++;
    led_counter &= 7;
    // clear all 3 bits
    GPIOA->ODR &= ~(7 << RED_PIN);
    // store current counter value in them
    GPIOA->ODR |= (led_counter << RED_PIN);
}


void EXTI4_15_IRQHandler(void) {
    // if the user button was pressed: zero counter and "quit" delay()
    if (EXTI->PR & (1 << BTN_PIN)) {
        led_counter = 0;
        // shorten delay to 1 iteration
        // zeroing n could lead delay() to rewind the integer
        n = 1;
        // reset the bit in pending register		                PM p. 218
        EXTI->PR |= (1 << BTN_PIN);
    }
}

void configure_GPIO(void) {
    /*configure LEDS: PA5, PA6, PA7*/
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= (1 << (RED_PIN << 1));
    GPIOA->MODER |= (1 << (GREEN_PIN << 1));
    GPIOA->MODER |= (1 << (BLUE_PIN << 1));
    GPIOA->OSPEEDR |= 3 << RED_PIN;
    GPIOA->OSPEEDR |= 3 << GREEN_PIN;
    GPIOA->OSPEEDR |= 3 << BLUE_PIN;
    
    /*configure BT1: PC13*/
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;

    GPIOC->MODER |= (1 << (BTN_PIN));
    GPIOC->OSPEEDR |= 3 << BTN_PIN;

    /*configure BT1 interrupt*/
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    // B1 button is GPIOC13                                 UM p. 23
    // its interrupt line is EXTI13                         RM p. 214

    // select GPIOC as EXTI line 13 source input            RM p. 171
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |= 0x0020;
    // configure trigger for our interrupt                  RM p. 216
    EXTI->RTSR |= (1 << BTN_PIN);  // rising edge ok
    EXTI->FTSR &= ~(1 << BTN_PIN);  // falling edge not ok
    // configure the corresponding mask bit                 RM p. 215
    EXTI->IMR |= (1 << BTN_PIN);

    // configure NVIC
    NVIC_SetPriority(7, 1);
    NVIC_EnableIRQ(7);
}


int main(void) {
    configure_GPIO();
    
    while (1) {
        update_leds();
        delay(10 * 72000);
    }
}


