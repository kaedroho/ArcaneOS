; Exported Function
[ORG 0x7C00]
[GLOBAL call_real]

section .text
; Implementation of function

; set_vesa: C-Prototyp: void set_vesa ();

[BITS 32]

call_real:

   sgdt   [saved_gdt]
   sidt   [saved_idt]

    ; Copy 16 bytes of parameter data
   mov eax, [esp+4]
   mov [param_list], eax
   mov eax, [esp+8]
   mov [param_list+4], eax
   mov eax, [esp+12]
   mov [param_list+8], eax
   mov eax, [esp+16]
   mov [param_list+12], eax

   push ebx

   mov  [saved_esp], esp

   mov  eax, cr0
   mov  [saved_cr0], eax
   and  eax,0x7FFFFFFF
   mov  cr0, eax

   lgdt  [gdtr]                ; Load the GDT descriptor

;Comments from http://download.intel.com/design/PentiumII/manuals/24319202.pdf
;1. Disable interrupts. A CLI instruction disables maskable hardware interrupts. NMI
;interrupts can be disabled with external circuitry.
   cli

;2. Transfer program control to a readable segment that has a limit of 64 KBytes (FFFFH).
;This operation loads the CS register with the segment limit required in real-address mode.
   jmp    RM_Code_Sel:pmode_16bit

;3. Load segment registers SS, DS, ES, FS, and GS with a selector for a descriptor containing
;the following values, which are appropriate for real-address mode:
;— Limit = 64 KBytes (0FFFFH)
;— Byte granular (G = 0)
;— Expand up (E = 0)
;— Writable (W = 1)
;— Present (P = 1)
;— Base = any value
;The segment registers must be loaded with nonnull segment selectors or the segment
;registers will be unusable in real-address mode. Note that if the segment registers are not
;reloaded, execution continues using the descriptor attributes loaded during protected
;mode.
[BITS 16]
pmode_16bit:

   mov    ax,RM_Data_Sel
   mov    ss,ax
   mov   ds,ax

;4. Execute an LIDT instruction to point to a real-address mode interrupt table that is within
;the 1-MByte real-address mode address range.
   ;lidt    [ridtr]

;5. Clear the PE flag in the CR0 register to switch to real-address mode.

   mov    eax, cr0
   and     al,0xfe
   mov   cr0, eax

;6. Execute a far JMP instruction to jump to a real-address mode program. This operation
;flushes the instruction queue and loads the appropriate base and access rights values in the
;CS register.
   mov   sp,0xFFFE

   mov   bx,[RealModeCS]
   push  bx
   lea   bx,[do_rm]
   push  bx

   retf

;7. Load the SS, DS, ES, FS, and GS registers as needed by the real-address mode code. If any
;of the registers are not going to be used in real-address mode, write 0s to them.
do_rm:

   mov   ax,cs
   mov    ds,ax
   mov    ss,ax
   nop
   mov    es,ax
   mov    fs,ax
   mov    gs,ax

; point to real-mode IDTR
   lidt    [ridtr]
;8. Execute the STI instruction to enable maskable hardware interrupts and perform the
;necessary hardware operation to enable NMI interrupts.
   sti

   mov bx,[param_list]
   shl bx,1
   mov ax,command_list
   add bx,ax
   mov ax,[bx]
   jmp ax

; ========================================================
; ===================    COMMANDS    =====================
; ========================================================

vbe_info:

    mov eax,'VBE2'
    mov [VbeInfoBlock], eax

    ; Call VESA function 0x00
    mov ax, 0
    mov es, ax
    mov ax, 0x4F00
    mov di, VbeInfoBlock
    int 0x10

    cmp   al,0x4F      ;Is Function supported?
    jne   vbe_info_fail
    cmp   ah,0x00      ;successful?
    jne   vbe_info_fail

    mov word [return_value],VbeInfoBlock
    mov word [return_value+2],0
    jmp continue_cga

vbe_info_fail:
    mov word [return_value],0
    mov word [return_value+2],0
    jmp continue_cga

vbe_mode_info:
    mov cx, [param_list+4]

    mov ax, 0
    mov es, ax
    mov ax, 0x4F01
    mov di, VbeModeInfoBlock
    int 0x10

    ;cmp   al,0x4F      ;Is Function supported?
    ;jne   vbe_mode_info_fail
    cmp   ah,0x00      ;successful?
    jne   vbe_mode_info_fail

    mov word [return_value],VbeModeInfoBlock
    mov word [return_value+2],0
    jmp continue_cga

vbe_mode_info_fail:
    mov word [return_value],0
    mov word [return_value+2],0
    jmp continue_cga

