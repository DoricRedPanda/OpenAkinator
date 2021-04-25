program_name := open-akinator

CC = gcc
CFLAGS = -Wall -Wextra -pedantic
linker_flags = -lm
debug_flags = -static -fno-builtin-strlen -g

source_dir := src
include_dir := include
bin_dir := bin

object_files := main.o game.o util.o
src_files := main.c game.c util.c

object_files := $(addprefix $(bin_dir)/, $(object_files))
src_files := $(addprefix $(source_dir)/, $(src_files))

VPATH := $(source_dir) $(include_dir) $(bin_dir)

$(program_name): $(object_files)
	$(CC) $^ -o $@ $(linker_flags)

$(bin_dir)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -I$(include_dir) -I$(source_dir) -o $@

debug: $(src_files)
	$(CC) -DDEBUG -I$(include_dir) $(src_files) $(CFLAGS) $(linker_flags) $(debug_flags) -o $@

clean:
	$(RM) -f $(program_name) $(bin_dir)/*.o

.PHONY: all clean
