#include <gtk/gtk.h>
#include "/usr/include/gtksourceview-3.0/gtksourceview/gtksourceview.h"
#include "/usr/include/gtksourceview-3.0/gtksourceview/gtksourcebuffer.h"
#include "/usr/include/gtksourceview-3.0/gtksourceview/gtksourcelanguage.h"
#include "/usr/include/gtksourceview-3.0/gtksourceview/gtksourcelanguagemanager.h"

typedef struct
{
        GtkWidget               *window;
        GtkWidget               *statusbar;
        GtkWidget               *text_view;
        guint                   statusbar_context_id;
        gchar                   *filename;
	GtkSourceBuffer   	*sBuf;
	gchar			*language;
	guint			font_size;
	GtkTextTag 		*foundTag;
	GtkTextTag 		*replaceTag;
	GtkWidget		*undoButton;
	GtkWidget		*redoButton;
	gchar* out;
	gboolean 		compiled;
	gchar* 			path;
	
} Coditor;
