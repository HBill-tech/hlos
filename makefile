# linux path
BUILD = ./build
SRC = ./src
TEST = ./test
INC = $(SRC)/inc
CFLAGS = -gdwarf-2 -O0 -c -m32 -I$(INC) -fno-pie -fno-stack-protector -nostdlib -nostdinc


# $(shell)和 $@ 等一同在 make 的解析阶段执行，如果 $(shell) 命令中含有 $@ 等符号，那么将会执行失败。
# 因为此时 $@ 还没有被解析出来
$(BUILD)/test/%.bin: $(TEST)/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -g -f bin $< -o $@

# 编译汇编文件
# shell: shell 命令
# mkdir -p 若目录不存在则创建，若存在不报错。若父目录也不存在那么也创建父目录
# dir $@ 提取目标文件的目录
$(BUILD)/%.o: $(SRC)/%.asm
	$(shell mkdir -p $(dir $@))
	nasm -g -f elf32 $< -o $@

# 编译C文件
$(BUILD)/%.o: $(SRC)/%.c
	$(shell mkdir -p $(dir $@))
	x86_64-elf-gcc $(CFLAGS) $< -o $@

# 将 .o 文件编译成 bin 文件
$(BUILD)/boot.bin: $(BUILD)/boot/boot.o
	$(shell mkdir -p $(dir $@))
	x86_64-elf-ld -m elf_i386 -Ttext=0x7c00 $^ -o $(BUILD)/boot.elf
	x86_64-elf-objcopy -O binary $(BUILD)/boot.elf $(BUILD)/boot.bin

$(BUILD)/kernel.bin: $(BUILD)/kernel/start.o \
	$(BUILD)/kernel/kernel.o
	$(shell mkdir -p $(dir $@))
	x86_64-elf-ld -m elf_i386 -Ttext=0x7e00 $^ -o $(BUILD)/kernel.elf
	x86_64-elf-objcopy -O binary $(BUILD)/kernel.elf $(BUILD)/kernel.bin

.PHONY: master
master: $(BUILD)/boot.bin \
	$(BUILD)/kernel.bin
	dd if=$(BUILD)/boot.bin of=master.img bs=512 count=1 conv=notrunc
	dd if=$(BUILD)/kernel.bin of=master.img bs=512 count=10 seek=1 conv=notrunc

.PHONY: bochs
bochs: master
	bochsdbg -q -f bochsrc.bxrc

.PHONY: clean-linux
clean-linux:
	@rm -rf $(BUILD)/*

.PHONY: clean
clean:
	@rm -rf $(BUILD)/*

.PHONY: qemu
# 这里的 qemu 启动命令逗号后面不要加空格
qemu: clean master
	qemu-system-i386w -s -S -m 128M -drive file=master.img,index=0,media=disk,format=raw

.PHONY: qemu-linux
qemu-linux: master
	qemu-system-x86_64 -m 128M -drive file=master.img,index=0,media=disk,format=raw

.PHONY: test_chs
test_chs: $(BUILD)/test/read_disk_chs.bin
	dd if=$(BUILD)/test/read_disk_chs.bin of=master.img bs=512  count=1
	bochsdbg -q -f bochsrc.bxrc

.PHONY: test_lba
test_lba: $(BUILD)/test/read_disk_lba.bin
	dd if=$(BUILD)/test/read_disk_lba.bin of=master.img bs=512 count=1 conv=notrunc
	bochsdbg -q -f bochsrc.bxrc