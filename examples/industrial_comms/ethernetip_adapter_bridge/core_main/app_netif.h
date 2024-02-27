//app_netif.h

typedef struct Icss_MacAddr_s
{
    /*! MAC address octets */
    uint8_t macAddr[6];
} Icss_MacAddr;

void EthApp_initNetif(void);

void App_shutdownNetworkStack();

// void EnetMp_togglePromisc(Enet_Type enetType, uint32_t instId);

void App_waitForBridgeUp();

// int32_t App_addMacFdbEntry(Enet_Type enetType, uint32_t instId, Icssg_MacAddr mac);

