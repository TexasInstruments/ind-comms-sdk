//app_netif.h

typedef struct Icss_MacAddr_s
{
    /*! MAC address octets */
    uint8_t macAddr[6];
} Icss_MacAddr;

void EthApp_initNetif(void);

void App_waitForBridgeUp();

int32_t AddNetif_addBridgeMcastEntry(Icss_MacAddr mac);

int32_t AddNetif_delBridgeMcastEntry(Icss_MacAddr mac);