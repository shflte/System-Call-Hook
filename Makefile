.PHONY: all step1 step2 trampoline logger copy_so clean

BUILD := build

all: step1 step2 trampoline logger copy_so

step1:
	@$(MAKE) -C step1

step2:
	@$(MAKE) -C step2

trampoline:
	@$(MAKE) -C trampoline

logger:
	@$(MAKE) -C logger

copy_so:
	@cp step1/build/libzpoline.so.1      .
	@cp step2/build/libzpoline.so.2      .
	@cp trampoline/build/libzpoline.so   .
	@cp logger/build/logger.so           .

clean:
	@$(MAKE) -C step1 clean
	@$(MAKE) -C step2 clean
	@$(MAKE) -C trampoline clean
	@$(MAKE) -C logger clean
	rm -rf ./*.so*