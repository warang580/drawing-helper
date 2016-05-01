#include "canvas.h"

G_DEFINE_TYPE (GtkDHCanvas, gtk_dhcanvas, GTK_TYPE_DRAWING_AREA);

// Events

static void
clamp_focus_point(GtkDHCanvas* canvas G_GNUC_UNUSED)
{
	gint cw_, ch_;
	gdk_drawable_get_size(GTK_WIDGET(canvas)->window, &cw_, &ch_);

	gdouble zoom = canvas->zoom_level;
	gdouble iw   = canvas->image_viewer->width;
	gdouble ih   = canvas->image_viewer->height;

	// Compute offset with zoom >= 100%
	zoom = (zoom >= 1.0) ? zoom : 1.0;

	// Using diagonal to get maximal offset (+ margin)
	gdouble diag = sqrt((iw * iw) + (ih * ih));
	gdouble xoff = (diag - iw) / 2.0 * DH_MARGIN;
	gdouble yoff = (diag - ih) / 2.0 * DH_MARGIN;

	// Computing offset for each direction
	gdouble xmin = zoom * -xoff;
	gdouble xmax = zoom * (iw + xoff);
	gdouble ymin = zoom * -yoff;
	gdouble ymax = zoom * (ih + yoff);

	// Clamp (x,y)
	canvas->focus_point_x = CLAMP(canvas->focus_point_x, xmin, xmax);
	canvas->focus_point_y = CLAMP(canvas->focus_point_y, ymin, ymax);
}

static void
gtk_dhcanvas_size_allocate(GtkWidget *self, GtkAllocation *alloc)
{
	GTK_WIDGET_CLASS(gtk_dhcanvas_parent_class)->size_allocate(self, alloc);
	if (GDK_IS_DRAWABLE(self->window))
	{
		clamp_focus_point(GTK_DHCANVAS(self));
	}
}

