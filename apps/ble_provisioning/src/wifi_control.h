#ifndef APPS_WIFI_CONTROL_H_
#define APPS_WIFI_CONTROL_H_

void pause_set(int pause);
void pause_set_answer(int pause);
int wifi_main(char *ssid, char *pw);
void wifi_destroy(bool state_connected);
#endif /* APPS_WIFI_CONTROL_H_ */
