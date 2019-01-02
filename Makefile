f = mul64
all: $(f).elf

$(f).elf: $(f).o
	gcc -o $@ $+

$(f).o: $(f).s
	as -o $@ $<

clean:
	rm -vf *.o *.out *.elf
