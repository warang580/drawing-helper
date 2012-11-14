#ifndef _DH_CANVAS_H_
#define _DH_CANVAS_H_

#include <gtk/gtk.h>
#include <cairo.h>
#include <math.h>
#include "viewer.h"

G_BEGIN_DECLS

#define GTK_DHCANVAS_TYPE      (gtk_dhcanvas_get_type())
#define GTK_DHCANVAS(o)        GTK_CHECK_CAST(o, GTK_DHCANVAS_TYPE, GtkDHCanvas)
#define GTK_IS_DHCANVAS(o)     GTK_CHECK_TYPE(o, GTK_DHCANVAS_TYPE)
#define GTK_DHCANVAS_CLASS(c)  GTK_CHECK_CLASS_CAST(c, GTK_DHCANVAS_TYPE, GtkDHCanvas)

typedef struct _GtkDHCanvas        GtkDHCanvas;
typedef struct _GtkDHCanvasClass   GtkDHCanvasClass;

struct _GtkDHCanvas
{
	GtkDrawingArea parent;

	viewer* image_viewer;
	gdouble focus_point_x;
	gdouble focus_point_y;
	gdouble zoom_level;

	gboolean drag;
	gdouble  drag_x;
	gdouble  drag_y;

	gboolean rotate;
	gdouble  angle;
	
	gboolean grid;
	gint     grid_size;
	struct { gdouble r, g, b, a; } grid_color;
};

struct _GtkDHCanvasClass
{
	GtkDrawingAreaClass parent;
};

GtkWidget* gtk_dhcanvas_new     (viewer*);
GtkType    gtk_dhcanvas_get_type(void);

void on_grid_size_changed(GtkSpinButton*, GtkDHCanvas*);
void on_grid_color_set   (GtkColorButton*, GtkDHCanvas*);

G_END_DECLS

#endif

