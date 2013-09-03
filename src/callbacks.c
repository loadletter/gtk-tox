#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <tox/tox.h>
#include "gtkwindow.h"
#include "storage.h"
#include "callbacks.h"
#include "misc.h"

/* friendlist */
typedef struct {
    uint8_t name[TOX_MAX_NAME_LENGTH];
    uint8_t status[TOX_MAX_STATUSMESSAGE_LENGTH];
    int num;
    int chatwin;
    int active;
    GtkWindow *window;
    GtkTreeIter iter;
} friend_t;

static friend_t friends[MAX_FRIENDS_NUM];
static int num_friends = 0;

/* friendrequests */
static uint8_t pending_requests[MAX_REQUESTS_NUM][TOX_CLIENT_ID_SIZE]; // XXX
static GtkTreeIter iter_requests[MAX_REQUESTS_NUM];
static uint8_t num_requests = 0; // XXX


static int add_req(uint8_t *public_key)
{
    memcpy(pending_requests[num_requests], public_key, TOX_CLIENT_ID_SIZE);
    ++num_requests;
    return num_requests - 1;
}

static void delete_friend(struct gtox_data *gtox, int f_num)
{
    int i;
    tox_delfriend(gtox->tox, f_num);
    memset(&(friends[f_num]), 0, sizeof(friend_t));

    for(i = num_friends; i > 0; --i) {
        if(friends[i-1].active)
            break;
    }

    if(store_data(gtox)) {
        dialog_show_error("Could not store Tox data!");
    }

    num_friends = i;
}

/* TOX CALLBACKS START */

void on_request(uint8_t *public_key, uint8_t *data, uint16_t length, void *userdata)
{
    uint8_t address[TOX_CLIENT_ID_SIZE];
    char *plaintext_id;
    struct gtox_data *gtox = userdata;
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gtox->friendreq_treeview)));
    GtkTreeSelection *treeselect = gtk_tree_view_get_selection(GTK_TREE_VIEW(gtox->friendreq_treeview));
    int n = add_req(public_key);
    
    /* set text */
    memcpy(address, public_key, TOX_CLIENT_ID_SIZE);
    plaintext_id = human_readable_id(address, TOX_CLIENT_ID_SIZE);
    
    /* add request to the list */
    gtk_list_store_append(store, &iter_requests[n]);
    gtk_list_store_set(store, &iter_requests[n], 0, plaintext_id, 1, data, 2, n, -1);
    
    /* update the number of requests in the tab label */
    update_friendrequest_tab(gtox->notebook, GTK_TREE_VIEW(gtox->friendreq_treeview));
    
    /* show the page */
    note_show_page(gtox->notebook, NOTEBOOK_FRIENDREQ);
    
    /* select the current request */
    gtk_notebook_set_current_page(gtox->notebook, NOTEBOOK_FRIENDREQ);
    gtk_tree_selection_select_iter(treeselect, &iter_requests[n]);
    
    /* show the non-blocking dialog */
    dialog_friendrequest_show(gtox->window, plaintext_id, (char *)data);

    free(plaintext_id);
}

void on_message(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata)
{

}

void on_action(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata)
{

}

void on_nickchange(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata)
{
    struct gtox_data *gtox = userdata;
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gtox->friends_treeview)));
    
    if (length >= TOX_MAX_NAME_LENGTH || friendnumber >= num_friends)
        return;

    memcpy((char *) &friends[friendnumber].name, (char *) string, length);
    friends[friendnumber].name[length] = 0;
    
    gtk_list_store_set(store, &friends[friendnumber].iter, 0, friends[friendnumber].name, -1);
}

void on_statuschange(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata)
{
    struct gtox_data *gtox = userdata;
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gtox->friends_treeview)));

    if (length >= TOX_MAX_STATUSMESSAGE_LENGTH || friendnumber >= num_friends)
        return;

    memcpy((char *) &friends[friendnumber].status, (char *) string, length);
    friends[friendnumber].status[length] = 0;
    
    gtk_list_store_set(store, &friends[friendnumber].iter, 1, friends[friendnumber].status, -1);
}

