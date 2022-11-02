/*
	Copyright (C) 2022 <alpheratz99@protonmail.com>

	This program is free software; you can redistribute it and/or modify it under
	the terms of the GNU General Public License version 2 as published by the
	Free Software Foundation.

	This program is distributed in the hope that it will be useful, but WITHOUT ANY
	WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
	FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

	You should have received a copy of the GNU General Public License along with
	this program; if not, write to the Free Software Foundation, Inc., 59 Temple
	Place, Suite 330, Boston, MA 02111-1307 USA

*/

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <string.h>
#include <setjmp.h>
#include <png.h>

struct image {
	int width, height;
	uint32_t *px;
};

struct geometry {
	int x, y;
	int width, height;
};

static void
die(const char *fmt, ...)
{
	va_list args;

	fputs("pngcrop: ", stderr);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n', stderr);
	exit(1);
}

static void
geometry_parse(const char *geometry, struct geometry *parsed_geometry)
{
	if (sscanf(geometry, "%dx%d+%d+%d",
			&parsed_geometry->width, &parsed_geometry->height,
			&parsed_geometry->x, &parsed_geometry->y) != 4)
		die("invalid geometry format");
}
	
static void
image_load(const char *path, struct image *image)
{
	FILE *fp;
	png_struct *png;
	png_info *pnginfo;
	png_byte **rows, bit_depth;
	int16_t x, y;

	if (NULL == (fp = fopen(path, "rb")))
		die("failed to open file %s: %s", path, strerror(errno));

	if (NULL == (png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
		die("png_create_read_struct failed");

	if (NULL == (pnginfo = png_create_info_struct(png)))
		die("png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png)) != 0)
		die("aborting due to libpng error");

	png_init_io(png, fp);
	png_read_info(png, pnginfo);

	image->width = png_get_image_width(png, pnginfo);
	image->height = png_get_image_height(png, pnginfo);
	image->px = malloc(sizeof(uint32_t) * image->width * image->height);

	bit_depth = png_get_bit_depth(png, pnginfo);

	png_set_interlace_handling(png);

	if (bit_depth == 16)
		png_set_strip_16(png);

	if (png_get_valid(png, pnginfo, PNG_INFO_tRNS))
		png_set_tRNS_to_alpha(png);

	switch (png_get_color_type(png, pnginfo)) {
		case PNG_COLOR_TYPE_RGB:
			png_set_filler(png, 0xff, PNG_FILLER_AFTER);
			break;
		case PNG_COLOR_TYPE_PALETTE:
			png_set_palette_to_rgb(png);
			png_set_filler(png, 0xff, PNG_FILLER_AFTER);
			break;
		case PNG_COLOR_TYPE_GRAY:
			if (bit_depth < 8)
				png_set_expand_gray_1_2_4_to_8(png);
			png_set_filler(png, 0xff, PNG_FILLER_AFTER);
			png_set_gray_to_rgb(png);
			break;
	}

	png_read_update_info(png, pnginfo);

	rows = png_malloc(png, sizeof(png_byte *) * image->height);

	for (y = 0; y < image->height; ++y)
		rows[y] = png_malloc(png, png_get_rowbytes(png, pnginfo));

	png_read_image(png, rows);

	for (y = 0; y < image->height; ++y) {
		for (x = 0; x < image->width; ++x) {
			image->px[y*image->width+x] = rows[y][x*4+0] << 16 |
				rows[y][x*4+1] << 8 |
				rows[y][x*4+2];
		}
		png_free(png, rows[y]);
	}

	png_free(png, rows);
	png_read_end(png, NULL);
	png_free_data(png, pnginfo, PNG_FREE_ALL, -1);
	png_destroy_read_struct(&png, NULL, NULL);
	fclose(fp);
}

static void
image_save(const char *path, struct image *image)
{
	int x, y;
	FILE *fp;
	png_struct *png;
	png_info *pnginfo;
	png_byte *row;

	if (NULL == (fp = fopen(path, "wb")))
		die("fopen failed: %s", strerror(errno));

	if (NULL == (png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)))
		die("png_create_write_struct failed");

	if (NULL == (pnginfo = png_create_info_struct(png)))
		die("png_create_info_struct failed");

	if (setjmp(png_jmpbuf(png)) != 0)
		die("aborting due to libpng error");

	png_init_io(png, fp);

	png_set_IHDR(
		png, pnginfo, image->width, image->height, 8, PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE
	);

	png_write_info(png, pnginfo);
	png_set_compression_level(png, 3);

	row = png_malloc(png ,image->width * 3);

	for (y = 0; y < image->height; ++y) {
		for (x = 0; x < image->width; ++x) {
			row[x*3+0] = (image->px[y*image->width+x] >> 16) & 0xff;
			row[x*3+1] = (image->px[y*image->width+x] >> 8) & 0xff;
			row[x*3+2] = image->px[y*image->width+x] & 0xff;
		}
		png_write_row(png, row);
	}

	png_free(png, row);
	png_write_end(png, NULL);
	png_free_data(png, pnginfo, PNG_FREE_ALL, -1);
	png_destroy_write_struct(&png, NULL);
	fclose(fp);
}

static void
image_crop(struct image *image, const char *geometry)
{
	int x, y;
	uint32_t *npx;
	struct geometry parsed_geometry;

	geometry_parse(geometry, &parsed_geometry);

	if (parsed_geometry.x < 0 || parsed_geometry.x >= image->width)
		die("invalid X coord");

	if (parsed_geometry.y < 0 || parsed_geometry.y >= image->height)
		die("invalid Y coord");

	if (parsed_geometry.width <= 0 || parsed_geometry.x + parsed_geometry.width >= image->width)
		die("invalid width");

	if (parsed_geometry.height <= 0 || parsed_geometry.y + parsed_geometry.height >= image->height)
		die("invalid height");

	npx = malloc(sizeof(uint32_t) * parsed_geometry.width * parsed_geometry.height);

	for (y = 0; y < parsed_geometry.height; ++y)
		for (x = 0; x < parsed_geometry.width; ++x)
			npx[y*parsed_geometry.width+x] =
				image->px[(parsed_geometry.y+y)*image->width+parsed_geometry.x+x];

	free(image->px);
	image->px = npx;
	image->width = parsed_geometry.width;
	image->height = parsed_geometry.height;
}

static void
usage(void)
{
	puts("usage: pngcrop [-hv] [geometry] [loadpath] [savepath]");
	exit(0);
}

static void
version(void)
{
	puts("pngcrop version "VERSION);
	exit(0);
}

int
main(int argc, char **argv)
{
	const char *geometry, *loadpath, *savepath;
	struct image image = { 0 };

	geometry = loadpath = savepath = NULL;

	while (++argv, --argc > 0) {
		if ((*argv)[0] == '-' && (*argv)[1] != '\0' && (*argv)[2] == '\0') {
			switch ((*argv)[1]) {
				case 'h': usage(); break;
				case 'v': version(); break;
				default: die("invalid option %s", *argv); break;
			}
		} else {
			if (NULL == geometry) geometry = *argv;
			else if (NULL == loadpath) loadpath = *argv;
			else if (NULL == savepath) savepath = *argv;
			else die("unexpected argument: %s", *argv);
		}
	}

	if (NULL == geometry)
		die("geometry argument is required");

	if (NULL == loadpath)
		die("loadpath argument is required");

	if (NULL == savepath)
		die("savepath argument is required");

	image_load(loadpath, &image);
	image_crop(&image, geometry);
	image_save(savepath, &image);

	return 0;
}
