[bits 16]
[org 0x7C00]

r_entry:
        ; We are already in 16-bit mode here!
        ; Interrupts should already be disabled!
	push ebx
	mov eax, esp
	mov [r_save_esp], eax
	mov ebx, [esp+8]        ; Get ID of function to call

	mov eax, [esp+12]        ; Get parameters
	mov [r_params+0], eax
	mov eax, [esp+16]
	mov [r_params+4], eax
	mov eax, [esp+20]
	mov [r_params+8], eax
	mov eax, [esp+24]
	mov [r_params+12], eax

	; Need 16-bit Protected Mode GDT entries!
	mov eax, 7	; 16-bit Protected Mode data selector.
	mov ds, eax
	mov es, eax
	mov fs, eax
	mov gs, eax

	; Disable paging (we need everything to be 1:1 mapped).
	mov eax, cr0
	mov [r_save_cr0], eax	; save pmode CR0
	and eax, 0x7FFFFFFe	; Disable paging bit & enable 16-bit pmode.
	mov cr0, eax

	jmp 0:r_enter		; Perform Far jump to set CS.

r_idt:
	dw 0x3ff		; 256 entries, 4b each = 1K
	dd 0			; Real Mode IVT @ 0x0000

r_save_cr0:
	dd 0			; Storage location for pmode CR0.
r_save_esp:
	dd 0			; Storage location for pmode ESP.
r_params:
	times 4 dd 0	; Storage location for parameters.

r_commandlist:
    dw r_test

r_enter:
	mov sp, 0x8000		; pick a stack pointer.
	mov ax, 0		; Reset segment registers to 0.
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	lidt [r_idt]
	sti			; Restore interrupts -- be careful, unhandled int's will kill it.
	shl bx,1
	jmp [r_commandlist+bx]

r_leave:
    mov eax, [r_save_cr0]
    mov cr0, eax
    jmp 0x8:r_exit

r_exit:
	mov eax, [r_save_esp]
	mov esp, eax
	pop ebx
	xor eax, eax
	mov ax, cx
	ret


r_test:
    jmp r_leave
