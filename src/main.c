/*
Compile with:
  gcc -Wall -g -o tutorial main.c `pkg-config --cflags --libs gtk+-3.0` -export-dynamic

*/
#include <gtk/gtk.h>

void on_window_destroy (GtkWidget *object, gpointer user_data)
{
    gtk_main_quit ();
}

/* treeview */

enum
{
  LIST_ITEM = 0,
  N_COLUMNS
};

static void add_to_list(GtkWidget *list, const gchar *str)
{
  GtkListStore *store;
  GtkTreeIter iter;

  store = GTK_LIST_STORE(gtk_tree_view_get_model
      (GTK_TREE_VIEW(list)));

  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, LIST_ITEM, str, -1);
}

/* timer */

static gboolean timer_handler(GtkWidget *widget) //change GtkWidget with Tox *
{
    printf("Test\n");
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget       *treeview;

    gtk_init (&argc, &argv);

    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "gtktox.ui", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
    treeview = GTK_WIDGET (gtk_builder_get_object (builder, "treeview1"));
    
    gtk_builder_connect_signals (builder, NULL);


    add_to_list(treeview, "vomit");
    
    
    g_signal_connect(G_OBJECT (window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);

    g_object_unref (G_OBJECT (builder));
    
    g_timeout_add(1000, (GSourceFunc) timer_handler, (gpointer) window);
    gtk_widget_show (window);                
    gtk_main ();

    return 0;
}
