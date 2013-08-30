#pragma once
#include "gtkwindow.h"

/* DHTservers stuff */
#define MAXLINE 90    /* Approx max number of chars in a sever line (IP + port + key) */
#define MINLINE 70
#define MAXSERVERS 50

int init_connection(struct gtox_data *gtox);
char *get_full_configpath(const char *filename);

/* Userdata storage */
int store_data(struct gtox_data *gtox);
int load_data(struct gtox_data *gtox);
