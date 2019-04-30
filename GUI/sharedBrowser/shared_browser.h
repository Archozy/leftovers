#pragma once

#define SHARED_FOLDER_PATH "shared_data/"

typedef struct file_t {
	gchar *path;
	GtkWidget *widget;
	GdkPixbuf *pb;
} file_t;

typedef struct file_browser_t{
	file_t *files;
	GtkWidget *wrapper;
} file_browser_t;

GtkWidget *shared_browser(file_browser_t *browser_original);

void delete_browser(file_t *files);
