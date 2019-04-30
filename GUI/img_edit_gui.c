#include <gtk/gtk.h>
#include <time.h>
#include "img_edit_gui.h"
#include "ImageEditing/image_edit.h"
#include "client.h"

typedef struct colors {
	gdouble r, g, b;
} colors;
int sockfd;
colors cols;
images_structure *images;
file_t files;


void remove_children(GtkWidget *widget) {
	GList *children, *iter;

	children = gtk_container_get_children(GTK_CONTAINER(widget));
	for(iter = children; iter != NULL; iter = g_list_next(iter))
	  gtk_widget_destroy(GTK_WIDGET(iter->data));
	g_list_free(children);
}

void on_close(GtkButton *closeBtn) {
	GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (closeBtn));
	gtk_widget_destroy(toplevel);
}

void on_delete(GtkWidget *window) {
	free(images);
	gtk_widget_destroy(window);
}

void on_send(GtkButton *sendBtn, file_browser_t *browser) {
	
	time_t timestamp = time(NULL);
	gchar* filename = malloc(20*sizeof(gchar));
	sprintf(filename, "%ld.jpg", timestamp);
	
	
	
	gchar* full_path = malloc(strlen(SHARED_FOLDER_PATH)+strlen(filename));
	strcpy(full_path, SHARED_FOLDER_PATH);
	strcat(full_path, filename);
	free(filename);

	delete_browser(browser->files);
	if (gdk_pixbuf_save (images->current, full_path, "jpeg", NULL, NULL))
		g_message("Image saved.");
	else 
		g_warning("Image could not be saved.");

	sendPicture(&sockfd, full_path);
	GList *children;
	children = gtk_container_get_children(GTK_CONTAINER(browser->wrapper));
	gtk_widget_destroy(GTK_WIDGET(children->data));
	g_list_free(children);

	GtkWidget *w = shared_browser(browser);
	gtk_container_add(GTK_CONTAINER(browser->wrapper), w);
	gtk_widget_show_all(browser->wrapper);

	GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (sendBtn));
	gtk_widget_destroy(toplevel);
}

void on_apply(GtkButton *applyBtn __attribute__((unused))) {
	images->current = gdk_pixbuf_copy(images->edited);
}

void on_remove(GtkButton *removeBtn __attribute__((unused))) {
	images->current = gdk_pixbuf_copy(images->original);
	images->edited = gdk_pixbuf_copy(images->original);
	image_show(images);
}

void on_gray(GtkButton *button __attribute__((unused)), GtkWidget *settingsView) {
	remove_children(settingsView);
	grayscale(images);
	image_show(images);
}

gboolean on_tint_scaled(GtkWidget *scale, GdkEvent *event __attribute__((unused)), gdouble *color) {
	*color = gtk_range_get_value(GTK_RANGE(scale));
	tint(images, cols.r, cols.g, cols.b);
	image_show(images);
	return FALSE;
}

void on_tint(GtkButton *button __attribute__((unused)), GtkWidget *settingsView) {
	images->edited = gdk_pixbuf_copy(images->current);
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	
	GtkWidget *redLabel = gtk_label_new("Red:");
	gtk_widget_set_halign(redLabel, GTK_ALIGN_START);
	GtkWidget *redScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
	                                            -100, 100, 10);
	gtk_scale_set_value_pos(GTK_SCALE(redScale),GTK_POS_BOTTOM);
	gtk_scale_set_has_origin(GTK_SCALE(redScale),FALSE);
	gtk_range_set_value(GTK_RANGE(redScale), 0);
	gtk_widget_set_valign(redScale, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), redLabel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), redScale, FALSE, FALSE, 0);
	
	GtkWidget *greenLabel = gtk_label_new("Green:");
	gtk_widget_set_halign(greenLabel, GTK_ALIGN_START);
	GtkWidget *greenScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
	                                            -100, 100, 10);
	gtk_scale_set_value_pos(GTK_SCALE(greenScale),GTK_POS_BOTTOM);
	gtk_scale_set_has_origin(GTK_SCALE(greenScale),FALSE);
	gtk_range_set_value(GTK_RANGE(greenScale), 0);
	gtk_widget_set_valign(greenScale, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), greenLabel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), greenScale, FALSE, FALSE, 0);

	GtkWidget *blueLabel = gtk_label_new("Blue:");
	gtk_widget_set_halign(blueLabel, GTK_ALIGN_START);
	GtkWidget *blueScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
	                                            -100, 100, 10);
	gtk_scale_set_value_pos(GTK_SCALE(blueScale),GTK_POS_BOTTOM);
	gtk_scale_set_has_origin(GTK_SCALE(blueScale),FALSE);
	gtk_range_set_value(GTK_RANGE(blueScale), 0);
	gtk_widget_set_valign(blueScale, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), blueLabel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), blueScale, FALSE, FALSE, 0);

	remove_children(settingsView);
	gtk_container_add(GTK_CONTAINER(settingsView), box);

	cols.r = 0;
	cols.g = 0;
	cols.b = 0;
	g_signal_connect(redScale, "button-release-event", G_CALLBACK(on_tint_scaled), &(cols.r));
	g_signal_connect(greenScale, "button-release-event", G_CALLBACK(on_tint_scaled), &(cols.g));
	g_signal_connect(blueScale, "button-release-event", G_CALLBACK(on_tint_scaled), &(cols.b));
	

	gtk_widget_show_all(settingsView);

	image_show(images);
}

