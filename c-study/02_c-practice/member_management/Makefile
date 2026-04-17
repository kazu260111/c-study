cc = gcc
CFLAGS = -Wall -Wextra -g

TARGET = member_management_system

SRCS = main.c file_io.c ui.c member.c tool.c

OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
