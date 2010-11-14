[BITS 32]

%include "x86/gdt32.mac"

REAL_MODE_STACK_ADDR equ real_mode_stack
GDT_COUNT equ 8
GDT_SIZE equ (GDT_COUNT*8)

extern g_gdtp
extern g_idtp
extern g_gdt
extern irq_remap
extern pg_push_state
extern pg_pop_state
extern pg_disable_paging

global real_to_prot
global prot_to_real
global real_mode_gdt

prot_mode_stack_ptr:
    dd 0

real_mode_idt:
    dw 0x03FF
    dd 0x00000000

real_mode_gdt_ptr:
    dw GDT_SIZE-1
    dd g_gdt

real_to_prot:
	[BITS 16]
	cli

	; load the GDT register
	xor	ax, ax
	mov	ds, ax
	lgdt	[real_mode_gdt_ptr]

	; turn on protected mode
	mov	eax, cr0
	or	eax, CPU_CR0_PE_ON
	mov	cr0, eax

	; jump to relocation, flush prefetch queue, and reload cs
	jmp dword GDT_KERNEL_PROT_MODE_CSEG:protcseg

	[BITS 32]
protcseg:
	; reload other segment registers
	mov	ax, GDT_KERNEL_PROT_MODE_DSEG
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	; put the return address in a known safe location
	mov	eax, [esp]
	mov	[REAL_MODE_STACK_ADDR], eax

	; get protected mode stack
	mov	eax, [prot_mode_stack_ptr]
	mov	esp, eax
	mov	ebp, eax

	; get return address onto the right stack
	mov	eax, [REAL_MODE_STACK_ADDR]
	mov	[esp+4], eax

	; zero eax */
	xor	eax, eax

        ; Restore IDT
        lidt [g_idtp]

        push eax
        push ecx
        push edx
        call irq_remap
        call pg_pop_state
        pop edx
        pop ecx
        pop eax

        ; Restore flags
        popf

	; return on the old (or initialized) stack!
	ret

prot_to_real:
        ; Save flags
        pushf

        push eax
        push ecx
        push edx
        call pg_push_state
        call pg_disable_paging
        pop edx
        pop ecx
        pop eax

	; just in case, set GDT
	lgdt	[real_mode_gdt_ptr]

	; save the protected mode stack
	mov	eax, esp
	mov	[prot_mode_stack_ptr], eax

	; get the return address
	mov	eax, [esp+4]
	mov	[REAL_MODE_STACK_ADDR], eax

	; set up new stack
	mov	eax, REAL_MODE_STACK_ADDR
	mov	esp, eax
	mov	ebp, eax

	; set up segment limits
	mov	ax, GDT_KERNEL_REAL_MODE_DSEG
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	; this might be an extra step
	; jump to a 16 bit segment
	jmp GDT_KERNEL_REAL_MODE_CSEG:tmpcseg

	[BITS 16]
tmpcseg:

	; clear the PE bit of CR0
	mov	eax, cr0
	and 	eax, ~CPU_CR0_PE_ON
	mov	cr0, eax

	; flush prefetch queue, reload cs
	jmp dword 0x0:realcseg

realcseg:
	; we are in real mode now
	; set up the real mode segment registers : DS, SS, ES
	;
	; zero eax
	xor	eax, eax

	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	; restore interrupts
        lidt [real_mode_idt]

	sti

        ret
	[BITS 32]



    times 1024 db 0
real_mode_stack:
    times 16 db 0