gboolean on_shade_scaled(GtkWidget *scale, GdkEvent *event __attribute__((unused)), gdouble *shadeVal) {
	*shadeVal = gtk_range_get_value(GTK_RANGE(scale));
	shade(images, *shadeVal);
	image_show(images);
	return FALSE;
}

void on_shade(GtkButton *button __attribute__((unused)), GtkWidget *settingsView) {
	images->edited = gdk_pixbuf_copy(images->current);
	GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
	GtkWidget *shadeLabel = gtk_label_new("Shading:");
	gtk_widget_set_halign(shadeLabel, GTK_ALIGN_START);
	GtkWidget *shadeScale = gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL,
	                                            -100, 100, 10);
	gtk_scale_set_value_pos(GTK_SCALE(shadeScale),GTK_POS_BOTTOM);
	gtk_scale_set_has_origin(GTK_SCALE(shadeScale),FALSE);
	gtk_range_set_value(GTK_RANGE(shadeScale), 0);
	gtk_widget_set_valign(shadeScale, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(box), shadeLabel, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(box), shadeScale, FALSE, FALSE, 0);

	remove_children(settingsView);
	gtk_container_add(GTK_CONTAINER(settingsView), box);

	gdouble shadeVal = 0;
	g_signal_connect(shadeScale, "button-release-event", G_CALLBACK(on_shade_scaled), &shadeVal);

	gtk_widget_show_all(settingsView);

	image_show(images);
}

void on_blur(GtkButton *button __attribute__((unused)), GtkWidget *settingsView) {
	remove_children(settingsView);
	blur(images, TRUE);
	image_show(images);
}

void on_sharpen(GtkButton *button __attribute__((unused)), GtkWidget *settingsView) {
	remove_children(settingsView);
	sharpen(images);
	image_show(images);
}

