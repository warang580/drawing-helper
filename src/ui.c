#include "ui.h"

// Menu interface string
const gchar* DH_interface_string =
	"<ui>"
		"<menubar name=\"main_menu\">"
			"<menu action=\"file_menu_action\">"
				"<menuitem action=\"open_action\"/>"
				"<menuitem action=\"save_action\"/>"
				"<separator/>"
				"<menuitem action=\"quit_action\"/>"
			"</menu>"

			"<menu action=\"view_menu_action\">"
				"<menuitem action=\"zoomp_action\" />"
				"<menuitem action=\"zoomm_action\" />"
				"<menuitem action=\"fullview_action\" />"
				"<menuitem action=\"idealview_action\" />"
				"<separator />"
				"<menuitem action=\"resetangle_action\" />"
				"<menuitem action=\"fullangle_action\" />"
				"<menuitem action=\"turnright_action\" />"
				"<menuitem action=\"turnleft_action\" />"
				"<separator />"
				"<menuitem action=\"resetview_action\"/>"
			"</menu>"
			
			"<menu action=\"filter_menu_action\">"
				"<menu action=\"filter_dithering_menu_action\">"
					"<menuitem action=\"filter_average_threshold_action\" />"
					"<menuitem action=\"filter_median_threshold_action\" />"
					"<menuitem action=\"filter_random_action\" />"
					"<menuitem action=\"filter_fixed_threshold_action\" />"
					"<separator />"
					"<menuitem action=\"filter_dithering_action\" />"
				"</menu>"
				
				"<menu action=\"filter_grayscale_menu_action\">"
					"<menuitem action=\"filter_grayscale_average_action\" />"
					"<menuitem action=\"filter_grayscale_radiance_action\" />"
					"<menuitem action=\"filter_sepia_action\" />"
					"<separator/>"
					"<menuitem action=\"filter_posterize_action\" />"
				"</menu>"
				
				"<menu action=\"filter_effects_menu_action\">"
					"<menuitem action=\"filter_edge_detection_action\" />"
					"<menuitem action=\"filter_blur_action\" />"
				"</menu>"
				
				"<menu action=\"filter_colors_menu_action\">"
					"<menuitem action=\"filter_remove_red_action\" />"
					"<menuitem action=\"filter_remove_green_action\" />"
					"<menuitem action=\"filter_remove_blue_action\" />"
					"<separator/>"
					"<menuitem action=\"filter_invert_action\" />"
				"</menu>"
				"<separator/>"
				
				"<menuitem action=\"toggle_view_action\" />"
				"<menuitem action=\"remove_filters_action\" />"
				
			"</menu>" 
			
			"<menu action=\"grid_menu_action\">"
				"<menuitem action=\"grid_configure_action\" />"
				"<separator/>"
				"<menuitem action=\"grid_toggle_action\"/>"
			"</menu>"

			"<menu action=\"help_menu_action\">"
				"<menuitem action=\"summary_action\"/>"
				"<separator/>"
				"<menuitem action=\"about_action\"/>"
			"</menu>"
		"</menubar>"
	"</ui>";

