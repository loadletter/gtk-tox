#pragma once
uint32_t resolve_addr(const char *address);
unsigned char *hex_string_to_bin(char hex_string[]);
char *own_id(Tox *m);
char *human_readable_id(uint8_t address[TOX_FRIEND_ADDRESS_SIZE]);

/* ensure that we sleep in milliseconds */
#ifdef WIN32
#define c_sleep(x) Sleep(x)
#else
#define c_sleep(x) usleep(1000*x)
#endif
