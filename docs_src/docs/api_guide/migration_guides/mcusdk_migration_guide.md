# Migration Guide {#MIGRATION_GUIDES}

Components under Industrial Communications SDK were available in MCU+ SDK 7.x and 8.x releases. The folder structure of components in Indsutrial Communications SDK is kept similar to MCU SDK folder strucutre to keep the backward compatibilty as much as possible

If you are a user of MCU+ SDK, then items listed on this page that will assist you in migration to Industrial Communications SDK.

\cond SOC_AM64X || SOC_AM243X

- The examples, drivers and PRU-ICSS firmwares for Industrial Protocols and ICSSG Enet LLD are moved MCU+ SDK to Industrial Communications SDK. Folder location changes are also shown below.

## Industrial Communication Protocols
   Module                      | MCU+ SDK Folder Location                                                                       | Industrial Communications SDK Folder Location
   ----------------------------|------------------------------------------------------------------------------------------------|----------------------------------------------------------------------------------------------
   MDIO WA firmware sources    | `examples/pru_io/mdio_fw`                                                                      | `source/industrial_comms/mdio_fw` 
   EtherCAT examples           | `examples/industrial_comms/ethercat_slave_demo`                                                | `examples/industrial_comms/ethercat_slave_demo/device_profiles`
   Custome phy module          | `examples/industrial_comms/example_name/customPhy`                                             | `examples/industrial_comms/custom_phy`
   IOLINK master demo sources  | `examples/industrial_comms/iolink_master_demo`                                                 | `examples/industrial_comms/iolink_master_demo/device`

## Networking Component
ICSSG based ethernet networking examples are considered in this category.
- Examples are moved from MCU Plus SDK to Industrial Communications SDK.
- Low level Drivers including Enet-LLD is reused from MCU Plus SDK.
- Stacks and Middleware such as LwIP, MbedTLS and TSN stack is reused from MCU Plus SDK.
- The application design is still maintained similar to CPSW base ethernet protocols.
- No change to CPSW based ethernet networking; it is continued to be supported withing MCU Plus SDK.

Module                                      | MCU+ SDK Folder Location                                                                           | Industrial Communications SDK Folder Location
--------------------------------------------|----------------------------------------------------------------------------------------------------|---------------------------------------------------------
Ethernet Low Level Driver (Enet-LLD)        | `source/networking/enet/core`                                                                      | Reused from MCU Plus SDK
LwIP, TSN, MbedTLS                          | `source/networking/lwip`\n `source/networking/mbedtls-library`\n `source/networking/tsn`           | Reused from MCU Plus SDK
ICSSG Examples                              | `examples/networking/enet_icssg_tas` \n `examples/networking/enet_layer2_icssg` \n `examples/networking/enet_loopback/enet_icssg_loopback` \n `examples/networking/enet_vlan_icssg` \n `examples/networking/lwip/enet_icssg_tcpserver` \n `examples/networking/lwip/enet_lwip_icssg` |  `examples/networking/enet_icssg_tas` \n `examples/networking/enet_layer2_icssg` \n `examples/networking/enet_loopback/enet_icssg_loopback` \n `examples/networking/enet_vlan_icssg` \n `examples/networking/lwip/enet_icssg_tcpserver` \n `examples/networking/lwip/enet_lwip_icssg`| 

\endcond


- Industrial Communications SDK also includes \htmllink{@VAR_MCU_SDK_DOCS_PATH/index.html, @VAR_SOC_NAME MCU+ SDK} under `mcu_plus_sdk` folder. 