[BITS 32]

%include "gdt32.mac"

REAL_MODE_STACK_ADDR equ 0x2000

extern g_gdtp
extern g_idtp

global real_to_prot
global prot_to_real

prot_mode_stack_ptr:
    dd 0

real_mode_idt:
    dw 0x03FF
    dd 0x00000000

real_to_prot:
	[BITS 16]
	cli

	; load the GDT register
	xor	ax, ax
	mov	ds, ax
	lgdt	[dword g_gdtp]

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
	mov	[esp], eax

	; zero eax */
	xor	eax, eax

        ; Restore IDT
        lidt [g_idtp]

	; return on the old (or initialized) stack!
	ret

prot_to_real:
	; just in case, set GDT
	lgdt	[g_gdtp]

	; save the protected mode stack
	mov	eax, esp
	mov	[prot_mode_stack_ptr], eax

	; get the return address
	mov	eax, [esp]
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
        lidt [dword real_mode_idt]
	sti

	; 32 bit return on new stack!
	db 0x66
	db 0xC3
	[BITS 32]


