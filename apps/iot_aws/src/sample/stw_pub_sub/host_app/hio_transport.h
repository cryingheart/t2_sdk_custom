#pragma once

int init_hio_transport(char *device);
int write_hio_message(char *message, int msg_len);
int read_hio_message(char *message, int len);
void deinit_hio_transport() ;
int send_command(int group_id, int msg_id, void *data, int size, int transid);
