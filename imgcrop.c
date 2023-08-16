/*
	Copyright (C) 2022-2023 <alpheratz99@protonmail.com>

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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "geometry.h"
#include "image.h"

static void
die(const char *fmt, ...)
{
	va_list args;

	fputs("imgcrop: ", stderr);
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	fputc('\n', stderr);
	exit(1);
}

static void
usage(void)
{
	puts("usage: imgcrop [-hv] [-i input] [-o output] [geometry]");
	exit(0);
}

static void
version(void)
{
	puts("imgcrop version "VERSION);
	exit(0);
}

int
main(int argc, char **argv)
{
	const char *inpath, *outpath, *geometry_str;
	Image_t *in_img, *out_img;
	Geometry_t *geom;

	geometry_str = NULL;
	inpath = outpath = NULL;

	while (++argv, --argc > 0) {
		if ((*argv)[0] == '-' && (*argv)[1] != '\0' && (*argv)[2] == '\0') {
			switch ((*argv)[1]) {
			case 'h': usage(); break;
			case 'v': version(); break;
			case 'i': --argc; inpath = *++argv; break;
			case 'o': --argc; outpath = *++argv; break;
			default: die("invalid option %s", *argv); break;
			}
		} else {
			if (NULL != geometry_str)
				die("unexpected argument: %s", *argv);
			geometry_str = *argv;
		}
	}

	if (NULL == geometry_str)
		die("geometry argument is required");

	if (NULL == inpath || NULL == outpath)
		die("you must specify an input and output path");

	if (NULL == (geom = geometry_parse(geometry_str)))
		die("invalid geometry");

	if (NULL == (in_img = image_load(inpath)))
		die("couldn't load image");

	if (NULL == (out_img = image_crop(in_img, geom)))
		die("couldn't crop image");

	if (!image_save(out_img, outpath))
		die("couldn't save image");

	geometry_free(geom);
	image_free(in_img);
	image_free(out_img);

	return 0;
}
