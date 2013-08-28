#pragma once
#include "include/Messenger.h"

struct window_m {
    Messenger *m;
    GtkWidget *window;
};

void dht_draw(struct window_m *);


