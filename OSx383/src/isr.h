#ifndef ISR_H
#define ISR_H
#include "common.h"

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

typedef struct registers
{
    u32int ds;                                              // Data segment selector
    u32int edi, esi, ebp, esp, ebx, edx, ecx, eax;          // Pushed by `pusha`
    u32int int_no, err_code;                                // Interrupt number and error code (if applicable)
    u32int eip, cs, eflags, useresp, ss;                    // Pushed by the processor automatically.
} registers_t;

typedef void (*isr_t)(registers_t);
void register_interrupt_handler(u8int n, isr_t handler);
#endif
