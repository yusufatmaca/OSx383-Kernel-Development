#include "common.h"
#include "descriptor_tables.h"
#include "isr.h"


extern void gdt_flush(u32int);
extern void idt_flush(u32int);

static void init_gdt();
static void init_idt();
static void gdt_set_gate(s32int, u32int, u32int, u8int, u8int);
static void idt_set_gate(u8int, u32int, u16int, u8int);

gdt_entry_t gdt_entries[5];
gdt_ptr_t   gdt_ptr;
idt_entry_t idt_entries[256];
idt_ptr_t   idt_ptr;

extern isr_t interrupt_handlers[];

// Initialize descriptor tables
void init_descriptor_tables() {
    init_gdt();
    init_idt();
    memset(&interrupt_handlers, 0, sizeof(isr_t) * 256);
}

// Initialize Global Descriptor Table
static void init_gdt() {
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base  = (u32int)&gdt_entries;

    gdt_set_gate(0, 0, 0, 0, 0);                    // Null segment
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);     // Code segment
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);     // Data segment
    gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);     // User mode code segment
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);     // User mode data segment

    gdt_flush((u32int)&gdt_ptr);
}

// Set a single GDT entry
static void gdt_set_gate(s32int num, u32int base, u32int limit, u8int access, u8int gran) {
    gdt_entries[num].base_low    = (base & 0xFFFF);
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high   = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low   = (limit & 0xFFFF);
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    
    gdt_entries[num].granularity |= gran & 0xF0;
    gdt_entries[num].access      = access;
}

// Initialize Interrupt Descriptor Table
static void init_idt() {
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_ptr.base  = (u32int)&idt_entries;

    memset(&idt_entries, 0, sizeof(idt_entry_t)*256);

    /*  ### Remap the IRQ table
        The PIC remapping involves sending commands and data to the PIC's command and data ports to change the IRQ numbers so they don't overlap with the CPU exceptions (0-31).
        
        * Master    - command: 0x20, data: 0x21
        * Slave     - command: 0xA0, data: 0xA1

        `outb(0x20, 0x11)` and `outb(0xA0, 0x11)` start the initialization of the master and slave PICs.
        `outb(0x21, 0x20)` and `outb(0xA1, 0x28)` set the new interrupt vectors for the master (0x20) and slave (0x28) PICs.
        `outb(0x21, 0x04)` and `outb(0xA1, 0x02)` configure the cascading between master and slave PICs.
        `outb(0x21, 0x01)` and `outb(0xA1, 0x01)` set the PICs to operate in 8086/88 mode.
        `outb(0x21, 0x0)` and `outb(0xA1, 0x0)` reset the data registers.
    */
    outb(0x20, 0x11);
    outb(0xA0, 0x11);
    outb(0x21, 0x20);
    outb(0xA1, 0x28);
    outb(0x21, 0x04);
    outb(0xA1, 0x02);
    outb(0x21, 0x01);
    outb(0xA1, 0x01);
    outb(0x21, 0x0);
    outb(0xA1, 0x0);

    /*
    These lines set up the IDT gates for each ISR (Interrupt Service Routine) and IRQ (Interrupt Request) handler.
    
    The idt_set_gate function sets up each IDT entry with the handler address, segment selector (`0x08` for the kernel code segment), and access flags (`0x8E` for *present*, *ring 0*, `32-bit` interrupt gate).
    */

    idt_set_gate(0 , (u32int)isr0 , 0x08, 0x8E);
    idt_set_gate(1 , (u32int)isr1 , 0x08, 0x8E);
    idt_set_gate(2 , (u32int)isr2 , 0x08, 0x8E);
    idt_set_gate(3 , (u32int)isr3 , 0x08, 0x8E);
    idt_set_gate(4 , (u32int)isr4 , 0x08, 0x8E);
    idt_set_gate(5 , (u32int)isr5 , 0x08, 0x8E);
    idt_set_gate(6 , (u32int)isr6 , 0x08, 0x8E);
    idt_set_gate(7 , (u32int)isr7 , 0x08, 0x8E);
    idt_set_gate(8 , (u32int)isr8 , 0x08, 0x8E);
    idt_set_gate(9 , (u32int)isr9 , 0x08, 0x8E);
    idt_set_gate(10, (u32int)isr10, 0x08, 0x8E);
    idt_set_gate(11, (u32int)isr11, 0x08, 0x8E);
    idt_set_gate(12, (u32int)isr12, 0x08, 0x8E);
    idt_set_gate(13, (u32int)isr13, 0x08, 0x8E);
    idt_set_gate(14, (u32int)isr14, 0x08, 0x8E);
    idt_set_gate(15, (u32int)isr15, 0x08, 0x8E);
    idt_set_gate(16, (u32int)isr16, 0x08, 0x8E);
    idt_set_gate(17, (u32int)isr17, 0x08, 0x8E);
    idt_set_gate(18, (u32int)isr18, 0x08, 0x8E);
    idt_set_gate(19, (u32int)isr19, 0x08, 0x8E);
    idt_set_gate(20, (u32int)isr20, 0x08, 0x8E);
    idt_set_gate(21, (u32int)isr21, 0x08, 0x8E);
    idt_set_gate(22, (u32int)isr22, 0x08, 0x8E);
    idt_set_gate(23, (u32int)isr23, 0x08, 0x8E);
    idt_set_gate(24, (u32int)isr24, 0x08, 0x8E);
    idt_set_gate(25, (u32int)isr25, 0x08, 0x8E);
    idt_set_gate(26, (u32int)isr26, 0x08, 0x8E);
    idt_set_gate(27, (u32int)isr27, 0x08, 0x8E);
    idt_set_gate(28, (u32int)isr28, 0x08, 0x8E);
    idt_set_gate(29, (u32int)isr29, 0x08, 0x8E);
    idt_set_gate(30, (u32int)isr30, 0x08, 0x8E);
    idt_set_gate(31, (u32int)isr31, 0x08, 0x8E);

    idt_set_gate(32, (u32int)irq0 , 0x08, 0x8E);
    idt_set_gate(33, (u32int)irq1 , 0x08, 0x8E);
    idt_set_gate(34, (u32int)irq2 , 0x08, 0x8E);
    idt_set_gate(35, (u32int)irq3 , 0x08, 0x8E);
    idt_set_gate(36, (u32int)irq4 , 0x08, 0x8E);
    idt_set_gate(37, (u32int)irq5 , 0x08, 0x8E);
    idt_set_gate(38, (u32int)irq6 , 0x08, 0x8E);
    idt_set_gate(39, (u32int)irq7 , 0x08, 0x8E);
    idt_set_gate(40, (u32int)irq8 , 0x08, 0x8E);
    idt_set_gate(41, (u32int)irq9 , 0x08, 0x8E);
    idt_set_gate(42, (u32int)irq10, 0x08, 0x8E);
    idt_set_gate(43, (u32int)irq11, 0x08, 0x8E);
    idt_set_gate(44, (u32int)irq12, 0x08, 0x8E);
    idt_set_gate(45, (u32int)irq13, 0x08, 0x8E);
    idt_set_gate(46, (u32int)irq14, 0x08, 0x8E);
    idt_set_gate(47, (u32int)irq15, 0x08, 0x8E);

    idt_flush((u32int)&idt_ptr);
}

// Set a single IDT entry
static void idt_set_gate(u8int num, u32int base, u16int sel, u8int flags) {
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    idt_entries[num].flags   = flags /* | 0x60 */; // Uncomment for user mode
}
