Note before using the ESI file in TwinCAT
-------------------------------------------

(1) EtherCAT DPRAM size is different for AM64x/AM243x platforms and AM26x platforms:
	- 59KB for AM243x and AM64x platforms.
	- 28KB for AM26x (AM263x, AM263Px and AM261x) platforms.

(2) Currently, the EtherCAT Product Code is kept same accross platforms. This will be updated going forward.

(3) Make sure to use the correct DPRAM size for the corresponding platform.
	- To use the ESI files for AM26x, update the DPRAMSize (under <EtherCATController>) from 60416 to 28672.
	- To use the ESI files for AM64x/AM243x, no change is required.