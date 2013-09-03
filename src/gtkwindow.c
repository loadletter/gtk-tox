#include <gtk/gtk.h>
#include "gtkwindow.h"

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

void dialog_show_error(const gchar *message)
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

gint dialog_show_question(GtkWidget *widget, gpointer window, gchar *title, gchar *message)
{
  GtkWidget *dialog;
  gint rc;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_QUESTION,
            GTK_BUTTONS_YES_NO,
            message);
  gtk_window_set_title(GTK_WINDOW(dialog), title);
  rc = gtk_dialog_run(GTK_DIALOG(dialog));

  gtk_widget_destroy(dialog);
  return rc;
}

void dialog_friendrequest_show(gpointer window, gchar *text_id, gchar *text_msg)
{
    GtkWidget *dialog;
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *image;
    gchar     *text;
    
    dialog = gtk_dialog_new_with_buttons("New friend request!",
                                          GTK_WINDOW(window),
                                          GTK_DIALOG_DESTROY_WITH_PARENT,
                                          GTK_STOCK_OK, GTK_RESPONSE_NONE,
                                          NULL );
    
    /* Ensure that the dialog box is destroyed when the user responds */
    g_signal_connect_swapped(dialog, "response", G_CALLBACK (gtk_widget_destroy), dialog);
   
    box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    
    text = g_strdup_printf ("You received a new friend request with the message:\n\"%s\"\nFrom the following ID:\n%s", text_msg, text_id);
    label = gtk_label_new(text);
    g_free(text);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    
    image = gtk_image_new_from_file(ICON_FRIEND_ADD);
    
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
    
    gtk_widget_show_all(dialog);
}

gint dialog_friendrequest_accept(gpointer window, gchar *text_id, gchar *text_msg)
{
    GtkWidget *dialog;
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *image;
    gchar     *text;
    gint rv;
    
    dialog = gtk_dialog_new_with_buttons("Add friend",
                                          GTK_WINDOW(window),
                                          GTK_DIALOG_DESTROY_WITH_PARENT,
                                          "_Accept", GTK_RESPONSE_ACCEPT,
                                          "_Ignore", GTK_RESPONSE_CANCEL,
                                          "_Delete", GTK_RESPONSE_REJECT,
                                          NULL );
    
    box = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    
    text = g_strdup_printf ("Accept this friend?\nID: %s\nMessage:\n\"%s\"", text_id, text_msg);
    label = gtk_label_new(text);
    g_free(text);
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    
    image = gtk_image_new_from_file(ICON_FRIEND_ADD);
    
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
    
    gtk_widget_show_all(dialog);

    rv = gtk_dialog_run(GTK_DIALOG(dialog));
    
    gtk_widget_destroy(dialog);
    
    return rv;
}

void update_friendrequest_tab(GtkNotebook *notebook, GtkTreeView *treeview)
{
    GtkWidget *friendreq_tab = gtk_notebook_get_nth_page(notebook, NOTEBOOK_FRIENDREQ);
    GtkTreeModel *model = gtk_tree_view_get_model(treeview);
    gchar *tab_label_text;
    gint displayed_reqs;
        
    /* get the number of rows of the liststore */
    displayed_reqs = gtk_tree_model_iter_n_children(model, NULL);
    
    /* make/apply/free the label */
    tab_label_text = g_strdup_printf("Friend Requests (%i)", displayed_reqs);
    gtk_notebook_set_tab_label_text(notebook, friendreq_tab, tab_label_text);
    g_free(tab_label_text);

    /* hide the notebook if there are no requests */
    if(displayed_reqs == 0)
        note_hide_page(notebook, NOTEBOOK_FRIENDREQ);
}
