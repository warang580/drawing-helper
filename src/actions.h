#ifndef _DH_ACTIONS_H_
#define _DH_ACTIONS_H_

// Includes files

#include <gtk/gtk.h>
#include <glib.h>
#include "interface.h"
#include "filters.h"

// Prototypes

// File menu

void on_open(GtkWidget*, gpointer);
void on_save(GtkWidget*, gpointer);
void on_quit(GtkWidget*, gpointer);

// View menu

void on_full_view (GtkWidget*, gpointer);
void on_zoomp_view(GtkWidget*, gpointer);
void on_zoomm_view(GtkWidget*, gpointer);
void on_ideal_view(GtkWidget*, gpointer);

void on_reset_angle(GtkWidget*, gpointer);
void on_turn_right (GtkWidget*, gpointer);
void on_turn_left  (GtkWidget*, gpointer);
void on_full_angle (GtkWidget*, gpointer);


void on_reset_view(GtkWidget*, gpointer);

// Filters menu

void on_custom_threshold (GtkWidget*, gpointer);
void on_average_threshold(GtkWidget*, gpointer);
void on_median_threshold (GtkWidget*, gpointer);
void on_dithering        (GtkWidget*, gpointer);
void on_random           (GtkWidget*, gpointer);

void on_grayscale_average (GtkWidget*, gpointer);
void on_grayscale_radiance(GtkWidget*, gpointer);
void on_posterize         (GtkWidget*, gpointer);
void on_sepia             (GtkWidget*, gpointer);

void on_blur          (GtkWidget*, gpointer);
void on_edge_detection(GtkWidget*, gpointer);

void on_invert      (GtkWidget*, gpointer);
void on_remove_red  (GtkWidget*, gpointer);
void on_remove_green(GtkWidget*, gpointer);
void on_remove_blue (GtkWidget*, gpointer);

void on_toggle_view   (GtkToggleAction*, gpointer);
void on_remove_filters(GtkWidget*, gpointer);

// Grid menu

void on_configure_grid(GtkWidget*, gpointer);
void on_grid_toggle   (GtkWidget*, gpointer);

// Help menu

void on_summary(GtkWidget*, gpointer);
void on_about  (GtkWidget*, gpointer);

#endif
