f = _out
all: $(f).elf

$(f).elf: $(f).o
	ld -o $@ $+ -emain

$(f).o: $(f).s
	as -ggdb -mfpu=vfpv2 -o $@ $<

clean:
	rm -vf *.o *.out *.elf

exam:
	./comp.py print_a.b
	make
	vim _out.s

