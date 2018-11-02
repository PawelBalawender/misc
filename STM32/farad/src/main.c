/*
 * Vcc: 3.33 V
 * Ports:
 *      -PA0 ADC_IN0
 *      -PA1 COMP1_INP
 *      -PA4 DAC_OUT1, COMP1_INM4
 *      -PA5 LD2
 *      -PA9 USART1_TX
 *      -PA10 USART1_RX
 *      -PB2 custom; used to (dis)charge the capacitor
 *      -PC13 B1
 */
#include<stm32f0xx.h>
#define LED_PIN 5U /* LD2 is PA5 (UM p. 23) */
#define LED_ON() (GPIOA->BSRR |= (1U << LED_PIN))
#define LED_OFF() (GPIOA->BRR |= (1U << LED_PIN))
#define LED_TOGGLE() (GPIOA->ODR ^= (1U << LED_PIN))

#define CAP_PIN 2U /* we use PB2 to manage the capacitor */
#define CAP_CHARGE() (GPIOB->BRR |= (1U << CAP_PIN))
#define CAP_DISCHARGE() (GPIOB->BSRR |= (1U << CAP_PIN))
#define TIMER_START() (TIM2->CR1 |= TIM_CR1_CEN)
#define TIMER_STOP() (TIM2->CR1 &= ~TIM_CR1_CEN)

#define VOLTAGE_CHARGED (2950U) /* in milivolts */
#define DAC_CHARGED ((VOLTAGE_CHARGED * 4096U) / 3300U) /* 3723 */
#define VOLTAGE_ALMOST ((2U * VOLTAGE_CHARGED) / 3U) /* 2000 mV */
#define DAC_ALMOST ((VOLTAGE_ALMOST * 4096U) / 3300U) /* 2482 */

#define DMA_TIM2 (DMA1_Channel4)

uint8_t adc_val;
double tim_clk = 48000000.0;
uint32_t R = 100;
double cap;
uint32_t calibration_val;
uint32_t timer_val;
volatile unsigned int n;

void configure_RCC_FLASH(void);
void configure_DBG(void);
void configure_GPIO_LD2(void);
void configure_GPIO_B1(void);
void configure_GPIO_DAC(void);
void configure_GPIO_ADC(void);
void configure_GPIO_USART(void);
void configure_GPIO_CHARGER(void);
void configure_GPIO_COMP(void);
void configure_DAC(void);
void configure_ADC(void);
void configure_USART(void);
void configure_DMA_ADC(void);
void configure_DMA_TIM(void);
void configure_TIM2(void);
void configure_COMP(void);
void configure_IRQ_USART(void);
void configure_IRQ_B1(void);
void configure_IRQ_COMP(void);
void EXTI4_15_IRQHandler(void);
void USART1_IRQHandler(void);
void TIM2_IRQHandler(void);
void ADC1_COMP_IRQHandler(void);
void delay(unsigned int);
void send_string(char*);
void send_char(uint8_t);
void blink(void);

/* timer: use TI1 to start counter on gpio exti 13 */
/* info: real cnt_en is 1 clock cycle after cen */
/* USE TSEL to start DAC by timer */


int main() {
    configure_RCC_FLASH();
    //configure_DBG(); 

    configure_GPIO_CHARGER();
    CAP_DISCHARGE();

    configure_GPIO_LD2();
    configure_GPIO_B1();
    configure_IRQ_B1();
    configure_GPIO_DAC();
    configure_GPIO_ADC();
    configure_GPIO_USART();
    configure_GPIO_COMP();
    
    configure_DAC();

    configure_USART();
    configure_ADC();
    configure_DMA_ADC();
    while(ADC1->DR) {}

    configure_COMP();
    configure_IRQ_COMP();
    configure_TIM2();
    configure_DMA_TIM();

    blink();

    while (1) {
    }

    return 0;
}

void EXTI4_15_IRQHandler(void) {
    if (EXTI->PR & EXTI_PR_PIF13) {
        DAC->DHR12R1 &= (1 << 12);
        DAC->DHR12R1 |= DAC_ALMOST;
        TIM2->EGR |= TIM_EGR_UG;
        TIM2->CR1 |= TIM_CR1_CEN;
        CAP_CHARGE();

        LED_ON();
        EXTI->PR |= EXTI_PR_PIF13;
    }
}

