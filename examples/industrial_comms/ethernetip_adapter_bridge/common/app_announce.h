//common rp message announce header file
#ifndef _APP_ANNOUNCE_H_
#define _APP_ANNOUNCE_H_

void ipc_rpmsg_echo_main(void *args);

void AppCtrl_createSendTask();

void AppCtrl_createRecvTask();

#endif
