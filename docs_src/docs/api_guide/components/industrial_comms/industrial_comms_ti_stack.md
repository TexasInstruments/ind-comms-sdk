
# Full Protocol Stacks from TI {#INDUSTRIAL_COMMS_TI_STACK}

[TOC]

\cond SOC_AM263X
\attention Full Protocol Stacks from TI is under development and not available in this release.
\endcond

\cond SOC_AM64X || SOC_AM243X || SOC_AM263PX

Industrial protocol stacks and examples included in this SDK are supported directly by TI.

Following industrial communication protocols are supported:

## EtherCAT SubDevice

EtherCAT (Ethernet for Control Automation Technology) is a real-time industrial Ethernet standard for industrial automation applications, such as input/output (I/O) devices, sensors and programmable logic controllers (PLCs). It was originally developed by Beckhoff Automation GmbH but is now overseen by the EtherCAT Technology Group that was set up to help with proliferation of the EtherCAT standard. EtherCAT technology adds
certain features on Ethernet and enforces certain configurations to make it a very efficient network technology for automation while fully conforming to the Ethernet specifications. The design of EtherCAT enables any standard PC to be used as an EtherCAT MainDevice and communicate with EtherCAT SubDevices, which are specialized devices compliant with the EtherCAT specification. Together, the MainDevice and SubDevice EtherCAT devices can be used in all devices in the factory network – automation controllers, operator interfaces, remote input/output units, sensors, actuators, drives and others.

### Data Sheet

\cond SOC_AM64X

For information about supported features and key performance parameters, see \htmllink{../am64x/ethercat_slave/ethercat_datasheet.html, EtherCAT SubDevice Data Sheet}

\endcond

\cond SOC_AM243X

For information about supported features and key performance parameters, see \htmllink{../am243x/ethercat_slave/ethercat_datasheet.html, EtherCAT Slave Data Sheet}

\endcond

\cond SOC_AM263PX

For information about supported features and key performance parameters, see \htmllink{../am263px/ethercat_slave/ethercat_datasheet.html, EtherCAT SubDevice Data Sheet}

\endcond

### Certificate

\cond SOC_AM64X

For information about the certificate, see \htmllink{../am64x/ethercat_slave/ethercat_certificate.pdf, EtherCAT Certificate}

\endcond

\cond SOC_AM243X

For information about the certificate, see \htmllink{../am243x/ethercat_slave/ethercat_certificate.pdf, EtherCAT Certificate}

\endcond

\cond SOC_AM263PX

For information about the certificate, see \htmllink{../am263px/ethercat_slave/ethercat_certificate.pdf, EtherCAT Certificate}

**NOTE**: The certification was done on a similar device - AM243x-Launchpad (LP).
\endcond

### Examples

For more details regarding the pre-integrated stack and example applications, see \ref EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_SLAVE_DEMOS.

## EtherNet/IP Adapter

EtherNet/IP is a member of a family of network protocols that implements the Common Industrial Protocol (CIP) at its upper layers. EtherNet/IP is the name given to CIP when it is implemented over standard Ethernet as defined by IEEE 802.3. EtherNet/IP is managed by the Open DeviceNet Vendor Association, Inc. (ODVA), which also has responsibility for publishing The EtherNet/IP Specification and coordinating conformance testing. Because EtherNet/IP uses standard Ethernet and TCP/IP technologies, compatibility and coexistence with other applications and protocols is assured.

### Data Sheet

\cond SOC_AM64X

For information about supported features and key performance parameters, see \htmllink{../am64x/ethernetip_adapter/eip_datasheet.html, EtherNet/IP Adapter Data Sheet}

\endcond

\cond SOC_AM243X

For information about supported features and key performance parameters, see \htmllink{../am243x/ethernetip_adapter/eip_datasheet.html, EtherNet/IP Adapter Data Sheet}

\endcond

\cond SOC_AM263PX

For information about supported features and key performance parameters, see \htmllink{../am263px/ethernetip_adapter/eip_datasheet.html, EtherNet/IP Adapter Data Sheet}

\endcond

### Certificate

\cond SOC_AM64X

For information about the certificate, see \htmllink{../am64x/ethernetip_adapter/ethernetip_certificate.pdf, EtherNet/IP Certificate}

\endcond

\cond SOC_AM243X

For information about the certificate, see \htmllink{../am243x/ethernetip_adapter/ethernetip_certificate.pdf, EtherNet/IP Certificate}

\endcond

\cond SOC_AM263PX

