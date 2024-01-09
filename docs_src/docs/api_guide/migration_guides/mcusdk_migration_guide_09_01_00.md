# Migration Guide {#MIGRATION_GUIDES}

Components under Industrial Communications SDK were available in MCU+ SDK 7.x and 8.x releases. The folder structure of components in Indsutrial Communications SDK is kept similar to MCU SDK folder strucutre to keep the backward compatibilty as much as possible

If you are a user of MCU+ SDK, then items listed on this page that will assist you in migration to Industrial Communications SDK.

\cond SOC_AM64X || SOC_AM243X

- The examples, drivers and PRU-ICSS firmwares for Industrial Protocols and ICSSG Enet LLD are moved MCU+ SDK to Industrial Communications SDK. Folder location changes are also shown below.

## Industrial Communication Protocols
   Module                                  | MCU+ SDK Folder Location                                          | Industrial Communications SDK Folder Location
   ----------------------------------------|-------------------------------------------------------------------|----------------------------------------------------------------------------------------------
   MDIO WA firmware sources                | `examples/pru_io/mdio_fw`                                         | `source/industrial_comms/mdio_fw`
   EtherCAT examples                       | `examples/industrial_comms/ethercat_slave_demo`                   | `examples/industrial_comms/ethercat_slave_demo/device_profiles`
   Custome phy module                      | `examples/industrial_comms/example_name/customPhy`                | `examples/industrial_comms/custom_phy`
   IOLINK master demo sources              | `examples/industrial_comms/iolink_master_demo`                    | `examples/industrial_comms/iolink_master_demo/device`
   EtherCAT Stack Libraries                | `source/commercial/industrial_comms_libs/ethercat_slave`          | `source/industrial_comms/ethercat_slave/stack/lib`
   EtherNet/IP Stack Libraries             | `source/commercial/industrial_comms_libs/ethernetip_adapter`      | `source/industrial_comms/ethernetip_adapter/stack/lib`
   IO-Link Stack Libraries                 | `source/commercial/industrial_comms_libs/iolink`                  | `source/industrial_comms/iolink/stack/lib`
   EtherCAT-IOLink Gateway Stack Libraries | `source/commercial/industrial_comms_libs/ethercat_iolink_gateway` | `source/industrial_comms/ethercat_iolink_gateway/stack/lib`

\endcond

- Industrial Communications SDK also includes \htmllink{@VAR_MCU_SDK_DOCS_PATH/index.html, @VAR_SOC_NAME MCU+ SDK} under `mcu_plus_sdk` folder.

- \ref UPGRADE_AND_COMPATIBILITY_INFORMATION_9_1_0 has details on changes which can affect migration of applications based on MCU+ SDK 08.06.00 to Industrial Communications SDK 09.01.00.

\note To build MCU+ SDK examples/libraries using CCS projects, user has to add <b>${SDK_INSTALL_PATH}/mcu_plus_sdk</b> to "Product discovery path" in CCS.
