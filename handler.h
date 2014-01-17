#include "model.h"





/*
We call error_message() any time we want to display an error message to the
user. It will both show an error dialog and log the error to the terminal
window.
*/



void
error_message (const gchar *message)
{
        GtkWidget               *dialog;
        
        /* log to terminal window */
        g_warning (message);
        
        /* create an error message dialog and display modally to the user */
        dialog = gtk_message_dialog_new (NULL, 
                                         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_ERROR,
                                         GTK_BUTTONS_OK,
                                         message);
        
        gtk_window_set_title (GTK_WINDOW (dialog), "Oops ! Coditor Error");
        gtk_dialog_run (GTK_DIALOG (dialog));      
        gtk_widget_destroy (dialog);         
}

int show_question_dialog(const gchar* title, const gchar *question,Coditor* coditor){
	GtkWidget               *dialog;
	dialog= gtk_dialog_new_with_buttons(title,
						coditor->window,
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
						GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
						NULL						
						 ) ;
}

void show_message_dialog(const gchar* title,const gchar* msg){

	GtkWidget               *dialog;
        
        /* log to terminal window */
	g_print("Notify : %s\n",msg);
        
        /* create an error message dialog and display modally to the user */
        dialog = gtk_message_dialog_new (NULL, 
                                         GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                         GTK_MESSAGE_INFO,
                                         GTK_BUTTONS_OK,
                                         msg);
        gtk_window_set_title (GTK_WINDOW (dialog), title);
        gtk_dialog_run (GTK_DIALOG (dialog));      
        gtk_widget_destroy (dialog);  
}

/*
We call get_open_filename() when we want to get a filename to open from the
user. It will present the user with a file chooser dialog and return the 
newly allocated filename or NULL.
*/
gchar * get_open_filename (Coditor *coditor)
{
        GtkWidget               *chooser;
        gchar                   *filename=NULL;
                
        chooser = gtk_file_chooser_dialog_new ("Open File...",
                                               GTK_WINDOW (coditor->window),
                                               GTK_FILE_CHOOSER_ACTION_OPEN,
                                               GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                               GTK_STOCK_OPEN, GTK_RESPONSE_OK,
                                               NULL);
                                               
        if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
        {
                filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
        }
        
        gtk_widget_destroy (chooser);
        return filename;
}


/*
We call get_save_filename() when we want to get a filename to save from the
user. It will present the user with a file chooser dialog and return the 
newly allocated filename or NULL.
*/
gchar * get_save_filename (Coditor *coditor)
{
        GtkWidget               *chooser;
        gchar                   *filename=NULL;
                
        chooser = gtk_file_chooser_dialog_new ("Save File...",
                                               GTK_WINDOW (coditor->window),
                                               GTK_FILE_CHOOSER_ACTION_SAVE,
                                               GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
                                               GTK_STOCK_SAVE, GTK_RESPONSE_OK,
                                               NULL);
                                               
        if (gtk_dialog_run (GTK_DIALOG (chooser)) == GTK_RESPONSE_OK)
        {
                filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (chooser));
        }
        
        gtk_widget_destroy (chooser);
        return filename;
}



/*
We call reset_default_status() when we want to remove the last default status
and set it again based on the current file. This is typically after a file is
opened, saved, or a new file is created. "Default" simply means a non-temporary
status. It's typically something like "File: filename.txt"
*/
void 
reset_default_status (Coditor *coditor)
{
        gchar           *file;
        gchar           *status;
        gint lines= gtk_text_buffer_get_line_count(coditor->sBuf),ch = gtk_text_buffer_get_char_count(coditor->sBuf);
        if (coditor->filename == NULL)
        {
                file = g_strdup ("(Untitled Document)");
        }
        else file = g_path_get_basename (coditor->filename);
        if(coditor->language !=NULL){
		status = g_strdup_printf ("File: %s\t\t\t\t\tLanguage Detected : %s\t\t\t\t%d Lines | %d Characters", file,coditor->language,lines,ch);
	}
	else
		status = g_strdup_printf ("File: %s\t\t\t\t\tNo Language Detected\t\t\t\t%d Lines | %d Characters", file,lines,ch);
        
        gtk_statusbar_pop (GTK_STATUSBAR (coditor->statusbar),
                           coditor->statusbar_context_id);
        gtk_statusbar_push (GTK_STATUSBAR (coditor->statusbar),
                            coditor->statusbar_context_id, status);
        g_free (status);
        g_free (file);
}



   

