#ifndef _DH_VIEWER_H_
#define _DH_VIEWER_H_

// Included files

#include <gtk/gtk.h>
#include <glib.h>
#include <cairo.h>
#include "utils.h"

// Structures

typedef struct sview *view, viewer;

struct sview
{
	// Window widgets
	GtkWidget* window;
	GtkWidget* canvas;
	GtkWidget* status_label;
	
	// Image pixels
	GdkPixbuf* pixbuf;
	cairo_surface_t* surface;
	// Image
	gchar* path;
	gint   width;
	gint   height;

	// Altered image
	GdkPixbuf* alt;
	// view = original(TRUE), altered(FALSE)
	gboolean view;

	// Filetred image menu actions
	GtkActionGroup* actions_image;
	GtkActionGroup* actions_grid;

	// Canvas need to be updated !
	gboolean update;
};

// ...

#include "interface.h"
#include "filters.h"

// Prototypes

viewer*   new_viewer      (void);
void      free_viewer     (viewer*);
gboolean  viewer_load_file(viewer*, const gchar*);

void viewer_init         (viewer*, gboolean);
void viewer_zoom_auto    (viewer*, gboolean);
void viewer_show_original(viewer*);
void viewer_show_altered (viewer*);
void viewer_toggle_view  (viewer*);

void viewer_apply_filter        (viewer*, ImageFilterFunc,        gint);
void viewer_apply_filter_special(viewer*, ImageSpecialFilterFunc, gint);

#endif
