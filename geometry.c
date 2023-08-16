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

#include <stdbool.h>
#include <stdlib.h>
#include "geometry.h"

extern Geometry_t *
geometry_new(int x, int y, int w, int h)
{
	Geometry_t *geometry;

	if (w == 0 || h == 0)
		return NULL;

	geometry = malloc(sizeof(Geometry_t));

	if (!geometry)
		return NULL;

	if (w < 0) x += w, w *= -1;
	if (h < 0) y += h, h *= -1;

	geometry->x = x;
	geometry->y = y;
	geometry->width = w;
	geometry->height = h;

	return geometry;
}

static int
xstrtol(const char *restrict __nptr, char **restrict __endptr, int *out)
{
	*out = strtol(__nptr, __endptr, 10);
	return __nptr == *__endptr ? -1 : 0;
}

extern Geometry_t *
geometry_parse(const char *str)
{
	int x, y, w, h;
	char *walk = (char *)str;
	if (xstrtol(walk, &walk, &w) < 0) return NULL;
	if (*walk++ != 'x')               return NULL;
	if (xstrtol(walk, &walk, &h) < 0) return NULL;
	if (*walk == '\0')                return geometry_new(0, 0, w, h);
	if (*walk++ != '+')               return NULL;
	if (xstrtol(walk, &walk, &x) < 0) return NULL;
	if (*walk++ != '+')               return NULL;
	if (xstrtol(walk, &walk, &y) < 0) return NULL;
	if (*walk != '\0')                return NULL;
	return geometry_new(x, y, w, h);
}

extern bool
geometry_is_valid(const Geometry_t *geometry)
{
	if (geometry->width <= 0 || geometry->height <= 0)
		return false;
	return true;
}

extern bool
geometry_contains(const Geometry_t *parent, const Geometry_t *child)
{
	if (!geometry_is_valid(parent) || !geometry_is_valid(child)) return false;
	if (child->x < parent->x) return false;
	if (child->y < parent->y) return false;
	if (child->x + child->width > parent->x + parent->width) return false;
	if (child->y + child->height > parent->y + parent->height) return false;
	return true;
}

extern void
geometry_free(Geometry_t *geometry)
{
	free(geometry);
}
