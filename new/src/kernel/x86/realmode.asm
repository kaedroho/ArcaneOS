[BITS 32]

%include "gdt32.mac"

REAL_MODE_STACK_ADDR equ 0x2000

prot_mode_stack_ptr:
    dd 0

extern g_gdtp

real_to_prot:
	[BITS 16]
	cli

	/* load the GDT register */
	xor	ax, ax
	mov	ds, ax
	lgdt	g_gdtp

	/* turn on protected mode */
	mov	eax, cr0
	or	eax, CPU_CR0_PE_ON
	mov	cr0, eax

	/* jump to relocation, flush prefetch queue, and reload cs */
	jmp	GDT_KERNEL_PROT_MODE_CSEG:protcseg

	[BITS 32]
protcseg:
	/* reload other segment registers */
	mov	ax, GDT_KERNEL_PROT_MODE_DSEG
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	/* put the return address in a known safe location */
	mov	eax, [esp]
	mov	[REAL_MODE_STACK_ADDR], eax

	/* get protected mode stack */
	mov	eax, [prot_mode_stack_ptr]
	mov	esp, eax
	mov	ebp, eax

	/* get return address onto the right stack */
	mov	eax, [REAL_MODE_STACK_ADDR]
	mov	[esp], eax

	/* zero eax */
	xor	eax, eax

	/* return on the old (or initialized) stack! */
	ret

prot_to_real:
	/* just in case, set GDT */
	lgdt	g_gdtp

	/* save the protected mode stack */
	mov	eax, esp
	mov	[prot_mode_stack_ptr], eax

	/* get the return address */
	mov	eax, [esp]
	mov	[REAL_MODE_STACK_ADDR], eax

	/* set up new stack */
	mov	eax, REAL_MODE_STACK_ADDR
	mov	esp, eax
	mov	ebp, eax

	/* set up segment limits */
	mov	ax, GDT_KERNEL_REAL_MODE_DSEG
	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	/* this might be an extra step */
	/* jump to a 16 bit segment */
	jmp	GDT_KERNEL_REAL_MODE_CSEG:tmpcseg

tmpcseg:
	[BITS 16]

	/* clear the PE bit of CR0 */
	mov	eax, cr0
	and 	eax, ~CPU_CR0_PE_ON
	mov	cr0, eax

	/* flush prefetch queue, reload cs */
	jmp	0:realcseg

realcseg:
	/* we are in real mode now
	 * set up the real mode segment registers : DS, SS, ES
	 */
	/* zero eax */
	xor	eax, eax

	mov	ds, ax
	mov	es, ax
	mov	fs, ax
	mov	gs, ax
	mov	ss, ax

	/* restore interrupts */
	sti

	/* return on new stack! */
	ret

	[BITS 32]

