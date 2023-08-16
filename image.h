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

#include <stdint.h>
#include <stdbool.h>

#include "geometry.h"

typedef struct {
	int width;
	int height;
	unsigned char *px;
} Image_t;

Image_t *image_load(const char *path);
Image_t *image_crop(const Image_t *img, const Geometry_t *geom);
bool image_save(const Image_t *img, const char *path);
void image_free(Image_t *img);