void reset_window_title (Coditor *coditor){
	gchar *file;
	gchar *title;
	if(coditor->filename ==NULL){
		file = g_strdup("Untitled Document");
	}
	else file = g_path_get_basename(coditor->filename);
	title = g_strdup_printf("%s - Coditor",file);	
	gtk_window_set_title (GTK_WINDOW (coditor->window), title);
	g_free(file);
	g_free(title);
}

void star_window_title (Coditor *coditor){
	gchar *file;
	gchar *title;
	if(coditor->filename ==NULL){
		file = g_strdup("*Untitled Document");
	}
	else file = g_path_get_basename(coditor->filename);
	title = g_strdup_printf("*%s - Coditor",file);	
	gtk_window_set_title (GTK_WINDOW (coditor->window), title);
	g_free(file);
	g_free(title);
}

void on_buffer_changed(GObject *object,Coditor * coditor){
	star_window_title(coditor);
	reset_default_status(coditor);
	
}
/* 
This function will check to see if the text buffer has been
modified and prompt the user to save if it has been modified.
*/
gboolean
check_for_save (Coditor *coditor)
{
        gboolean                ret = FALSE;
        GtkTextBuffer           *buffer = GTK_TEXT_BUFFER(coditor->sBuf);
        
        
        if (gtk_text_buffer_get_modified (buffer) == TRUE)
        {
                /* we need to prompt for save */
                
                GtkWidget       *dialog;
                
                const gchar *msg  = "Do you want to save the changes you have made?";
                
                dialog = gtk_message_dialog_new (NULL, 
                                                 GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                                 GTK_MESSAGE_QUESTION,
                                                 GTK_BUTTONS_YES_NO,
                                                 msg);
        
                gtk_window_set_title (GTK_WINDOW (dialog), "Save?");
                if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_NO)
                {
                        ret = FALSE;
                }      
                else ret = TRUE;
                
                gtk_widget_destroy (dialog);      
        }     
        
        return ret;
}

/* 
When our window is destroyed, we want to break out of the GTK main loop. We do
this by calling gtk_main_quit(). We could have also just specified gtk_main_quit
as the handler in Glade!
*/
void 
on_window_destroy (GObject *object, Coditor *coditor)
{
        gtk_main_quit();
}

/*
When the window is requested to be closed, we need to check if they have 
unsaved work. We use this callback to prompt the user to save their work before
they exit the application. From the "delete-event" signal, we can choose to
effectively cancel the close based on the value we return.
*/

/*
Called when the user clicks the 'New' menu. We need to prompt for save if the
file has been modified, and then delete the buffer and clear the modified flag.
*/
void on_new_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        GtkTextBuffer           *buffer;
        
        if (check_for_save (coditor) == TRUE)
        {
              on_save_menu_item_activate (NULL, coditor);  
        }
        
        /* clear coditor for a new file */
        coditor->filename = NULL;
	coditor->language = NULL;
        buffer = coditor->sBuf;
        gtk_text_buffer_set_text (GTK_TEXT_BUFFER(buffer), "", -1);
        gtk_text_buffer_set_modified (GTK_TEXT_BUFFER(buffer), FALSE);
        
        reset_default_status (coditor);
	reset_window_title (coditor);
}

/*
Called when the user clicks the 'Open' menu. We need to prompt for save if the
file has been modified, allow the user to choose a file to open, and then call
load_file() on that file.
*/
void on_open_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        gchar                   *filename;
        
        if (check_for_save (coditor) == TRUE)
        {
              on_save_menu_item_activate (NULL, coditor);  
        }
        
        filename = get_open_filename (coditor);
        if (filename != NULL) load_file (coditor, filename); 
}

