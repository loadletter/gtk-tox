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

static void do_tox(struct storage_data *stor_d)
{
    static int conn_try = 0;
    static int conn_err = 0;
    static int dht_on = FALSE;
    Tox *m = stor_d->tox;
    gchar *status;

    if (!dht_on && !tox_isconnected(m) && !(conn_try++ % 100)) {
        if (!conn_err) {
            conn_err = init_connection(stor_d);
            
            status = g_strdup_printf ("Establishing connection...");
            gtk_statusbar_push(GTK_STATUSBAR (stor_d->statusbar), stor_d->statusbar_context_id, status);
                        
            if (conn_err) {
                status = g_strdup_printf("Auto-connect failed with error code %d", conn_err);
                gtk_statusbar_push(GTK_STATUSBAR (stor_d->statusbar), stor_d->statusbar_context_id, status);
            }
            
            g_free(status);
        }
    } else if (!dht_on && tox_isconnected(m)) {
        dht_on = TRUE;
        status = g_strdup_printf ("DHT connected.");
        gtk_statusbar_push (GTK_STATUSBAR (stor_d->statusbar), stor_d->statusbar_context_id, status);
        g_free(status);
    } else if (dht_on && !tox_isconnected(m)) {
        dht_on = FALSE;
        status = g_strdup_printf("DHT disconnected. Attempting to reconnect.");
        gtk_statusbar_push(GTK_STATUSBAR (stor_d->statusbar), stor_d->statusbar_context_id, status);
        g_free(status);
    }

    tox_do(m);
}

/* exits from gtk_main after closing */

void on_window_destroy (GtkWidget *object, gpointer user_data)
{
    gtk_main_quit ();
}

/* timer */

static gboolean core_timer_handler(struct storage_data *stor_d)
{
    do_tox(stor_d);
    return TRUE;
}

static gboolean dhtprint_timer_handler(struct dht_tree_data *dht_d)
{
    dht_draw(dht_d);
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget       *statusbar;
    GtkWidget       *friends_treeview;
    GtkWidget       *dht_treeview;
    guint           statusbar_context_id;
    PangoFontDescription    *font_desc;
    struct dht_tree_data dht_d;
    struct storage_data stor_d;

    gtk_init (&argc, &argv);

    Tox *m = init_tox();
    
    stor_d.tox = m;
    stor_d.srvlist_path = get_full_configpath("DHTservers");
    stor_d.datafile_path = get_full_configpath("data");
    
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "gtktox.ui", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
    friends_treeview = GTK_WIDGET (gtk_builder_get_object (builder, "treeview1"));
    dht_treeview = GTK_WIDGET (gtk_builder_get_object (builder, "treeview2"));
    statusbar = GTK_WIDGET (gtk_builder_get_object (builder, "statusbar1"));
    
    gtk_builder_connect_signals (builder, NULL);

   /* tw_add_to_list(friends_treeview, 0, "vomit");
    tw_add_to_list(friends_treeview, 0, "chan");*/
    
    g_signal_connect(G_OBJECT (window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);
    g_object_unref (G_OBJECT (builder));
    
    /* initialize statusbar */
    stor_d.statusbar = statusbar;
    statusbar_context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar), "gtk-tox");
    stor_d.statusbar_context_id = statusbar_context_id;
    
    /* set monospace font on the DHT treeview */
    font_desc = pango_font_description_from_string("monospace");
    gtk_widget_override_font(dht_treeview, font_desc);     
    pango_font_description_free(font_desc);
    
    /* add timeout callbacks */
    g_timeout_add(50, (GSourceFunc) core_timer_handler, &stor_d);
    dht_d.gtk = dht_treeview;
    dht_d.m = m;
    g_timeout_add(400, (GSourceFunc) dhtprint_timer_handler, &dht_d);
    
    gtk_widget_show (window);                
    gtk_main ();
    
    /* cleanup */
    free(stor_d.srvlist_path); /* TODO: make a cleanup function */
    free(stor_d.datafile_path);
    tox_kill(m);
    
    return 0;
}
