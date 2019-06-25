#include"math/_math.h"
#include"common.h"
#include"configure.h"
#define DELIM (0xFF0FU)

volatile char c;
volatile uint32_t data, buf;
volatile int index, in_delimiter;

// void DMA1_Ch2_3_DMA2_Ch1_2_IRQHandler(void);
void USART1_IRQHandler(void);
void asend_int(int);

 
int main() {
    configure_GPIO_LD2();
    configure_USART1();
    configure_GPIO_USART1();
    configure_IRQ_USART1();
    // configure_DMAT(to_send);
    // configure_DMAR(&data);

    uint16_t x, y;
    uint16_t dest_x, dest_y;
    uint16_t velocity;
    float angle;
    
    angle = x = y = 0;
    velocity = 1000;

    while(1){
        dest_x = data >> 16;
        dest_y = data & ((2 << 16) - 1U);
        angle = atan2_(dest_y-y, dest_x-x);

        y += velocity * sin_(angle);
        x += velocity * cos_(angle);

        asend_int(x << 16 | y);
        send_char('\0');
        send_char('\n');
        send_char('\0');
    }
    
    return 0;
}

void USART1_IRQHandler(void) {
    if (USART1->ISR & USART_ISR_RXNE) {
        c = USART1->RDR;
        if (c == 0x0FU && in_delimiter) {
            // if data in buffer is correct, push it
            if (index == 0) data = buf;
            // restart receive variables
            index = 24;
            buf = 0;
            in_delimiter = 0;
        } else if (c == 0xFFU && (in_delimiter == 0)) {
            in_delimiter = 1;
        } else {
            if (in_delimiter) {
                // something went wrong, restart buffer and variables
                index = 24;
                buf = 0;
                in_delimiter = 0;
            }
            buf |= (c << index);
            // will be set to 24 in IF above otherwise
            if (index) index -= 8;
        }
    } else {
        LED_ON();
        while(1) {}
    }

}

void asend_int(int x) {
    for(int i=0; i < 4; i++) {
       send_char((char)(x & 255));
       x >>= 8;
    }
}

