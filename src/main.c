/*
*Compile with:
*  gcc -Wall -g -o tutorial configdir.c misc.c storage.c main.c `pkg-config --cflags --libs gtk+-3.0 libtoxcore` -export-dynamic
*
* TODO: Makefile
* 
* TODO: CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP CLEANUP
* 
* TODO: more error checking
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

static void do_tox(struct gtox_data *gtox)
{
    static int conn_try = 0;
    static int conn_err = 0;
    static int dht_on = FALSE;
    Tox *m = gtox->tox;
    gchar *status;

    if (!dht_on && !tox_isconnected(m) && !(conn_try++ % 100)) {
        if (!conn_err) {
            conn_err = init_connection(gtox);
            
            status = g_strdup_printf ("Establishing connection...");
            gtk_statusbar_push(GTK_STATUSBAR (gtox->statusbar), gtox->statusbar_context_id, status);
                        
            if (conn_err) {
                status = g_strdup_printf("Auto-connect failed with error code %d", conn_err);
                gtk_statusbar_push(GTK_STATUSBAR (gtox->statusbar), gtox->statusbar_context_id, status);
            }
            
            g_free(status);
        }
    } else if (!dht_on && tox_isconnected(m)) {
        dht_on = TRUE;
        status = g_strdup_printf ("DHT connected.");
        gtk_statusbar_push (GTK_STATUSBAR (gtox->statusbar), gtox->statusbar_context_id, status);
        g_free(status);
    } else if (dht_on && !tox_isconnected(m)) {
        dht_on = FALSE;
        status = g_strdup_printf("DHT disconnected. Attempting to reconnect.");
        gtk_statusbar_push(GTK_STATUSBAR (gtox->statusbar), gtox->statusbar_context_id, status);
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

static gboolean core_timer_handler(struct gtox_data *gtox)
{
    do_tox(gtox);
    return TRUE;
}

static gboolean dhtprint_timer_handler(struct gtox_data *gtox)
{
    dht_draw(gtox);
    return TRUE;
}

int main(int argc, char *argv[])
{
    GtkBuilder      *builder; 
    GtkWidget       *window;
    GtkWidget       *statusbar;
    GtkWidget       *friends_treeview;
    GtkWidget       *dht_treeview;
    GtkNotebook     *notebook;
    guint           statusbar_context_id;
    gchar           *window_title;
    PangoFontDescription    *font_desc;
    struct gtox_data gtox;
    char *our_ID;
    int rc = 0;
    
    Tox *m = init_tox();
    gtox.tox = m;
    
    /*rc = create_user_config_dir(user_config_dir); TODO:fix */
    if(rc) {
    /* error, load from current dir */
        gtox.srvlist_path = strdup("DHTservers");
        gtox.datafile_path = strdup("data");
    } else {
    /* ok, load from user config dir */
        gtox.srvlist_path = get_full_configpath("DHTservers");
        gtox.datafile_path = get_full_configpath("data");
    }
    
    /* initialize and load the gui */    
    gtk_init (&argc, &argv);
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, "gtktox.ui", NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "window1"));
    friends_treeview = GTK_WIDGET (gtk_builder_get_object (builder, "treeview1"));
    dht_treeview = GTK_WIDGET (gtk_builder_get_object (builder, "treeview2"));
    statusbar = GTK_WIDGET (gtk_builder_get_object (builder, "statusbar1"));
    notebook = GTK_NOTEBOOK (gtk_builder_get_object (builder, "notebook1"));
    
    gtk_builder_connect_signals (builder, NULL);

   
    g_signal_connect(G_OBJECT (window), "destroy",
        G_CALLBACK(gtk_main_quit), NULL);
    
    g_object_unref (G_OBJECT (builder));
    
    /* copy some gtkwidgets to gtox_data */
    gtox.dht_treeview = dht_treeview;
    gtox.statusbar = statusbar;
    gtox.notebook = notebook;
    gtox.friends_treeview = friends_treeview;

    /* load the data */
    load_data(&gtox);
    our_ID = own_id(m);
    
    /*  set the title of the window */
    window_title = g_strdup_printf("GtkTox - ID: %s", our_ID);
    gtk_window_set_title(GTK_WINDOW (window), window_title);
    g_free(window_title);
        
    /* initialize statusbar */
    statusbar_context_id = gtk_statusbar_get_context_id (GTK_STATUSBAR (statusbar), "gtk-tox");
    gtox.statusbar_context_id = statusbar_context_id;
    
    /* set monospace font on the DHT treeview */
    font_desc = pango_font_description_from_string("monospace");
    gtk_widget_override_font(dht_treeview, font_desc);     
    pango_font_description_free(font_desc);
    
    /* add tox callbacks */
/*    tox_callback_friendrequest(m, on_request, NULL);                  //TODO
    tox_callback_friendmessage(m, on_message, NULL);
    tox_callback_namechange(m, on_nickchange, NULL);
    tox_callback_statusmessage(m, on_statuschange, NULL);
    tox_callback_action(m, on_action, NULL);*/
    /* add timeout callbacks */
    
    g_timeout_add(50, (GSourceFunc) core_timer_handler, &gtox);
    g_timeout_add(400, (GSourceFunc) dhtprint_timer_handler, &gtox);
    
    gtk_widget_show (window);                
    gtk_main ();
    
    /* cleanup */
    free(gtox.srvlist_path); /* TODO: make a cleanup function */
    free(gtox.datafile_path);
    free(our_ID);
    tox_kill(m);
    
    return 0;
}
