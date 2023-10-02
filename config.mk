# Copyright (C) 2022-2023 <alpheratz99@protonmail.com>
# This program is free software.

VERSION = 1.0.0

PREFIX = /usr/local
MANPREFIX = $(PREFIX)/share/man

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Os $(INCS) -DVERSION=\"$(VERSION)\"
LDFLAGS = -s -lm

CC = cc
