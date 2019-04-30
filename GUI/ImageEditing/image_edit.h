#pragma once
#include <gtk/gtk.h>

typedef struct images {
	GdkPixbuf *current, *edited, *original, *viewed;
	GtkWidget *frame, *view;
} images_structure;

void grayscale(images_structure *images);

void tint(images_structure *images, gdouble r, gdouble g, gdouble b);

void shade(images_structure *images, gdouble shadeVal);

void blur(images_structure *images, gboolean gaussian);

void sharpen(images_structure *images);

void image_show(images_structure *images);