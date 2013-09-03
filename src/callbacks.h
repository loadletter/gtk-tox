#pragma once

#define MAX_FRIENDS_NUM 100
#define MAX_REQUESTS_NUM 256

/* CALLBACKS START */
void on_request(uint8_t *public_key, uint8_t *data, uint16_t length, void *userdata);
void on_message(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata);
void on_action(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata);
void on_nickchange(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata);
void on_statuschange(Tox *m, int friendnumber, uint8_t *string, uint16_t length, void *userdata);
void on_friendadded(struct gtox_data *gtox, int num);
/* CALLBACKS END */

gboolean on_friendrequest_clicked(GtkTreeView *treeview, GtkTreePath *path, GtkTreeViewColumn *col, gpointer userdata);

/* FRIENDS MENU CALLBACKS END */
void on_friends_menu_delete(GtkWidget *menuitem, gpointer userdata);
void friends_popup_menu(GtkWidget *treeview, GdkEventButton *event, gpointer userdata);
gboolean on_friends_button_pressed(GtkWidget *treeview, GdkEventButton *event, gpointer userdata);
/* FRIENDS MENU CALLBACKS END */
