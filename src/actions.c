#include "actions.h"

// File menu

void
on_quit(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Quit");
	
	// Data = image_viewer
	free_viewer(data);
	
	// Done !
	gtk_exit(0);
}

void
on_save(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Save as");

	viewer* v = (viewer*) data;

	if (v->alt)
	{
		GtkWidget* dialog = gtk_file_chooser_dialog_new("Save filtered image (PNG)",
			                                            GTK_WINDOW(v->window),
			                                            GTK_FILE_CHOOSER_ACTION_SAVE,
			                                            GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			                                            GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			                                            NULL);
		gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), g_path_get_dirname(v->path));

		if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT)
		{
			gchar* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
			GError* error = NULL;
	 		gdk_pixbuf_save(v->alt, filename, "png", &error, NULL);

			if (error)
			{
				_error("Error while saving file : %s", error->message);
			}

			g_free(filename);
		}

		gtk_widget_destroy(dialog);
	}
}

void
on_open(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Open");

	GtkWidget*     dialog;
	GtkFileFilter* filter;
	
	// Data = image_viewer
	viewer* image_viewer = data;

	// File chooser dialog
	dialog = gtk_file_chooser_dialog_new("Open an image file",
	                  GTK_WINDOW(image_viewer->window),
	                  GTK_FILE_CHOOSER_ACTION_OPEN,
	                  GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                  GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	                  NULL);
	
	// Current folder = last used folder
	if (image_viewer->path != NULL)
	{
		gchar* dir = g_path_get_dirname(image_viewer->path);
		gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), dir);
		g_free(dir);
	}

	// Filter to only choose images
	filter = gtk_file_filter_new();
	gtk_file_filter_set_name(GTK_FILE_FILTER(filter), "Images");
	gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/jpeg");
	gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/png");
	gtk_file_filter_add_mime_type(GTK_FILE_FILTER(filter), "image/gif");
	gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), GTK_FILE_FILTER(filter));
	
	// Runnning dialog
	if (gtk_dialog_run(GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		gchar* path;
		
		// Getting file URL from dialog
		path = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));

		// Loading image ...
		viewer_load_file(image_viewer, path);
		
		g_free(path);
	}
	
	// Closing dialog
	gtk_widget_destroy(dialog);
}

void
on_full_view (GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);

	canvas->zoom_level = 1.0;

	// Redrawing image
	gtk_widget_queue_draw(v->canvas);
}

void
on_zoomp_view(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);

	canvas->zoom_level *= (1 + DH_SCROLL);

	// Redrawing image
	gtk_widget_queue_draw(v->canvas);
}

void
on_zoomm_view(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);

	canvas->zoom_level *= (1 - DH_SCROLL);

	// Redrawing image
	gtk_widget_queue_draw(v->canvas);
}

void
on_ideal_view(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;

	// Auto-zoom
	viewer_zoom_auto(v, FALSE);

	// Redrawing image
	gtk_widget_queue_draw(v->canvas);
}

void
on_reset_angle(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);

	canvas->angle = 0.0;

	gtk_widget_queue_draw(v->canvas);
}

void
on_turn_right (GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);

	canvas->angle += (DH_ROTATION * G_PI / 180);

	gtk_widget_queue_draw(v->canvas);
}

void
on_turn_left  (GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);

	canvas->angle -= (DH_ROTATION * G_PI / 180);

	gtk_widget_queue_draw(v->canvas);
}

void
on_full_angle (GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);

	canvas->angle = (G_PI);

	gtk_widget_queue_draw(v->canvas);
}


void
on_reset_view(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;
	
	viewer_init(v, FALSE);

	// Redrawing image
	gtk_widget_queue_draw(v->canvas);
}

