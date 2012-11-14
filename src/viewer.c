#include "viewer.h"

viewer*
new_viewer(void)
{
	viewer* v = g_malloc(sizeof(viewer));
	
	// Viewer initialization	
	v->pixbuf  = NULL;
	v->surface = NULL;
	v->path    = NULL;
	v->width   = 0;
	v->height  = 0;

	v->alt     = NULL;
	v->view    = TRUE;

	v->actions_image = NULL;
	v->actions_grid  = NULL;

	v->update = FALSE;

	return v;
}

gboolean
viewer_load_file(viewer* v, const gchar* path)
{
	GError* error;

	// Delete latest file and image if exists
	g_free(v->path);
	if (v->pixbuf)
	{
		g_object_unref(v->pixbuf);
		v->pixbuf = NULL;
	}

	// Original image
	v->view = TRUE;

	if (v->alt)
	{
		g_object_unref(v->alt);
		v->alt = NULL;
	}

	// Loading new file
	error   = NULL;
	v->path = g_strdup(path);
	
	// If an image URL was given to load
	if (v->path != NULL)
	{
		v->pixbuf = gdk_pixbuf_new_from_file(v->path, &error);
	
		if (!v->pixbuf)
		{
			_error("Error while loading image '%s' :\n"
			       "%s\n", v->path, error->message);
			
			// Displaying error with a dialog
			GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(v->window),
			                                           GTK_DIALOG_DESTROY_WITH_PARENT,
			                                           GTK_MESSAGE_ERROR,
			                                           GTK_BUTTONS_CLOSE,
			                                           "Error while loading file '%s': %s",
			                                           v->path, error->message);
			gtk_dialog_run(GTK_DIALOG(dialog));
			gtk_widget_destroy(dialog);
			g_error_free(error);
			
			// No image to load but we need to fix window size
			gtk_window_resize(GTK_WINDOW(v->window), DH_W, DH_H);
		}
		else
		{
			_debug("--\nLoading image '%s'", v->path);

			// Init view (centered & zoomed)
			viewer_init(v, TRUE);

			// Centering window
			gtk_window_set_position(GTK_WINDOW(v->window), GTK_WIN_POS_CENTER);

			// Redrawing image
			v->update = TRUE;
			gtk_widget_queue_draw(v->canvas);
		}
	}
	else
	{
		// No image to load but we need to fix window size
		gtk_window_resize(GTK_WINDOW(v->window), DH_W, DH_H);
	}

	// Impossible to use altered image actions at first
	gtk_action_group_set_sensitive(v->actions_image, FALSE);
	GtkAction* action = gtk_action_group_get_action(v->actions_image, "toggle_view_action");
	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(action), FALSE);

	return (v->pixbuf != NULL);
}

void
viewer_init(viewer* v, gboolean new)
{
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);
	int w = gdk_pixbuf_get_width(v->pixbuf),
	    h = gdk_pixbuf_get_height(v->pixbuf);
	
	v->width  = w;
	v->height = h;

	// Auto-Zoom
	viewer_zoom_auto(v, new);

	if (new)
	{
		canvas->grid = FALSE;
	}

	// Angle
	canvas->angle = 0.0;
}

void
viewer_zoom_auto(viewer* v, gboolean resize)
{
	GdkScreen*   screen = gdk_screen_get_default();
	GtkDHCanvas* canvas = GTK_DHCANVAS(v->canvas);
	
	int w  = gdk_pixbuf_get_width(v->pixbuf),
	    h  = gdk_pixbuf_get_height(v->pixbuf),
	    rw = w,
	    rh = h,
	    iw = gdk_screen_get_width(screen),
	    ih = gdk_screen_get_height(screen),
	    maxw = DH_PROPORT * iw,
	    maxh = DH_PROPORT * ih;

	// Auto-zoom (when > max sizes)
	if (rw > maxw || rh > maxh)
	{
		gdouble autozoom   = d_min(maxw/(1.0 * rw),
		                           maxh/(1.0 * rh));
		canvas->zoom_level = autozoom;
		w *= autozoom;
		h *= autozoom;
	}
	else
	{
		// Zoom : 100%
		canvas->zoom_level = 1.0;

		// Minimal size
		if (w < DH_W) { w = DH_W; }
		if (h < DH_H) { h = DH_H; }
	}

	// Changing focus point
	canvas->focus_point_x = rw / 2;
	canvas->focus_point_y = rh / 2;

	// Resizing windows to the good size
	if (resize)
	{
		gtk_window_set_default_size(GTK_WINDOW(v->window), w, h);
		gtk_window_resize(GTK_WINDOW(v->window), w, h);
	}
}

void
viewer_show_original(viewer* v)
{
	if (v->pixbuf)
	{
		v->view = TRUE;
	}

	GtkAction *action = gtk_action_group_get_action(v->actions_image, "toggle_view_action");
	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(action), FALSE);

	// Redrawing image
	v->update = TRUE;
	gtk_widget_queue_draw(v->canvas);
}

void
viewer_show_altered(viewer* v)
{
	if (v->alt)
	{
		v->view = FALSE;
	}

	GtkAction *action = gtk_action_group_get_action(v->actions_image, "toggle_view_action");
	gtk_toggle_action_set_active(GTK_TOGGLE_ACTION(action), TRUE);

	// Redrawing image
	v->update = TRUE;
	gtk_widget_queue_draw(v->canvas);
}

void
viewer_toggle_view(viewer* v)
{
	// Original -> Altered
	if (v->view)
	{
		viewer_show_altered(v);
	}
	// Altered -> Original
	else 
	{
		viewer_show_original(v);
	}
}

void
viewer_apply_filter(viewer* v, ImageFilterFunc f, gint param)
{
	// If altered version of image isn't already created,
	// we copy it from the original image
	if (v->pixbuf != NULL && v->alt == NULL)
	{
		v->alt = gdk_pixbuf_copy(v->pixbuf);
	}

	// Applying filter to "altered" image
	// and displaying it
	if (v->alt != NULL)
	{
		apply_filter(v->alt, f, param);
		v->update = TRUE;
		viewer_show_altered(v);
	}

	// Image actions are enabled
	gtk_action_group_set_sensitive(v->actions_image, TRUE);
}

static void
viewer_apply_filter_special_finished(gpointer data)
{
	viewer *v = data;

	v->update = TRUE;
	viewer_show_altered(v);
	gtk_action_group_set_sensitive(v->actions_image, TRUE);
}

void
viewer_apply_filter_special(viewer* v, ImageSpecialFilterFunc f, gint param)
{
	// If altered version of image isn't already created,
	// we copy it from the original image
	if (v->pixbuf != NULL && v->alt == NULL)
	{
		v->alt = gdk_pixbuf_copy(v->pixbuf);
	}

	// Applying special filter to "altered" image
	// and displaying it
	if (v->alt != NULL)
	{
		/*apply_filter_special(v->alt, f, param);
		v->update = TRUE;
		viewer_show_altered(v);*/
		apply_filter_special_async(v->alt, f, param, viewer_apply_filter_special_finished, v);
	}

	// Image actions are enabled
	/*gtk_action_group_set_sensitive(v->actions_image, TRUE);*/
}


void
free_viewer(viewer* v)
{
	if (v)
	{
		if (v->pixbuf)
		{
			g_object_unref(v->pixbuf);
		}

		if (v->alt)
		{
			g_object_unref(v->alt);
		}

		if (v->surface)
		{
			cairo_surface_destroy(v->surface);
		}
		g_free(v->path);
		g_free(v);
	}
}
