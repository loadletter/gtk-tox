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
#include "dht.h"
#include "gtkwindow.h"

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

/* timer */

static gboolean core_timer_handler(Tox *m)
{
    do_tox(m);
    return TRUE;
}

static gboolean dhtprint_timer_handler(struct window_m *w_m)
{
    dht_draw(w_m);
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget       *friends_treeview;
    GtkWidget       *dht_treeview;
    struct window_m *w_m;

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
    friends_treeview = GTK_WIDGET (gtk_builder_get_object (builder, "treeview1"));
    
    gtk_builder_connect_signals (builder, NULL);


    tw_add_to_list(friends_treeview, 0, "vomit");
    tw_add_to_list(friends_treeview, 0, "chan");
    
    
    g_signal_connect(G_OBJECT (window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref (G_OBJECT (builder));
    
    g_timeout_add(50, (GSourceFunc) core_timer_handler, m);
    w_m->window = window;
    w_m->m = m;
    g_timeout_add(400, (GSourceFunc) dhtprint_timer_handler, w_m);
    
    gtk_widget_show (window);                
    gtk_main ();
    
    free(SRVLIST_FILE); /* TODO: make a cleanup function */
    tox_kill(m);
    
    return 0;
}
