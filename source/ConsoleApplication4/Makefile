# The Makefile configuration stuff:
CC = g++
RM = rm -f
AR = ar
CXXFLAGS = -Wall -fmessage-length=0 -ansi -pedantic -O3 -s
#CXXFLAGS = -Wall -fmessage-length=0 -ansi -pedantic -g -pg

OUTPUT = libvuwlgp.a

OBJECTS = Rand.o



# The main bit:
all: $(OBJECTS)
	$(AR) -rv $(OUTPUT) $?



# Cleaning up after ourselves...
.PHONY: clean
clean:
	$(RM) $(OUTPUT)

.PHONY: realclean
realclean: clean
	$(RM) *.o
	$(RM) *~