/*
Called when the user clicks the 'Save' menu. We need to allow the user to choose 
a file to save if it's an untitled document, and then call write_file() on that 
file.
*/
void on_save_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        gchar                   *filename;
        
        if (coditor->filename == NULL) 
        {
                filename = get_save_filename (coditor);
                if (filename != NULL) write_file (coditor, filename); 
        }
        else write_file (coditor, NULL);
        
}

/*
Called when the user clicks the 'Save As' menu. We need to allow the user to 
choose a file to save and then call write_file() on that file.
*/
void on_save_as_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        gchar                   *filename;
        
        filename = get_save_filename (coditor);
        if (filename != NULL) write_file (coditor, filename); 
}


void on_print_menu_item_activate(GtkMenuItem *menuitem, Coditor *coditor){

}
/*
Called when the user clicks the 'Quit' menu. We need to prompt for save if the
file has been modified and then break out of the GTK+ main loop.
*/

void on_close_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        if (check_for_save (coditor) == TRUE)
        {
              on_save_menu_item_activate (NULL, coditor);  
        }
        on_new_menu_item_activate(NULL,coditor);
}
void on_quit_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        if (check_for_save (coditor) == TRUE)
        {
              on_save_menu_item_activate (NULL, coditor);  
        }
        gtk_main_quit();
}


void on_undo_menu_item_activate(GtkWidget *menuitem, Coditor *coditor){
	if(gtk_source_buffer_can_undo(coditor->sBuf)==FALSE){
		gtk_source_buffer_undo(coditor->sBuf);
	}
	
	if(gtk_source_buffer_can_undo(coditor->sBuf)==FALSE){
		gtk_widget_set_sensitive (coditor->undoButton,FALSE);
	}
}




void on_redo_menu_item_activate(GtkMenuItem *menuitem, Coditor *coditor){
	gtk_source_buffer_redo(coditor->sBuf);
	if(gtk_source_buffer_can_redo(coditor->sBuf)==FALSE){
		gtk_widget_set_sensitive (coditor->redoButton,FALSE);
	}
}
/*
Called when the user clicks the 'Cut' menu. 
*/
void on_cut_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        GtkTextBuffer           *buffer;
        GtkClipboard            *clipboard;
        
        clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
        buffer = GTK_TEXT_BUFFER(coditor->sBuf);
        gtk_text_buffer_cut_clipboard (buffer, clipboard, TRUE);
}

/*
Called when the user clicks the 'Copy' menu. 
*/
void on_copy_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        GtkTextBuffer           *buffer;
        GtkClipboard            *clipboard;
        
        clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
        buffer = GTK_TEXT_BUFFER(coditor->sBuf);	
        gtk_text_buffer_copy_clipboard (buffer, clipboard);
}

/*
Called when the user clicks the 'Paste' menu. 
*/
void on_paste_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        GtkTextBuffer           *buffer;
        GtkClipboard            *clipboard;
        
        clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
        buffer = GTK_TEXT_BUFFER(coditor->sBuf);
        gtk_text_buffer_paste_clipboard (buffer, clipboard, NULL, TRUE);
}

/*
Called when the user clicks the 'Delete' menu. 
*/
void on_delete_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
        GtkTextBuffer           *buffer;

        buffer = GTK_TEXT_BUFFER(coditor->sBuf);
        gtk_text_buffer_delete_selection (buffer, FALSE, TRUE);
}

void on_line_number_menu_item_activate(GtkMenuItem* menuItem, Coditor *coditor){
	if(gtk_source_view_get_show_line_numbers(coditor->text_view)==TRUE){
		gtk_source_view_set_show_line_numbers(coditor->text_view,FALSE);		
	}
	else{
		gtk_source_view_set_show_line_numbers(coditor->text_view,TRUE);
	}
}

void on_fullscreen_menu_item_toggled(GtkMenuItem *menuItem,Coditor *coditor){
	gtk_window_fullscreen(coditor->window);
}

void on_restore_menu_item_toggled(GtkMenuItem *menuItem,Coditor *coditor){
	gtk_window_unfullscreen(coditor->window);
}

void on_zoom_in_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor){
	GtkWidget* tv= coditor->text_view;
	coditor->font_size += 5;	
	g_print("\nFontSize Set to  %d\n",coditor->font_size);	
	gchar * font = g_strdup_printf("monospace %d",coditor->font_size);
        PangoFontDescription* font_desc = pango_font_description_from_string (font);
        gtk_widget_modify_font (tv, font_desc);     
        pango_font_description_free (font_desc); 
	g_free(font);
}

