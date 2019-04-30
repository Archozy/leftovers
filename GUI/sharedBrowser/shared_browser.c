#include <gtk/gtk.h>
#include "shared_browser.h"

#define W_MAX 150
#define MAX_FILES 30

file_browser_t *browser;

void delete_browser(file_t *files) {
	for (int i = 0; i < MAX_FILES; i++) {
		if (files[i].path != NULL)
			free(files[i].path);
		else 
			break;
	}
	free(files);
}

gboolean confirm(GtkWidget *window, const gchar* message) {
	GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(window),
		                                       GTK_DIALOG_DESTROY_WITH_PARENT,
		                                       GTK_MESSAGE_QUESTION,
		                                       GTK_BUTTONS_OK_CANCEL,
		                                       message);

	if (gtk_dialog_run (GTK_DIALOG(dialog)) != GTK_RESPONSE_OK) {
		gtk_widget_destroy (dialog);
		return FALSE;
	} else {
		gtk_widget_destroy (dialog);
		return TRUE;
	}
}

void on_chosen(GtkWidget *box, gpointer button_click, file_t *file) {
	if (confirm(gtk_widget_get_toplevel(box), "Do you want to resend the file?"))
		img_edit_window(file->path, browser);
}

GtkWidget *shared_browser(file_browser_t *browser_original) {
	GtkWidget *main_box;

	browser = browser_original;

	main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	gtk_widget_set_size_request(main_box, W_MAX, 500);

	GDir *dir = g_dir_open(SHARED_FOLDER_PATH, 0, NULL);

	(browser->files) = malloc(MAX_FILES*sizeof(file_t));


	const gchar *names[MAX_FILES];
	int files_n = 0;
	while (files_n < MAX_FILES && (names[files_n] = g_dir_read_name(dir)) != NULL){
		int i = files_n-1;
		while (i >= 0 && strcmp(names[i], names[i+1]) < 0) {
			const gchar *tmp = names[i];
			names[i] = names[i+1];
			names[i+1] = tmp;
			i--;
		}

		files_n++;	
	}

	int i;
	for (i = 0; i < files_n; i++) {
		gchar* full_path = malloc(strlen(SHARED_FOLDER_PATH)+strlen(names[i]));
		strcpy(full_path, SHARED_FOLDER_PATH); 
		strcat(full_path, names[i]);

		(browser->files)[i].pb = gdk_pixbuf_new_from_file(full_path, NULL);

		(browser->files)[i].path = full_path;

		int w = gdk_pixbuf_get_width((browser->files)[i].pb);
		int h = gdk_pixbuf_get_height((browser->files)[i].pb);
		
		if (w > W_MAX) {
			h *= (float)W_MAX/w;
			w = W_MAX;
		}

		(browser->files)[i].pb = gdk_pixbuf_scale_simple((browser->files)[i].pb, w, h, GDK_INTERP_BILINEAR);
		(browser->files)[i].widget = gtk_image_new_from_pixbuf((browser->files)[i].pb);

		GtkWidget *box = gtk_event_box_new();
		gtk_container_add(GTK_CONTAINER(box), (browser->files)[i].widget); 
		
		gtk_box_pack_start(GTK_BOX(main_box), box, FALSE, FALSE, 0);
    	
	    gtk_widget_set_events (box, GDK_BUTTON_PRESS_MASK);
	    g_signal_connect (box, "button_press_event", G_CALLBACK(on_chosen), (browser->files)+i);
	}

	if (files_n < MAX_FILES) {
		(browser->files)[files_n].path = NULL;
	}
	g_dir_close (dir);

	return main_box;
}