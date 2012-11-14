#include "filters.h"
#include <string.h>

// Black & White filters

void
filter_threshold(const GdkPixbuf* pixbuf     G_GNUC_UNUSED,
                 guchar*          p,
                 gint             n_channels G_GNUC_UNUSED,
                 gint             param)
{
	gint   t   = (is_pixel(param)) ? param : 128;
	guchar val = (guchar) pixel_val((p[R] + p[G] + p[B]) / 3);
	guchar n   = (val >= t) ? DH_WH : DH_BK;

	p[R] = n;
	p[G] = n;
	p[B] = n;
}

void
filter_random(const GdkPixbuf* pixbuf     G_GNUC_UNUSED,
              guchar*          p,
              gint             n_channels G_GNUC_UNUSED,
              gint             param      G_GNUC_UNUSED)
{
	guchar val = (guchar) pixel_val((p[R] + p[G] + p[B]) / 3);
	guchar n   = ((rand() % (DH_WH - DH_BK)) + DH_BK < val)
	             ? DH_WH : DH_BK;

	p[R] = n;
	p[G] = n;
	p[B] = n;
}

// Grayscale filters

void
filter_average (const GdkPixbuf* pixbuf     G_GNUC_UNUSED,
                guchar*          p,
                gint             n_channels G_GNUC_UNUSED,
                gint             param      G_GNUC_UNUSED)
{
	guchar val = (guchar) pixel_val((p[R] + p[G] + p[B]) / 3);
	
	p[R] = val;
	p[G] = val;
	p[B] = val;
}

void
filter_radiance(const GdkPixbuf* pixbuf     G_GNUC_UNUSED,
                guchar*          p,
                gint             n_channels G_GNUC_UNUSED,
                gint             param      G_GNUC_UNUSED)
{
	guchar val = (guchar) pixel_val(0.299 * p[R] + 0.587 * p[G] + 0.114 * p[B]);
	
	p[R] = val;
	p[G] = val;
	p[B] = val;
}

// Special filters
void
filter_ordered_dithering(GdkPixbuf*       pixbuf,
                         const GdkPixbuf* original    G_GNUC_UNUSED,
                         gint             n_channels,
                         gint             param       G_GNUC_UNUSED)
{
	gint pattern[4][4] = {{1, 9,  3, 11}, {13, 5, 15, 7},
	                      {4, 12, 2, 10}, {16, 8, 14, 6}};

	gint w          = gdk_pixbuf_get_width(pixbuf);
	gint h          = gdk_pixbuf_get_height(pixbuf);
	gint rowstride  = gdk_pixbuf_get_rowstride(pixbuf);
	guchar* p       = gdk_pixbuf_get_pixels(pixbuf);
	gint x,
	     y;

	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			// Current color
			guchar *c = p + x * n_channels + y * rowstride;
			gint g = (c[R] + c[G] + c[B]) / 3;
			// Compare grey level to pattern
			gint n = (pattern[x % 4][y % 4] / 17.0 * 255 > g)
			    ? DH_BK : DH_WH;

			c[R] = n;
			c[G] = n;
			c[B] = n;
		}
	}
}


void
filter_blur(GdkPixbuf*       pixbuf,
            const GdkPixbuf* original,
            gint             n_channels,
            gint             param)
{
	gint x,
	     y;
	gint off = ( is_between(param, 1, 15) ? param : 3 ) / 2;
	
	gint w         = gdk_pixbuf_get_width(pixbuf);
	gint h         = gdk_pixbuf_get_height(pixbuf);
	gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);
	
	guchar* start          = gdk_pixbuf_get_pixels(pixbuf);
	guchar* start_original = gdk_pixbuf_get_pixels(original);
	guchar* end_original   = start_original
	                       + ((w-1) * n_channels)
	                       + ((h-1) * rowstride);
	
	gdouble avg[3] = {0};
	
	// For each pixel
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			gint i,
			     j,
			     a,
			     n = 0;
			guchar *p = start + (x * n_channels) + (y * rowstride);
			
			// For each pixel of kernel
			for (i = -off; i <= off; i++)
			{
				for (j = -off; j <= off; j++)
				{
					guchar* row_start = start_original + (y - j) * rowstride;
					guchar *row_end   = row_start + (w - 1) * n_channels;
					guchar *k         = row_start + (x - i) * n_channels;
					
					// If pixel really exists in kernel
					if (k >  start_original && k < end_original &&
					    k >= row_start      && k < row_end) 
					{
						n++;
						
						for (a = 0; a < 3; a++)
						{
							avg[a] += k[a];
						}
					}
				}
			}
			
			if (n > 0)
			{
				for (a = 0; a < 3; a++)
				{
					p[a]   = (guchar) pixel_val(avg[a] / n);
					avg[a] = 0;
				}
			}
		}
	}
}

