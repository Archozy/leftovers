#include <gtk/gtk.h>
#include "client.h"
#include "img_edit_gui.h"
#include "sharedBrowser/shared_browser.h"
#include "encryption.h"
#include "client.h"

int sockfd = 0;
GtkWidget *chat;
gboolean connected;
file_browser_t *browser;

void recieved_text (gchar *m) {
	GtkTextIter e;

	GtkTextBuffer *chatBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat));
	gtk_text_buffer_get_end_iter(chatBuf, &e);
	gtk_text_buffer_insert(chatBuf, &e, m, -1);
	gtk_text_buffer_insert(chatBuf, &e, "\n", 1);
} 

static void on_open_image (GtkButton* button) {	
	if (!connected) {
		recieved_text("You are not connected to a server...");
		return;
	}

	// set up a dialog
	GtkWidget *toplevel = gtk_widget_get_toplevel (GTK_WIDGET (button));
	GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open image",
	                                                 GTK_WINDOW (toplevel),
	                                                 GTK_FILE_CHOOSER_ACTION_OPEN,
	                                                 "_Open", GTK_RESPONSE_ACCEPT,
	                                                 "_Cancel", GTK_RESPONSE_CANCEL,
	                                                 NULL);

	// set up a filter for only images
	GtkFileFilter *filter = gtk_file_filter_new ();
	gtk_file_filter_add_pixbuf_formats (filter);
	gtk_file_chooser_add_filter (GTK_FILE_CHOOSER (dialog), filter);
	
	// If dialog ended successfuly, manage the image
	if (gtk_dialog_run (GTK_DIALOG (dialog)) != GTK_RESPONSE_ACCEPT) {
		gtk_widget_destroy (dialog);
		return;
	}
	gchar *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
	gtk_widget_destroy(dialog);
	
	img_edit_window(filename, browser, &sockfd);
}

static void on_send_text (GtkButton* button __attribute__((unused)), GtkWidget *textFields[2]) {
	if (!connected) {
		recieved_text("You are not connected to a server...");
		return;
	}

	GtkTextBuffer *messageBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textFields[0]));
	
	GtkTextIter s, e;
	gtk_text_buffer_get_bounds(messageBuf, &s, &e);
	gchar *message = gtk_text_buffer_get_text(messageBuf, &s, &e, FALSE);
	gtk_text_buffer_set_text(messageBuf, "", -1);

	// | name this function however you want, #include the headerfile to your file in this file
	// | it takes gchar * as an argument, edit it however you want, 
	// V the result will then be shown in the main window (appended at the end)
	
	sentence(message, &sockfd);
	

	/*GtkTextBuffer *chatBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(textFields[1]));
	gtk_text_buffer_get_end_iter(chatBuf, &e);
	gtk_text_buffer_insert(chatBuf, &e, message, -1);*/
} 

static void on_connect(GtkButton* connectBtn, GtkWidget *name) {
	if (connected) {
		gtk_button_set_label(connectBtn, "Connect");
		recieved_text("Disconnected...");
		g_message("Disconnected");
		connected = FALSE;
		return; 
	}
	GtkWidget *window = gtk_widget_get_toplevel (GTK_WIDGET (connectBtn));
	GtkWidget *dialog;
	GtkDialogFlags flags = GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT;
	dialog = gtk_dialog_new_with_buttons ("Connect to a server",
	                                      GTK_WINDOW(window),
	                                      flags,
	                                      "_Connect", GTK_RESPONSE_ACCEPT,
	                                      "_Cancel", GTK_RESPONSE_REJECT,
	                                      NULL);
	GtkWidget *content = gtk_dialog_get_content_area (GTK_DIALOG (dialog));
	GtkWidget *grid;
	grid = gtk_grid_new();
	gtk_grid_set_column_spacing (GTK_GRID(grid), 5);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 5);

	GtkWidget *serverName;
	GtkWidget *serverText;
	GtkWidget *userName;
	GtkWidget *userText;
	GtkWidget *portName;
	GtkWidget *portText;

	serverName = gtk_label_new("Server:");
	gtk_widget_set_halign(serverName, GTK_ALIGN_END);
	userName = gtk_label_new("Username:");
	gtk_widget_set_halign(userName, GTK_ALIGN_END);
	portName = gtk_label_new("Port:");
	gtk_widget_set_halign(portName, GTK_ALIGN_END);
	serverText = gtk_text_view_new();
	gtk_widget_set_size_request(serverText, 120, 20);
	userText = gtk_text_view_new();
	gtk_widget_set_size_request(userText, 120, 20);
	portText = gtk_text_view_new();
	gtk_widget_set_size_request(portText, 120, 20);

	gtk_grid_attach(GTK_GRID(grid), serverName, 0, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), portName, 0, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), userName, 0, 2, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), serverText, 1, 0, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), portText, 1, 1, 1, 1);
	gtk_grid_attach(GTK_GRID(grid), userText, 1, 2, 1, 1);

	gtk_container_set_border_width(GTK_CONTAINER(grid), 10);
	gtk_container_add(GTK_CONTAINER(content), grid);
	gtk_widget_show_all (content);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) != GTK_RESPONSE_ACCEPT) {
		gtk_widget_destroy (dialog);
		return;
	}

	GtkTextIter s, e;
	GtkTextBuffer *buffer;
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(serverText));
	gtk_text_buffer_get_bounds(buffer, &s, &e);
	gchar *server = gtk_text_buffer_get_text(buffer, &s, &e, FALSE);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(portText));
	gtk_text_buffer_get_bounds(buffer, &s, &e);
	gchar *port = gtk_text_buffer_get_text(buffer, &s, &e, FALSE);
	buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(userText));
	gtk_text_buffer_get_bounds(buffer, &s, &e);
	gchar *user = gtk_text_buffer_get_text(buffer, &s, &e, FALSE);

	g_message("Connecting to server: %s:%s", server, port);
	g_message("as: %s", user);

	gchar userLabel[150];
	sprintf(userLabel, "<big>%s</big>", user);
	gtk_label_set_text(GTK_LABEL(name), userLabel);
	gtk_label_set_use_markup(GTK_LABEL(name), TRUE);
	gtk_button_set_label(GTK_BUTTON(connectBtn), "Disconnect");
	connectionServer(&sockfd, server, atoi(port), user);
	connected = TRUE;

	gtk_widget_destroy (dialog);
}  
  
