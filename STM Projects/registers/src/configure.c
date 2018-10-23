/* This module contains configuring functions for the kisaburo project
 * author: Pawel Balawender
 * contact: voilagrange@gmail.com
 */
#include<stm32f0xx.h>
#include"configure.h"


void configure_GPIO_LD2(void) {
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    GPIOA->MODER |= GPIO_MODER_MODER5_0;
}


void configure_GPIO_USART1(void) {
    /* USART1_TX IS PA9 AND USART1_RX IS PA10 */
    /* SELECT ALTERNATE FUNCTION MODE FOR USART1_TX AND USART1_RX */
    GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10);
    GPIOA->MODER |= (GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1);
    /* SELECT AF1 FUNCTION FOR USART1_TX AND USART1_RX */
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFRH1 | GPIO_AFRH_AFRH2);
    GPIOA->AFR[1] |= ((0x0001U) << GPIO_AFRH_AFRH1_Pos);
    GPIOA->AFR[1] |= ((0x0001U) << GPIO_AFRH_AFRH2_Pos);
    /* SELECT THE HIGHEST SPEED FOR USART1_TX AND USART1_RX */
    GPIOA->OSPEEDR |= (GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10);
}


void configure_USART1(void) {
    /* SELECT PCLK AS USART1 CLOCK */
    RCC->CFGR3 &= ~RCC_CFGR3_USART1SW;
    RCC->CFGR3 |= RCC_CFGR3_USART1SW_PCLK;
    /* ENABLE USART1 */
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* M: 1 START BIT, 8 DATA BITS
     * PCE: NO PARITY CONTROL
     * OVER8: OVERSAMPLE x16
     */
    USART1->CR1 &= ~(USART_CR1_M | USART_CR1_PCE | USART_CR1_OVER8);
    // USART1->CR1 |= USART_CR1_M0;
    /* 1 STOP BIT */
    USART1->CR2 &= ~USART_CR2_STOP;
    /* SET BAUD RATE (07.10.2018: 9600, but may change) */
    USART1->BRR = APB_FREQ / USART1_BAUD_RATE; /* RM p. 703, 743 */
    /* ENABLE DMA FOR TRANSMITTER & RECEIVER */
    // USART1->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;
    /* NO OVERRUN INTERRUPT */
    USART1->CR3 |= USART_CR3_OVRDIS;
    /* FULL-DUPLEX */
    USART1->CR3 &= ~USART_CR3_HDSEL;
    /* START TRANSMITTER & RECEIVER */
    USART1->CR1 |= USART_CR1_TE | USART_CR1_RE;
    USART1->CR1 |= USART_CR1_UE;
    
    /* POLL IDLE FRAME TRANSMISSION AND CLEAR TC FLAG AFTER IT */
    while ((USART1->ISR & USART_ISR_TC) == 0) {}
    USART1->ICR |= USART_ICR_TCCF;
}


void configure_IRQ_USART1(void) {
    // USART1->CR1 |= USART_CR1_TXEIE;
    // USART1->CR1 |= USART_CR1_TCIE;
    USART1->CR1 |= USART_CR1_RXNEIE;
    // USART1->CR1 |= USART_CR1_IDLEIE;
    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn, 0);
}


void configure_DMAT(char* address_mem) {
    RCC->AHBENR |= RCC_AHBENR_DMAEN;
    DMA_Channel_TypeDef *dma = DMA1_Channel2;
    DMA1->CSELR &= ~DMA_CSELR_C2S;
    DMA1->CSELR |= (0x1000U << DMA_CSELR_C2S_Pos);

    if ((dma->CCR & DMA_CCR_EN) == 0) {
        dma->CMAR = (uint32_t)(address_mem);
        dma->CPAR = (uint32_t)(&(USART1->TDR));
        dma->CNDTR &= ~DMA_CNDTR_NDT;
        dma->CNDTR |= (4U << DMA_CNDTR_NDT_Pos);
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
    //dma->CCR &= ~DMA_CCR_MINC; /* (4b) */
    dma->CCR |= DMA_CCR_MINC;
    dma->CCR &= ~DMA_CCR_PSIZE; /* (5) */
    // dma->CCR |= DMA_CCR_PSIZE_0;
    dma->CCR &= ~DMA_CCR_MSIZE; /* (6) */
    // dma->CCR |= DMA_CCR_MSIZE_0;
   
    /* TURN DMA ON */
    dma->CCR |= DMA_CCR_EN;
}


void configure_DMAR(char* address_mem) {
    RCC->AHBENR |= RCC_AHBENR_DMAEN;
    DMA_Channel_TypeDef *dma = DMA1_Channel1;
    DMA1->CSELR &= ~DMA_CSELR_C1S;
    DMA1->CSELR |= (0x1000U << DMA_CSELR_C1S_Pos);

    if ((dma->CCR & DMA_CCR_EN) == 0) {
        dma->CMAR = (uint32_t) (address_mem);
        dma->CPAR = (uint32_t) (&(USART1->RDR));
        dma->CNDTR &= ~DMA_CNDTR_NDT;
        dma->CNDTR |= (1U << DMA_CNDTR_NDT_Pos);
    }
    /* CHANNEL PRIORITY: HIGHEST(1),
     * TRANSFER DIRECTION: PERIPH TO MEM (2),
     * CIRCULAR MODE (3),
     * DON'T INCREMENT MEM ADDRESS (4),
     * DON'T INCREMENT PERIPH ADDRESS (5),
     * MEM DATA SIZE: 8 (6),
     * PERIPH DATA SIZE: 8 (7)
     */

    dma->CCR |= DMA_CCR_MEM2MEM;
    // dma->CCR &= ~DMA_CCR_PL; /* (1) */
    dma->CCR |= DMA_CCR_PL;
    dma->CCR &= ~DMA_CCR_DIR; /* (2) */
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
    /* TURN ON HSI48 (48MHz) */
    RCC->CR2 |= RCC_CR2_HSI48ON;
    while ((RCC->CR2 & RCC_CR2_HSI48RDY) == 0) {
        /* time-out */
    }

    /* SELECT HSI48 (48MHz) AS SYSTEM CLOCK (SYSCLK) */
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI48;
    /* ALIGN FLASH LATENCY TO THE SYSTEM CLOCK FREQUENCY */
    FLASH->ACR &= ~FLASH_ACR_LATENCY;
    /* SELECT SYSCLK/4 (12MHz) FOR HCLK (AHB) CLOCK*/
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    RCC->CFGR |= RCC_CFGR_HPRE_DIV4;
    /* SELECT HCLK WITHOUT PRESCALER (12MHz) FOR PCLK (APB) CLOCK */
    RCC->CFGR &= ~RCC_CFGR_PPRE;
}
