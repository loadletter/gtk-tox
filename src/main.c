/*
*Compile with:
*  gcc -Wall -g -o tutorial configdir.c misc.c storage.c main.c `pkg-config --cflags --libs gtk+-3.0 libtoxcore` -export-dynamic
*
*TODO: Makefile
* 
*TODO: CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP
*/
#include <gtk/gtk.h>
#include <tox/tox.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "configdir.h"
#include "misc.h"
#include "storage.h"

static Tox *init_tox()
{
    /* Init core */
    Tox *m = tox_new();

    /* Callbacks */
/*    tox_callback_friendrequest(m, on_request, NULL);                  //TODO
    tox_callback_friendmessage(m, on_message, NULL);
    tox_callback_namechange(m, on_nickchange, NULL);
    tox_callback_statusmessage(m, on_statuschange, NULL);
    tox_callback_action(m, on_action, NULL);*/
#ifdef __linux__
    tox_setname(m, (uint8_t *) "Cool guy", sizeof("Cool guy"));
#elif defined(WIN32)
    tox_setname(m, (uint8_t *) "I should install GNU/Linux", sizeof("I should install GNU/Linux"));
#elif defined(__APPLE__)
    tox_setname(m, (uint8_t *) "Hipster", sizeof("Hipster")); //This used to users of other Unixes are hipsters
#else
    tox_setname(m, (uint8_t *) "Registered Minix user #4", sizeof("Registered Minix user #4"));
#endif
    return m;
}

static void do_tox(Tox *m)
{
    static int conn_try = 0;
    static int conn_err = 0;
    static int dht_on = FALSE;

    if (!dht_on && !tox_isconnected(m) && !(conn_try++ % 100)) {
        if (!conn_err) {
            conn_err = init_connection(m);
            printf("Establishing connection...\n");
            if (conn_err)
                printf("Auto-connect failed with error code %d\n", conn_err);
        }
    } else if (!dht_on && tox_isconnected(m)) {
        dht_on = TRUE;
        printf("DHT connected.\n");
    } else if (dht_on && !tox_isconnected(m)) {
        dht_on = FALSE;
        printf("DHT disconnected. Attempting to reconnect.\n");
    }

    tox_do(m);
}

/* exits from gtk_main after closing */

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

static gboolean timer_handler(Tox *m) //change GtkWidget with Tox *
{
    do_tox(m);
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget       *treeview;

    gtk_init (&argc, &argv);

    Tox *m = init_tox();
    
    char *user_config_dir = get_user_config_dir();
    SRVLIST_FILE = malloc(strlen(user_config_dir) + strlen(CONFIGDIR) + strlen("DHTservers") + 1);
    strcpy(SRVLIST_FILE, user_config_dir);
    strcat(SRVLIST_FILE, CONFIGDIR);
    strcat(SRVLIST_FILE, "DHTservers");
    
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "gtktox.ui", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
    treeview = GTK_WIDGET (gtk_builder_get_object (builder, "treeview1"));
    
    gtk_builder_connect_signals (builder, NULL);


    add_to_list(treeview, "vomit");
    
    
    g_signal_connect(G_OBJECT (window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);

    g_object_unref (G_OBJECT (builder));
    
    g_timeout_add(50, (GSourceFunc) timer_handler, m);
    gtk_widget_show (window);                
    gtk_main ();
    
    free(SRVLIST_FILE); /* TODO: make a cleanup function */
    return 0;
}