int MSG(){
	char rec[1024];
	int y;
	char msg[1024];
	char usr[30];
	char endbuf[1024];
	int u = 0;
	memset(rec, 0, 1024);
	memset(msg, 0, 1024);
	memset(endbuf, 0, 1024);
	memset(usr, 0, 30);
	y = recv(sockfd, rec, 1024, 0);
	if(y <= 0){
		return -1;
	}
	printf("Brute -- %s\n", rec);
	if(rec[1] == '1'){
		int w = 0;
		for(int i = 2 ; rec[i] != 0; i++){
			if(rec[i] == ':'){
				w = i;
				i++;
			}
			if(w == 0){
				usr[i - 2] = rec[i];
			}
			else{
				msg[i - w - 1] = rec[i]; 
			}
		}
		decrypt(msg, 1);
		printf("<%s>%s\n", usr, msg);
		strcat(endbuf, usr);
		strcat(endbuf, ":");
		strcat(endbuf, msg);
		
		GtkTextIter e;

		GtkTextBuffer *chatBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat));
		gtk_text_buffer_get_end_iter(chatBuf, &e);
		gtk_text_buffer_insert(chatBuf, &e, endbuf, -1);
		gtk_text_buffer_insert(chatBuf, &e, "\n", 1);
	}
	if(rec[1] == '2'){
		GtkTextIter e;

		GtkTextBuffer *chatBuf = gtk_text_view_get_buffer(GTK_TEXT_VIEW(chat));
		gtk_text_buffer_get_end_iter(chatBuf, &e);
		gtk_text_buffer_insert(chatBuf, &e, "List of Connected people:", -1);
		gtk_text_buffer_insert(chatBuf, &e, "\n", 1);
		for(int i = 2 ; i < strlen(rec) ; i++){
			endbuf[i - 2] = rec[i];
		}
		printf("List of People on the Server \n %s", endbuf);
		gtk_text_buffer_get_end_iter(chatBuf, &e);
		gtk_text_buffer_insert(chatBuf, &e, endbuf, -1);
		gtk_text_buffer_insert(chatBuf, &e, "\n", 1);
		gtk_text_buffer_get_end_iter(chatBuf, &e);
		gtk_text_buffer_insert(chatBuf, &e, "------------------------------------------", -1);
		gtk_text_buffer_insert(chatBuf, &e, "\n", 1);
	}
}

void exit_app(GtkWidget* window __attribute__((unused)), gboolean *runtime) {
	*runtime = FALSE;
}