// Menu entries
GtkActionEntry DH_menu_entries[] = {
	
	// File
	{
	  "file_menu_action",  NULL,
	  "_File", NULL, NULL, NULL
	},
		
		// File/Open
		{
		  "open_action", GTK_STOCK_OPEN,
		  "_Open ...", "<CTRL>O",
		  "Open an image file",
		  G_CALLBACK(on_open)
		},
		
		// File/Quit
		{
		  "quit_action", GTK_STOCK_QUIT,
		  "_Quit", "<CTRL>Q",
		  "Quit Drawing Helper",
		  G_CALLBACK(on_quit)
		},
	
	// View
	{
	  "view_menu_action", NULL,
	  "_View", NULL, NULL, NULL
	},

		// View/Zoom+
		{
		  "zoomp_action", GTK_STOCK_ZOOM_IN,
		  "Zoom _in", "<CTRL>plus",
		  "Zoom in (+)",
		  G_CALLBACK(on_zoomp_view)
		},

		// View/Zoom-
		{
		  "zoomm_action", GTK_STOCK_ZOOM_OUT,
		  "Zoom out", "<CTRL>minus",
		  "Zoom out (-)",
		  G_CALLBACK(on_zoomm_view)
		},

		// View/Full view
		{
		  "fullview_action", GTK_STOCK_ZOOM_100,
		  "_Full zoom", "<CTRL>0",
		  "Full view (zoom at 100%%)",
		  G_CALLBACK(on_full_view)
		},

		// View/Ideal zoom
		{
		  "idealview_action", GTK_STOCK_ZOOM_FIT,
		  "_Ideal zoom", "<CTRL>I",
		  "Ideal view (auto-zoom)",
		  G_CALLBACK(on_ideal_view)
		},

		// View/Angle 0°
		{
		  "resetangle_action", NULL,
		  "Angle _0°", NULL,
		  "Reset angle (0°)",
		  G_CALLBACK(on_reset_angle)
		},

		// View/180°
		{
		  "fullangle_action", NULL,
		  "Angle _180°", NULL,
		  "Angle: 180°",
		  G_CALLBACK(on_full_angle)
		},

		// View/+30°
		{
		  "turnright_action", NULL,
		  "Rotation +30°", NULL,
		  "Angle: +30°",
		  G_CALLBACK(on_turn_right)
		},

		// View/-30°
		{
		  "turnleft_action", NULL,
		  "Rotation -30°", NULL,
		  "Angle: -30°",
		  G_CALLBACK(on_turn_left)	
		},

		// View/Reset view
		{
		  "resetview_action", NULL,
		  "_Reset view", "<CTRL>R",
		  "Reset view (focus/zoom)",
		  G_CALLBACK(on_reset_view)
		},

	// Filters
	{
	  "filter_menu_action",  NULL,
	  "F_ilters", NULL, NULL, NULL
	},

		// Filters/Thresholding/
		{
		  "filter_dithering_menu_action", NULL,
		  "Thresolding", NULL, NULL, NULL
		},
		
			// Filtres/Thresholding/B&W fixed threshold
			{
			  "filter_fixed_threshold_action", NULL,
			  "Custom ...", NULL,
			  "Custom threshold (user value)",
			  G_CALLBACK(on_custom_threshold)
			},

			// Filters/Thresholding/B&W average threshold
			{
			  "filter_average_threshold_action", NULL,
			  "Average", NULL,
			  "Black and white threshold (pixel gray value > threshold => white, else black)",
			  G_CALLBACK(on_average_threshold)
			},
			
			// Filters/Thresholding/B&W median threshold
			{
			  "filter_median_threshold_action", NULL,
			  "Median", NULL,
			  "Black and white threshold (pixel gray value > threshold => white, else black)",
			  G_CALLBACK(on_median_threshold)
			},
			
			// Filters/Thresholding/Random threshold
			{
			  "filter_random_action", NULL,
			  "Random", NULL,
			  "Each pixel is comparated with a random threshold",
			  G_CALLBACK(on_random)
			},

			// Filters/Thresholding/B&W dithering
			{
			  "filter_dithering_action", NULL,
			  "Dithering", NULL,
			  "Black and white dithering (ink effect)",
			  G_CALLBACK(on_dithering)
			},
		
		// Filters/Grayscale
		{
		  "filter_grayscale_menu_action", NULL,
		  "Grayscale", NULL, NULL, NULL
		},
		
			// Filters/Grayscale/Grayscale (average)
			{
			  "filter_grayscale_average_action", NULL,
			  "Average", NULL,
			  "Grayscale using average values of R,G,B",
			  G_CALLBACK(on_grayscale_average)
			},
		
			// Filters/Grayscale/Grayscale (radiance)
			{
			  "filter_grayscale_radiance_action", NULL,
			  "Radiance", NULL,
			  "Grayscale using specific proportions of R,G,B to be lighter",
			  G_CALLBACK(on_grayscale_radiance)
			},
		
			// Filters/Grayscale/Posterize
			{
			  "filter_posterize_action", NULL,
			  "Posterize ...", NULL,
			  "Grayscale using only a fixed number of gray colors",
			  G_CALLBACK(on_posterize)
			},
		
			// Filters/Grayscale/Sepia
			{
			  "filter_sepia_action", NULL,
			  "Sepia", NULL,
			  "Sepia (old photo style) filter",
			  G_CALLBACK(on_sepia)
			},
		
		// Filters/Colors
		{
		  "filter_colors_menu_action", NULL,
		  "Colors", NULL, NULL, NULL
		},
		
			// Filters/Colors/Invert
			{
			  "filter_invert_action", NULL,
			  "Invert colors", NULL,
			  "Invert R,G,B colors",
			  G_CALLBACK(on_invert)
			},
		
			// Filters/Colors/Remove red
			{
			  "filter_remove_red_action", NULL,
			  "Remove red channel", NULL,
			  "Remove red channel of the image",
			  G_CALLBACK(on_remove_red)
			},
		
			// Filters/Colors/Remove green
			{
			  "filter_remove_green_action", NULL,
			  "Remove green channel", NULL,
			  "Remove green channel of the image",
			  G_CALLBACK(on_remove_green)
			},
		
			// Filters/Colors/Remove blue
			{
			  "filter_remove_blue_action", NULL,
			  "Remove blue channel", NULL,
			  "Remove blue channel of the image",
			  G_CALLBACK(on_remove_blue)
			},
		
		// Filters/Effects
		{
		  "filter_effects_menu_action", NULL,
		  "Effects", NULL, NULL, NULL
		},
		
			// Filters/Effects/Blur
			{
			  "filter_blur_action", NULL,
			  "Blur ...", NULL,
			  "Blur",
			  G_CALLBACK(on_blur)
			},
		
			// Filters/Effects/Edge detection
			{
			  "filter_edge_detection_action", NULL,
			  "Edge detection", NULL,
			  "Edge detection",
			  G_CALLBACK(on_edge_detection)
			},
	
	// Grid
	{
		"grid_menu_action", NULL,
		"_Grid", NULL, NULL, NULL
	},
		// Grid configure
		{
		  "grid_configure_action", NULL,
		  "_Configure grid ...", NULL,
		  "Configure grid",
		  G_CALLBACK(on_configure_grid)
		},

	// Help
	{
	  "help_menu_action",  NULL,
	  "_Help", NULL, NULL, NULL
	},
	
		// Help/Summary
		{
		  "summary_action", GTK_STOCK_HELP,
		  "_Summary ...", "F1",
		  "Help summary",
		  G_CALLBACK(on_summary)
		},
		
		// Help/About
		{
		  "about_action", GTK_STOCK_ABOUT,
		  "_About ...", NULL,
		  "About Drawing Helper", 
		  G_CALLBACK(on_about)
		}

};

