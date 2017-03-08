SRCS=${shell ls *.c}
INCS=${shell ls *.h}
TARGET=ntshell

$(TARGET) : $(SRCS) $(INCS)
	$(CC) -o $@ $(SRCS) -ggdb -std=gnu89 -Wall -Wextra -Werror

clean:
	rm -f $(TARGET)
