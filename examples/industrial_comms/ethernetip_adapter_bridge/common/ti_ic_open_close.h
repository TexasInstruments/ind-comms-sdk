//ti_ic_lwipif.h

#include <intercore/intercore.h>
#include <lwip_ic.h>
#include <intercore/include/ShdMemCircularBufferP_nortos.h>
#include "app_control.h"
// #include "app_announce.h"

Ic_Object_Handle App_doIcOpen(uint32_t instId);

int32_t App_getSharedMemInfo(Icve_respMsg *respMsg, uint32_t remoteCoreId);