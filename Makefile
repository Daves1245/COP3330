BUILD_DIR=oop/example
BIN_DIR=bin
CC=javac
VM=java
VM_ARGS=--class-path $(BIN_DIR)
ARGS=-d $(BIN_DIR)
RUN_ARGS=

# https://stackoverflow.com/questions/2214575/passing-arguments-to-make-run
# If the first argument is "run"...
ifeq (run,$(firstword $(MAKECMDGOALS)))
	# use the rest as arguments for "run"
RUN_ARGS=$(wordlist 2,$(words $(MAKECMDGOALS)),$(MAKECMDGOALS))
	# ...and turn them into do-nothing targets
$(eval $(RUN_ARGS):;@:)
endif

.PHONY: all run clean

all:
	@echo test
	$(ls $(BUILD_DIR) | grep java)
	@echo test2
	$(CC) $(ARGS) $(BUILD_DIR)/*.java

# java --class-path bin oop.example/Welcome1
# $@ returns name of target of the rule (first argument to make)
# for PROG_NAME in $(ARGS); do $(VM) $(VM_ARGS) $(BUILD_DIR)/$(PROG_NAME); done
run: # filter-out A, B returns B / A. $(MAKECMDGOALS) returns list of targets.
	$(foreach elf_name,$(RUN_ARGS),$(VM) $(VM_ARGS) $(BUILD_DIR)/$(elf_name))

clean:
	rm -rf bin/
