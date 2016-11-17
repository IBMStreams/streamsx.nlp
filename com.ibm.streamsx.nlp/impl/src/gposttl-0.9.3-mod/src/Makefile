#
# modified by Sihyung Lee
#	to create a re-usable C++ library based on GPoSTTL 0.9.3S
#

# use g++ compiler
CC=g++

#options for the compiler
#	create object files
CFLAGS=-c -Wall

# options for the compiler, for the codes that will be included in the library
# -fPIC: emit position-independent code, according to the way SPL accesses libs
CFLAGS_L=-fPIC

all: header libGPoSTTL.so

header: bool.h darray.h darrayP.h enhance_penntag.h memory.h sysdep.h tokenizer.h gposttl.h registry.h tagger_controller.h useful.h lex.h registryP.h tagger.h
	mkdir -p ../../../include/gposttl/
	cp *.h ../../../include/gposttl/

libGPoSTTL.so: gposttl.o tagger_controller.o darray.o enhance_penntag.o lex.o memory.o registry.o start-state-tagger.o final-state-tagger.o tagger.o tokenizer.o useful.o
	$(CC) -shared -o $@ $^
	cp libGPoSTTL.so ../../../lib/

gposttl.o: gposttl.h gposttl.c
	$(CC) $(CFLAGS) $(CFLAGS_L) gposttl.c

tagger_controller.o: tagger_controller.h tagger_controller.c
	$(CC) $(CFLAGS) $(CFLAGS_L) tagger_controller.c

darray.o: darray.h darray.c
	$(CC) $(CFLAGS) $(CFLAGS_L) darray.c

enhance_penntag.o: enhance_penntag.h enhance_penntag.c
	$(CC) $(CFLAGS) $(CFLAGS_L) enhance_penntag.c

lex.o: lex.h lex.c
	$(CC) $(CFLAGS) $(CFLAGS_L) lex.c

memory.o: memory.h memory.c
	$(CC) $(CFLAGS) $(CFLAGS_L) memory.c

registry.o: registry.h registry.c
	$(CC) $(CFLAGS) $(CFLAGS_L) registry.c

start-state-tagger.o: start-state-tagger.c
	$(CC) $(CFLAGS) $(CFLAGS_L) start-state-tagger.c

final-state-tagger.o: final-state-tagger.c
	$(CC) $(CFLAGS) $(CFLAGS_L) final-state-tagger.c

tagger.o: tagger.h tagger.c
	$(CC) $(CFLAGS) $(CFLAGS_L) tagger.c

tokenizer.o: tokenizer.h tokenizer.c
	$(CC) $(CFLAGS) $(CFLAGS_L) tokenizer.c

useful.o: useful.h useful.c
	$(CC) $(CFLAGS) $(CFLAGS_L) useful.c

# create the GPoSTTL lib (used by test application)
# c: create, r: insert the listed members into the archive
libGPoSTTL.a: gposttl.o tagger_controller.o darray.o enhance_penntag.o lex.o memory.o registry.o start-state-tagger.o final-state-tagger.o tagger.o tokenizer.o useful.o
	$(AR) -r -c $@ $^

clean:
	rm -f *.o *.so
	rm -f ../../../lib/libGPoSTTL.so
	rm -rf ../../../include/gposttl/