void on_friendadded(struct gtox_data *gtox, int num)
{
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gtox->friends_treeview)));
    int i;

    if(num_friends == MAX_FRIENDS_NUM) {
        dialog_show_error("Friend limit reached!");
        return;
    }
    g_assert(num_friends >= 0);

    for(i = 0; i <= num_friends; ++i) {
        if(!friends[i].active) {
            friends[i].num = num;
            friends[i].active = TRUE;
            friends[i].chatwin = FALSE;
            strcpy((char *) friends[i].name, "unknown");
            strcpy((char *) friends[i].status, "unknown");
            
            gtk_list_store_append(store, &friends[i].iter);
            gtk_list_store_set(store, &friends[i].iter, 0, friends[i].name, 1, friends[i].status, -1);

            if(i == num_friends)
                ++num_friends;
            
            break;
        }
    }
    
    if(store_data(gtox)) {
        dialog_show_error("Could not store Tox data!");
    }
    
}
/* TOX CALLBACKS END */

/* callback to accept friendrequests */
gboolean on_friendrequest_clicked(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata)
{
    struct gtox_data *gtox = userdata;
    GtkTreeModel *model;
    GtkListStore *store;
    GtkTreeIter iter;
    gchar *id, *msg;
    gint reqid, rv;
    int n = -1;
 
    store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
    model = gtk_tree_view_get_model(treeview);
    
    if(gtk_tree_model_get_iter(model, &iter, path)) {
        gtk_tree_model_get(model, &iter, 0, &id, 1, &msg, 2, &reqid, -1);
        g_assert(reqid < num_requests);
    
        rv = dialog_friendrequest_accept(gtox->window, id, msg);
        switch(rv) {
            case GTK_RESPONSE_ACCEPT:
                n = tox_addfriend_norequest(gtox->tox, pending_requests[reqid]);
                if(n == -1)
                    dialog_show_error("Failed to add friend!");
                else {
                    /* remove from list and add/store */
                    gtk_list_store_remove(store, &iter);
                    on_friendadded(gtox, n);
                    
                    /* update the number of requests in the tab label */
                    update_friendrequest_tab(gtox->notebook, treeview);
                }
                break;
            case GTK_RESPONSE_CANCEL:
                break;
            case GTK_RESPONSE_REJECT:
                /* remove from list and update tab */
                gtk_list_store_remove(store, &iter);
                update_friendrequest_tab(gtox->notebook, treeview);
                break;
            default:
                break;
        }
        
        g_free(id);
        g_free(msg);
    }
    return TRUE;
}


/* FRIENDS MENU CALLBACKS START */

void on_friends_menu_delete(GtkWidget *menuitem, gpointer userdata)
{
/* we passed the view as userdata when we connected the signal */
GtkTreeView *treeview = GTK_TREE_VIEW(userdata);

g_print ("Do something!\n");
}

void friends_popup_menu(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
    GtkWidget *menu, *menuitem_del;

    menu = gtk_menu_new();
    menuitem_del = gtk_menu_item_new_with_label("Delete friend");

    g_signal_connect(menuitem_del, "activate", (GCallback) on_friends_menu_delete, treeview);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_del);
    
    gtk_widget_show_all(menu);

    /* Note: event can be NULL here when called from on_friend_button_pressed;
     *  gdk_event_get_time() accepts a NULL argument */
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
                   (event != NULL) ? event->button : 0,
                   gdk_event_get_time((GdkEvent*)event));
}

gboolean on_friends_button_pressed(GtkWidget *treeview, GdkEventButton *event, gpointer userdata)
{
     GtkTreeSelection *selection;
     GtkTreePath *path;
     
    /* single click with the right mouse button? */
    if (event->type == GDK_BUTTON_PRESS  &&  event->button == 3) {
        g_print ("Single right click on the tree view.\n");
        
        /* get the current selection */
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
        
        /* if only one is selected, change it to the current one */
        if (gtk_tree_selection_count_selected_rows(selection)  <= 1) {
           /* get tree path for row that was clicked */
            if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview), (gint)event->x, (gint)event->y, &path, NULL, NULL, NULL)) {
             gtk_tree_selection_unselect_all(selection);
             gtk_tree_selection_select_path(selection, path);
             gtk_tree_path_free(path);
           }
        }
        friends_popup_menu(treeview, event, userdata);
        
        return TRUE;
    }
    
    return FALSE;
}

/* FRIENDS MENU CALLBACKS END */