static gboolean
gtk_dhcanvas_expose_event(GtkWidget* self, GdkEventExpose* event)
{
	GtkDHCanvas* canvas = GTK_DHCANVAS(self);
	GdkPixbuf*   pixbuf = NULL;

	cairo_t* context = gdk_cairo_create(self->window);
	cairo_rectangle(context, event->area.x,     event->area.y,
		                     event->area.width, event->area.height);
	cairo_clip(context);

	// Background
	cairo_set_source_rgb(context, DH_WH/255., DH_WH/255., DH_WH/255.);
	cairo_paint(context);

	// Viewer
	viewer* v = canvas->image_viewer;

	// Changing window title & status bar
	gchar* title;

	if (v->update)
	{
		// Choose original/altered image
		if (v->view)
		{
			pixbuf = v->pixbuf;
		}
		else
		{
			pixbuf = v->alt;
		}

		// Delete old v->surface
		if (v->surface)
		{
			cairo_surface_destroy(v->surface);
			v->surface = NULL;
		}

		if (pixbuf)
		{
			v->surface = cairo_surface_create_similar(cairo_get_target(context),
			                                          CAIRO_CONTENT_COLOR,
			                                          v->width,
			                                          v->height);
			cairo_t* tmp = cairo_create(v->surface);
			gdk_cairo_set_source_pixbuf(tmp, pixbuf, 0, 0);
			cairo_paint(tmp);
			cairo_destroy(tmp);
		}

		// Update done !
		v->update = FALSE;
	}

	// Display image
	if (v->surface)
	{
		// Apply zoom
		gdouble zoom = canvas->zoom_level;
		cairo_scale(context, zoom, zoom);

		// Canvas area size
		gint cw, ch;
		gdk_drawable_get_size(self->window, &cw, &ch);

		// Relation : img(x,y) + focus(fx, fy) * zoom
		//          = canvas_center(cw/2, ch/2)
		gdouble x = (cw / 2) - (canvas->focus_point_x * zoom);
		gdouble y = (ch / 2) - (canvas->focus_point_y * zoom);

		// Rotation
		cairo_translate(context, cw / 2 / zoom, ch / 2 / zoom);
		cairo_rotate(context, canvas->angle);
		cairo_translate(context, -(cw / 2 / zoom), -(ch / 2 / zoom));

		// Mirror
		if (FALSE)
		{
			cairo_matrix_t mcurrent;
			cairo_matrix_t mreverse;
			cairo_matrix_t mnew;

			cairo_get_matrix(context, &mcurrent);
			cairo_matrix_init(&mreverse,
				              -1.0, 0.0,
				              0.0, 1.0,
				              v->width, 0.0);
			cairo_matrix_multiply(&mnew, &mcurrent, &mreverse);

			cairo_set_matrix(context, &mnew);
		}

		// Draw pixbuf
		cairo_translate(context, x / zoom, y / zoom);
		cairo_set_source_surface(context, v->surface, 0, 0);
		// Set the extend to PAD for the scale not to introduce black borders
		cairo_pattern_t* pattern = cairo_get_source(context);
		cairo_pattern_set_extend(pattern, CAIRO_EXTEND_PAD);
		cairo_pattern_set_filter(pattern, CAIRO_FILTER_GAUSSIAN);
		// Clip image and paint
		cairo_save(context);
		cairo_rectangle(context, 0, 0, v->width, v->height);
		cairo_clip(context);
		cairo_paint(context);
		cairo_restore(context);

		#ifdef DEBUG
			// Draw focus point position (blue)
			cairo_set_source_rgb(context, 52/255., 35/255., 190/255.);
 			cairo_set_line_width(context, 2/zoom);
			// Horizontal
			cairo_move_to(context, canvas->focus_point_x - (DH_FOCUS * 1/zoom), canvas->focus_point_y);
			cairo_line_to(context, canvas->focus_point_x + (DH_FOCUS * 1/zoom), canvas->focus_point_y);
			cairo_stroke(context);
			// Vertical
			cairo_move_to(context, canvas->focus_point_x, canvas->focus_point_y - (DH_FOCUS * 1/zoom));
			cairo_line_to(context, canvas->focus_point_x, canvas->focus_point_y + (DH_FOCUS * 1/zoom));
			cairo_stroke(context);
			// ... mixed with orange
			cairo_set_source_rgb(context, 1.0, 175/255., 0.0);
			// Horizontal
			cairo_move_to(context, canvas->focus_point_x - (DH_FOCUS * 1/zoom) / 2.0, canvas->focus_point_y);
			cairo_line_to(context, canvas->focus_point_x + (DH_FOCUS * 1/zoom) / 2.0, canvas->focus_point_y);
			cairo_stroke(context);
			// Vertical
			cairo_move_to(context, canvas->focus_point_x, canvas->focus_point_y - (DH_FOCUS * 1/zoom) / 2.0);
			cairo_line_to(context, canvas->focus_point_x, canvas->focus_point_y + (DH_FOCUS * 1/zoom) / 2.0);
			cairo_stroke(context);

		#endif

		// Grid
		if (canvas->grid)
		{
			cairo_set_source_rgba(context, canvas->grid_color.r,
				                           canvas->grid_color.g,
				                           canvas->grid_color.b,
				                           canvas->grid_color.a);
			cairo_set_line_width(context, 0.6/zoom);
			gdouble i;
			gdouble step = (v->width - 1.0) / canvas->grid_size;

			for (i = 0; i < v->width; i += step)
			{
				cairo_move_to(context, i, 0);
				cairo_line_to(context, i, v->height);
				cairo_stroke(context);
			}
			for (i = 0; i < v->height; i += step)
			{
				cairo_move_to(context, 0, i);
				cairo_line_to(context, v->width, i);
				cairo_stroke(context);
			}
		}

		// Change title
		title = g_strdup_printf("Drawing Helper - %s",
		                         g_path_get_basename(v->path));

		gint angle = ((gint) (canvas->angle * 180 / G_PI)) % 360;
		if (angle == 0)
		{
			canvas->angle = 0.0;
		}

		gtk_label_set_text(GTK_LABEL(v->status_label), g_strdup_printf(" Size : %d x %d  -  Zoom : %.0f%%  -  Angle : %d°",
		                                      canvas->image_viewer->width,
		                                      canvas->image_viewer->height,
		                                      canvas->zoom_level*100,
		                                      angle));
	}
	else
	{
		// Default title
		title = g_strdup_printf("Drawing Helper");
	}

	// Clear all
	cairo_destroy(context);

	// Changing window title
	gtk_window_set_title(GTK_WINDOW(canvas->image_viewer->window), title);
	g_free(title);

	return FALSE;
}

