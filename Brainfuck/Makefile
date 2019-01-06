f = hello
all: $(f).elf

$(f).elf: $(f).o
	ld -o $@ $+ -emain

$(f).o: $(f).s
	as -o $@ $<

clean:
	rm -vf *.o *.out *.elf _out.s

exam:
	./comp.py print_a.b
	make
	vim _out.s

attr:
	make
	objcopy --dump-section .ARM.attributes=.ARM.attributes.o $(f).elf

