;VGA functions

[BITS 32]

;Enable vga
global video_vga_enable13
video_vga_enable13:
    mov ah,0x00
    mov al,0x13
    int 0x10

global video_vga_disable
video_vga_disable:
    mov ah,0x00
    mov al,0x03
    int 0x10