static gboolean
gtk_dhcanvas_scroll_event(GtkWidget* self, GdkEventScroll* event)
{
	GtkDHCanvas* canvas = GTK_DHCANVAS(self);

	if (canvas->image_viewer->pixbuf)
	{
		gint    canvas_w;
		gint    canvas_h;
		gdouble zoom_r = 1;
		gdouble zoom_new;

		// Scrolling direction
		if (event->direction == GDK_SCROLL_UP &&
		    canvas->zoom_level < DH_ZOOM_MAX)
		{
			zoom_r = (1 + DH_SCROLL);
		}
		else if (event->direction == GDK_SCROLL_DOWN &&
			     canvas->zoom_level > DH_ZOOM_MIN)
		{
			zoom_r = (1 - DH_SCROLL);
		}
		else
		{
			zoom_r = 1;
		}

		// New zoom
		zoom_new = canvas->zoom_level * zoom_r;
		gdk_drawable_get_size(self->window, &canvas_w, &canvas_h);

		gdouble dx = event->x - canvas_w/2;
		gdouble dy = event->y - canvas_h/2;

		gdouble dnx = dx * cos(canvas->angle) +
		              dy * sin(canvas->angle);
		gdouble dny = dx * -sin(canvas->angle) +
		              dy * cos(canvas->angle);

		// Change focus_point with zoom
		if (zoom_r > 1)
		{
			canvas->focus_point_x += dnx * (DH_SCROLL/zoom_new);
			canvas->focus_point_y += dny * (DH_SCROLL/zoom_new);
		}
		else if (zoom_r < 1)
		{
			canvas->focus_point_x -= dnx * (DH_SCROLL/zoom_new);
			canvas->focus_point_y -= dny * (DH_SCROLL/zoom_new);
		}

		// Change zoom
		canvas->zoom_level = zoom_new;

		// Focus point must be ~centered
		clamp_focus_point(canvas);

		// Redraw canvas
		gtk_widget_queue_draw(self);
	}

	return FALSE;
}

static gboolean
gtk_dhcanvas_button_press_event(GtkWidget* self, GdkEventButton* event)
{
	GtkDHCanvas* canvas = GTK_DHCANVAS(self);

	if (canvas->image_viewer->pixbuf)
	{
		GdkCursor* cursor = NULL;

		// event->button : 1=left, 2=middle, 3=right (mouse click)
		if (event->button == 3)
		{
			canvas->rotate = TRUE;
			cursor = gdk_cursor_new_for_display(
				gtk_widget_get_display(self),
				GDK_EXCHANGE
			);
		}
		else
		{
			canvas->drag = TRUE;
			cursor = gdk_cursor_new_for_display(
				gtk_widget_get_display(self),
				GDK_FLEUR
			);
		}

		// Updating mouse position
		canvas->drag_x  = event->x;
		canvas->drag_y  = event->y;

		// Setting cursor
		gdk_window_set_cursor(gtk_widget_get_window(self), cursor);
		gdk_cursor_unref(cursor);
	}

	return FALSE;
}

static gboolean
gtk_dhcanvas_motion_event(GtkWidget* self, GdkEventMotion* event)
{
	GtkDHCanvas* canvas = GTK_DHCANVAS(self);

	if (canvas->image_viewer->pixbuf &&
	   (canvas->drag || canvas->rotate))
	{
		gdouble dx = (canvas->drag_x - event->x);
		gdouble dy = (canvas->drag_y - event->y);

		// Rotation
		if (canvas->rotate)
		{
			gint cw, ch;
			gdk_drawable_get_size(self->window, &cw, &ch);
			cw /= 2.;
			ch /= 2.;

			gdouble a = dist(cw, ch, canvas->drag_x, canvas->drag_y);
			gdouble b = dist(canvas->drag_x, canvas->drag_y, event->x, event->y);
			gdouble c = dist(cw, ch, event->x, event->y);
			gint d = 1;

			// Looking for "major move" (across x/y ?)
			if (abs(canvas->drag_x - event->x) > abs(canvas->drag_y - event->y) )
			{
				// Dx is major
				if (canvas->drag_y < ch)
				{
					d = (canvas->drag_x - event->x > 0) ? -1 : 1;
				}
				else
				{
					d = (canvas->drag_x - event->x > 0) ? 1 : -1;
				}
			}
			else
			{
				// Dy is major
				if (canvas->drag_x < cw)
				{
					d = (canvas->drag_y - event->y > 0) ? 1 : -1;
				}
				else
				{
					d = (canvas->drag_y - event->y > 0) ? -1 : 1;
				}
			}

			canvas->angle += d * acos( ((c*c) + (a*a) - (b*b)) / (2 * a * c) );
		}
		// Translation
		else if (canvas->drag)
		{
			gdouble dnx = dx * cos(canvas->angle) +
			              dy * sin(canvas->angle);
			gdouble dny = dx * -sin(canvas->angle) +
			              dy * cos(canvas->angle);

			canvas->focus_point_x += dnx / canvas->zoom_level;
			canvas->focus_point_y += dny / canvas->zoom_level;
		}

		canvas->drag_x = event->x;
		canvas->drag_y = event->y;

		// Focus point must be ~centered
		clamp_focus_point(canvas);

		// Redraw canvas
		gtk_widget_queue_draw(self);
	}

	return FALSE;
}