void
on_custom_threshold(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Custom threshold");

	viewer* v = (viewer*) data;

	// Create dialog
	GtkWidget* dialog = gtk_dialog_new_with_buttons(
		"Custom threshold configuration",
		GTK_WINDOW(v->window),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_REJECT,
		GTK_STOCK_OK,
		GTK_RESPONSE_ACCEPT,
		NULL
	);

	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

	// Widget
	GtkBox* content = GTK_BOX(GTK_DIALOG(dialog)->vbox);

	// Thresold value
	GtkWidget* threshold_label = gtk_label_new_with_mnemonic("Threshold value (5-250)");
	GtkWidget* threshold_value = gtk_spin_button_new_with_range(5, 250, 1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(threshold_label), threshold_value);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(threshold_value), 128);
	gtk_entry_set_activates_default(GTK_ENTRY(threshold_value), TRUE);

	// Adding widget to dialog
	gtk_box_pack_start(content, threshold_label, FALSE, TRUE, 0);
	gtk_box_pack_start(content, threshold_value, FALSE, TRUE, 0);

	// Show widgets
	gtk_widget_show_all(dialog);

	// Running dialog
	gint result = gtk_dialog_run(GTK_DIALOG(dialog));
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT :
			// Using user value to threshold
			viewer_apply_filter(v, filter_threshold,
				                gtk_spin_button_get_value(GTK_SPIN_BUTTON(threshold_value)));
			break;
		default :
			// Nothing to do !
			break;
	}

	// Destroying dialog
	gtk_widget_destroy(dialog);
}

void
on_average_threshold(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Average threshold");

	// Computing average value of gray of image
	viewer* v = (viewer*) data;
	
	// If an image is already loaded
	GdkPixbuf* pixbuf = (v->alt != NULL) ? v->alt : v->pixbuf;
	if (pixbuf == NULL)
	{
		return;
	}
	
	gint width        = gdk_pixbuf_get_width(pixbuf);
	gint height       = gdk_pixbuf_get_height(pixbuf);
	gint rowstride    = gdk_pixbuf_get_rowstride(pixbuf);
	gint n_channels   = gdk_pixbuf_get_n_channels(pixbuf);
	guchar* p         = gdk_pixbuf_get_pixels(pixbuf);
	guchar* c;

	gint i,
	     j,
	     n = 0,
	     val = 0;
	
	// For each pixel
	for (i=0; i<width; i++)
	{
		for (j=0; j<height; j++)
		{
			// Computing sum of grey pixels
			n++;
			c = p + i*n_channels + j*rowstride;
			val += (c[R]+c[G]+c[B])/3;
		}
	}
	
	// Using average to threshold
	viewer_apply_filter((viewer*) data, filter_threshold, val/n);
}

void
on_median_threshold(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Median threshold");

	// Computing median value of gray of image
	viewer* v = (viewer*) data;
	
	// If an image is already loaded
	GdkPixbuf* pixbuf = (v->alt != NULL) ? v->alt : v->pixbuf;
	if (pixbuf == NULL)
	{
		return;
	}
	
	gint width        = gdk_pixbuf_get_width(pixbuf);
	gint height       = gdk_pixbuf_get_height(pixbuf);
	gint rowstride    = gdk_pixbuf_get_rowstride(pixbuf);
	gint n_channels   = gdk_pixbuf_get_n_channels(pixbuf);
	guchar* p         = gdk_pixbuf_get_pixels(pixbuf);
	guchar* c;
	
	gint i,
	     j,
	     g,
	     n = 0,
	     m = 128;

	// Array of grey values set to 0 for each value
	gint grey_values[256];
	for (i=0; i<256; i++)
	{
		grey_values[i] = 0;
	}

	// For each pixel
	for (i=0; i<width; i++)
	{
		for (j=0; j<height; j++)
		{
			// We get the current color and compute its grey value
			c = p + i*n_channels + j*rowstride;
			g = (c[R]+c[G]+c[B])/3;
			
			// We found another <g> grey value
			grey_values[g] ++;

			// One pixel more
			n++;
		}
	}

	// Looking for median
	for (i=0, j=0; i<256; i++)
	{
		g = grey_values[i];

		// We found our median value
		if (j < n/2 && j + g > n/2)
		{
			m = g;
		}

		// Calculating sum of precedent values (ECC)
		j += grey_values[i];
	}

	// Applying threshold with median
	viewer_apply_filter((viewer*) data, filter_threshold, m);
}