void ADC1_COMP_IRQHandler (void) {
    if (EXTI->PR & EXTI_PR_PIF21) {
        if (TIM2->CNT) {
            LED_OFF();
            CAP_DISCHARGE();
            while (adc_val != 0) {}
            // C = t/R = (timer/freq.)/R
            // then multiply it by 1e6 to make the result human
            cap = (timer_val / (tim_clk / 1e6)) / R;
            DMA_TIM2->CCR &= ~DMA_CCR_EN;
            DMA_TIM2->CNDTR &= ~DMA_CNDTR_NDT;
            DMA_TIM2->CNDTR |= (8U << DMA_CNDTR_NDT_Pos);
            DMA_TIM2->CCR |= DMA_CCR_EN;
            blink();
        }
        EXTI->PR |= EXTI_PR_PIF21;
    }
}

void TIM2_IRQHandler(void) {
    if (TIM2->SR & TIM_SR_CC4IF) {
        DAC->DHR12R1 &= (1 << 12);
        DAC->DHR12R1 |= DAC_CHARGED;
        
        /* CLEAR FLAG */
        TIM2->CCR4;
    }
}

/*---------------------------------------------------------------------------*/

void configure_TIM2(void) {
    /* USE TIM2 BECAUSE IT HAS A 32-BIT COUNTER REGISTER
     * NO FCK_CNT PRESCALER, SO IT IS SAME AS APB FREQ. (RM p. 96)
     * COUNT UPWARDS
     * UPDATE ONLY WHEN THE COUNTER WOULD EXCEED uint32_t SIZE
     * MODE: ONE-PULSE
     */
    TIM2->PSC &= ~TIM_PSC_PSC;
    TIM2->CR1 &= ~TIM_CR1_DIR;
    TIM2->ARR = (uint32_t)0xFFFFFFFF;
    TIM2->CR1 |= TIM_CR1_OPM;

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
    TIM2->DIER |= TIM_DIER_CC4IE;
    /* ENABLE INTERRUPT IN NVIC */
    NVIC_EnableIRQ(TIM2_IRQn);
    /* SET PRIORITY TO HIGHEST */
    NVIC_SetPriority(TIM2_IRQn, 0);

    /* ENABLE TIMER */
    TIM2->CR1 |= TIM_CR1_CEN;
}

void configure_COMP(void) {
    /* COMP1 NON-INVERTING INPUT IS COMP1_INP (PA1) */
    /* SELECT MEDIUM HYSTERESIS */
    COMP->CSR &= ~COMP_CSR_COMP1HYST;
    COMP->CSR |= COMP_CSR_COMP1HYST_1;
    /* SELECT PA4 (DAC_OUT1) AS INVERTING INPUT */
    COMP->CSR &= ~COMP_CSR_COMP1INSEL;
    COMP->CSR |= (4 << COMP_CSR_COMP1INSEL_Pos);
    /* REDIRECT OUTPUT TO TIM2 INPUT CAPTURE */
    COMP->CSR &= ~COMP_CSR_COMP1OUTSEL;
    COMP->CSR |= (4 << COMP_CSR_COMP1OUTSEL_Pos);
    /* ENABLE COMP1 */
    COMP->CSR |= COMP_CSR_COMP1EN;
}

void configure_DAC(void) {
    /* ENABLE DAC AND CLEAR HOLDING REGISTER */
    DAC->CR |= DAC_CR_EN1;
}

void configure_USART(void) {
    /* COMMUNICATION:
     * DMA ENABLED FOR TRANSMITER
     * 1 START BIT
     * 8 DATA BITS
     * 1 STOP BIT
     * NO PARITY BIT
     * BAUD RATE: 9600
     * OVERSAMPLE x16
     */
    USART1->CR1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_OVER8);
    USART1->CR2 &= ~USART_CR2_STOP;
    USART1->BRR = 8000000 / 9600; /* RM p. 703, 743 */
    USART1->CR3 |= USART_CR3_DMAT;
    USART1->CR1 |= USART_CR1_TE | USART_CR1_TE | USART_CR1_UE;

    /* POLL IDLE FRAME TRANSMISSION AND CLEAR TC FLAG AFTER IT */
    while ((USART1->ISR & USART_ISR_TC) == 0) {}
    USART1->ICR |= USART_ICR_TCCF;
}