void
filter_posterize(GdkPixbuf*       pixbuf,
                 const GdkPixbuf* original,
                 gint             n_channels,
                 gint             param)
{
	gint x, y, i;
	gint w          = gdk_pixbuf_get_width(pixbuf);
	gint h          = gdk_pixbuf_get_height(pixbuf);
	gint rowstride  = gdk_pixbuf_get_rowstride(pixbuf);
	guchar* p       = gdk_pixbuf_get_pixels(pixbuf);
	guchar* o       = gdk_pixbuf_get_pixels(original);
	// Number of grey levels
	gint levels = is_between(param, 1, 20) ? param : 6;
	
	// K-Means algorithm
	// 1. Choose <levels> values of grey
	// We divide grey color (0-255) by <levels> parts
	gdouble* means = malloc(levels * sizeof(gdouble));
	gdouble* sum   = malloc(levels * sizeof(gdouble));
	gdouble* count = malloc(levels * sizeof(gdouble));
	gboolean cont  = TRUE;  

	gint gmin = 255,
	     gmax = 0;
	// Looking for min and max values of grey in image
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			guchar* c = o + x * n_channels + y * rowstride;
			gint g = (c[R] + c[G] + c[B]) / 3;

			gmin = (g < gmin) ? g : gmin;
			gmax = (g > gmax) ? g : gmax;
		}
	}

	// We take the center of each part
	gdouble cdiv = (gmax - gmin) / (1.0 * levels);
	for (i = 0; i < levels; i++)
	{
		means[i] = ((i * cdiv + gmin) + ((i+1) * cdiv + gmin)) / 2.0;
		count[i] = 0;
		sum[i]   = 0;
	}

	// Looking for real values in image for parts
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			guchar* c = o + x * n_channels + y * rowstride;
			gint g = (c[R] + c[G] + c[B]) / 3;

			// using count[] as temporary array
			for (i = 0; i < levels; i++)
			{
				if (fabs(g - means[i]) < fabs(count[i] - means[i]))
				{
					count[i] = g;
				}
			}
		}
	}

	// Assignating this values to means
	for (i = 0; i < levels; i++)
	{
		means[i] = count[i];
		count[i] = 0;
	}

	while (cont)
	{
		cont = FALSE;

		for (i = 0; i < levels; i++)
		{
			sum[i]   = 0.0;
			count[i] = 0.0;
		}

		for (y = 0; y < h; y++)
		{
			for (x = 0; x < w; x++)
			{
				// 2. Assign each pixel to the nearest grey value
				guchar* c = o + x * n_channels + y * rowstride;
				guchar* n = p + x * n_channels + y * rowstride;
				gint g = (c[R] + c[G] + c[B]) / 3;

				i = d_nearest(g, means, levels);
				count[i]++;
				sum[i] += g; 

				n[R] = means[i];
				n[G] = means[i];
				n[B] = means[i];
			}
		}

		// 3. Compute new mean of each set
		for (i = 0; i < levels; i++)
		{
			gdouble avg = sum[i] / count[i];
			// 4. Go to 2. unless mean changed "a lot"
			if (fabs(avg - means[i]) > 3)
			{
				cont = TRUE;
			}
			means[i] = avg;
		}
	}

	free(means);
	free(sum);
	free(count);
}

void
filter_edge_detection(GdkPixbuf*       pixbuf,
                      const GdkPixbuf* original,
                      gint             n_channels,
                      gint             param G_GNUC_UNUSED)
{
	gint x,
	     y;
	
	gint w         = gdk_pixbuf_get_width(pixbuf);
	gint h         = gdk_pixbuf_get_height(pixbuf);
	gint rowstride = gdk_pixbuf_get_rowstride(pixbuf);
	
	guchar* start          = gdk_pixbuf_get_pixels(pixbuf);
	guchar* start_original = gdk_pixbuf_get_pixels(original); 
	guchar* end_original   = start_original
	                       + ((w-1) * n_channels)
	                       + ((h-1) * rowstride);
	
	gint mask_h[3][3] = { {-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}   };
	gint mask_v[3][3] = { {1, 2, 1},  {0, 0, 0},  {-1, -2, -1} };
	
	// For each pixel
	for (y = 0; y < h; y++)
	{
		for (x = 0; x < w; x++)
		{
			guchar *p = start + (x * n_channels) + (y * rowstride);
			gint val_h = 0;
			gint val_v = 0;
			gint val;
			gint i,
			     j;
			
			// For each pixel of kernel
			for (i = 0; i < 3; i++)
			{
				for (j = 0; j < 3; j++)
				{
					guchar* row_start = start_original + (y - j) * rowstride;
					guchar *row_end   = row_start + (w - 1) * n_channels;
					guchar *k         = row_start + (x - i) * n_channels;
					
					// If pixel really exists in kernel
					if (k >  start_original && k < end_original &&
					    k >= row_start      && k < row_end) 
					{
						// Looking for colors in original image
						k = start_original
						  + ((x+i) * n_channels)
						  + ((y+j) * rowstride);
						val = (guchar) pixel_val((k[R] + k[G] + k[B]) / 3);
						val_h += mask_h[i][j] * val;
						val_v += mask_v[i][j] * val;
					}
				}
			}
			
			// Applying filter to pixel
			val = pixel_val(sqrt(val_h*val_h + val_v*val_v));
			p[R] = val;
			p[G] = val;
			p[B] = val;
		}
	}
}