void
on_dithering(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Dithering");

	viewer_apply_filter_special((viewer*) data, filter_ordered_dithering, -1);
}

void
on_random(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Random threshold");

	viewer_apply_filter((viewer*) data, filter_random, -1);
}

void
on_grayscale_average(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Grayscale (average)");

	viewer_apply_filter((viewer*) data, filter_average, -1);
}

void
on_grayscale_radiance(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Grayscale (radiance)");

	viewer_apply_filter((viewer*) data, filter_radiance, -1);
}

void
on_posterize(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Posterize");

	viewer* v = (viewer*) data;

	// Create dialog
	GtkWidget* dialog = gtk_dialog_new_with_buttons(
		"Posterize configuration",
		GTK_WINDOW(v->window),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_REJECT,
		GTK_STOCK_OK,
		GTK_RESPONSE_ACCEPT,
		NULL
	);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

	// Widget
	GtkBox* content = GTK_BOX(GTK_DIALOG(dialog)->vbox);

	// Posterisation : number of grey levels
	GtkWidget* posterize_label = gtk_label_new_with_mnemonic("Grey levels (3-15)");
	GtkWidget* posterize_value = gtk_spin_button_new_with_range(3, 15, 1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(posterize_label), posterize_value);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(posterize_value), 6);
	gtk_entry_set_activates_default(GTK_ENTRY(posterize_value), TRUE);

	// Adding widget to dialog
	gtk_box_pack_start(content, posterize_label, FALSE, TRUE, 0);
	gtk_box_pack_start(content, posterize_value, FALSE, TRUE, 0);

	// Show widgets
	gtk_widget_show_all(dialog);

	// Running dialog
	gint result = gtk_dialog_run(GTK_DIALOG(dialog));
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT :
			// Using user value to threshold
			viewer_apply_filter_special(v, filter_posterize,
				               gtk_spin_button_get_value(GTK_SPIN_BUTTON(posterize_value)));
			break;
		default :
			// Nothing to do !
			break;
	}

	// Destroying dialog
	gtk_widget_destroy(dialog);
}

void
on_sepia(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Sepia");

	viewer_apply_filter((viewer*) data, filter_sepia, -1);
}

void
on_blur(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Blur");

	viewer* v = (viewer*) data;

	// Create dialog
	GtkWidget* dialog = gtk_dialog_new_with_buttons(
		"Blur configuration",
		GTK_WINDOW(v->window),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_REJECT,
		GTK_STOCK_OK,
		GTK_RESPONSE_ACCEPT,
		NULL
	);

	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

	// Widget
	GtkBox* content = GTK_BOX(GTK_DIALOG(dialog)->vbox);

	// Blur level
	GtkWidget* blur_label = gtk_label_new_with_mnemonic("Blur level (2-15)");
	GtkWidget* blur_value = gtk_spin_button_new_with_range(2, 15, 1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(blur_label), blur_value);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(blur_value), 6);
	gtk_entry_set_activates_default(GTK_ENTRY(blur_value), TRUE);

	// Adding widget to dialog
	gtk_box_pack_start(content, blur_label, FALSE, TRUE, 0);
	gtk_box_pack_start(content, blur_value, FALSE, TRUE, 0);

	// Show widgets
	gtk_widget_show_all(dialog);

	// Running dialog
	gint result = gtk_dialog_run(GTK_DIALOG(dialog));

	switch (result)
	{
		case GTK_RESPONSE_ACCEPT :
			// Using user value to threshold
			viewer_apply_filter_special(v, filter_blur,
				                gtk_spin_button_get_value(GTK_SPIN_BUTTON(blur_value)));
			break;
		default :
			// Nothing to do !
			break;
	}

	// Destroying dialog
	gtk_widget_destroy(dialog);
}

