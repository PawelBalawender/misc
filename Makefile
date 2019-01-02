f = calc_better_loop
all: $(f).elf

$(f).elf: $(f).o
	gcc -o $@ $+

$(f).o: $(f).s
	as -o $@ $<

clean:
	rm -vf *.o *.out *.elf
