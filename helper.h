#include "handler.h"
#define XML_LAYOUT_FILE "layout.xml"


/*
We call init_app() when our program is starting to load our Coditor struct
with references to the widgets we need. This is done using GtkBuilder to read
the XML file we created using Glade.
*/
gboolean 
init_app (Coditor *coditor)
{
        GtkBuilder              *builder;
        GError                  *err=NULL;      
        guint                   id;
        PangoFontDescription    *font_desc;
        
        /* use GtkBuilder to build our interface from the XML file */
        builder = gtk_builder_new ();
        if (gtk_builder_add_from_file (builder, XML_LAYOUT_FILE, &err) == 0)
        {
                error_message (err->message);
                g_error_free (err);
                return FALSE;
        }
        
        /* get the widgets which will be referenced in callbacks */
        coditor->window = GTK_WIDGET (gtk_builder_get_object (builder, 
                                                             "window"));
        coditor->statusbar = GTK_WIDGET (gtk_builder_get_object (builder, 
                                                                "statusbar"));
        GtkWidget * scrollWindow = GTK_WIDGET (gtk_builder_get_object (builder, 
                                                             "scrolledwindow"));
	coditor->undoButton = GTK_WIDGET (gtk_builder_get_object (builder, 
                                                             "undo_button"));
	coditor->redoButton = GTK_WIDGET (gtk_builder_get_object (builder, 
                                                             "redo_menu_item"));
	if(coditor->undoButton==NULL){
		g_print("is NULL");
	}	
	if(GTK_IS_WIDGET(coditor->undoButton)==TRUE){
		g_print("is widget");
	}
	gtk_builder_connect_signals (builder, coditor);
	g_object_unref (G_OBJECT (builder));
	GtkSourceLanguageManager *lm;
	lm = gtk_source_language_manager_new();
	coditor->sBuf = GTK_SOURCE_BUFFER (gtk_source_buffer_new (NULL));
	g_signal_connect(G_OBJECT(coditor->sBuf),"changed",G_CALLBACK(on_buffer_changed),coditor);
	g_object_ref (lm);
	g_object_set_data_full ( G_OBJECT (coditor->sBuf), "languages-manager",
		lm, (GDestroyNotify) g_object_unref);
	
        GtkWidget * sView = gtk_source_view_new_with_buffer(coditor->sBuf);   

 
	gtk_container_add (GTK_CONTAINER (scrollWindow), GTK_WIDGET (sView));                                                   
        /* connect signals, passing our Coditor struct as user data */
        gtk_widget_show_all (scrollWindow);
                    
        /* free memory used by GtkBuilder object */
        
        
        /* set the text view font */
        font_desc = pango_font_description_from_string ("monospace 15");
	
        gtk_widget_modify_font (sView, font_desc);     
        pango_font_description_free (font_desc); 
        
        /* set the default icon to the GTK "edit" icon */
        gtk_window_set_default_icon_name (GTK_STOCK_EDIT);
        /* setup and initialize our statusbar */
        id = gtk_statusbar_get_context_id (GTK_STATUSBAR (coditor->statusbar),
                                           "Starting Coditor....");
        coditor->statusbar_context_id = id;
        reset_default_status (coditor);
        reset_window_title(coditor);
        /* set filename to NULL since we don't have an open/saved file yet */
        coditor->filename = NULL;
	coditor->language = NULL;
	coditor->font_size =15;
	coditor->text_view = sView;
        load_file(coditor->sBuf,"Untitled");
	coditor->compiled=FALSE;
	coditor->foundTag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(coditor->sBuf), "found","background","yellow");
	coditor->replaceTag = gtk_text_buffer_create_tag(GTK_TEXT_BUFFER(coditor->sBuf), "replace","background","pink");
	//on_about_menu_item_activate(NULL,coditor);
        return TRUE;
}

gboolean 
on_window_delete_event (GtkWidget *widget, GdkEvent *event, 
                        Coditor *coditor)
{
        if (check_for_save (coditor) == TRUE)
        {
              on_save_menu_item_activate (NULL, coditor);  
        }
        return FALSE;   /* propogate event */
}

gchar* get_extension_from_filename(gchar* filename){
	gchar ** bit = g_strsplit(filename,".",-1);
	gint len = g_strv_length(bit);
	len--;
	//g_print("%s",bit[len]);
	gchar *ext = g_strdup_printf ("%s",bit[len]);
	return ext;
}

gchar* get_language_from_extension(gchar* ext){
	if(!g_ascii_strcasecmp(ext,"pl"))
			return "perl";	
	if(!g_ascii_strcasecmp(ext,"h"))
			return "cpp";		
	return ext;
}

void set_new_language(Coditor* coditor, gchar* ext){
	GtkSourceBuffer *sBuf = coditor->sBuf;
        GtkSourceLanguageManager *lm;
	GtkSourceLanguage *language = NULL;
	//g_print(ext);
	g_return_val_if_fail (sBuf != NULL, FALSE);
	g_return_val_if_fail (GTK_SOURCE_BUFFER (sBuf), FALSE);

	/* get the Language for ext source mimetype */
	lm = g_object_get_data (G_OBJECT (sBuf), "languages-manager");
	coditor->language = get_language_from_extension(ext);
	language = gtk_source_language_manager_get_language (lm,coditor->language);
	g_print("Language: [%s]\n", gtk_source_language_get_name(language));

	if (language == NULL)
	{
		g_print ("No language found for mime type `%s'\n", "text/x-c");
		g_object_set (G_OBJECT (sBuf), "highlight", FALSE, NULL);
	}
	else
	{
		gtk_source_buffer_set_language (sBuf, language);
		g_object_set (G_OBJECT (sBuf), "highlight", TRUE, NULL);
		gtk_source_view_set_show_line_numbers(coditor->text_view,TRUE);
		gtk_source_view_set_auto_indent(coditor->text_view,TRUE);
	}
}