// Toggle menu entries
GtkToggleActionEntry DH_menu_entries_toggle[] = {
	// Grid/Toggle
	{
	  "grid_toggle_action", NULL,
	  "Display _grid", "<CTRL>G",
	  "Toggle grid",
	  G_CALLBACK(on_grid_toggle),
	  FALSE
	},
};

// Menu entries related to alt image
GtkActionEntry DH_menu_entries_image[] = {

		// File/Save
		{
		  "save_action", GTK_STOCK_SAVE,
		  "_Save as ...", "<CTRL>S",
		  "Save as (filtered image)",
		  G_CALLBACK(on_save)
		},

		// Filters/Remove filters
		{
		  "remove_filters_action", GTK_STOCK_CLEAR,
		  "Remove filters", "<CTRL>D",
		  "Remove all filters and show original image",
		  G_CALLBACK(on_remove_filters)
		},

};

// Toggle menu entries related to alt image
GtkToggleActionEntry DH_menu_entries_image_toggle[] = {
		// Filters/Toggle original-filtered
		{
		  "toggle_view_action", GTK_STOCK_CONVERT,
		  "Display filtered image", "<CTRL>T",
		  "Toggle between original and filtered image",
		  G_CALLBACK(on_toggle_view),
		  TRUE
		},
};

guint DH_menu_nb_entries              = G_N_ELEMENTS(DH_menu_entries);
guint DH_menu_nb_entries_toggle       = G_N_ELEMENTS(DH_menu_entries_toggle);
guint DH_menu_nb_entries_image        = G_N_ELEMENTS(DH_menu_entries_image);
guint DH_menu_nb_entries_image_toggle = G_N_ELEMENTS(DH_menu_entries_image_toggle);