void on_zoom_out_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor){
	GtkWidget* tv= coditor->text_view;
	if(coditor->font_size==5){
		error_message("Minimum Font Size Limit reached.");
		return;
	}
	coditor->font_size -= 5;	
	g_print("FontSize Set To  %d\n",coditor->font_size);	
	gchar * font = g_strdup_printf("monospace %d",coditor->font_size);
        PangoFontDescription* font_desc = pango_font_description_from_string (font);
        gtk_widget_modify_font (tv, font_desc);     
        pango_font_description_free (font_desc); 
	g_free(font);
}

void search_till_eof_and_mark(gint* count,gchar* text , GtkTextIter start, Coditor* coditor){
	GtkTextIter match_start,match_end;
	gboolean found = gtk_text_iter_forward_search  (&start, text, GTK_TEXT_SEARCH_TEXT_ONLY,&match_start,&match_end,NULL);
	if(found==TRUE){
		(*count)++;
		GtkTextBuffer* buffer = GTK_TEXT_BUFFER(coditor->sBuf)	;

		gtk_text_buffer_apply_tag(buffer,coditor->foundTag,&match_start,&match_end);
		search_till_eof_and_mark(count,text,match_end,coditor);
	}
}

int find_string_and_highlight(gchar* find, Coditor *coditor){
		GtkTextIter start;
		GtkTextBuffer * buffer = GTK_TEXT_BUFFER(coditor->sBuf);
		//g_print("Buffersize = %d",gtk_text_buffer_get_char_count(buffer));
		gtk_text_buffer_get_start_iter(buffer,&start);
		gint count=0;
		search_till_eof_and_mark(&count,find,start,coditor);
		return count;
}




void on_find_menu_item_activate(GtkMenuItem* menuItem , Coditor *coditor){
	GtkWidget* findDialog;
	findDialog= gtk_dialog_new_with_buttons("Find",
						coditor->window,
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_STOCK_FIND, GTK_RESPONSE_ACCEPT,
						GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
						NULL						
						 ) ;
	GtkContainer* content = gtk_dialog_get_content_area(findDialog);
	gtk_container_add(content,gtk_label_new("Search for"));
	GtkEntry* entry = gtk_entry_new();
	gtk_container_add(content,entry);
	gtk_widget_show_all (content);
	if(gtk_dialog_run(findDialog)==GTK_RESPONSE_ACCEPT){
		gchar* find=	gtk_entry_get_text(entry);
		g_print("Finding %s\n",find);
		gint count = find_string_and_highlight(find,coditor);
		gchar* result = g_strdup_printf(" %d matches found!\n\nThe Matches have been highlighted.",count);
		//g_print(result);
		gtk_widget_destroy(findDialog);
		show_message_dialog("Search Result:",result);		
		}
	else{
		gtk_widget_destroy(findDialog);
	}	
}

void search_till_eof_and_replace(gchar* text ,gchar* replace , GtkTextIter start, Coditor* coditor){
	GtkTextIter match_start,match_end;
	gboolean found = gtk_text_iter_forward_search  (&start, text, GTK_TEXT_SEARCH_TEXT_ONLY,&match_start,&match_end,NULL);
	if(found==TRUE){
		
		GtkTextBuffer* buffer = GTK_TEXT_BUFFER(coditor->sBuf)	;
		gtk_text_buffer_delete(buffer,&match_start,&match_end);
		gtk_text_buffer_insert_with_tags(buffer,&match_start,replace,-1,coditor->replaceTag,NULL);
		search_till_eof_and_replace(text,replace,match_start,coditor);
	}
}


