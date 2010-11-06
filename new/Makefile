x86-32: bin/arcane_x86-32.bin
	@echo Built ArcaneOS x86-32

bin/arcane_x86-32.bin: loader_x86-32 drivers_x86-32 kernel_x86-32 libraries_x86-32
	rm -f bin/arcane_x86-32.bin
	ld -T linkx86-32.ld -o bin/arcane_x86-32.bin bin/start/start32.asm.o bin/o/* > ld.txt
	img/update


loader_x86-32: bin/start/start32.asm.o bin/o/startup.c.o bin/o/boot.c.o

bin/start/start32.asm.o: src/loader/x86/start32.asm
	rm -f bin/start/start32.asm.o
	nasm -i ./inc/ -f aout -o bin/start/start32.asm.o src/loader/x86/start32.asm
	
bin/o/startup.c.o: src/loader/all/startup.c
	rm -f bin/o/startup.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/startup.c.o src/loader/all/startup.c

bin/o/boot.c.o: src/loader/all/boot.c
	rm -f bin/o/boot.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/boot.c.o src/loader/all/boot.c



drivers_x86-32: bin/o/initrd.c.o bin/o/clock.c.o bin/o/console.c.o bin/o/ibmpc.c.o bin/o/kb.c.o bin/o/screen.c.o bin/o/vbe_real.asm.o

bin/o/initrd.c.o: src/drivers/all/initrd.c
	rm -f bin/o/initrd.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/initrd.c.o src/drivers/all/initrd.c


bin/o/clock.c.o: src/drivers/x86/clock.c
	rm -f bin/o/clock.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/clock.c.o src/drivers/x86/clock.c


bin/o/console.c.o: src/drivers/all/console.c
	rm -f bin/o/console.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/console.c.o src/drivers/all/console.c


bin/o/ibmpc.c.o: src/drivers/x86/ibmpc/ibmpc.c
	rm -f bin/o/ibmpc.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/ibmpc.c.o src/drivers/x86/ibmpc/ibmpc.c


bin/o/kb.c.o: src/drivers/x86/ibmpc/kb.c
	rm -f bin/o/kb.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/kb.c.o src/drivers/x86/ibmpc/kb.c


bin/o/screen.c.o: src/drivers/x86/ibmpc/screen.c
	rm -f bin/o/screen.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/screen.c.o src/drivers/x86/ibmpc/screen.c


bin/o/vbe_real.asm.o: src/drivers/x86/ibmpc/vbe_real.asm
	rm -f bin/o/vbe_real.asm.o
	nasm -i ./inc/ -f aout -o bin/o/vbe_real.asm.o src/drivers/x86/ibmpc/vbe_real.asm


kernel_x86-32: bin/o/vfs.c.o bin/o/cmos.c.o bin/o/kload.c.o bin/o/cpuid32.asm.o bin/o/cpuid.c.o bin/o/realmode.asm.o bin/o/gdt32.asm.o bin/o/gdt.c.o bin/o/idt32.asm.o bin/o/idt.c.o bin/o/irq32.asm.o bin/o/irq.c.o bin/o/isr32.asm.o bin/o/isr.c.o bin/o/mm.c.o bin/o/paging.c.o bin/o/timer.c.o

bin/o/vfs.c.o: src/kernel/all/vfs/vfs.c
	rm -f bin/o/vfs.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/vfs.c.o src/kernel/all/vfs/vfs.c


bin/o/cmos.c.o: src/kernel/x86/cmos.c
	rm -f bin/o/cmos.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/cmos.c.o src/kernel/x86/cmos.c

bin/o/kload.c.o: src/kernel/x86/kload.c
	rm -f bin/o/kload.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/kload.c.o src/kernel/x86/kload.c

bin/o/cpuid32.asm.o: src/kernel/x86/cpuid32.asm
	rm -f bin/o/cpuid32.asm.o
	nasm -i ./inc/ -f aout -o bin/o/cpuid32.asm.o src/kernel/x86/cpuid32.asm

bin/o/cpuid.c.o: src/kernel/x86/cpuid.c
	rm -f bin/o/cpuid.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/cpuid.c.o src/kernel/x86/cpuid.c


bin/o/realmode.asm.o: src/kernel/x86/realmode.asm
	rm -f bin/o/realmode.asm.o
	nasm -i ./inc/ -f aout -o bin/o/realmode.asm.o src/kernel/x86/realmode.asm

bin/o/gdt32.asm.o: src/kernel/x86/gdt32.asm
	rm -f bin/o/gdt32.asm.o
	nasm -i ./inc/ -f aout -o bin/o/gdt32.asm.o src/kernel/x86/gdt32.asm

bin/o/gdt.c.o: src/kernel/x86/gdt.c
	rm -f bin/o/gdt.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/gdt.c.o src/kernel/x86/gdt.c


bin/o/idt32.asm.o: src/kernel/x86/idt32.asm
	rm -f bin/o/idt32.asm.o
	nasm -i ./inc/ -f aout -o bin/o/idt32.asm.o src/kernel/x86/idt32.asm

bin/o/idt.c.o: src/kernel/x86/idt.c
	rm -f bin/o/idt.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/idt.c.o src/kernel/x86/idt.c


bin/o/irq32.asm.o: src/kernel/x86/irq32.asm
	rm -f bin/o/irq32.asm.o
	nasm -i ./inc/ -f aout -o bin/o/irq32.asm.o src/kernel/x86/irq32.asm

bin/o/irq.c.o: src/kernel/x86/irq.c
	rm -f bin/o/irq.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/irq.c.o src/kernel/x86/irq.c


bin/o/isr32.asm.o: src/kernel/x86/isr32.asm
	rm -f bin/o/isr32.asm.o
	nasm -i ./inc/ -f aout -o bin/o/isr32.asm.o src/kernel/x86/isr32.asm

bin/o/isr.c.o: src/kernel/x86/isr.c
	rm -f bin/o/isr.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/isr.c.o src/kernel/x86/isr.c


bin/o/mm.c.o: src/kernel/x86/mm.c
	rm -f bin/o/mm.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/mm.c.o src/kernel/x86/mm.c


bin/o/paging.c.o: src/kernel/x86/paging.c
	rm -f bin/o/paging.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/paging.c.o src/kernel/x86/paging.c

bin/o/timer.c.o: src/kernel/x86/timer.c
	rm -f bin/o/timer.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/timer.c.o src/kernel/x86/timer.c



libraries_x86-32: bin/o/string.c.o
	

bin/o/string.c.o: src/libraries/all/string.c
	rm -f bin/o/string.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/string.c.o src/libraries/all/string.c