int main (int argc, char *argv[]) {
	GtkWidget *window;
	GtkWidget *mainBox;
	GtkWidget *grid;
	GtkWidget *topBox;
	GtkWidget *botBox;
	GtkWidget *header;

	GtkWidget *name;
	
	GtkWidget *message;
	GtkWidget *shared_column;
	GtkWidget *sendBtn;
	GtkWidget *connectBtn;
	GtkWidget *imgBtn;

	connected = FALSE;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_widget_set_size_request (window, 400, 500);

	gtk_window_set_title(GTK_WINDOW(window), "Chat client");
	
	mainBox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

	header = gtk_header_bar_new();
	
	name = gtk_label_new("");
	gtk_header_bar_set_custom_title(GTK_HEADER_BAR(header), name);

	connectBtn = gtk_button_new_with_label("Connect");
	gtk_widget_set_size_request(connectBtn, 70, 30);
	imgBtn = gtk_button_new_with_label("Send an image");
	gtk_widget_set_size_request(imgBtn, 70, 30);
	gtk_header_bar_pack_end(GTK_HEADER_BAR(header), imgBtn);
	gtk_header_bar_pack_start(GTK_HEADER_BAR(header), connectBtn);

	gtk_box_pack_start(GTK_BOX(mainBox), header, FALSE, FALSE, 0);

	grid = gtk_grid_new();
	gtk_grid_set_column_spacing (GTK_GRID(grid), 15);
	gtk_grid_set_row_spacing(GTK_GRID(grid), 5);

	gtk_container_set_border_width(GTK_CONTAINER(grid), 15);

	topBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	gtk_widget_set_hexpand(topBox, TRUE);


	chat = gtk_text_view_new();
	gtk_text_view_set_editable(GTK_TEXT_VIEW(chat), FALSE);
	gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(chat), FALSE);
	gtk_widget_set_vexpand (chat, TRUE);
	gtk_widget_set_hexpand (chat, TRUE);
	gtk_grid_attach(GTK_GRID(grid), chat, 0, 0, 1, 2);


	GtkWidget *shared_label = gtk_label_new("<big>Shared Files:</big>");
	gtk_label_set_use_markup(GTK_LABEL(shared_label), TRUE);
	gtk_grid_attach(GTK_GRID(grid), shared_label, 1, 0, 1, 1);

	browser = malloc(sizeof(file_browser_t));
	browser->wrapper = gtk_scrolled_window_new(NULL,NULL);
	gtk_container_set_border_width(GTK_CONTAINER(browser->wrapper), 5);
	gtk_widget_set_size_request(browser->wrapper, 170, 500);
	gtk_widget_set_vexpand (browser->wrapper, TRUE);

	GtkWidget *w = shared_browser(browser);
	gtk_widget_set_valign (w, GTK_ALIGN_START);

	gtk_container_add(GTK_CONTAINER(browser->wrapper), w);
	gtk_grid_attach(GTK_GRID(grid), browser->wrapper, 1, 1, 1, 1);


	botBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 3);
	message = gtk_text_view_new();
	gtk_widget_set_size_request(message, 70, 30);
	gtk_widget_set_hexpand (message, TRUE);	

	sendBtn = gtk_button_new_with_label("Send");
	gtk_widget_set_size_request(sendBtn, 70, 30);
	gtk_box_pack_start(GTK_BOX(botBox), message, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(botBox), sendBtn, FALSE, FALSE, 0);

	gtk_grid_attach(GTK_GRID(grid), botBox, 0, 2, 2, 1);
	gtk_widget_set_vexpand (grid, TRUE);
	gtk_widget_set_hexpand (grid, TRUE);
	gtk_widget_set_halign (grid, GTK_ALIGN_FILL);
	gtk_widget_set_valign (grid, GTK_ALIGN_FILL);
	
	gtk_box_pack_start(GTK_BOX(mainBox), grid, TRUE, TRUE, 0);
	gtk_container_add(GTK_CONTAINER(window), mainBox);

	GtkWidget *textFields[2];
	textFields[0] = message;
	textFields[1] = chat;
	
	g_signal_connect(imgBtn, "clicked", G_CALLBACK (on_open_image), NULL);
	g_signal_connect(sendBtn, "clicked", G_CALLBACK (on_send_text), textFields);
	g_signal_connect(connectBtn, "clicked", G_CALLBACK (on_connect), name);

	//g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(gtk_main_quit), G_OBJECT(window));
	gboolean runtime = TRUE;
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(exit_app), &runtime);

	gtk_widget_show_all(window);

	g_message("GUI started");

	on_connect(GTK_BUTTON(connectBtn), name);

	while (gtk_main_iteration_do(FALSE)) {
		if (!runtime)
			break;
		MSG();
		//other callback handling

		//this loop needs to be running infinitely, 
		//if you need to wait in your program anywhere, 
		//(and it cannot be done only once before the loop)
		//we will need to make it into threads

	}

	delete_browser(browser->files);

	return EXIT_SUCCESS;
}
