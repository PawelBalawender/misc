#include"stm32f0xx.h"
#include"stm32f0xx_it.h"
#include"common.h"

uint8_t adc_val;
double cap;
uint32_t timer_val;


void EXTI4_15_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PIF13) {
        TIM2->EGR |= TIM_EGR_UG;
        TIM2->CR1 |= TIM_CR1_CEN;
        CAP_CHARGE();
        LED_ON();
        EXTI->PR |= EXTI_PR_PIF13;
    }
}

void ADC1_COMP_IRQHandler (void) {
    cap = 0.95 * (timer_val / (long double)SystemCoreClock) / R;
    //if (EXTI->PR & EXTI_PR_PIF21) {
        //if (TIM2->CNT) {
            LED_OFF();
            CAP_DISCHARGE();
            //while (adc_val != 0) {}
            // C = t/R = (timer/freq.)/R
            // then multiply it by 1e6 to make the result human
            // cap = (timer_val / (tim_clk / 1e6)) / R;
            DMA_TIM2->CCR &= ~DMA_CCR_EN;
            DMA_TIM2->CNDTR &= ~DMA_CNDTR_NDT;
            DMA_TIM2->CNDTR |= (8U << DMA_CNDTR_NDT_Pos);
            DMA_TIM2->CCR |= DMA_CCR_EN;
            //blink();
        //}
        EXTI->PR |= EXTI_PR_PIF21;
    //}
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_CC4IF) {
        //DAC->DHR12R1 &= (1 << 12);
        //DAC->DHR12R1 |= DAC_CHARGED;
        
        /* CLEAR FLAG */
        TIM2->CCR4;
    }
}

