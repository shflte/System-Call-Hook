.PHONY: all trampoline logger clean

BUILD := build

all: trampoline logger

trampoline:
	@$(MAKE) -C trampoline

logger:
	@$(MAKE) -C logger

clean:
	@$(MAKE) -C trampoline clean
	@$(MAKE) -C logger clean
	rm -rf ./*.so*
