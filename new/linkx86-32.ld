OUTPUT_FORMAT("binary")
ENTRY(start)
phys = 0x00100000;
low_phys = 0x00000500;
SECTIONS
{
  . = phys;
  code = .;
  .text : {
    *(EXCLUDE_FILE (bin/o/realmode.asm.o bin/o/vbe_real.asm.o) .text)
    *(.rodata)
    . = ALIGN(4096);
  }

  low_code = .;
  .low low_phys : AT(low_code) {
    low_code_tmp = .;
    bin/o/realmode.asm.o(.text)
    bin/o/vbe_real.asm.o(.text)
    low_code_size = . - low_code_tmp;
  }
  . = low_code + low_code_size;
  low_code_end = .;
  . = ALIGN(4096);

  data = .;
  .data data : AT(data)
  {
    *(.data)
    . = ALIGN(4096);
  }

  bss = .;
  .bss bss : AT(bss)
  {
    *(.bss)
    . = ALIGN(4096);
  }
  end = .;
}