#ifndef _DH_UTILS_H_
#define _DH_UTILS_H_

// Included files

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <gdk/gdk.h>
#include <math.h>

// Prototypes

void _debug(const char* str, ...) G_GNUC_PRINTF(1, 2);
void _error(const char* str, ...) G_GNUC_PRINTF(1, 2);

gint      pixel_val (gint);
gboolean  is_pixel  (gint);
gboolean  is_between(gint, gint, gint);
gdouble   d_min(gdouble, gdouble);
gdouble   d_max(gdouble, gdouble);
gdouble   dist (gdouble, gdouble, gdouble, gdouble);
gint      d_nearest(gdouble value, gdouble* list, gint size);

#endif
