all:
	mkdir -p build
	nasm -f bin bootloader.asm -o build/bootloader.bin
	nasm -f elf32 entry.asm -o build/entry.o
	gcc -m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector \
	    -nostdlib -nostartfiles -c kernel.c -o build/kernel.o
	gcc -m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector \
	    -nostdlib -nostartfiles -c shell.c -o build/shell.o
	gcc -m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector \
	    -nostdlib -nostartfiles -c commands.c -o build/commands.o
	gcc -m32 -ffreestanding -fno-pic -fno-pie -fno-stack-protector \
	    -nostdlib -nostartfiles -c fs.c -o build/fs.o
	ld -m elf_i386 -T linker.ld -e _entry \
	    build/entry.o build/kernel.o build/shell.o build/commands.o build/fs.o \
	    memman/target/i686-unknown-linux-gnu/release/libmemman.a \
	    -o build/kernel.elf
	objcopy -O binary build/kernel.elf build/kernel.bin
	dd if=/dev/zero of=build/shellOS.img bs=512 count=4096
	dd if=build/bootloader.bin of=build/shellOS.img conv=notrunc bs=512 seek=0
	dd if=build/kernel.bin of=build/shellOS.img conv=notrunc bs=512 seek=1

package: all
	mkdir -p dist
	rm -f dist/shellOS.vdi
	VBoxManage convertfromraw build/shellOS.img dist/shellOS.vdi --format VDI
	mkdir -p iso/boot
	cp build/shellOS.img iso/boot/
	xorriso -as mkisofs \
	  -b boot/shellOS.img \
	  -no-emul-boot \
	  -boot-load-size 4 \
	  -boot-info-table \
	  -o dist/shellOS.iso \
	  iso/
	echo "==> dist/shellOS.img  (raw)"
	echo "==> dist/shellOS.vdi  (VirtualBox)"
	echo "==> dist/shellOS.iso  (ISO)"

clean:
	rm -rf build/ dist/ iso/

run:
	qemu-system-x86_64 -drive format=raw,file=build/shellOS.img,index=0,media=disk