// Colors filters

void
filter_sepia(const GdkPixbuf* pixbuf     G_GNUC_UNUSED,
             guchar*          p,
             gint             n_channels G_GNUC_UNUSED,
             gint             param      G_GNUC_UNUSED)
{
	p[R] = pixel_val((p[R] * 0.393) + (p[G] * 0.769) + (p[B] * 0.189));
	p[G] = pixel_val((p[R] * 0.349) + (p[G] * 0.686) + (p[B] * 0.168));
	p[B] = pixel_val((p[R] * 0.272) + (p[G] * 0.534) + (p[B] * 0.131));
}

void
filter_invert(const GdkPixbuf* pixbuf     G_GNUC_UNUSED,
              guchar*          p,
              gint             n_channels G_GNUC_UNUSED,
              gint             param      G_GNUC_UNUSED)
{
	p[R] = pixel_val(255 - p[R]);
	p[G] = pixel_val(255 - p[G]);
	p[B] = pixel_val(255 - p[B]);
}

void
filter_remove_red(const GdkPixbuf* pixbuf     G_GNUC_UNUSED,
                  guchar*          p,
                  gint             n_channels G_GNUC_UNUSED,
                  gint             param      G_GNUC_UNUSED)
{
	p[R] = 0;
}

void
filter_remove_green(const GdkPixbuf* pixbuf     G_GNUC_UNUSED,
                    guchar*          p,
                    gint             n_channels G_GNUC_UNUSED,
                    gint             param      G_GNUC_UNUSED)
{
	p[G] = 0;
}

void
filter_remove_blue(const GdkPixbuf* pixbuf     G_GNUC_UNUSED,
                   guchar*          p,
                   gint             n_channels G_GNUC_UNUSED,
                   gint             param      G_GNUC_UNUSED)
{
	p[B] = 0;
}

// "Apply filter" functions

void
apply_filter(GdkPixbuf*      pixbuf,
	         ImageFilterFunc filter,
	         gint param)
{
	gint x;
	gint y;
	gint width       = gdk_pixbuf_get_width(pixbuf);
	gint height      = gdk_pixbuf_get_height(pixbuf);
	gint rowstride   = gdk_pixbuf_get_rowstride(pixbuf);
	gint n_channels  = gdk_pixbuf_get_n_channels(pixbuf);
	guchar* pixels   = gdk_pixbuf_get_pixels(pixbuf);
	
	// We work with a copy of the original image while the original image changes
	GdkPixbuf* copy  = gdk_pixbuf_copy(pixbuf);
	
	// Applying filter to each pixel of pixbuf
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			filter(copy,
			       &pixels[(n_channels * x) + (rowstride * y)],
			       n_channels,
			       param);
		}
	}
	g_object_unref(copy);
}

void
apply_filter_special(GdkPixbuf* pixbuf,
	                 ImageSpecialFilterFunc filter,
	                 gint param)
{
	GdkPixbuf *copy = gdk_pixbuf_copy(pixbuf);

	filter(pixbuf,
		   copy,
		   gdk_pixbuf_get_n_channels(pixbuf),
		   param);
	g_object_unref(copy);
}

struct FilterSpecialData
{
	GdkPixbuf *original;
	GdkPixbuf *copy;
	ImageSpecialFilterFunc filter;
	gint param;

	struct {
		FilterApplyFinishedCallback callback;
		gpointer data;
	} handler;
};

static gboolean
apply_filter_special_async_finish(gpointer data_)
{
	struct FilterSpecialData *data = data_;

	/* copy the pixels back to the working copy */
	memcpy(gdk_pixbuf_get_pixels(data->original),
		   gdk_pixbuf_get_pixels(data->copy),
		   gdk_pixbuf_get_rowstride(data->original) * gdk_pixbuf_get_height(data->original));

	g_object_unref(data->original);
	g_object_unref(data->copy);

	data->handler.callback(data->handler.data);
	g_free(data);

	return FALSE;
}

static gpointer
apply_filter_special_async_worker(gpointer data_)
{
	struct FilterSpecialData *data = data_;

	GTimer *t = g_timer_new();
	data->filter(data->copy, data->original,
				 gdk_pixbuf_get_n_channels(data->copy),
				 data->param);
	_debug("Timer : %gs", g_timer_elapsed(t, NULL));
	g_timer_destroy(t);

	g_idle_add(apply_filter_special_async_finish, data);

	return NULL;
}

void
apply_filter_special_async(GdkPixbuf *pixbuf,
						   ImageSpecialFilterFunc filter,
						   gint param,
						   FilterApplyFinishedCallback finished_handler,
						   gpointer user_data)
{
	struct FilterSpecialData *data = g_malloc(sizeof *data);

	data->original = g_object_ref(pixbuf);
	data->copy = gdk_pixbuf_copy(data->original);
	data->filter = filter;
	data->param = param;

	data->handler.callback = finished_handler;
	data->handler.data = user_data;

	g_thread_create(apply_filter_special_async_worker, data, FALSE, NULL);
}
