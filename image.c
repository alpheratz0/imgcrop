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

#include <stdlib.h>
#include <stdbool.h>
#include "image.h"
#include "geometry.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

extern Image_t *
image_load(const char *path)
{
	Image_t *img;

	img = malloc(sizeof(Image_t));

	if (!img)
		return NULL;

	img->px = stbi_load(path, &img->width, &img->height, NULL, 4);

	if (NULL == img->px) {
		free(img);
		return NULL;
	}

	return img;
}

extern Image_t *
image_crop(const Image_t *img, const Geometry_t *geom)
{
	int x, y;
	Image_t *res;
	Geometry_t img_geom = {
		.x = 0,
		.y = 0,
		.width = img->width,
		.height = img->height
	};

	if (!geometry_contains(&img_geom, geom))
		return NULL;

	res = malloc(sizeof(Image_t));

	if (!res)
		return NULL;

	res->width = geom->width;
	res->height = geom->height;
	res->px = malloc(sizeof(uint32_t) * geom->width * geom->height);

	if (!res->px) {
		free(res);
		return NULL;
	}

	for (y = 0; y < geom->height; ++y)
		for (x = 0; x < geom->width; ++x)
			memcpy(&res->px[y*geom->width*4+x*4],
					&img->px[(y+geom->y)*img->width*4+(x+geom->x)*4],
					4);

	return res;
}

extern bool
image_save(const Image_t *img, const char *path)
{
	return stbi_write_png(path,
			img->width, img->height, 4,
			img->px, img->width * 4) != 0;
}

extern void
image_free(Image_t *img)
{
	free(img->px);
	free(img);
}
