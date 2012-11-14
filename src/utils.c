#include "utils.h"

void
_debug(const char* format, ...)
{
	#ifdef DEBUG
	va_list args;
	va_start(args, format);
	// Printing formatted string
	vfprintf(stdout, format, args);
	va_end(args);
	fprintf(stdout, "\n");
	#endif
}

void
_error(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	// Printing formatted string
	vfprintf(stderr, format, args);
	va_end(args);
	fprintf(stdout, "\n");
}

// Returns a pixel value between 0 and 255
gint
pixel_val(gint x)
{
	return   (x < 0)   ? 0   :
	         (x > 255) ? 255
	                   : x;
}

// Returns if a value is between 0 and 255
gboolean
is_pixel(gint x)
{
	return is_between(x, 0, 255);
}

gboolean
is_between(gint x, gint a, gint b)
{
	return (x >= a && x <= b);
}

gdouble
d_min(gdouble a, gdouble b)
{
	return (a < b) ? a : b;
}

gdouble
d_max(gdouble a, gdouble b)
{
	return (a > b) ? a : b;
}

gdouble
dist(gdouble a_x, gdouble a_y, gdouble b_x, gdouble b_y)
{
	gdouble a = (b_x - a_x); a *= a;
	gdouble b = (b_y - a_y); b *= b;
	return sqrt(a + b);
}

gint
d_nearest(gdouble value, gdouble* list, gint size)
{
	gint    i, imin = -1;
	gdouble d, dmin = DBL_MAX;

	for (i = 0; i < size; i++)
	{
		d = fabs(list[i] - value);
		if (dmin > d)
		{
			imin = i;
			dmin = d;
		}
	}

	return imin;
}
