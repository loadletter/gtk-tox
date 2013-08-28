#pragma once
#include "include/Messenger.h"

/* TODO: make it useful */
struct window_m {
    Messenger *m;
    GtkWidget *window;
};

void dht_draw(struct window_m *);


