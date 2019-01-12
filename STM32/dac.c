#include <stm32f0xx.h>
#define _GPIO_MODER_ADC GPIO_MODER_MODER0
#define _GPIO_MODER_LED GPIO_MODER_MODER5_0
#define _GPIO_MODER_TX GPIO_MODER_MODER9_1
#define _GPIO_MODER_RX GPIO_MODER_MODER10_1;
#define _GPIO_OSPEEDR_ADC GPIO_OSPEEDR_OSPEEDR0
#define _GPIO_OSPEEDR_LED GPIO_OSPEEDR_OSPEEDR5

#define ADC_PIN 0U // use PA0 as ADC input channel
#define LED_PIN 5U // LD2 is PA5 (UM p. 23, p. 38)

#define LED_ON() (GPIOA->BSRR |= (1 << LED_PIN))
#define LED_OFF() (GPIOA->BRR |= (1 << LED_PIN))

volatile unsigned int n;
unsigned short adc_val;


void delay(volatile unsigned int time) {
    n = time;
    while (n) {n--;}
}

void configure_GPIO_DAC(void) {
    /* DAC_OUT IS PA4 */
    /* ENABLE CLOCK FOR GPIO PORT A*/
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    /* SELECT ANALOG MODE */
    GPIOA->MODER |= (3 << GPIO_MODER_MODER4_Pos);
    /* SELECT OPEN-DRAIN OUTPUT TYPE */
    GPIOA->OTYPER |= GPIO_OTYPER_OT_4;
    /* NO PULL-UP NOR PULL-DOWN */
    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPDR4;
}

void configure_DAC(void) {
    /* ENABLE CLOCK FOR DAC */
    RCC->APB1ENR |= RCC_APB1ENR_DACEN;

    /* ENABLE DAC CHANNEL 1 TRIGGER */
    DAC->CR |= DAC_CR_TEN1;

    /* NO WAVE GENERATION FOR DAC CHANNEL 1 */
    DAC->CR &= ~DAC_CR_WAVE1;
    /* NO TRIGGER */
    DAC->CR &= ~DAC_CR_TEN1;
    /* 3.3V */
    DAC->DHR12R1 |= 0xFFF;

    /* ENABLE DAC CHANNEL 1 */
    DAC->CR |= DAC_CR_EN1;
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

void configure_GPIO(void) {
    // USART1 pins are PA9 (TX) and PA10 (RX), so it's GPIO port A  DS p. 38
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    // set GPIO modes (RM p. 157)
    GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10 | GPIO_MODER_MODER5 | GPIO_MODER_MODER0);
    GPIOA->MODER |= _GPIO_MODER_RX; // PA9, PA10: alternate function
    GPIOA->MODER |= _GPIO_MODER_TX;
    GPIOA->MODER |= _GPIO_MODER_LED; // PA5: output
    GPIOA->MODER |= _GPIO_MODER_ADC; // PA0: analog
    // select AF1 for USART1						                DS p. 41
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFRH1 | GPIO_AFRH_AFRH2);
    GPIOA->AFR[1] |= (0x0001) << GPIO_AFRH_AFRH1_Pos;
    GPIOA->AFR[1] |= (0x0001) << GPIO_AFRH_AFRH2_Pos;
    // set speed to highest
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR9;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR10;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR5;
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR0;
}

void configure_UART(void) {
    /* SELECT HSI AS USART1 CLOCK SOURCE                            RM p. 699*/
    // enable HSI clock
    RCC->CR |= RCC_CR_HSION;
    // wait till HSI is ready
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {
        // add here timeout management
    }
    // select HSI as USART1 clock
    RCC->CFGR3 &= ~RCC_CFGR3_USART1SW;
    RCC->CFGR3 |= RCC_CFGR3_USART1SW_HSI;
    // USART1 clock enable
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
    
    /* CONFIGURE COMMUNICATION*/
    // 1 start bit, 8 data bits
    USART1->CR1 &= ~USART_CR1_M;
    // 1 stop bit
    USART1->CR2 &= ~USART_CR2_STOP;
    // no parity bit
    USART1->CR1 &= ~USART_CR1_PCE;

    /* CONFIGURE BAUD RATE                                          RM p. 703
     * clock source is HSI, which frequency is 8MHz                 DS p. 72
     * thus our f_CK from the formula baud = f_CK / USARTDIV is 8MHz
     * we will set oversampling to 16 and baud rade to 9600, so:
     * baud = f_CK / USARTDIV <=> USARTDIV = f_CK / baud
     * USARTDIV = 8000000 / 9600 = 833,(3)
     * since we oversample x16, BRR = USARTDIV                      RM p. 743
     */
    // oversampling by 16
    USART1->CR1 &= ~USART_CR1_OVER8;
    // baud rate = 9600
    USART1->BRR = 8000000 / 9600;
    
    /* START THE TRANSMISSION*/
    // turn on the whole USART, then transmitter and receiver
    USART1->CR1 |= USART_CR1_UE;
    USART1->CR1 |= USART_CR1_RE;
    USART1->CR1 |= USART_CR1_TE; 
    
    /* CONFIGURE INTERRUPTIONS AND NVIC                             RM p. 729
    // enable TC, TXE
    USART1->CR1 |= USART_CR1_TCIE | USART_CR1_TXEIE;
    // configure NVIC
    NVIC_SetPriority(USART1_IRQn, 0);
    NVIC_EnableIRQ(USART1_IRQn);
    */

    // clear Framing Error and Character Match flags
    USART1->ICR |= USART_ICR_FECF;
    // poll idle frame transmission
    while ((USART1->ISR & USART_ISR_TC) == 0) {
        /* add here timeout management */
    }
    // clear TC flag after idle frame transmition
    USART1->ICR |= USART_ICR_TCCF;
}
/*
void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_TC) {
        sent %= len;
        USART1->TDR = to_send[sent++];
    }
}
*/
void send_char(uint8_t data) {
    // wait till there's space in transmitter register
    while ((USART1->ISR & USART_ISR_TXE) == 0) {}
    // write
    USART1->TDR = data;
    // wait till it's finished
    while ((USART1->ISR & USART_ISR_TC) == 0) {}
}

void send_string(char* string, unsigned int length) {
    for (int i=0; i < length; i++) {
        send_char(string[i]);
    }
}

void itoa(unsigned short x, char string[4]) {
    char const digits[] = "0123456789";
    char* p = &string[3];

    while (*p) {
        *p-- = digits[x % 10];
        x /= 10;
    }
}


int main() {
    configure_GPIO();
    configure_GPIO_DAC();
    configure_ADC();
    configure_DMA();
    configure_UART();
    configure_DAC();

    while (1) {
        send_char(adc_val & 255);
        // 8640 ~ 0.1s
        delay(8640);
    }

    return 0;
}

