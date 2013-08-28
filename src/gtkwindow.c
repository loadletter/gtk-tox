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