void configure_DMA_ADC(void) {
    /* TRANSFER DATA FROM ADC1 TO MEMORY, CIRCULARLY (RM P. 191) */
    /* PERIPH FROM: ADC1->DR
     * PERIPH TO: USART1->TDR
     * 1 TRANSFER TO ACCOMPLISH; IT'S CIRCULAR MODE SO IT WON'T BE 0
     */
    if ((DMA1_Channel1->CCR & DMA_CCR_EN) == 0) {
        DMA1_Channel1->CPAR = (uint32_t)(&(ADC1->DR));
        DMA1_Channel1->CMAR = (uint32_t)(&USART1->TDR);
        DMA1_Channel1->CNDTR |= (1U << DMA_CNDTR_NDT_Pos);
    }
    /* CHANNEL PRIORITY: MEDIUM (1)
     * TRANSFER DIRECTION: PERIPHERAL TO "MEMORY" (2)
     * MODE: CIRCULAR (3)
     * DO NOT INCREMENT ADDRESSES (4)
     * ADC1->DR DATA SIZE: 16-BIT (RM p. 244) (5)
     * USART1->TDR DATA SIZE: 8-BIT (RM p. 752) (6)
     */
    DMA1_Channel1->CCR &= ~DMA_CCR_PL; /* (1) */
    DMA1_Channel1->CCR |= DMA_CCR_PL_0;
    DMA1_Channel1->CCR &= ~DMA_CCR_DIR; /* (2) */
    DMA1_Channel1->CCR |= DMA_CCR_CIRC; /* (3) */
    DMA1_Channel1->CCR &= ~DMA_CCR_MINC; /* (4a) */
    DMA1_Channel1->CCR &= ~DMA_CCR_PINC; /* (4b) */
    DMA1_Channel1->CCR &= ~DMA_CCR_PSIZE; /* (5) */
    DMA1_Channel1->CCR |= DMA_CCR_PSIZE_0;
    DMA1_Channel1->CCR &= ~DMA_CCR_MSIZE; /* (6) */
    DMA1_Channel1->CCR |= DMA_CCR_MSIZE_0; 

    /* TURN DMA ON */
    DMA1_Channel1->CCR |= DMA_CCR_EN;
}

void configure_DMA_TIM(void) {
    /* TRANSFER DATA FROM TIM2->CCR TO MEMORY, ON REQUEST
     * DMA AND CHANNEL SELECTION: RM p. 197
     * PERIPH FROM: TIM2->CCR4
     * PERIPH TO: uint32_t values[2]
     * 2 TRANSFERS TO ACCOMPLISH
     */
    if ((DMA_TIM2->CCR & DMA_CCR_EN) == 0) {
        DMA_TIM2->CPAR = (uint32_t)(&(TIM2->CCR4));
        DMA_TIM2->CMAR = (uint32_t)(&timer_val);
        DMA_TIM2->CNDTR |= (32U << DMA_CNDTR_NDT_Pos);
    }
    /* CHANNEL PRIORITY: HIGHEST (1)
     * TRANSFER DIRECTION: PERIPHERAL TO MEMORY (2)
     * MODE: SINGLE TRANSFER (3)
     * DO NOT INCREMENT PERIPH ADDRESS (4a)
     * INCREMENT MEMORY ADDRESS (4b)
     * TIM2->CCR4 DATA SIZE: 32-bit (RM p. ) (5)
     * uint32_t values[2] DATA SIZE: 32-bit (6)
     */
    DMA_TIM2->CCR &= ~DMA_CCR_PL; /* (1) */
    DMA_TIM2->CCR |= DMA_CCR_PL_0;
    DMA_TIM2->CCR &= ~DMA_CCR_DIR; /* (2) */
    DMA_TIM2->CCR &= ~DMA_CCR_CIRC; /* (3) */
    DMA_TIM2->CCR &= ~DMA_CCR_PINC; /* (4a) */
    DMA_TIM2->CCR |= DMA_CCR_MINC; /* (4b) */
    DMA_TIM2->CCR &= ~DMA_CCR_PSIZE; /* (5) */
    DMA_TIM2->CCR |= DMA_CCR_PSIZE_1;
    DMA_TIM2->CCR &= ~DMA_CCR_MSIZE; /* (6) */
    DMA_TIM2->CCR |= DMA_CCR_MSIZE_1;

    /* TURN DMA ON */
    DMA_TIM2->CCR |= DMA_CCR_EN;
}

