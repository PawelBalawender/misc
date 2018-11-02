#include <stm32f0xx.h>

#define LED_PIN (5U)
#define _GPIO_MODER_LED GPIO_MODER_MODER5_0
#define _GPIO_OSPEEDR_LED GPIO_OSPEEDR_OSPEEDR5
#define LED_ON() (GPIOA->BSRR |= (1U << LED_PIN))
#define LED_OFF() (GPIOA->BRR |= (1U << LED_PIN))
#define TOGGLE_LED() (GPIOA->ODR ^= (1U << LED_PIN))

#define _GPIO_MODER_I2C1_SDA GPIO_MODER_MODER0_1
#define _GPIO_MODER_I2C1_SCL GPIO_MODER_MODER1_1
#define _GPIO_AFRL_I2C1_SDA (1U << GPIO_AFRL_AFSEL0_Pos)
#define _GPIO_AFRL_I2C1_SCL (1U << GPIO_AFRL_AFSEL1_Pos)
/* I2C reserved addresses: 0 to 7, 120 to 127 (I2C-bus specification, p. 17)*/
#define _I2C1_OWN_ADDRESS (0x5A)
#define _I2C2_OWN_ADDRESS (0x4C)

volatile unsigned int n;

void delay(volatile unsigned int);
void configure_RCC(void);
void configure_GPIO_I2C(void);
void configure_GPIO_LED(void);
void configure_I2C(void);
void configure_I2C1_master(void);
void configure_I2C2_slave(void);
void send_char(char);

int main() {
    configure_RCC();
    configure_I2C();
    configure_GPIO_I2C();
    configure_GPIO_LED();
    configure_I2C1_master();
    configure_I2C2_slave();

    while(1) {
        if (I2C1->ISR & I2C_ISR_TXE) {
        I2C1->TXDR = (uint8_t)'A';
        I2C1->CR2 |= I2C_CR2_START;
        }

        if (I2C2->ISR & I2C_ISR_ADDR) I2C2->ICR |= I2C_ICR_ADDRCF;
        if (I2C2->ISR & I2C_ISR_RXNE) {
            I2C2->RXDR;
        }
    }

    return 0;
}

void delay(volatile unsigned int time) {
    n = time;
    while (n) asm("nop");
}

void configure_RCC(void) {
    /* SELECT FLASH LATENCY TO 1 WAIT STATE (IT'S FOR 48MHz SYSCLK) */
    FLASH->ACR &= ~(7 << FLASH_ACR_LATENCY_Pos);
    FLASH->ACR |= (1 << FLASH_ACR_LATENCY_Pos);
    
    /* ENABLE HSI48 */
    RCC->CR2 |= RCC_CR2_HSI48ON;
    while ((RCC->CR2 & RCC_CR2_HSI48RDY) == 0) {}
    /* SELECT HSI48 AS SYSTEM CLOCK */
    RCC->CFGR |= RCC_CFGR_SW;
    /* WAIT FOR IT TO SWITCH */
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI48) {}

    /* HCLK == SYSCLK, not divided */
    RCC->CFGR &= ~RCC_CFGR_HPRE;
    /* PCLK == HCLK, not divided */
    RCC->CFGR &= ~RCC_CFGR_PPRE;

    /* ENABLE CLOCK FOR GPIOA, GPIOB */
    RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;
    /* ENALE APB == PCLK == 48MHz CLOCK FOR I2C1 AND I2C2 */
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN | RCC_APB1ENR_I2C2EN;
    /* SELECT AF1 FOR PB6 AND PB7, AF5 FOR PB13 AND PB14 */
    GPIOB->AFR[0] &= ~(GPIO_AFRL_AFRL6 | GPIO_AFRL_AFRL7);
    GPIOB->AFR[1] &= ~(GPIO_AFRH_AFRH5 | GPIO_AFRH_AFRH6);
    GPIOB->AFR[0] |= (1 << (6 * 4)) | (1 << (7 * 4));
    GPIOB->AFR[1] |= (5 << (5 * 4)) | (5 << (6 * 4));
}

void configure_GPIO_I2C(void) {
    /* PORT: PB6 (I2C1_SCL), PB7 (I2C1_SDA), PB13 (I2C2_SCL), PB14(I2C2_SDA) */
    /* AF: AF1 (PB6, PB7), AF5 (PB13, PB14) */
    /* SELECT ALTERNATE FUNCTION MODE */
    GPIOB->MODER &= ~(GPIO_MODER_MODER6 | GPIO_MODER_MODER7);
    GPIOB->MODER &= ~(GPIO_MODER_MODER13 | GPIO_MODER_MODER14);
    GPIOB->MODER |= GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1;
    GPIOB->MODER |= GPIO_MODER_MODER13_1 | GPIO_MODER_MODER14_1;
    /* SELECT THE LOWEST SPEED */
    /* VDDIO for PB6, PB7, PB13 and PB14 is VDDIO1, which is equal to VDD,
     * which in turn is equal to 3.3V thus higher than 2V. Suppose that
     * the capacitive load of the circuit doesn't exceed 50pF, so
     * the IO timing is: max freq: 2MHz, max fall time = max rise time = 125ns
     * DS p. 14, 40, 49
     */
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR6 | GPIO_OSPEEDR_OSPEEDR7);
    GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEEDR13 | GPIO_OSPEEDR_OSPEEDR14);
    /* SELECT OPEN-DRAIN OUTPUT TYPE */
    GPIOB->OTYPER |= GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7;
    GPIOB->OTYPER |= GPIO_OTYPER_OT_13 | GPIO_OTYPER_OT_14;
    /* SELECT NO PULL-UP NOR PULL-DOWN */
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR6 | GPIO_PUPDR_PUPDR7);
    GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPDR13 | GPIO_PUPDR_PUPDR14);
}

