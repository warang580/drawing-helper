#ifndef _DH_FILTERS_H_
#define _DH_FILTERS_H_

#define DH_BK 5
#define DH_WH 245

#define R 0
#define G 1
#define B 2
#define A 3

#include <gtk/gtk.h>
#include <stdlib.h>
#include <math.h>
#include "utils.h"

typedef void (*ImageFilterFunc)        (const GdkPixbuf*, guchar*,    gint, gint);
typedef void (*ImageSpecialFilterFunc) (GdkPixbuf*, const GdkPixbuf*, gint, gint);

void apply_filter        (GdkPixbuf*, ImageFilterFunc,        gint);
void apply_filter_special(GdkPixbuf*, ImageSpecialFilterFunc, gint);

typedef void (*FilterApplyFinishedCallback) (gpointer user_data);

void apply_filter_special_async(GdkPixbuf *pixbuf,
						   ImageSpecialFilterFunc filter,
						   gint param,
						   FilterApplyFinishedCallback finished_handler,
						   gpointer user_data);

// Black & White filters

void filter_threshold(const GdkPixbuf*, guchar*, gint, gint);
void filter_random   (const GdkPixbuf*, guchar*, gint, gint);

// Grayscale filters

void filter_average  (const GdkPixbuf*, guchar*, gint, gint);
void filter_radiance (const GdkPixbuf*, guchar*, gint, gint);
void filter_sepia    (const GdkPixbuf*, guchar*, gint, gint);

// Special filters

void filter_blur             (GdkPixbuf*, const GdkPixbuf*, gint, gint);
void filter_posterize        (GdkPixbuf*, const GdkPixbuf*, gint, gint);
void filter_edge_detection   (GdkPixbuf*, const GdkPixbuf*, gint, gint);
void filter_ordered_dithering(GdkPixbuf*, const GdkPixbuf*, gint, gint);

// Colors filters

void filter_invert      (const GdkPixbuf*, guchar*, gint, gint);
void filter_remove_red  (const GdkPixbuf*, guchar*, gint, gint);
void filter_remove_green(const GdkPixbuf*, guchar*, gint, gint);
void filter_remove_blue (const GdkPixbuf*, guchar*, gint, gint);

#endif
