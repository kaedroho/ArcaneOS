[BITS 32]

;This loads the IDT
global idt_load
extern idt_idtp
idt_load:
    lidt [idt_idtp]
    ret