void check_language_change(Coditor *coditor){
	gchar* lan = get_language_from_extension(get_extension_from_filename(coditor->filename));
	set_new_language(coditor,lan);
}


void 
load_file (Coditor *coditor, gchar *filename)
{
	GtkSourceBuffer *sBuf = coditor->sBuf;
	GError *err = NULL;
	gboolean reading;
	GtkTextIter iter;
	GIOChannel *io;
	gchar *buffer;
	gchar* ext = get_extension_from_filename(filename);
	g_print("Extension Detected : %s\n" ,ext);
	g_return_val_if_fail (sBuf != NULL, FALSE);
	g_return_val_if_fail (filename != NULL, FALSE);
	g_return_val_if_fail (GTK_SOURCE_BUFFER (sBuf), FALSE);

	set_new_language(coditor, get_language_from_extension(ext));
	
	/* Now load the file from Disk */
	io = g_io_channel_new_file (filename, "r", &err);
	if (!io)
	{
		g_print("error: %s %s\n", (err)->message, filename);
		return FALSE;
	}

	if (g_io_channel_set_encoding (io, "utf-8", &err) != G_IO_STATUS_NORMAL)
	{
		g_print("err: Failed to set encoding:\n%s\n%s", filename, (err)->message);
		return FALSE;
	}	

	gtk_source_buffer_begin_not_undoable_action (sBuf);

	gtk_text_buffer_set_text (GTK_TEXT_BUFFER (sBuf), "", 0);
	buffer = g_malloc (4096);
	reading = TRUE;
	while (reading)
	{
		gsize bytes_read;
		GIOStatus status;

		status = g_io_channel_read_chars (io, buffer, 4096, &bytes_read, &err);
		switch (status)
		{
			case G_IO_STATUS_EOF: reading = FALSE;

			case G_IO_STATUS_NORMAL:
			if (bytes_read == 0) continue;
			gtk_text_buffer_get_end_iter ( GTK_TEXT_BUFFER (sBuf), &iter);
			gtk_text_buffer_insert (GTK_TEXT_BUFFER(sBuf),&iter,buffer,bytes_read);
			break;

			case G_IO_STATUS_AGAIN: continue;

			case G_IO_STATUS_ERROR:

			default:
				g_print("err (%s): %s", filename, (err)->message);
				/* because of error in input we clear already loaded text */
				gtk_text_buffer_set_text (GTK_TEXT_BUFFER (sBuf), "", 0);

				reading = FALSE;
				break;
		}
	}
	g_free (buffer);

	gtk_source_buffer_end_not_undoable_action (sBuf);
	g_io_channel_unref (io);

	if (err)
	{
		g_error_free (err);
		return FALSE;
	}

	gtk_text_buffer_set_modified (GTK_TEXT_BUFFER (sBuf), FALSE);

	/* move cursor to the beginning */
	gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (sBuf), &iter);
	gtk_text_buffer_place_cursor (GTK_TEXT_BUFFER (sBuf), &iter);

	g_object_set_data_full (G_OBJECT (sBuf),"filename", g_strdup (filename),
	(GDestroyNotify) g_free);
	coditor->filename=g_strdup(filename);
	reset_default_status(coditor);
	reset_window_title(coditor);
	g_print("Loaded New File %s.\n",filename);
	return TRUE;
}

/* 
We call write_file() when we have a filename and want to load the current text
buffer for the GtkTextView into the file.
*/
void 
write_file (Coditor *coditor, gchar *filename)
{
        GError                  *err=NULL;
        gchar                   *status;
        gchar                   *text;
        gboolean                result;
        GtkTextBuffer           *buffer;
        GtkTextIter             start, end;
        
        /* add Saving message to status bar and ensure GUI is current */
        if (filename != NULL)
            status = g_strdup_printf ("Saving %s...", filename);
        else
            status = g_strdup_printf ("Saving %s...", coditor->filename);
            
        gtk_statusbar_push (GTK_STATUSBAR (coditor->statusbar),
                            coditor->statusbar_context_id, status);
        g_free (status);
        while (gtk_events_pending()) gtk_main_iteration();
        
        /* disable text view and get contents of buffer */ 
        gtk_widget_set_sensitive (GTK_TEXT_VIEW(coditor->text_view), FALSE);
        buffer = GTK_TEXT_BUFFER(coditor->sBuf);
        gtk_text_buffer_get_start_iter (buffer, &start);
        gtk_text_buffer_get_end_iter (buffer, &end);
        text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);       
        gtk_text_buffer_set_modified (buffer, FALSE);
        gtk_widget_set_sensitive (GTK_TEXT_VIEW(coditor->text_view), TRUE);
        
        /* set the contents of the file to the text from the buffer */
        if (filename != NULL)        
                result = g_file_set_contents (filename, text, -1, &err);
        else
                result = g_file_set_contents (coditor->filename, text, -1, &err);
                
        if (result == FALSE)
        {
                /* error saving file, show message to user */
                error_message (err->message);
                g_error_free (err);
        }        
        
        /* don't forget to free that memory! */ 
        g_free (text); 
        
        if (filename != NULL) 
        {
                /* we need to free the memory used by coditor->filename and set 
                 it to the new filename instead */
                if (coditor->filename != NULL) g_free (coditor->filename);
                coditor->filename = filename;
        }
        
        /* clear saving status and restore default */
        gtk_statusbar_pop (GTK_STATUSBAR (coditor->statusbar),
                           coditor->statusbar_context_id);
        
	reset_window_title(coditor);
	check_language_change(coditor);
	reset_default_status (coditor);   
}



