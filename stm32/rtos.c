#include<stm32f0xx.h>
#include"common.h"
#define USER_MEM (0x20008000U)
#define STACK_SIZE (1024U)

typedef struct regs {
    int32_t r0, r1, r2, r3, r4, r5, r6,
            r7, r8, r9, r10, r11, r12,
            sp, lr, pc, psr, aspr, ipsr,
            epsr, primask, control;
} regs;

volatile uint32_t *thread_id;
volatile regs *cont_dummy, *cont_a, *cont_b;
volatile int stack_a[STACK_SIZE], stack_b[STACK_SIZE];

void configure_TIM2(void);


int main() {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER5_0;

    thread_id = (uint32_t*)(USER_MEM);
    thread_id += 2;
    *thread_id = 0;
    cont_dummy = (regs*)(thread_id + 1);
    cont_a = cont_dummy + 1;
    cont_b = cont_a + 1;

    configure_TIM2();
    while (1) {}
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
    // TIM2->CCER |= TIM_CCER_CC4E;
    /* ENABLE DMA REQUESTS */
    // TIM2->DIER |= TIM_DIER_CC4DE;

    /* ENABLE INTERRUPT */
    TIM2->DIER |= TIM_DIER_UIE;
    /* ENABLE INTERRUPT IN NVIC */
    NVIC_EnableIRQ(TIM2_IRQn);
    /* SET PRIORITY TO HIGHEST */
    NVIC_SetPriority(TIM2_IRQn, 0);

    /* ENABLE TIMER */
    TIM2->CR1 |= TIM_CR1_CEN;
}