For information about the certificate, see \htmllink{https://software-dl.ti.com/processor-industrial-sw/esd/ind_comms_sdk/am243x/09_02_00_15/docs/am243x/ethernetip_adapter/ethernetip_certificate.pdf, EtherNet/IP Certificate}

**NOTE**: The certification was done on a similar device - AM243x-Launchpad (LP).

\endcond

### Examples

For more details regarding the pre-integrated stack and example applications, see \ref EXAMPLES_INDUSTRIAL_COMMS_ETHERNETIP_ADAPTER_DEMOS.

## IO-Link Controller

IO-Link (International Electrotechnical Commission [IEC] 61131-9) is an open standards protocol that addresses the need for intelligent control of small devices such as sensors and actuators. This standard provides lowspeed point-to-point serial communication between a device and a controller that normally serves as a gateway to a fieldbus and PLC. The intelligent link established enables ease of communication for data exchange, configuration, and diagnostics. The many advantages of an IO-Link system include standardized wiring, increased data availability, remote monitoring and configuration, simple replacement of devices and advanced diagnostics.

### Data Sheet

\cond SOC_AM64X

For information about supported features and key performance parameters, see \htmllink{../am64x/iolink_master/iolink_datasheet.html, IO-Link Controller Data Sheet}

\endcond

\cond SOC_AM243X

For information about supported features and key performance parameters, see \htmllink{../am243x/iolink_master/iolink_datasheet.html, IO-Link Controller Data Sheet}

\endcond

\cond SOC_AM263PX

For information about supported features and key performance parameters, see \htmllink{../am263px/iolink_master/iolink_datasheet.html, IO-Link Controller Data Sheet}

\endcond

### Test Report

\cond SOC_AM64X

For information about Test Report, see \htmllink{../am64x/iolink_master/iolink_test_report.pdf, IO-Link Controller Test Report}

\endcond

\cond SOC_AM243X

For information about Test Report, see \htmllink{../am243x/iolink_master/iolink_test_report.pdf, IO-Link Controller Test Report}

\endcond

\cond SOC_AM263PX

For information about Test Report, see \htmllink{https://software-dl.ti.com/processor-industrial-sw/esd/ind_comms_sdk/am243x/09_02_00_15/docs/am243x/iolink_master/iolink_test_report.pdf, IO-Link Controller Test Report}

**NOTE**: The testing was done on a similar device - AM243x-Launchpad (LP).

\endcond

### Examples

For more details regarding the pre-integrated stack and example applications, see \ref EXAMPLES_INDUSTRIAL_COMMS_IOLINK_MASTER_DEMO.

\cond SOC_AM64X || SOC_AM243X
## EtherCAT-IOLink Gateway

EtherCAT-IOLink gateway solution which combines EtherCAT industrial Ethernet fieldbus technology and IO-Link intelligent communication technology.

### Data Sheet

\cond SOC_AM64X

For information about supported features and key performance parameters, see \htmllink{../am64x/ethercat_iolink_gateway/ethercat_datasheet.html, EtherCAT-IOLink Gateway Data Sheet}

\endcond

\cond SOC_AM243X

For information about supported features and key performance parameters, see \htmllink{../am243x/ethercat_iolink_gateway/ethercat_datasheet.html, EtherCAT-IOLink Gateway Data Sheet}

\endcond

### Examples

For more details regarding the pre-integrated stack and example applications, see \ref EXAMPLES_INDUSTRIAL_COMMS_ETHERCAT_IOLINK_GATEWAY_DEMO.

## Profinet Device {#INDUSTRIAL_COMMS_TI_STACK_PROFINET}

PROFINET is a real-time Ethernet standard for the high-speed, deterministic communications required for a wide range of industrial applications including factory automation, process automation and building automation. Originally developed under the leadership of Siemens GmbH as a network extension to the popular PROFIBUS fieldbus technology, PROFINET is now supported by PROFIBUS & PROFINET International. In 2003, PROFINET was integrated into the IEC 61158 and IEC 61784 standards.

### Data Sheet

\cond SOC_AM64X

For information about supported features and key performance parameters, see \htmllink{../am64x/profinet_device/profinet_datasheet.html, Profinet Device Data Sheet}

\endcond

\cond SOC_AM243X

For information about supported features and key performance parameters, see \htmllink{../am243x/profinet_device/profinet_datasheet.html, Profinet Device Data Sheet}

\endcond

### Test Report

\cond SOC_AM64X

For information about ART Test report, see \htmllink{../am64x/profinet_device/art_testreport.pdf, Automated RT Tester Report}

\endcond

\cond SOC_AM243X

For information about ART Test report, see \htmllink{../am243x/profinet_device/art_testreport.pdf, Automated RT Tester Report}

\endcond

### Examples

For more details regarding the pre-integrated stack and example applications, see \ref EXAMPLES_INDUSTRIAL_COMMS_PROFINET_DEVICE_DEMO.

\endcond

\endcond