void configure_ADC(void) {
    /* ADC RESOLUTION: 8-BIT */
    if ((ADC1->CR & ADC_CR_ADEN) == 0) {
        ADC1->CFGR1 &= ~ADC_CFGR1_RES;
        ADC1->CFGR1 |= ADC_CFGR1_RES_1;
    }

    /* SAMPLING: 239.5 CYCLES
     * CHANNEL: PA0
     * CONVERSION MODE: CONTINUOUS (THUS IT IS STARTED BY SOFTWARE)
     * IN CASE OF OVERRUN: OVERWRITE ADC1->D
     * SCANNING DIRECTION: CHANNEL 0 TO CHANNEL 18
     * DATA ALIGNMENT: RIGHT-ALIGNED
     */
    if ((ADC1->CR & ADC_CR_ADSTART) == 0) {
        ADC1->SMPR &= ~ADC_SMPR_SMP;
        ADC1->SMPR |= ADC_SMPR_SMP;
        ADC1->CHSELR |= ADC_CHSELR_CHSEL0;
        ADC1->CFGR1 |= ADC_CFGR1_CONT;
        ADC1->CFGR1 &= ~ADC_CFGR1_DISCEN;
        ADC1->CFGR1 &= ~ADC_CFGR1_EXTEN;
        ADC1->CFGR1 |= ADC_CFGR1_OVRMOD;
        ADC1->CFGR1 &= ~ADC_CFGR1_SCANDIR;
        ADC1->CFGR1 &= ~ADC_CFGR1_ALIGN;
    }
    
    /* ENSURE ADRDY IS RESET, THEN ENABLE ADC AND WAIT UNTIL IT IS READY */
    if (ADC1->ISR & ADC_ISR_ADRDY) ADC1->ISR |= ADC_ISR_ADRDY;
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {}

    /* ENSURE THAT ADEN IS RESET, CONFIGURE DMA AND CALIBRATE ADC */
    if (ADC1->CR & ADC_CR_ADEN) ADC1->CR |= ADC_CR_ADDIS;
    while (ADC1->CR & ADC_CR_ADEN) {}
    ADC1->CFGR1 |= ADC_CFGR1_DMAEN | ADC_CFGR1_DMACFG;
    ADC1->CR |= ADC_CR_ADCAL; 
    while (ADC1->CR & ADC_CR_ADCAL) {}

    /* ENSURE ADRDY IS RESET, THEN ENABLE ADC AND WAIT UNTIL IT IS READY */
    if (ADC1->ISR & ADC_ISR_ADRDY) ADC1->ISR |= ADC_ISR_ADRDY;
    ADC1->CR |= ADC_CR_ADEN;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {}

    /* ENABLE REFERENCE VOLTAGE, START CONVERSION AND WAIT UNTIL IT'S READY */
    ADC->CCR |= ADC_CCR_VREFEN;
    ADC1->CR |= ADC_CR_ADSTART;
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {}
}

/*---------------------------------------------------------------------------*/

void configure_IRQ_COMP(void) {
    /* ENABLE INTERRUPT ON EXTI LINE 21 */
    EXTI->IMR |= EXTI_IMR_IM21;
    /* SELECT RISING EDGE */
    EXTI->RTSR |= EXTI_RTSR_RT21;

    /* ENABLE INTERRUPT IN NVIC */
    NVIC_EnableIRQ(ADC1_COMP_IRQn);
    /* SELECT THE HIGHEST PRIORITY */
    NVIC_SetPriority(ADC1_COMP_IRQn, 0);
}

void configure_IRQ_USART(void) {
    /* ENABLE TRANSFER COMPLETEY INTERRUPT */
    USART1->CR1 |= USART_CR1_TCIE;

    /* ENABLE INTERRUPT IN NVIC */
    NVIC_EnableIRQ(USART1_IRQn);
    /* SET PRIORITY TO HIGHEST */
    NVIC_SetPriority(USART1_IRQn, 0);
}

