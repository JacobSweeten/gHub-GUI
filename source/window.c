#include"window.h"

static void initWindow(GtkApplication* app, gpointer user_data)
{
	GtkWidget* window;

	window = gtk_application_window_new(app);
	gtk_window_set_title(GTK_WINDOW (window), DISPLAYNAME);
	gtk_window_set_default_size(GTK_WINDOW (window), 500, 400);
	gtk_widget_show_all(window);

	GList* deviceList = NULL;
	deviceList = g_list_append(deviceList, "Test1");
	deviceList = g_list_append(deviceList, "Test2");

	GtkWidget* comboBox = gtk_combo_box_new();
	gtk_combo_box_set_
}

int initDisplay()
{
	GtkApplication* app;

	app = gtk_application_new("com.github.what", G_APPLICATION_DEFAULT_FLAGS);
	g_signal_connect(app, "activate", G_CALLBACK(initWindow), NULL);
	int status = g_application_run(G_APPLICATION(app), 0, NULL);
	g_object_unref(app);

	return status;
}