void on_find_and_replace_menu_item_activate(GtkMenuItem* menuItem , Coditor *coditor){
	GtkWidget* findDialog,nextDialog;
	findDialog= gtk_dialog_new_with_buttons("Find and Replace",
						coditor->window,
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_STOCK_FIND, GTK_RESPONSE_ACCEPT,
						GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
						NULL						
						 ) ;
	GtkContainer* content = gtk_dialog_get_content_area(findDialog);
	gtk_container_add(content,gtk_label_new("Search for"));
	GtkEntry* findEntry = gtk_entry_new();
	GtkEntry* replaceEntry = gtk_entry_new();
	gtk_container_add(content,findEntry);
	gtk_container_add(content,gtk_label_new("Replace with"));
	gtk_container_add(content,replaceEntry);
	gtk_widget_show_all (content);
	if(gtk_dialog_run(findDialog)==GTK_RESPONSE_ACCEPT){
		gchar* find=	gtk_entry_get_text(findEntry);
		gchar* replace=	gtk_entry_get_text(replaceEntry);
		g_print("Finding %s\n",find);
		gint count = find_string_and_highlight(find,coditor);	
		gchar* result = g_strdup_printf(" %d matches found!\n\nAll Occurences of \"%s\"(Highlighted in Yellow) will be replaced with \"%s\".\n\n To revert hit UNDO.",count,find,replace);
		
		show_message_dialog("Replacing...",result);
		GtkTextIter start;
		gtk_text_buffer_get_start_iter(coditor->sBuf,&start);
		search_till_eof_and_replace(find,replace,start,coditor);
		result = g_strdup_printf(" %d Occurences of \"%s\"were replaced with \"%s\" (Now Highlighted in Pink). \n\nTo revert hit UNDO.",count,find,replace);
		show_message_dialog("Result:",result);
		gtk_widget_destroy(findDialog);	
		}
	else{
		gtk_widget_destroy(findDialog);
	}	
}


void on_jump_to_menu_item_activate(GtkMenuItem* menuItem, Coditor *coditor)
{
	GtkWidget* dialog;
	dialog= gtk_dialog_new_with_buttons("Jump to",
						coditor->window,
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
						GTK_STOCK_CANCEL, GTK_RESPONSE_REJECT,
						NULL						
						 ) ;
	GtkContainer* content = gtk_dialog_get_content_area(dialog);
	gtk_container_add(content,gtk_label_new("Line Number :"));
	GtkEntry* entry = gtk_entry_new();
	gtk_container_add(content,entry);
	gtk_widget_show_all (content);
	if(gtk_dialog_run(dialog)==GTK_RESPONSE_ACCEPT){
		gchar* find=	gtk_entry_get_text(entry);
		gint number = -1;
		if(find && g_utf8_strlen(find,-1) > 0)
        	{
                number = atoi(find);
        	}	
		g_print("Jumping To Line: %d\n",number);
		GtkTextIter  iter;
		gtk_text_buffer_get_iter_at_line_offset(coditor->sBuf,&iter,number,0);
		gtk_text_view_scroll_to_iter(coditor->text_view,&iter,0.0,TRUE,0.0,0.0);
		gtk_text_view_place_cursor_onscreen (coditor->text_view);
		}
	
	gtk_widget_destroy(dialog);

}

gchar* get_class_name(Coditor * coditor){
	gchar * filename = coditor->filename;
	gchar ** bit = g_strsplit(filename,"/",-1);
	gint len = g_strv_length(bit);
	len--;
	gchar* path ="";
	gint i=0;
	for(i=1;i<len;i++){
		path= g_strdup_printf("%s/%s",path,bit[i]);
	}	
	g_print(path);
	coditor->path= path;
	//g_print("%s",bit[len]);
	gchar *name = g_strdup_printf ("%s",bit[len]);
	gchar **sp = g_strsplit(name,".","2");
	
	return g_strdup_printf("%s",sp[0]);
}

