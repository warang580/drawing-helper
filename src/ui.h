#ifndef _DH_UI_H_
#define _DH_UI_H_

#include <gtk/gtk.h>
#include "actions.h"
#include "interface.h"

extern const gchar*         DH_interface_string;
extern GtkActionEntry       DH_menu_entries[];
extern guint                DH_menu_nb_entries;
extern GtkToggleActionEntry DH_menu_entries_toggle[];
extern guint                DH_menu_nb_entries_toggle;
extern GtkActionEntry       DH_menu_entries_image[];
extern guint                DH_menu_nb_entries_image;
extern GtkToggleActionEntry DH_menu_entries_image_toggle[];
extern guint                DH_menu_nb_entries_image_toggle;

#endif
