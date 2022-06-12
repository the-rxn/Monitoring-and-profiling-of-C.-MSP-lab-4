CFLAGS=-Wall -Wextra -Werror -O2
TARGETS=libaotN3246.so lab1aotN3246

.PHONY: all clean

all: $(TARGETS)

clean:
	rm -rf *.o $(TARGETS)

lab1aotN3246: lab1aotN3246.c plugin_api.h
	gcc $(CFLAGS) -o lab1aotN3246 lab1aotN3246.c -ldl -g

libaotN3246.so: libaotN3246.c plugin_api.h
	gcc $(CFLAGS) -shared -fPIC -o libaotN3246.so libaotN3246.c -ldl -lm


