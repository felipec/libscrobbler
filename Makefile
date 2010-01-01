CC := gcc

EXTRA_WARNINGS := -Wall -Wextra -ansi -std=c99 -Wno-unused-parameter

CFLAGS := -ggdb -Wall $(EXTRA_WARNINGS)

all:

libscrobble.a: scrobble.o

all: libscrobble.a

# pretty print
V = @
Q = $(V:y=)
QUIET_CC    = $(Q:@=@echo '   CC         '$@;)
QUIET_LINK  = $(Q:@=@echo '   LINK       '$@;)
QUIET_CLEAN = $(Q:@=@echo '   CLEAN      '$@;)

%.a::
	$(QUIET_LINK)$(AR) rcs $@ $^

%.o:: %.c
	$(QUIET_CC)$(CC) $(CFLAGS) -MMD -o $@ -c $<

clean:
	$(QUIET_CLEAN)$(RM) *.o *.d *.a

-include *.d
