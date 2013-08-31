#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <gtk/gtk.h>
#include <tox/tox.h>
#include "gtkwindow.h"
#include "storage.h"
#include "callbacks.h"

typedef struct {
    uint8_t name[TOX_MAX_NAME_LENGTH];
    uint8_t status[TOX_MAX_STATUSMESSAGE_LENGTH];
    int num;
    int chatwin;
    GtkWindow *window;
    GtkTreeIter iter;
} friend_t;

static friend_t friends[MAX_FRIENDS_NUM];
static int num_friends = 0;

/* CALLBACKS START */
void on_request(uint8_t *public_key, uint8_t *data, uint16_t length, void *userdata)
{
    /*int n = add_req(public_key);
    wprintw(prompt->window, "\nFriend request from:\n");

    int i;

    for (i = 0; i < KEY_SIZE_BYTES; ++i) {
        wprintw(prompt->window, "%02x", public_key[i] & 0xff);
    }

    wprintw(prompt->window, "\nWith the message: %s\n", data);
    wprintw(prompt->window, "\nUse \"accept %d\" to accept it.\n", n);

    for (i = 0; i < MAX_WINDOWS_NUM; ++i) {
        if (windows[i].onFriendRequest != NULL)
            windows[i].onFriendRequest(&windows[i], public_key, data, length);
    }*/
}

void on_message(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata)
{

}

void on_action(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata)
{

}

void on_nickchange(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata)
{

}

void on_statuschange(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata)
{

}

void on_friendadded(struct gtox_data *gtox, int num)
{
    GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(gtox->friends_treeview)));
    
    if (num_friends == MAX_FRIENDS_NUM) {
        printf("Friend limit reached\n"); //FIX
        return;
    }

    friends[num_friends].num = num;
    tox_getname(gtox->tox, num, friends[num_friends].name);
    strcpy((char *) friends[num_friends].name, "unknown");
    strcpy((char *) friends[num_friends].status, "unknown");
    friends[num_friends].chatwin = FALSE;
        
    gtk_list_store_append(store, &friends[num_friends].iter);
    gtk_list_store_set(store, &friends[num_friends].iter, 0, friends[num_friends].name, 1, friends[num_friends].status, -1);
    
    num_friends++;
    
    if (store_data(gtox)) {
        printf("Could not store Tox data\n"); //FIX
    }
    
}
/* CALLBACKS END */