void img_edit_window(gchar *path, file_browser_t *browser, int* sock) {
	
	GtkWidget *window;
	GtkWidget *header;
	GtkWidget *grid;
	GtkWidget *mainBox;

	GtkWidget *filters;
	GtkWidget *filtersView;
	GtkWidget *filtersFrame;
	GtkWidget *settingsView;
	GtkWidget *settingsFrame;
	GtkWidget *btnBox;

	GtkWidget *name;
	
	GtkWidget *gray;
	GtkWidget *tint;
	GtkWidget *shade;
	GtkWidget *blur;
	GtkWidget *sharpen;

	GtkWidget *closeBtn;
	GtkWidget *sendBtn;
	GtkWidget *removeBtn;
	GtkWidget *applyBtn;	

	GError *err = NULL;

	files = files;
	sockfd = *sock;

	images = (images_structure *) malloc(sizeof(images_structure));

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_set_size_request (window, 900, 600);

	gtk_window_set_title(GTK_WINDOW(window), "Edit image");

	mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	header = gtk_header_bar_new();
	gchar *filename = strrchr(path, '/');
	char name_text[256];
	if (filename)
		sprintf(name_text, "<big>%s</big>", filename+1);
	else	
		sprintf(name_text, "<big>%s</big>", path);
	name = gtk_label_new(name_text);
	gtk_label_set_use_markup(GTK_LABEL(name), TRUE);
	gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header), name);
	closeBtn = gtk_button_new_with_label("Close");
	gtk_widget_set_size_request(closeBtn, 70, 30);
	sendBtn = gtk_button_new_with_label("Send");
	gtk_widget_set_size_request(sendBtn, 70, 30);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), closeBtn);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(header), sendBtn);

	gtk_box_pack_start(GTK_BOX(mainBox), header, FALSE, FALSE, 0);

	grid = gtk_grid_new();
	gtk_grid_set_column_spacing (GTK_GRID(grid), 15);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 10);


	images->frame = gtk_scrolled_window_new(NULL,NULL);
	images->original = gdk_pixbuf_new_from_file(path, &err);
	images->edited = gdk_pixbuf_copy(images->original);
	images->current = gdk_pixbuf_copy(images->original);
	images->viewed = gdk_pixbuf_copy(images->original);
	images->view = gtk_image_new_from_pixbuf(images->viewed);
	gtk_widget_set_vexpand (images->frame, TRUE);
	gtk_widget_set_hexpand (images->frame, TRUE);

	gtk_container_add(GTK_CONTAINER(images->frame),images->view);
	gtk_grid_attach(GTK_GRID(grid), images->frame, 0, 1, 1, 3);


	filtersFrame = gtk_frame_new("Filters");
	gtk_frame_set_label_align(GTK_FRAME(filtersFrame), 0.1, 0.5);
	filtersView = gtk_scrolled_window_new(NULL,NULL);
	gtk_container_set_border_width(GTK_CONTAINER(filtersView), 5);
	filters = gtk_button_box_new(GTK_ORIENTATION_VERTICAL);
	gtk_box_set_spacing(GTK_BOX(filters), 5);
	gray = gtk_button_new_with_label("Grayscale");
	tint = gtk_button_new_with_label("Color scheme");
	shade = gtk_button_new_with_label("Shade");
	blur = gtk_button_new_with_label("Blur");
	sharpen = gtk_button_new_with_label("Sharpen");
	gtk_box_pack_start(GTK_BOX(filters), gray, FALSE, FALSE, 0);
	gtk_widget_set_valign (gray, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(filters), tint, FALSE, FALSE, 0);
	gtk_widget_set_valign (tint, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(filters), shade, FALSE, FALSE, 0);
	gtk_widget_set_valign (shade, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(filters), blur, FALSE, FALSE, 0);
	gtk_widget_set_valign (blur, GTK_ALIGN_START);
	gtk_box_pack_start(GTK_BOX(filters), sharpen, FALSE, FALSE, 0);
	gtk_widget_set_valign (sharpen, GTK_ALIGN_START);
	gtk_widget_set_valign (filters, GTK_ALIGN_START);

	gtk_container_add(GTK_CONTAINER(filtersView),filters);
	gtk_widget_set_size_request(filtersFrame, 155, 200);
	gtk_widget_set_vexpand (filtersView, TRUE);
	gtk_widget_set_vexpand (filtersFrame, TRUE);
	gtk_container_add(GTK_CONTAINER(filtersFrame),filtersView);
	gtk_grid_attach(GTK_GRID(grid), filtersFrame, 1, 1, 1, 1);


	settingsFrame = gtk_frame_new("Settings");
	gtk_frame_set_label_align(GTK_FRAME(settingsFrame), 0.1, 0.5);
	settingsView = gtk_scrolled_window_new(NULL,NULL);
	gtk_container_set_border_width(GTK_CONTAINER(settingsView), 5);
	gtk_widget_set_size_request(settingsFrame, 155, 100);
	gtk_widget_set_vexpand (settingsView, TRUE);
	gtk_widget_set_vexpand (settingsFrame, TRUE);
	gtk_container_add(GTK_CONTAINER(settingsFrame),settingsView);
	gtk_grid_attach(GTK_GRID(grid), settingsFrame, 1, 2, 1, 1);


	btnBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
	gtk_widget_set_size_request(btnBox, 150, 30);
	gtk_widget_set_vexpand (btnBox, FALSE);
	removeBtn = gtk_button_new_with_label("Remove all");
	gtk_widget_set_size_request(removeBtn, 75, 30);

	applyBtn = gtk_button_new_with_label("Apply");
	gtk_widget_set_size_request(applyBtn, 75, 30);
	gtk_box_pack_start(GTK_BOX(btnBox), removeBtn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(btnBox), applyBtn, FALSE, FALSE, 0);

	gtk_grid_attach(GTK_GRID(grid), btnBox, 1, 3, 1, 1);

	g_object_set (grid, "margin", 15, NULL);
	gtk_box_pack_start(GTK_BOX(mainBox), grid, TRUE, TRUE, 0);

	gtk_container_add(GTK_CONTAINER(window), mainBox);

	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(on_delete), G_OBJECT(window));
	g_signal_connect(closeBtn, "clicked", G_CALLBACK(on_close), NULL);
	g_signal_connect(sendBtn, "clicked", G_CALLBACK(on_send), browser);

	g_signal_connect(applyBtn, "clicked", G_CALLBACK(on_apply), NULL);
	g_signal_connect(removeBtn, "clicked", G_CALLBACK(on_remove), NULL);
	
	g_signal_connect(gray, "clicked", G_CALLBACK(on_gray), settingsView);
	g_signal_connect(tint, "clicked", G_CALLBACK(on_tint), settingsView);
	g_signal_connect(shade, "clicked", G_CALLBACK(on_shade), settingsView);
	g_signal_connect(blur, "clicked", G_CALLBACK(on_blur), settingsView);
	g_signal_connect(sharpen, "clicked", G_CALLBACK(on_sharpen), settingsView);
	
	gtk_widget_show_all(window);

	image_show(images);
}
