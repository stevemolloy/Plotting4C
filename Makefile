CC = clang
CFLAGS = -Wall -Werror -Wpedantic -Wconversion -std=c99 -O2
CINCLUDES = -I/home/smolloy/Code/plotting_in_C/raylib/src -I/home/smolloy/Code/plotting_in_C/raylib/src/external
CLIBS = -L/home/smolloy/Code/plotting_in_C/raylib/src -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -latomic

SRC = src
OBJ = obj

SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BINDIR = bin
BIN = $(BINDIR)/testing

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(CFLAGS) $(CINCLUDES) $(CLIBS)

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CINCLUDES) -c $< -o $@

clean:
	rm -rf $(BINDIR) $(OBJ)

