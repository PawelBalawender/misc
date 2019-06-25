#include<stm32f0xx.h>
#include"arm_math.h"
#include"../../src/math/arm_sin_f32.c"
#include"../../src/math/arm_cos_f32.c"
#define RADIUS (5U)
#define CIRC (6.28 * RADIUS)

volatile uint32_t x, y, next_x, next_y, dest_x, dest_y;
uint8_t v;


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
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;

    /* POLL IDLE FRAME TRANSMISSION AND CLEAR TC FLAG AFTER IT */
    while ((USART1->ISR & USART_ISR_TC) == 0) {}
    USART1->ICR |= USART_ICR_TCCF;
}

void configure_IRQ_USART(void) {
    USART1->CR1 |= USART_CR1_TXEIE;
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 0);
}

void configure_DMA(uint32_t address_mem, uint32_t address_periph) {
    DMA_Channel_TypeDef* dma = DMA1_Channel1;
    if ((dma->CCR & DMA_CCR_EN) == 0) {
        dma->CMAR = (uint32_t)(address_mem);
        dma->CPAR = (uint32_t)(address_periph);
        dma->CNDTR &= ~DMA_CNDTR_NDT;
        dma->CNDTR |= (1U << DMA_CNDTR_NDT_Pos);
    }
    /* CHANNEL PRIORITY: HIGHEST(1),
     * TRANSFER DIRECTION: MEM TO PERIPH (2),
     * CIRCULAR MODE (3),
     * DON'T INCREMENT MEM ADDRESS (4),
     * DON'T INCREMENT PERIPH ADDRESS (5),
     * MEM DATA SIZE: 8 (6),
     * PERIPH DATA SIZE: 8 (7)
     */
    
    dma->CCR |= DMA_CCR_MEM2MEM;
    // dma->CCR &= ~DMA_CCR_PL; /* (1) */
    dma->CCR |= DMA_CCR_PL;
    dma->CCR |= DMA_CCR_DIR; /* (2) */
    dma->CCR |= DMA_CCR_CIRC; /* (3) */
    dma->CCR &= ~DMA_CCR_PINC; /* (4a) */
    dma->CCR &= ~DMA_CCR_MINC; /* (4b) */
    dma->CCR &= ~DMA_CCR_PSIZE; /* (5) */
    // dma->CCR |= DMA_CCR_PSIZE_0;
    dma->CCR &= ~DMA_CCR_MSIZE; /* (6) */
    // dma->CCR |= DMA_CCR_MSIZE_0;

    /* TURN DMA ON */
    dma->CCR |= DMA_CCR_EN;
}

    
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

void delay(unsigned int time) {
    while (time--) __NOP();
}


void send_char(uint8_t c) {
    while ((USART1->ISR & USART_ISR_TXE) == 0) {}
    USART1->TDR = c;
}

void send_int(uint32_t num) {
    uint32_t mask = 0xFF000000;
    for (int i=3; i>=0; i--) { 
        send_char(((num & mask) >> (i * 8)));
        mask >>= 8;
    }
}

/* fmod from arm_math is broken */
float _fmod(float a, float b) {
    while (a > b) a -= b;
    return a;
}

int main() {
    configure_RCC_FLASH();
    configure_USART();
    configure_GPIO_USART();
    // configure_IRQ_USART();
    // configure_DMA((uint32_t)(&x), (uint32_t)(&(USART1->TDR)));

    unsigned int x_ok, y_ok, x_min, y_min;
    float car_dir;
    v = 10;
    x_min = y_min = v;
    uint8_t x_max, y_max;
    x_max = y_max = -1-v;
    car_dir = PI / 4.0 - 0.1;
    x_ok = y_ok = 0;

    x = x_min + 1;
    y = y_min + 1;

    while(1) {
        send_int(x); send_int(y); send_char('\n');

        next_x = x + arm_cos_f32(car_dir) * v;
        next_y = y + arm_sin_f32(car_dir) * v;

        x_ok = (x_min < next_x && next_x < x_max);
        y_ok = (y_min < next_y && next_y < y_max);
        
        if (x_ok == 0) {car_dir *= -1; car_dir += PI; continue;}
        if (y_ok == 0) {car_dir *= -1; continue;}

        x = next_x;
        y = next_y;
        
        delay(75000);
    }

    return 0;
}


