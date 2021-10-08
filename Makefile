CFLAGS=-O0 -g -Wall -Wextra -Wno-unused-parameter -Iinclude -Wno-deprecated-declarations # The last argument is to get rid of warnings when using malloc hooks
CPPFLAGS=-MMD
LDFLAGS=
LDLIBS=

OBJS=tests/test_allocator.o src/my_allocator.o
DEPS=$(OBJS:.o=.d)

.PHONY: all clean

all: test_allocator

-include $(DEPS)

test_allocator: $(OBJS)
	gcc $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

%.o: %.c
	gcc -c $(CFLAGS) $(CPPFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(DEPS) test_allocator