vbe_set_mode:
    mov bx, [param_list+4]
    mov ax, 0x4F02

    int 0x10

    ;cmp   al,0x4F      ;Is Function supported?
    ;jne   vbe_set_mode_fail
    cmp   ah,0x00      ;successful?
    jne   vbe_set_mode_fail

    mov word [return_value],1
    mov word [return_value+2],0
    jmp continue_cga

vbe_set_mode_fail:
    mov word [return_value],0
    mov word [return_value+2],0
    jmp continue_cga


; ========================================================
; ===============    END OF COMMANDS    ==================
; ========================================================

continue_cga:

activate_pm:

;1. Disable interrupts. A CLI instruction disables maskable hardware interrupts.
;NMI interrupts can be disabled with external circuitry. (Software must guarantee
;that no exceptions or interrupts are generated during the mode switching
;operation.)
   cli

;2. Execute the LGDT instruction to load the GDTR register with the base
;address of the GDT.
   lgdt   [gdtr]

;3. Execute a MOV CR0 instruction that sets the PE flag (and optionally
;the PG flag) in control register CR0.
   mov   eax,cr0
   or      al,0x01
   mov   cr0,eax

;4. Immediately following the MOV CR0 instruction, execute a far JMP
;or far CALL instruction. (This operation is typically a far jump or call to
;the next instruction in the instruction stream.)
;
;The JMP or CALL instruction immediately after the MOV CR0 instruction
;changes the flow of execution and serializes the processor.
;
;If paging is enabled, the code for the MOV CR0 instruction and the JMP or
;CALL instruction must come from a page that is identity mapped (that is, the
;linear address before the jump is the same as the physical address
;after paging and protected mode is enabled). The target instruction for the JMP or CALL
;instruction does not need to be identity mapped.
   jmp   PM_Code_Sel:do_pm

;5. If a local descriptor table is going to be used, execute the LLDT instruction
;to load the segment selector for the LDT in the
;LDTR register.

;6. Execute the LTR instruction to load the task register with a segment
;selector to the initial protected-mode task or to a writable area of memory that can be
;used to store TSS information on a task switch.
   do_pm:

;7. After entering protected mode, the segment registers conntinue to hold the contents
;they had in real-address mode. The JMP or CALL instruction in step 4 resets the CS register.
;Perform one of the following operations to update the contents of the remaining segment
;registers.
;— Reload segment registers DS, SS, ES, FS, and GS. If the ES, FS, and/or
;GS registers are not going to be used, load them with a null selector.
;— Perform a JMP or CALL instruction to a new task, which automatically resets
;the values of the segment registers and branches to a new code segment.
   mov   ax,0x10
   mov   ds,ax
   mov   es,ax
   mov   fs,ax
   mov   gs,ax
   mov    ss,ax
   mov   ds,ax

;8. Execute the LIDT instruction to load the IDTR register with the
;address and limit of the protected-mode IDT.

[BITS 32]

   mov   esp,[saved_esp]

   mov eax, [saved_cr0]
   mov cr0, eax

   pop ebx

   lidt   [saved_idt]
   lgdt   [saved_gdt]

   mov eax,[return_value]

;9. Execute the STI instruction to enable maskable hardware
;interrupts and perform the necessary hardware operation to enable NMI interrupts.

   ret

section .data

;variables for "Real Mode <-> Protected Mode"-Switching

ridtr:
   dw 0xFFFF      ; limit=0xFFFF
   dd 0            ; base=0

RealModeCS:
   dw   0x0

gdtr:
   dw gdt_end-1
   dd gdt

gdt:

   dw   0,0,0,0            ;null desrciptor

PM_Code_Sel equ $-gdt
   dw   0xFFFF
   dw   0x0000
   dw   0x9A00
   dw   0x00CF

PM_Data_Sel   equ $-gdt
   dw   0xFFFF
   dw   0x0000
   dw   0x9200
   dw   0x00CF

RM_Code_Sel equ $-gdt
   dw   0xFFFF
   dw   0x0000
   dw   0x9A00
   dw   0x0000

RM_Data_Sel   equ $-gdt
   dw   0xFFFF
   dw 0x0000
   dw 0x9200
   dw 0x0000

gdt_end:

saved_gdt:
   dw   0
   dd   0

saved_idt:
   dw   0
   dd   0

saved_esp:
   dw   0

saved_gdtr:
   dw   0

saved_cr0:
    dd 0

param_list:
    times 4 dd 0

command_list:
    dw vbe_info
    dw vbe_mode_info
    dw vbe_set_mode

return_value:
    dd 0

VbeInfoBlock:
     times 512 db 0

VbeModeInfoBlock:
     times 256 db 0
