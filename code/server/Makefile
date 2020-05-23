# Makefile
# Chaos The Game Backend

# defines the compiler and flags
COMPILER = clang++
COMPILER_FLAGS = -Wall -g -fsanitize=address -fno-omit-frame-pointer

# the linker flags, only needed for Boost library as of now
LINKER_FLAGS = -lboost_system

# the list of files to be compiled, when this gets larger I'll set so only changes get compiled
OBJECTS = gameserver.cpp arena.cpp player.cpp

a.out: $(OBJECTS)
	$(COMPILER) $(COMPILER_FLAGS) $(LINKER_FLAGS) $(OBJECTS)

.PHONY: clean
clean:
	-rm -f *.o *~ a.out
