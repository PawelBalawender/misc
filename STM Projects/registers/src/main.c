#include<stm32f0xx.h>
#include"common.h"

void configure_TIM2(void);
extern void TIM2_IRQHandler(void);


int main() {
    __asm__ __volatile__(".global TIM2_IRQHandler\n" \
            "gpioa_addr: .word 0x48000000\n" \
            "LDR R0, gpioa_addr\n" \
            "MOV R2, #0x20\n" \
            "STR R2, [R0]\n");

    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    configure_TIM2();
    blink();

    __asm__ volatile("mov r0, #7");
    register volatile int r0 __asm__("r0");

    while (1) {
        // blink();
    }
}


void configure_TIM2(void) {
    /* ENABLE TIM2 CLOCK */
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    /* USE TIM2 BECAUSE IT HAS A 32-BIT COUNTER REGISTER
     * NO FCK_CNT PRESCALER, SO IT IS SAME AS APB FREQ. (RM p. 96)
     * COUNT UPWARDS
     * UPDATE ONLY WHEN THE COUNTER WOULD EXCEED uint32_t SIZE
     * MODE: NOT ONE-PULSE
     */
    TIM2->PSC &= ~TIM_PSC_PSC;
    TIM2->CR1 &= ~TIM_CR1_DIR;
    TIM2->ARR = (uint32_t)0x00FFFFFF;
    // TIM2->CR1 |= TIM_CR1_OPM;

    /* CONFIGURE INPUT CAPTURE 4 (RM p. 412) */
    if ((TIM2->CCER & TIM_CCER_CC4E) == 0) {
        TIM2->CCMR2 &= ~TIM_CCMR2_CC4S;
        TIM2->CCMR2 |= TIM_CCMR2_CC4S_0;
    }

    /* FILTER: N=4 (RM p. 447 FOR ICxF DESCRIPTION) */
    // No filter
    //TIM2->CCMR2 &= ~TIM_CCMR2_IC4F;
    TIM2->CCMR2 |= (2 << TIM_CCMR2_IC4F_Pos);

    /* SELECT RISING EDGE FOR THE ACTIVE TRANSITION */
    TIM2->CCER &= ~(TIM_CCER_CC4P | TIM_CCER_CC4NP);
    /* NO PRESCALER */
    TIM2->CCMR2 &= ~TIM_CCMR2_IC4PSC;
    /* ENABLE CAPTURE FROM THE COUNTER TO THE CAPTURE REGISTER */
    TIM2->CCER |= TIM_CCER_CC4E;
    /* ENABLE DMA REQUESTS */
    TIM2->DIER |= TIM_DIER_CC4DE;

    /* ENABLE INTERRUPT */
    TIM2->DIER |= TIM_DIER_UIE;
    /* ENABLE INTERRUPT IN NVIC */
    NVIC_EnableIRQ(TIM2_IRQn);
    /* SET PRIORITY TO HIGHEST */
    NVIC_SetPriority(TIM2_IRQn, 0);

    /* ENABLE TIMER */
    TIM2->CR1 |= TIM_CR1_CEN;
}
/*
#define TIM2_IRQHandler() \
({ \
*/
/*\
})
*/
/*
void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_UIF) {
        blink();
        TIM2->SR &= ~TIM_SR_UIF;
    }
}*/

