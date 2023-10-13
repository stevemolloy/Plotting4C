CC = clang
CFLAGS = -Wall -Werror -Wpedantic -Wconversion -std=c99 -ggdb
CINCLUDES = -I/home/smolloy/Code/plotting_in_C/raylib/src
CLIBS = -L/home/smolloy/Code/plotting_in_C/raylib/src -lraylib -lm

SRC = src
OBJ = obj

SRCS = $(wildcard $(SRC)/*.c)
OBJS = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

BINDIR = bin
BIN = $(BINDIR)/testing

EXAMPLES = examples
EXAMPLE_SRCS = $(wildcard $(EXAMPLES)/*.c)
EXAMPLE_BINS = $(patsubst $(EXAMPLES)/%.c, $(BINDIR)/%, $(EXAMPLE_SRCS))

all: $(BIN) $(EXAMPLE_BINS)

$(BIN): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $^ -o $@ $(CLIBS)

$(BINDIR)/%: $(EXAMPLES)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CINCLUDES) $< -o $@ $(CLIBS)

$(OBJ)/%.o: $(SRC)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) $(CINCLUDES) -c $< -o $@

clean:
	rm -rf $(BINDIR) $(OBJ)

