OBJS = server.o func.o
TARGET = my_server

$(TARGET): $(OBJS)
	$(CC) $^ -o $@
	
clean:
	rm $(OBJS) $(TARGET) $(wildcard *.d)
.PHONY: clean

vpath %.h ../../include
vpath %.c ../../src

%.o: %.c
	gcc -c -I ../../include -MD $<

include $(wildcard *.d)
