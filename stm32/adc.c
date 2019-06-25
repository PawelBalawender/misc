/**
 * This module implements an example of built-in A/D converter usage
 */
#include <stm32f0xx.h>
#define _GPIO_MODER_ADC GPIO_MODER_MODER0
#define _GPIO_MODER_LED GPIO_MODER_MODER5_0
#define _GPIO_OSPEEDR_ADC GPIO_OSPEEDR_OSPEEDR0
#define _GPIO_OSPEEDR_LED GPIO_OSPEEDR_OSPEEDR5
#define ADC_PIN 0U // use PA0 as ADC input channel
#define LED_PIN 5U // LD2 is PA5 (UM p. 23, p. 38)

#define LED_ON() ((GPIOA->BSRR |= (1 << LED_PIN)))
#define LED_OFF() ((GPIOA->BRR |= (1 << LED_PIN)))
#define TOGGLE_LED() ((GPIOA->ODR ^= (1 << LED_PIN)))

volatile unsigned int n;
unsigned short adc_val;

void delay(unsigned int time) {
    n = time;
    while (n) {n--;}
}

void configure_DMA(void) {
    // procedure: RM p. 191
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;

    if ((DMA1_Channel1->CCR & DMA_CCR_EN) == 0) {
        // select peripheral and memory addresses: from ADC to a variable
        DMA1_Channel1->CPAR = (uint32_t)(&(ADC1->DR));
        DMA1_Channel1->CMAR = (uint32_t)(&adc_val);
        // transfer only 1 byte of data
        DMA1_Channel1->CNDTR |= (0x1U << DMA_CNDTR_NDT_Pos);
    }

    // channel priority: highest
    DMA1_Channel1->CCR &= ~DMA_CCR_PL;
    // transfer direction: peripheral to memory
    DMA1_Channel1->CCR &= ~DMA_CCR_DIR;
    // circular mode
    DMA1_Channel1->CCR |= DMA_CCR_CIRC;
    // do not increment memory nor periph adresses
    DMA1_Channel1->CCR &= ~DMA_CCR_MINC;
    DMA1_Channel1->CCR &= ~DMA_CCR_PINC;
    // periph data size, 16 bit (RM p. 244)
    DMA1_Channel1->CCR &= ~DMA_CCR_PSIZE;
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;
    // memory data size, 16 bit
    DMA1_Channel1->CCR &= ~DMA_CCR_MSIZE;
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0;
    
    // turn DMA on
    DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void configure_ADC(void) {
    RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
    
    // turn on the HSI14
    RCC->CR2 |= RCC_CR2_HSI14ON;
    
    // wait till HSI14 is ready
    while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) {}
    
    // select HSI14 as ADC clock source
    if ((ADC1->CR) == 0) {
        ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;
    }

    // set ADC resolution to 12 bits
    if ((ADC1->CR & ADC_CR_ADEN) == 0) {
        ADC1->CFGR1 &= ~ADC_CFGR1_RES;
    }

    if ((ADC1->CR & ADC_CR_ADSTART) == 0) {
        // set sampling to 239.5 cycles
        ADC1->SMPR &= ~ADC_SMPR_SMP;
        ADC1->SMPR |= ADC_SMPR_SMP;

        // select channel PA0
        ADC1->CHSELR |= ADC_CHSELR_CHSEL0;

        // select TIM15_TRGO as trigger
        ADC1->CFGR1 |= ADC_CFGR1_EXTSEL_2;
        // select rising edge as polarity
        ADC1->CFGR1 |= ADC_CFGR1_EXTEN_0;

        // set conversion mode to continuous
        ADC1->CFGR1 |= ADC_CFGR1_CONT;
        ADC1->CFGR1 &= ~ADC_CFGR1_DISCEN;
        // thus conversion is started by software
        ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN;

        // if overrun happened: overwrite ADC1->DR
        ADC1->CFGR1 |= ADC_CFGR1_OVRMOD;

        // set scanning direction to upward (CHSEL0 to CHSEL18)
        ADC1->CFGR1 &= ~ADC_CFGR1_SCANDIR;

        // set alignment to <custom>
        ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;
    }

    /*ENABLE ADC*/
    // ensure that ADRDY = 0
    if (ADC1->ISR & ADC_ISR_ADRDY) {
        ADC1->ISR |= ADC_ISR_ADRDY;
    }
    // enable ADC
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {}

    /* CALIBRATE ADC*/
    // ensure that ADEN = 0
    if (ADC1->CR & ADC_CR_ADEN) {
        ADC1->CR |= ADC_CR_ADDIS;
    }
    while (ADC1->CR & ADC_CR_ADEN) {}
    // enable DMA in circular mode
    ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
    // launch calibration
    ADC1->CR |= ADC_CR_ADCAL;
    // wait till it's finished
    while (ADC1->CR & ADC_CR_ADCAL) {}

    /*START CONVERSION*/
    // ensure that ADRDY = 0
    if (ADC1->ISR & ADC_ISR_ADRDY) {
        ADC1->ISR |= ADC_ISR_ADRDY;
    }
    // enable ADC
    ADC1->CR |= ADC_CR_ADEN;

    // start conversion
    ADC1->CR |= ADC_CR_ADSTART;
    // enable reference voltage
    ADC->CCR |= ADC_CCR_VREFEN;

    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {}
}

void configure_GPIO(void) {
    // enable clock for GPIOA
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    
    // set PA0: analog, PA5 (LD2): output RM p. 157
    GPIOA->MODER |= _GPIO_MODER_ADC;
    GPIOA->MODER |= _GPIO_MODER_LED;
    
    // output speed: fastest
    GPIOA->OSPEEDR |= _GPIO_OSPEEDR_ADC;
    GPIOA->OSPEEDR |= _GPIO_OSPEEDR_LED;
}

int main(void) {
    configure_GPIO();
    configure_ADC();
    configure_DMA();

    // digital 2482 ~ analog 2V
    while (1) {
        if (adc_val > 2482) {
            LED_ON();
        } else {
            LED_OFF();
        }
    }
    return 0;
}

