CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -g $(shell pkg-config --cflags libcurl json-c)
LDFLAGS = $(shell pkg-config --libs libcurl json-c)

SRC_DIR = all-files
SRCS = shell.c \
       $(SRC_DIR)/custom_commands.c \
       $(SRC_DIR)/custom_ls.c \
       $(SRC_DIR)/custom_ls_long.c \
       $(SRC_DIR)/custom_cat.c \
       $(SRC_DIR)/custom_cp.c \
       $(SRC_DIR)/custom_mkdir.c \
       $(SRC_DIR)/custom_rmdir.c \
       $(SRC_DIR)/custom_rm.c \
       $(SRC_DIR)/custom_echo.c \
       $(SRC_DIR)/custom_touch.c \
       $(SRC_DIR)/ai_integration.c

OBJS = $(SRCS:.c=.o)
TARGET = custom_shell

.PHONY: all clean test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
	rm -f *~ \#*

test: $(TARGET)
	./run_tests.sh 