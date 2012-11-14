#include "interface.h"

void
display_UI(viewer* image_viewer)
{
	// Main windows
	GtkWidget* window;
	
	// Window frame
	GtkWidget* window_frame;
	GtkWidget* canvas_frame;
	
	// Image canvas
	GtkWidget* canvas;

	// Image pixbuf, etc.
	GError* error;
	
	// Menu bar
	GtkWidget*       menu_bar;
	GtkUIManager*    menu_manager;
	GtkActionGroup*  menu_action_group;
	GtkActionGroup*  menu_action_image_group;
	
	// ------------------
	
	// Main windows
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Drawing helper");
	gtk_window_set_default_size(GTK_WINDOW(window), DH_W, DH_H);
	
	image_viewer->window = window;
	
	// Handling close button
	g_signal_connect(window, "destroy", G_CALLBACK(on_quit), image_viewer);
	
	// Window frame
	window_frame = gtk_vbox_new(FALSE, 0);
	
	// Menu
	menu_action_group       = gtk_action_group_new("menu_actions");
	menu_action_image_group = gtk_action_group_new("menu_actions_image");
	menu_manager = gtk_ui_manager_new();
	
	// Menu actions
	gtk_action_group_add_actions(menu_action_group,
	                             DH_menu_entries,
	                             DH_menu_nb_entries,
	                             image_viewer);
	gtk_action_group_add_toggle_actions(menu_action_group,
	                                    DH_menu_entries_toggle,
	                                    DH_menu_nb_entries_toggle,
	                                    image_viewer);
	gtk_action_group_add_actions(menu_action_image_group,
	                             DH_menu_entries_image,
	                             DH_menu_nb_entries_image,
	                             image_viewer);
	gtk_action_group_add_toggle_actions(menu_action_image_group,
	                                    DH_menu_entries_image_toggle,
	                                    DH_menu_nb_entries_image_toggle,
	                                    image_viewer);
	gtk_ui_manager_insert_action_group(menu_manager, menu_action_group, 0);
	gtk_ui_manager_insert_action_group(menu_manager, menu_action_image_group, 0);

	image_viewer->actions_image = menu_action_image_group;
	image_viewer->actions_grid  = menu_action_group;
	
	// Read menu UI from string (portable)
	error = NULL;
	gtk_ui_manager_add_ui_from_string(menu_manager, DH_interface_string, -1, &error);
	if (error)
	{
		_error("Building menu failed: %s", error->message);
		g_message("Building menu failed: %s", error->message);
		g_error_free(error);
	}
	
	// Adding menu to frame
	menu_bar = gtk_ui_manager_get_widget(menu_manager, "/main_menu");
	gtk_box_pack_start(GTK_BOX(window_frame), menu_bar, FALSE, FALSE, 0);
	
	// Make that the accelerators (keyboard shortcuts) work
	gtk_window_add_accel_group(GTK_WINDOW(window), 
	                           gtk_ui_manager_get_accel_group(menu_manager));

	// Canvas
	canvas = gtk_dhcanvas_new(image_viewer);
	image_viewer->canvas = canvas;

	// Canvas frame
	canvas_frame = gtk_hbox_new(FALSE, 0);

	// Status bar
	GtkWidget* statusbox = gtk_hbox_new(FALSE, 6);
	image_viewer->status_label = gtk_label_new(" File > Open (Ctrl+O) to open image");
	gtk_misc_set_alignment(GTK_MISC(image_viewer->status_label), 0.0, 0.5);
	
	gtk_box_pack_start(GTK_BOX(statusbox), image_viewer->status_label, TRUE, TRUE, 0);

	// Adding canvas to canvas frame
	gtk_box_pack_start(GTK_BOX(canvas_frame), canvas, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(canvas_frame), DH_MARGIN);

	// Adding canvas frame to window frame
	gtk_box_pack_start(GTK_BOX(window_frame), canvas_frame, TRUE, TRUE, 0);

	// Adding status bar to window frame
	gtk_box_pack_start(GTK_BOX(window_frame), statusbox, FALSE, TRUE, 0);

	// Adding window frame to windows
	gtk_container_add(GTK_CONTAINER(window), window_frame);
	
	// Centering window
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	// Showing window
	gtk_widget_show_all(window);
	
	// ---- Done ! ----
}


