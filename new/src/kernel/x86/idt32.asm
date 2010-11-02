[BITS 32]

;This loads the IDT
global idt_load
extern g_idtp
idt_load:
    lidt [g_idtp]
    ret