void
on_edge_detection(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Edge detection");

	viewer_apply_filter_special((viewer*) data, filter_edge_detection, -1);
}

void
on_invert(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Invert colors");

	viewer_apply_filter((viewer*) data, filter_invert, -1);
}

void
on_remove_red(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Remove red");

	viewer_apply_filter((viewer*) data, filter_remove_red, -1);
}

void
on_remove_green(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Remove green");

	viewer_apply_filter((viewer*) data, filter_remove_green, -1);
}

void
on_remove_blue(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Remove blue");

	viewer_apply_filter((viewer*) data, filter_remove_blue, -1);
}

void
on_toggle_view(GtkToggleAction* action, gpointer data)
{
	if (gtk_toggle_action_get_active(action))
	{
		viewer_show_altered(data);
	}
	else
	{
		viewer_show_original(data);
	}	
}

void
on_remove_filters(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("####");

	// Data = image_viewer
	viewer* v = data;
	
	// Displaying original image as long as 
	// the filtered image is being freed
	viewer_show_original(v);
	
	if (v->alt)
	{
		g_object_unref(v->alt);
		v->alt = NULL;
	}

	// Image actions are disabled
	gtk_action_group_set_sensitive(v->actions_image, FALSE);
}

// Grid menu

void
on_configure_grid(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v = (viewer*) data;
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);

	// Remember last config
	gboolean old_grid  = canvas->grid;
	gint     old_size  = canvas->grid_size;
	gdouble  old_alpha = canvas->grid_color.a * DH_COLORS;
	GdkColor old_color = {
		.red   = canvas->grid_color.r * DH_COLORS,
		.green = canvas->grid_color.g * DH_COLORS,
		.blue  = canvas->grid_color.b * DH_COLORS,
	};

	// We temporary show the grid to configure it
	canvas->grid = TRUE;
	gtk_widget_queue_draw(GTK_WIDGET(canvas));

	// Creating dialog
	GtkWidget* dialog = gtk_dialog_new_with_buttons(
		"Configure grid",
		GTK_WINDOW(v->window),
		GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CANCEL,
		GTK_RESPONSE_REJECT,
		GTK_STOCK_OK,
		GTK_RESPONSE_ACCEPT,
		NULL
	);
	gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_ACCEPT);

	// Widgets
	GtkBox* content = GTK_BOX(GTK_DIALOG(dialog)->vbox);

	// Grid size
	GtkWidget* grid_size_label = gtk_label_new_with_mnemonic("Grid _size");
	GtkWidget* grid_size = gtk_spin_button_new_with_range(5, 30, 1);
	gtk_label_set_mnemonic_widget(GTK_LABEL(grid_size_label), grid_size);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(grid_size), canvas->grid_size);
	gtk_entry_set_activates_default(GTK_ENTRY(grid_size), TRUE);

	// Grid size changed signal
	g_signal_connect(grid_size, "value-changed", G_CALLBACK(on_grid_size_changed), canvas);
	
	// Adding grid size to dialog
	gtk_box_pack_start(content, grid_size_label, FALSE, TRUE, 0);
	gtk_box_pack_start(content, grid_size, FALSE, TRUE, 0);

	// Color
	GtkWidget* grid_color_label = gtk_label_new_with_mnemonic("Grid _color");
	GtkWidget* grid_color = gtk_color_button_new();
	gtk_label_set_mnemonic_widget(GTK_LABEL(grid_color_label), grid_color);
	gtk_color_button_set_use_alpha(GTK_COLOR_BUTTON(grid_color), TRUE);

	gtk_color_button_set_color(GTK_COLOR_BUTTON(grid_color), &old_color);
	gtk_color_button_set_alpha(GTK_COLOR_BUTTON(grid_color), old_alpha);

	// Grid color changed signal
	g_signal_connect(grid_color, "color-set", G_CALLBACK(on_grid_color_set), canvas);

	gtk_box_pack_start(content, grid_color_label, FALSE, TRUE, 0);
	gtk_box_pack_start(content, grid_color, FALSE, TRUE, 0);

	// Showing all widgets
	gtk_widget_show_all(dialog);

	// Running dialog
	gint result = gtk_dialog_run(GTK_DIALOG(dialog));
	switch (result)
	{
		case GTK_RESPONSE_ACCEPT :
			// If grid wasn't shown, show it
			if (! old_grid)
			{
				canvas->grid = old_grid;
				GtkAction* action = gtk_action_group_get_action(v->actions_grid,
					                                            "grid_toggle_action");
				gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(action), TRUE);
			}
			break;
		default :
			canvas->grid         = old_grid;
			canvas->grid_size    = old_size;
			canvas->grid_color.a = old_alpha / DH_COLORS;
			canvas->grid_color.r = old_color.red   / DH_COLORS;
			canvas->grid_color.g = old_color.green / DH_COLORS;
			canvas->grid_color.b = old_color.blue  / DH_COLORS;
			break;
	}

	// Redrawing canvas
	gtk_widget_queue_draw(GTK_WIDGET(canvas));

	// Destroying dialog
	gtk_widget_destroy(dialog);
}

