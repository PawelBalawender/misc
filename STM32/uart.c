#include <stm32f0xx.h>
#define LED_PIN (5U)
#define len (15U)
#define LED_ON() (GPIOA->BSRR |= (1 << LED_PIN))
#define LED_OFF() (GPIOA->BRR |= (1 << LED_PIN))

volatile unsigned int n;
unsigned int sent = 0;
char to_send[len] = "Hello, World!\r\n";

void delay(volatile unsigned int);
void conf_gpio(void);
void conf_usart(void);
void USART1_IRQHandler(void);
void send_char(uint8_t data);


void delay(volatile unsigned int time) {
    n = time;
    while (n) {n--;}
}

void conf_gpio(void) {
    // USART1 pins are PA9 (TX) and PA10 (RX), so it's GPIO port A  DS p. 38
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
    // set PA9, PA10 as alternate function, PA5 (LD2) as output
    GPIOA->MODER &= ~(GPIO_MODER_MODER9 | GPIO_MODER_MODER10 | GPIO_MODER_MODER5);
    GPIOA->MODER |= GPIO_MODER_MODER9_1 | GPIO_MODER_MODER10_1;
    GPIOA->MODER |= GPIO_MODER_MODER5_0;
    // select AF1 for USART1						                DS p. 41
    GPIOA->AFR[1] &= ~(GPIO_AFRH_AFRH1 | GPIO_AFRH_AFRH2);
    GPIOA->AFR[1] |= (0x0001) << GPIO_AFRH_AFRH1_Pos;
    GPIOA->AFR[1] |= (0x0001) << GPIO_AFRH_AFRH2_Pos;
    // set speed to highest
    GPIOA->OSPEEDR |= GPIO_OSPEEDR_OSPEEDR9 | GPIO_OSPEEDR_OSPEEDR10 | GPIO_OSPEEDR_OSPEEDR5;
}

void conf_usart(void) {
    /* SELECT HSI AS USART1 CLOCK SOURCE                            RM p. 699 */
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

void send_string(uint8_t* string, unsigned int length) {
    for (int i=0; i < length; i++) {
        send_char(string[i]);
    }
    delay(3 * 72000);
}

int main() {
    
    conf_gpio();
    conf_usart();

    while (1) {
        send_string(to_send, len);
    }

    return 0;
}

