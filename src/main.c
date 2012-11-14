#include "main.h"

int
main(int argc, char** argv)
{
	_debug("Drawing Helper -- Debug mode");
	
	// GTK init'
	gtk_init(&argc, &argv);
	
	// Randomness
	srand(time(NULL));
	
	// Image viewer
	viewer* v = new_viewer();

	// User interface
	display_UI(v);
	
	// Loading/display image
	viewer_load_file(v, (argc > 1) ? g_strdup(argv[1]) : NULL);

	// GTK event loop
	gtk_main();
	
	return 0;
}