void configure_IRQ_B1(void) {
    /* SELECT GPIOC AS INTERRUPT SOURCE ON EXTI LINE 13 */
    SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;
    SYSCFG->EXTICR[3] |= (0x0002 << SYSCFG_EXTICR4_EXTI13_Pos);

    /* MASK INTERRUPTS ON LINE 13 */
    EXTI->IMR |= EXTI_IMR_MR13;
    /* ENABLE RISING EDGE TRIGGER FOR EXTI LINE 13 */
    EXTI->RTSR |= EXTI_RTSR_RT13;

    /* ENABLE INTERRUPT IN NVIC */
    NVIC_EnableIRQ(EXTI4_15_IRQn);
    /* SET PRIORITY TO HIGHEST */
    NVIC_SetPriority(EXTI4_15_IRQn, 0);
}

/*---------------------------------------------------------------------------*/

void configure_GPIO_COMP(void) {
    /* COMP1_INP IS IS PA1 AND COMP1_INM4 (DAC_OUT1) IS PA4 */
    /* SELECT ANALOG MODE FOR COMP1_INP AND COMP1_INM4 */
    GPIOA->MODER |= (GPIO_MODER_MODER1 | GPIO_MODER_MODER4);
    /* SELECT THE HIGHEST SPEED FOR COMP1_INP AND COMP1_INM4 */
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR1 | GPIO_OSPEEDR_OSPEEDR4);
}

void configure_GPIO_USART(void) {
    /* USART1_TX IS PA9 AND USART1_RX IS PA10 */
    /* SELECT ALTERNATE FUNCTION MODE FOR USART1_TX AND USART1_RX */
    GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
    GPIOA->MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1);
    /* SELECT AF1 FUNCTION FOR USART1_TX AND USART1_RX */
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFRH1 | GPIO_AFRH_AFRH2);
    GPIOA->AFR[1] |= ((0x0001) << GPIO_AFRH_AFRH1_Pos);
    GPIOA->AFR[1] |= ((0x0001) << GPIO_AFRH_AFRH2_Pos);
    /* SELECT THE HIGHEST SPEED FOR USART1_TX AND USART1_RX */
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10);
}

void configure_GPIO_ADC(void) {
    /* ADC_IN0 IS PA0 */
    /* SELECT ANALOG MODE FOR ADC_IN0 */
    GPIOA->MODER |= GPIO_MODER_MODER0;
    /* SELECT THE HIGHEST SPEED FOR ADC_IN0 */
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR0;
    /* SELECT WEAK PULL-DOWN RESISTOR FOR ADC_IN0 */
    GPIOA->PUPDR &= GPIO_PUPDR_PUPDR0;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_1;
}

void configure_GPIO_DAC(void) {
    /* DAC_OUT1 IS PA4 */
    /* SELECT ANALOG MOGE FOR DAC_OUT1 */
    GPIOA->MODER |= GPIO_MODER_MODER4;
    /* SELECT THE HIGHEST SPEED FOR DAC_OUT1 */
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR4;
}

void configure_GPIO_CHARGER(void) {
    /* WE OUTPUT CHARGE/DISCHARDE STATE ON PIN 9 GPIO PORT A */
    /* SELECT OUTPUT MODE FOR CHARGER */
    GPIOB->MODER &= ~GPIO_MODER_MODER2;
    GPIOB->MODER |= GPIO_MODER_MODER2_0;
    /* SELECT THE HIGHEST SPEED FOR CHARGER */
    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR2;
    /* SELECT PUSH-PULL OUTPUT MODE FOR CHARGER */
    GPIOB->OTYPER &= ~GPIO_OTYPER_OT_2;
    /* SELECT PULL-DOWN WEAK RESISTOR FOR CHARGER */
    GPIOB->PUPDR &= ~GPIO_PUPDR_PUPDR2;
    GPIOB->PUPDR |= GPIO_PUPDR_PUPDR2_1;
}

