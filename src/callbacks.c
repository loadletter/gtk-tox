#include "gtkwindow.h"

typedef struct {
    uint8_t name[TOX_MAX_NAME_LENGTH];
    uint8_t status[TOX_MAX_STATUSMESSAGE_LENGTH];
    int num;
    int chatwin;
} friend_t;

static friend_t friends[MAX_FRIENDS_NUM];
static int num_friends = 0;

/* CALLBACKS START */
void on_request(uint8_t *public_key, uint8_t *data, uint16_t length, void *userdata)
{
    int n = add_req(public_key);
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
    }
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

void on_friendadded(Tox *m, int friendnumber)
{

}
/* CALLBACKS END */