gchar* get_compile_command(Coditor* coditor){
	if(coditor->language==NULL){
		error_message("PLease SAve your Files with the program estension to Run.");
		return NULL;
	}
	if(!g_ascii_strcasecmp(coditor->language,"c")){
		coditor->out="./coditorOutput"	;
		return g_strdup_printf("gcc %s -o coditorOutput 2>cr.txt",coditor->filename);
	}
	if(!g_ascii_strcasecmp(coditor->language,"cpp")){
		coditor->out="./coditorOutput";
		return g_strdup_printf("g++ %s -o coditorOutput 2>cr.txt",coditor->filename);
	}
	if(!g_ascii_strcasecmp(coditor->language,"java")){
		gchar* classFile = get_class_name(coditor);
		coditor->out=g_strdup_printf("-e java %s",classFile);
		return g_strdup_printf("javac %s -d ~/coditor/ide 2>cr.txt",coditor->filename);
	}
	if(!g_ascii_strcasecmp(coditor->language,"py")){
		gchar* classFile = get_class_name(coditor);
		coditor->out=g_strdup_printf("-e python %s",classFile);
		return g_strdup_printf("javac %s -d ~/coditor/ide 2>cr.txt",coditor->filename);
	}
	else{
		error_message("This Language is Not supported by Coditor Right Now! :(");
		return NULL;
	}
}

void on_compile_menu_item_activate(GtkMenuItem* menuItem, Coditor * coditor){
	if(coditor->filename==NULL){
		on_save_menu_item_activate (NULL, coditor); 
		 
	}	
	else if (check_for_save (coditor) == TRUE)
        {
              on_save_menu_item_activate (NULL, coditor);  
        }
	gchar* cmd = get_compile_command(coditor);
	if(cmd==NULL){
		return;	
	}
	GtkTextView* tv = gtk_text_view_new();
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(tv);
	GtkWidget* dialog= gtk_dialog_new_with_buttons("Compilation Result",
						coditor->window,
						GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
						GTK_STOCK_OK, GTK_RESPONSE_ACCEPT,
						NULL						
						 ) ;
	GtkContainer* content = gtk_dialog_get_content_area(dialog);
	gtk_container_add(content,gtk_label_new("Compilation Process: "));
	//gtk_container_add(content,gtk_label_new(g_strdup_printf("Command : %s",cmd)));
	gtk_container_add(content,tv);
	system(cmd);
	while (gtk_events_pending()) gtk_main_iteration();
        
        /* get the file contents */
	gchar* text;
	GError                  *err=NULL;
        gboolean result = g_file_get_contents ("cr.txt", &text, NULL, &err);
        if (result == FALSE)
        {
                /* error loading file, show message to user */
                error_message ("Compilation Could not be Completed.");
                g_error_free (err);
        }
	
	gtk_text_buffer_set_text (buffer, text, -1);
	gint n = gtk_text_buffer_get_char_count(buffer);
	if(n==0){
		gtk_text_buffer_set_text (buffer, "Succesfully Compiled\n0 errors,\n0 warning", -1);
		coditor->compiled=TRUE;
	}		
	gtk_text_view_set_editable (tv,FALSE);
	g_free(text);
	gtk_widget_show_all (content);
	gtk_dialog_run(dialog);
	gtk_widget_destroy(dialog);
}


void on_run_menu_item_activate(GtkMenuItem* menuItem,Coditor *coditor){
	if(coditor->compiled==FALSE){
		on_compile_menu_item_activate(NULL,coditor);
	}
	gchar* runCmd= g_strdup_printf("$TERM %s &1",coditor->out);
	g_print(runCmd);
	system(runCmd);
	while (gtk_events_pending()) gtk_main_iteration();
}
/*
Called when the user clicks the 'About' menu. We use gtk_show_about_dialog() 
which is a convenience function to show a GtkAboutDialog. This dialog will
NOT be modal but will be on top of the main application window.
*/
void on_about_menu_item_activate (GtkMenuItem *menuitem, Coditor *coditor)
{
       static const gchar * const authors[] = {
		"Ashrith Sheshan <mailme@ashrith.in>",
		NULL
	};

	static const gchar copyright[] = \
		"Copyright \xc2\xa9 2013 Ashrith Sheshan";

	static const gchar comments[] = "Coditor - A Lightweight IDE";

	gtk_show_about_dialog (GTK_WINDOW (coditor->window),
			       "authors", authors,
			       "comments", comments,
			       "copyright", copyright,
			       "version", "0.1",
			       "website", "http://www.ashrith.in",
			       "program-name", "Coditor",
			       "logo-icon-name", GTK_STOCK_EDIT,
			       NULL);
}

void on_tips_menu_item_activate(GtkMenuItem *menuitem, Coditor *coditor){
	
}