void configure_GPIO_B1(void) {
    /* USER BUTTON (B1) is on PC13 (UM p. 23) */
    /* SELECT INPUT MODE FOR B1 */
    GPIOC->MODER &= ~GPIO_MODER_MODER13;
    /* SELECT THE HIGHEST SPEED FOR B1 */
    GPIOC->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR13;
    /* SELECT PUSH-PULL OUTPUT MODE FOR B1 */
    GPIOC->OTYPER &= ~GPIO_OTYPER_OT_13;
    /* SELECT PULL-DOWN WEAK RESISTOR FOR B1 */
    GPIOC->PUPDR &= ~GPIO_PUPDR_PUPDR13;
    GPIOC->PUPDR |= GPIO_PUPDR_PUPDR13_1;
}

void configure_GPIO_LD2(void) {
    /* USER LED (LD2) IS ON PA5 (UM p. 23) */
    /* SELECT OUTPUT MODE FOR LD2 */
    GPIOA->MODER &= ~GPIO_MODER_MODER5;
    GPIOA->MODER |= GPIO_MODER_MODER5_0;
    /* SELECT THE LOWEST SPEED FOR LD2 */
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR5;
    /* SELECT PUSH-PULL OUTPUT MODE FOR LD2 */
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;
    /* SELECT PULL-DOWN WEAK RESISTOR FOR LD2 */
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR5;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPDR5_1;
}

/*---------------------------------------------------------------------------*/

void delay(unsigned int time) {
    /* BLOCK THE PROCESSOR FOR A WHILE */
    /* N IS VOLATILE, SO THIS LOOP WILL NOT BE REMOVED BY COMPILER */
    for (; time > 0; time--) {
        __NOP();
    }
    //n = time;
    //while (n) {n--;}
}

void send_char(uint8_t data) {
    /* WAIT UNTIL THE TRANSMITTER REGISTER IS EMPTY */
    while ((USART1->ISR & USART_ISR_TXE) == 0) {}
    /* UPDATE ITS CONTENT WITH THE NEW DATA */
    USART1->TDR = data;
    /* WAIT UNTIL IT HAS FINISHED THE TRANSFER */
    while ((USART1->ISR & USART_ISR_TC) == 0) {}
}

void send_string(char* string) {
    /* AUTOMATICALLY INVOKE SEND_CHAR FUNCTION TO SEND THE WHOLE STRING */
    while (*string) send_char(*string++);
    send_char('\0');
}

void blink(void) {
    LED_ON();
    delay(100000);
    LED_OFF();
    delay(100000);
}

/*---------------------------------------------------------------------------*/

void configure_RCC_FLASH(void) {
    /* TURN ON HSI (8MHz) AND HSI14 (14MHz) */
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {}

    RCC->CR2 |= RCC_CR2_HSI14ON; 
    while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0) {}

    /* CONFIGURE SYSTEM CLOCK */
    /* SELECT HSI (8MHz) AS SYSTEM CLOCK (SYSCLK) */
    RCC->CFGR &= ~RCC_CFGR_SW;
    /* ALIGN FLASH LATENCY TO THE SYSTEM CLOCK FREQUENCY */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    /* SELECT SYSCLK WITHOUT PRESCALER FOR HCLK (AHB) CLOCK */
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    /* SELECT HCLK WITHOUT PRESCALER FOR PCLK (APB) CLOCK */
    RCC->CFGR &= ~RCC_CFGR_PPRE;

    /* SELECT HSI14 AS ADC1 CLOCK */
    RCC->APB2ENR |= RCC_APB2ENR_ADCEN;
    if ((ADC1->CR) == 0) ADC1->CFGR2 &= ~ADC_CFGR2_CKMODE;

    /* SELECT HSI 8MHz AS USART1 CLOCK */
    RCC->CFGR3 &= ~RCC_CFGR3_USART1SW;
    RCC->CFGR3 |= RCC_CFGR3_USART1SW_HSI;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* SYSCFG FOR EXTI LINE 13 AND COMP FOR COMP1 */ 
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGCOMPEN;
    /* DBGMCU TO STOP TIM2 IN DEBUGGING MODE */
    RCC->APB2ENR |= RCC_APB2ENR_DBGMCUEN;
    /* TIMER 2, DAC, DMA, GPIO PORTS*/
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;
    RCC->AHBENR |= RCC_AHBENR_DMA1EN;
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN;
}

void configure_DBG(void) {
    /* STOP THE TIM2 CLOCK WHEN MCU ENTERS DEBUG MODE (HALTED) */
    DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM2_STOP;
}

