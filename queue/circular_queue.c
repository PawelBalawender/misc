
/* Low-level circular queue data structure implementation for ARMv6-M
 * Issue: ARMv6-M doesn't support CLZ instruction
 * Since we don't have CLZ we'll use binary search with a little lookup-table
 */
#include<stdint.h>
#include<assert.h>
#include<stdio.h>
#define QUEUE_MAX (32U)

typedef unsigned int bool;
typedef struct {
    uint32_t CNTRL;
    int buffer[QUEUE_MAX];
} CircularQueue;

static const unsigned int clz_lookup[16] = {
    4, 3, 2, 2, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0
};

unsigned int clz32(uint32_t x);
void test_clz32(void);

bool is_empty(CircularQueue *Queue);
bool is_full(CircularQueue *Queue);
void enqueue(CircularQueue *Queue, int data);
int dequeue(CircularQueue *Queue);

int main() {
	printf("%s", "Pwele!");
    test_clz32();
    return 0;
}

unsigned int clz32(uint32_t x) {
    unsigned int n=0;
    if ((x & 0xFFFF0000) == 0) {n = 16; x <<= 16;}
    if ((x & 0xFF000000) == 0) {n += 8; x <<= 8;}
    if ((x & 0xF0000000) == 0) {n += 4; x <<= 4;}
    return n + clz_lookup[x >> (32 - 4)];
}

void test_clz32(void) {
    assert(clz32(0xFFFFFFFF) == 0);
    assert(clz32(0x80000000) == 0);
    assert(clz32(0x00000000) == 32);
    assert(clz32(0x00000001) == 31);
    assert(clz32(0x00000010) == 27);
    assert(clz32(0x00001000) == 19);
    assert(clz32(0x00010000) == 15);
    assert(clz32(0x00010001) == 15);
}

void enqueue(CircularQueue *Queue, int data) {
    /* Asm: mov r3, #1, ldr r2, =Queue [no padding before 1st element!],
     * rors r2, r3, [now check carry and handle it], str r2, =Queue
     */
}

/* RORS: right rotation, updates N, Z and C flags. C flag is updated to the
 * last bit shifted out, except when the shift length is 0; PM p. 52
 */
bool is_empty(CircularQueue *Queue) {
	return 1;
}

