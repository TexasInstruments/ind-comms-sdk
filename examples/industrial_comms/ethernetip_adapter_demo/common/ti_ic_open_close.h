//ti_ic_lwipif.h

#ifndef TI_IC_OPEN_CLOSE_H_
#define TI_IC_OPEN_CLOSE_H_
/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */
#ifdef __cplusplus
extern "C" {
#endif
#include <intercore/intercore.h>
#include <lwip_ic.h>
#include <intercore/include/ShdMemCircularBufferP_nortos.h>
#include "app_control.h"

/* ========================================================================== */
/*                          API/Public Function Declarations                  */
/* ========================================================================== */

Ic_Object_Handle App_doIcOpen(uint32_t instId);
int32_t App_getSharedMemInfo(Icve_respMsg *respMsg, uint32_t remoteCoreId);
extern uint32_t App_getSelfCoreId();

#ifdef __cplusplus
}
#endif
#endif /* TI_IC_OPEN_CLOSE_H_ */
