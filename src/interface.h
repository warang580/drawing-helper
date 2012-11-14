#ifndef _DH_INTERFACE_H_
#define _DH_INTERFACE_H_

// Included files

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "ui.h"
#include "viewer.h"
#include "actions.h"
#include "canvas.h"

// Constants

#define DH_W 400
#define DH_H 300
#define DH_MARGIN  0.4
#define DH_PROPORT 0.8

#define DH_SCROLL    0.06
#define DH_ZOOM_MIN  0.05
#define DH_ZOOM_MAX  5.5
#define DH_ROTATION  30
#define DH_GRID_SIZE 9
#define DH_FOCUS     6

#define DH_COLORS 65535.0

// Prototypes

void display_UI(viewer*);

#endif