void configure_GPIO_LED(void) {
    /* PORT: PA5 */
    /* SELECT OUTPUT MODE */
    GPIOA->MODER |= GPIO_MODER_MODER5_0;
    /* SELECT PUSH-PULL OUTPUT TYPE */
    GPIOA->OTYPER &= ~GPIO_OTYPER_OT_5;
    /* SELECT THE LOWEST SPEED */
    GPIOA->OSPEEDR &= ~GPIO_OSPEEDR_OSPEEDR5;
}

void configure_I2C(void) {
    /* CONFIGURE COMMON FACTORS OF BOTH I2Cs */
    /* SELECT SYSCLK == 48MHz AS I2C1 CLOCK
     * (clock of I2C2 is PCLK == 48Mhz)*/
    RCC->CFGR3 |= RCC_CFGR3_I2C1SW;
    /* RESET I2C1 AND I2C2 */
    // RCC->APB1RSTR |= RCC_APB1RSTR_I2C1RST | RCC_APB1RSTR_I2C2RST;
}

void configure_I2C1_master(void) {
    /* CONFIGURE I2C1 IN STANDARD MODE (100kb/s), 7-BIT ADDRESSING RM p. 626*/
    /* ENSURE IT'S DISABLED */ 
    //I2C1->CR1 &= ~I2C_CR1_PE;
    /* CONFIGURE THE NOISE FILTERS: ENABLE ANALOG FILTER, DISABLE DIGITAL */
    if ((I2C1->CR1 & I2C_CR1_PE) == 0) {
        I2C1->CR1 &= ~I2C_CR1_ANFOFF;
        I2C1->CR1 &= ~I2C_CR1_DNF;
    }

    /* CONFIGURE TIMING WITH AN4235 XLS FILE RM p. 623, 624
     * I2CCLK: 48MHz, RiseTime: 100ns, FallTime: 10ns, Frequency: 1000kHz,
     * Analog Filter ON;
     * Master: 0x00700818, Slave: 0x00700000
     */
    I2C1->TIMINGR = (uint32_t)0x00700818;
    
    /* ENABLE SCL STRETCHING */
    if ((I2C1->CR1 & I2C_CR1_PE) == 0) {
        //I2C1->CR1 &= ~I2C_CR1_NOSTRETCH;
    }



    /* CONFIGURE I2C
     *      autoend (1)
     *      write transfer (2)
     *      16 bytes to send (3)
     *      7-bit mode for own address 1 (4)
     *      slave address in a const (5); 7-bit so left-shift it by 1
     *      enable own address 1 (6)
     */
    //I2C1->CR2 |= I2C_CR2_AUTOEND; /* (1) */
    I2C1->CR2 |= I2C_CR2_AUTOEND | (1 << 16) | (_I2C2_OWN_ADDRESS << 1);
    I2C1->CR2 &= ~I2C_CR2_RD_WRN; /* (2) */
    if ((I2C1->CR2 & I2C_CR2_START) == 0) {
        //I2C1->CR2 &= ~I2C_CR2_NBYTES; /* (3) */
        //I2C1->CR2 |= (1 << I2C_CR2_NBYTES_Pos);
    }
    if ((I2C1->OAR1 & I2C_OAR1_OA1EN) == 0) {
        //I2C1->OAR1 &= ~I2C_OAR1_OA1MODE; /* (4) */
        //I2C1->CR2 &= ~I2C_CR2_ADD10; /* (4) also */
        //I2C1->OAR1 |= (_I2C1_OWN_ADDRESS << 1);
        //I2C1->CR2 &= ~I2C_CR2_SADD; /* (5) */
    
        // I2C2 is the slave
        //I2C1->CR2 |= (_I2C2_OWN_ADDRESS << 1);
    }
    //I2C1->OAR1 |= I2C_OAR1_OA1EN; /* (6) */

    /* ENABLE I2C */
    I2C1->CR1 |= I2C_CR1_PE;
    //I2C1->CR2 |= I2C_CR2_START;
}

void configure_I2C2_slave(void) {
    /* CONFIGURE I2C1 IN STANDARD MODE (100kb/s), 7-BIT ADDRESSING RM p. 626*/
    /* ENSURE IT'S DISABLED */ 
    //I2C2->CR1 &= ~I2C_CR1_PE;
    /* CONFIGURE THE NOISE FILTERS: ENABLE ANALOG FILTER, DISABLE DIGITAL */
    if ((I2C2->CR1 & I2C_CR1_PE) == 0) {
        I2C2->CR1 &= ~I2C_CR1_ANFOFF;
        I2C2->CR1 &= ~I2C_CR1_DNF;
    }

    /* CONFIGURE TIMING WITH AN4235 XLS FILE RM p. 623, 624
     * I2CCLK: 48MHz, RiseTime: 100ns, FallTime: 10ns, Frequency: 1000kHz,
     * Analog Filter ON;
     * Master: 0x00700818, Slave: 0x00700000
     */
    I2C2->TIMINGR = (uint32_t)0x00700000;

    /* SELECT AND ENABLE OWN ADDRESS; << 1 CAUSE IT'S 7-BIT */
    I2C2->OAR1 |= (uint32_t)(_I2C2_OWN_ADDRESS << 1);
    I2C2->OAR1 |= I2C_OAR1_OA1EN;

    /* ENABLE I2C2 */
    I2C2->CR1 |= I2C_CR1_PE;
}

void send_char(char c) {
    if (I2C1->ISR & I2C_ISR_TXE) {
        I2C1->TXDR = (uint8_t)c;
        I2C1->CR2 |= I2C_CR2_START;
    }
}

