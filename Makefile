# Copyright Horia-Valentin Moroianu 314CAa 2022-2023

# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets
TARGETS=image_editor

build:
	$(CC) $(CFLAGS) functions.c main.c -o image_editor -lm

pack:
	zip -FSr 314CA_MoroianuHoria-Valentin_Tema3.zip README Makefile *.c *.h

clean:
	rm -f $(TARGETS)

.PHONY: pack clean
