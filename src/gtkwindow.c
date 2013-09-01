#include <gtk/gtk.h>

/* treeview */
/* TODO: make it useful */

void tw_add_to_list(GtkWidget *list, int list_item, const gchar *str)
{
    GtkListStore *store;
    GtkTreeIter iter;

    store = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, list_item, str, -1);
}

void tw_clear_list(GtkWidget *list)
{
    GtkListStore *store;

    store = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));

    gtk_list_store_clear(store);
}


/* hide/show a notebook page */

void note_hide_page(GtkNotebook *notebook, int pagenumber)
{
        GtkWidget *page;

        page = gtk_notebook_get_nth_page(notebook, pagenumber);
        gtk_widget_hide(page);
}

void note_show_page(GtkNotebook *notebook, int pagenumber)
{
        GtkWidget *page;

        page = gtk_notebook_get_nth_page(notebook, pagenumber);
        gtk_widget_show(page);
        gtk_notebook_set_current_page(notebook, pagenumber);
}

void error_message (const gchar *message)
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
        
        gtk_window_set_title (GTK_WINDOW (dialog), "Error!");
        gtk_dialog_run (GTK_DIALOG (dialog));      
        gtk_widget_destroy (dialog);         
}
