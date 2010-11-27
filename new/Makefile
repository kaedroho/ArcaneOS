x86-32: bin/arcane_x86-32.bin
	@echo Built ArcaneOS x86-32

bin/arcane_x86-32.bin: loader_x86-32 drivers_x86-32 kernel_x86-32 libraries_x86-32
	rm -f bin/arcane_x86-32.bin
	ld -T linkx86-32.ld -o bin/arcane_x86-32.bin bin/start/start32.asm.o bin/o/* bin/o2/* > ld.txt
	img/update


loader_x86-32: bin/start/start32.asm.o bin/o/startup.c.o bin/o/boot.c.o

bin/start/start32.asm.o: src/loader/x86/start32.asm
	rm -f bin/start/start32.asm.o
	nasm -i ./inc/ -f aout -o bin/start/start32.asm.o src/loader/x86/start32.asm
	
bin/o/startup.c.o: src/loader/x86/startup.c
	rm -f bin/o/startup.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/startup.c.o src/loader/x86/startup.c

bin/o/boot.c.o: src/loader/x86/boot.c
	rm -f bin/o/boot.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/boot.c.o src/loader/x86/boot.c



drivers_x86-32: bin/o/initrd.c.o bin/o/ibmpc-clock.c.o bin/o/clock.c.o bin/o/console.c.o bin/o/ibmpc.c.o bin/o/kb.c.o bin/o/screen.c.o bin/o/vbe_real.asm.o bin/o/floppy.c.o

bin/o/initrd.c.o: src/drivers/x86/ibmpc/initrd.c
	rm -f bin/o/initrd.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/initrd.c.o src/drivers/x86/ibmpc/initrd.c

bin/o/ibmpc-clock.c.o: src/drivers/x86/ibmpc/clock.c
	rm -f bin/o/ibmpc-clock.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/ibmpc-clock.c.o src/drivers/x86/ibmpc/clock.c

bin/o/clock.c.o: src/drivers/all/clock.c
	rm -f bin/o/clock.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/clock.c.o src/drivers/all/clock.c


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

bin/o/floppy.c.o: src/drivers/x86/ibmpc/floppy.c
	rm -f bin/o/floppy.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o/floppy.c.o src/drivers/x86/ibmpc/floppy.c


kernel_x86-32: bin/o2/dm.c.o bin/o2/devfs.c.o bin/o2/io.c.o bin/o2/storage.c.o bin/o2/vfs.c.o bin/o2/cmos.c.o bin/o2/kload.c.o bin/o2/cpuid32.asm.o bin/o2/cpuid.c.o bin/o/realmode.asm.o bin/o2/gdt32.asm.o bin/o2/gdt.c.o bin/o2/idt32.asm.o bin/o2/idt.c.o bin/o2/irq32.asm.o bin/o2/irq.c.o bin/o2/isr32.asm.o bin/o2/isr.c.o bin/o2/mm.c.o bin/o2/paging.c.o bin/o2/paging32.asm.o bin/o2/timer.c.o bin/o2/lowmem.c.o bin/o2/mt.c.o bin/o2/syscall.c.o bin/o2/syscall32.asm.o bin/o2/sync.c.o

bin/o2/dm.c.o: src/kernel/all/driver-manager/dm.c
	rm -f bin/o2/dm.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/dm.c.o src/kernel/all/driver-manager/dm.c

bin/o2/devfs.c.o: src/kernel/all/driver-manager/devfs.c
	rm -f bin/o2/devfs.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/devfs.c.o src/kernel/all/driver-manager/devfs.c

bin/o2/io.c.o: src/kernel/all/driver-manager/classes/io.c
	rm -f bin/o2/io.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/io.c.o src/kernel/all/driver-manager/classes/io.c

bin/o2/storage.c.o: src/kernel/all/driver-manager/classes/storage.c
	rm -f bin/o2/storage.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/storage.c.o src/kernel/all/driver-manager/classes/storage.c


bin/o2/vfs.c.o: src/kernel/all/vfs/vfs.c
	rm -f bin/o2/vfs.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/vfs.c.o src/kernel/all/vfs/vfs.c


bin/o2/cmos.c.o: src/kernel/x86/cmos.c
	rm -f bin/o2/cmos.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/cmos.c.o src/kernel/x86/cmos.c

bin/o2/kload.c.o: src/kernel/x86/kload.c
	rm -f bin/o2/kload.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/kload.c.o src/kernel/x86/kload.c

bin/o2/cpuid32.asm.o: src/kernel/x86/cpuid32.asm
	rm -f bin/o2/cpuid32.asm.o
	nasm -i ./inc/ -f aout -o bin/o2/cpuid32.asm.o src/kernel/x86/cpuid32.asm

bin/o2/cpuid.c.o: src/kernel/x86/cpuid.c
	rm -f bin/o2/cpuid.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/cpuid.c.o src/kernel/x86/cpuid.c


bin/o/realmode.asm.o: src/kernel/x86/realmode.asm
	rm -f bin/o2/realmode.asm.o
	nasm -i ./inc/ -f aout -o bin/o/realmode.asm.o src/kernel/x86/realmode.asm

bin/o2/gdt32.asm.o: src/kernel/x86/gdt32.asm
	rm -f bin/o2/gdt32.asm.o
	nasm -i ./inc/ -f aout -o bin/o2/gdt32.asm.o src/kernel/x86/gdt32.asm

bin/o2/gdt.c.o: src/kernel/x86/gdt.c
	rm -f bin/o2/gdt.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/gdt.c.o src/kernel/x86/gdt.c


bin/o2/idt32.asm.o: src/kernel/x86/idt32.asm
	rm -f bin/o2/idt32.asm.o
	nasm -i ./inc/ -f aout -o bin/o2/idt32.asm.o src/kernel/x86/idt32.asm

bin/o2/idt.c.o: src/kernel/x86/idt.c
	rm -f bin/o2/idt.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/idt.c.o src/kernel/x86/idt.c


bin/o2/irq32.asm.o: src/kernel/x86/irq32.asm
	rm -f bin/o2/irq32.asm.o
	nasm -i ./inc/ -f aout -o bin/o2/irq32.asm.o src/kernel/x86/irq32.asm

bin/o2/irq.c.o: src/kernel/x86/irq.c
	rm -f bin/o2/irq.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/irq.c.o src/kernel/x86/irq.c


bin/o2/isr32.asm.o: src/kernel/x86/isr32.asm
	rm -f bin/o2/isr32.asm.o
	nasm -i ./inc/ -f aout -o bin/o2/isr32.asm.o src/kernel/x86/isr32.asm

bin/o2/isr.c.o: src/kernel/x86/isr.c
	rm -f bin/o2/isr.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/isr.c.o src/kernel/x86/isr.c


bin/o2/mm.c.o: src/kernel/x86/mm.c
	rm -f bin/o2/mm.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/mm.c.o src/kernel/x86/mm.c


bin/o2/paging.c.o: src/kernel/x86/paging.c
	rm -f bin/o2/paging.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/paging.c.o src/kernel/x86/paging.c

bin/o2/paging32.asm.o: src/kernel/x86/paging32.asm
	rm -f bin/o2/paging32.asm.o
	nasm -i ./inc/ -f aout -o bin/o2/paging32.asm.o src/kernel/x86/paging32.asm

bin/o2/timer.c.o: src/kernel/x86/timer.c
	rm -f bin/o2/timer.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/timer.c.o src/kernel/x86/timer.c

bin/o2/lowmem.c.o: src/kernel/x86/lowmem.c
	rm -f bin/o2/lowmem.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/lowmem.c.o src/kernel/x86/lowmem.c

bin/o2/mt.c.o: src/kernel/x86/mt.c
	rm -f bin/o2/mt.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/mt.c.o src/kernel/x86/mt.c

bin/o2/syscall.c.o: src/kernel/x86/syscall.c
	rm -f bin/o2/syscall.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/syscall.c.o src/kernel/x86/syscall.c

bin/o2/syscall32.asm.o: src/kernel/x86/syscall32.asm
	rm -f bin/o2/syscall32.asm.o
	nasm -i ./inc/ -f aout -o bin/o2/syscall32.asm.o src/kernel/x86/syscall32.asm

bin/o2/sync.c.o: src/kernel/x86/sync.c
	rm -f bin/o2/sync.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/sync.c.o src/kernel/x86/sync.c



libraries_x86-32: bin/o2/string.c.o
	

bin/o2/string.c.o: src/libraries/all/string.c
	rm -f bin/o2/string.c.o
	gcc -m32 -Wall -O0 -fstrength-reduce -fomit-frame-pointer -finline-functions -nostdinc -fno-builtin -fno-stack-protector -I./inc -c -o bin/o2/string.c.o src/libraries/all/string.c