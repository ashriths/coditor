#include "helper.h"
/* location of UI XML file relative to path in which program is running */




int
main (int argc, char *argv[])
{
        Coditor      *editor;

        /* allocate the memory needed by our Coditor struct */
        editor = g_slice_new (Coditor);

        /* initialize GTK+ libraries */
        gtk_init (&argc, &argv);

        if (init_app (editor) == FALSE) return 1; /* error loading UI */

        /* show the window */   
        gtk_widget_show (editor->window);

        /* enter GTK+ main loop */                   
        gtk_main ();

        /* free memory we allocated for Coditor struct */
        g_slice_free (Coditor, editor);
        
        return 0;
}









     



