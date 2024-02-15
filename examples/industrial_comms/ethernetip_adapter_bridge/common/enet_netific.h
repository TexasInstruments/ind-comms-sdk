//enet_netif.h
#ifndef ENET_NETIFIC_H_
#define ENET_NETIFIC_H_

void App_shutdownNetworkStack();

void EthApp_lwipMain(void *a0, void *a1);

void EthApp_setNetifCbs(struct netif *netif);

int32_t App_isNetworkUp(struct netif* netif_);

#endif /* ENET_NETIFIC_H_ */