static gboolean
gtk_dhcanvas_button_release_event(GtkWidget* self, GdkEventButton* event G_GNUC_UNUSED)
{
	GtkDHCanvas* canvas = GTK_DHCANVAS(self);

	// Dragging end
	canvas->drag = FALSE;
	// Rotation end
	canvas->rotate = FALSE;

	// Unset cursor
	gdk_window_set_cursor(gtk_widget_get_window(self), NULL);

	return FALSE;
}

void
on_grid_size_changed(GtkSpinButton* spin,
                     GtkDHCanvas*   canvas)
{
	canvas->grid_size = gtk_spin_button_get_value(spin);
	gtk_widget_queue_draw(GTK_WIDGET(canvas));
}

void
on_grid_color_set(GtkColorButton* button,
                   GtkDHCanvas*   canvas)
{
	GdkColor color;

	gtk_color_button_get_color(button, &color);
	canvas->grid_color.r = color.red   / DH_COLORS;
	canvas->grid_color.g = color.green / DH_COLORS;
	canvas->grid_color.b = color.blue  / DH_COLORS;
	canvas->grid_color.a = gtk_color_button_get_alpha(button) / DH_COLORS;

	gtk_widget_queue_draw(GTK_WIDGET(canvas));
}

// Constructors

static void
gtk_dhcanvas_class_init(GtkDHCanvasClass* class)
{
	GtkWidgetClass* widget_class = GTK_WIDGET_CLASS(class);

	// Events callbacks
	widget_class->button_release_event = gtk_dhcanvas_button_release_event;
	widget_class->button_press_event   = gtk_dhcanvas_button_press_event;
	widget_class->motion_notify_event  = gtk_dhcanvas_motion_event;
	widget_class->expose_event         = gtk_dhcanvas_expose_event;
	widget_class->scroll_event         = gtk_dhcanvas_scroll_event;
	widget_class->size_allocate        = gtk_dhcanvas_size_allocate;
}

static void
gtk_dhcanvas_init(GtkDHCanvas* self)
{
	// Event masks
	gtk_widget_add_events(GTK_WIDGET(self), GDK_BUTTON_PRESS_MASK   |
	                                        GDK_BUTTON_RELEASE_MASK |
	                                        GDK_POINTER_MOTION_MASK |
	                                        GDK_SCROLL_MASK);
}

GtkWidget*
gtk_dhcanvas_new(viewer* v)
{
	GtkDHCanvas* self = g_object_new(GTK_DHCANVAS_TYPE, NULL);

	self->image_viewer  = v;

	self->zoom_level    = 1.0;
	self->focus_point_x = 0.0;
	self->focus_point_y = 0.0;

	self->drag    = FALSE;
	self->drag_x  = 0.0;
	self->drag_y  = 0.0;

	self->rotate  = FALSE;
	self->angle   = 0.0;

	self->grid = FALSE;
	self->grid_size = DH_GRID_SIZE;
	self->grid_color.r = DH_WH/255.;
	self->grid_color.g = DH_WH/255.;
	self->grid_color.b = DH_WH/255.;
	self->grid_color.a = 0.6;

	return GTK_WIDGET(self);
}
