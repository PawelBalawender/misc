srcs := $(wildcard *.s)
objs := $(srcs: .c = .o)

all: $(objs)
	gcc $< -o $@

$(objs): $(srcs)
	as $^ -o $@

clean:
	rm -vf *.o *.out *.elf

