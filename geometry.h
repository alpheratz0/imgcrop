/*
	Copyright (C) 2023 <alpheratz99@protonmail.com>

	This program is free software; you can redistribute it and/or modify it
	under the terms of the GNU General Public License version 2 as published by
	the Free Software Foundation.

	This program is distributed in the hope that it will be useful, but WITHOUT
	ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
	FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
	more details.

	You should have received a copy of the GNU General Public License along
	with this program; if not, write to the Free Software Foundation, Inc., 59
	Temple Place, Suite 330, Boston, MA 02111-1307 USA

*/

#pragma once

#include <stdbool.h>

typedef struct {
	int x;
	int y;
	int width;
	int height;
} Geometry_t;

Geometry_t *geometry_new(int x, int y, int w, int h);
Geometry_t *geometry_parse(const char *str);
bool geometry_is_valid(const Geometry_t *geometry);
bool geometry_contains(const Geometry_t *parent, const Geometry_t *child);
void geometry_free(Geometry_t *geometry);
