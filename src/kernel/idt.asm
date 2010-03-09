[BITS 32]

;This loads the IDT
global idt_load
extern idtp
idt_load:
    lidt [idtp]
    ret