void
on_grid_toggle(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	viewer* v      = (viewer*) data;
	GtkDHCanvas* c = GTK_DHCANVAS(v->canvas);

	c->grid = !c->grid;

	// Redrawing image
	gtk_widget_queue_draw(v->canvas);
}

// Help menu

void
on_summary(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("Help summary");
	
	GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(((viewer*) data)->window),
	                                           GTK_DIALOG_DESTROY_WITH_PARENT,
	                                           GTK_MESSAGE_INFO,
	                                           GTK_BUTTONS_CLOSE,
	                                           "Help not done yet.. Please wait :)");
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

void
on_about(GtkWidget* widget G_GNUC_UNUSED, gpointer data)
{
	_debug("About Drawing Helper");
	
	const gchar *authors[] = {
	    "Sacha Wilt <sacha.wilt@gmail.com>",
	    "\nContributors :",
	    "Colomban Wendling",
	    NULL
	};
	
	const gchar *artists[] = {
	    "Sacha Wilt <sacha.wilt@gmail.com>",
	    NULL
	};
	
	const gchar *documenters[] = {
	    "Sacha Wilt <sacha.wilt@gmail.com>",
	    NULL
	};
	
	GtkWidget *dialog = g_object_new(
		GTK_TYPE_ABOUT_DIALOG,
	    "transient-for", ((viewer*) data)->window,
	    "program-name", "Drawing Helper",
	    "version", "0.1",
	    "copyright", "(C) 2011/2012 - Sacha Wilt",
	    "comments",
	    	"A tiny and shiny image viewer "
	        "with special filters/effects "
	        "to help drawing",
	    "license",
	    	"Drawing Helper - Copyright (C) 2011/2012 - Sacha Wilt\n\n"
	               
	        "This program is free software: you can redistribute it and/or modify\n"
	        "it under the terms of the GNU General Public License as published by\n"
	        "the Free Software Foundation, either version 3 of the License, or\n"
	        "(at your option) any later version.\n\n"
	        
	        "This program is distributed in the hope that it will be useful,\n"
	        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	        "GNU General Public License for more details.\n\n"
	        
	        "You should have received a copy of the GNU General Public License\n"
	        "along with this program.  If not, see <http://www.gnu.org/licenses/>.",
	    //"logo", logo_pixbuf,
	    "authors",     authors,
	    "artists",     artists,
	    "documenters", documenters,
	    NULL);
	
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}
