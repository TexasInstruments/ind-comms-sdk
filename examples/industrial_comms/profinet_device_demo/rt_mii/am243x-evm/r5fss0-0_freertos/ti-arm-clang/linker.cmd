

 /* This is the stack that is used by code running within main()
  * In case of NORTOS,
  * - This means all the code outside of ISR uses this stack
  * In case of FreeRTOS
  * - This means all the code until vTaskStartScheduler() is called in main()
  *   uses this stack.
  * - After vTaskStartScheduler() each task created in FreeRTOS has its own stack
  */

 --stack_size=4096
/* This is the heap size for malloc() API in NORTOS and FreeRTOS
* This is also the heap used by pvPortMalloc in FreeRTOS
*/
 --heap_size=524288
-e_vectors  /* This is the entry of the application, _vector MUST be placed starting address 0x0 */

/* This is the size of stack when R5 is in IRQ mode
 * In NORTOS,
 * - Here interrupt nesting is enabled
 * - This is the stack used by ISRs registered as type IRQ
 * In FreeRTOS,
 * - Here interrupt nesting is enabled
 * - This is stack that is used initally when a IRQ is received
 * - But then the mode is switched to SVC mode and SVC stack is used for all user ISR callbacks
 * - Hence in FreeRTOS, IRQ stack size is less and SVC stack size is more
 */
__IRQ_STACK_SIZE = 256;
/* This is the size of stack when R5 is in IRQ mode
 * - In both NORTOS and FreeRTOS nesting is disabled for FIQ
 */
__FIQ_STACK_SIZE = 256;
__SVC_STACK_SIZE = 4096; /* This is the size of stack when R5 is in SVC mode */
__ABORT_STACK_SIZE = 256;  /* This is the size of stack when R5 is in ABORT mode */
__UNDEFINED_STACK_SIZE = 256;  /* This is the size of stack when R5 is in UNDEF mode */

LINKER_CMD_VAL_START_OF_DATA_MEM = START(DATA_MEM);
LINKER_CMD_VAL_END_OF_DATA_MEM =   END(DATA_MEM);
LINKER_CMD_VAL_START_OF_HEAP_MEM = START(HEAP_MEM);
LINKER_CMD_VAL_END_OF_HEAP_MEM =   END(HEAP_MEM);


SECTIONS
{
    .vectors  : {
    } > R5F_VECS   , palign(8) 

    .text.hwi  : {
    } > R5F_TCMA   , palign(8) 

    .text_tcm  : {
    } > R5F_TCMA   , palign(8) 


    GROUP  :   {
    .ARM.exidx : {
    } palign(8)
    .init_array : {
    } palign(8)
    .fini_array : {
    } palign(8)
    } > MSRAM  


    GROUP  :   {
    .text.cache : {
    } palign(8)
    .text.mpu : {
    } palign(8)
    .text.boot : {
    } palign(8)
    } > MSRAM  


    GROUP  :   {
    .text : {
    } palign(8)
    .rodata : {
    } palign(8)
    } > CODE_MEM  


    GROUP  :   {
    .data : {
    } palign(8)
    .threadstack : {
    } palign(8)
    .stack : {
    } palign(8)
    } > MSRAM  


    GROUP  :   {
    .bss : {
    } palign(8)
    RUN_START(__BSS_START)
    RUN_END(__BSS_END)
    } > DATA_MEM  


    GROUP  :   {
    .irqstack : {
        . = . + __IRQ_STACK_SIZE;
    } align(8)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)
    .fiqstack : {
        . = . + __FIQ_STACK_SIZE;
    } align(8)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)
    .svcstack : {
        . = . + __SVC_STACK_SIZE;
    } align(8)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)
    .abortstack : {
        . = . + __ABORT_STACK_SIZE;
    } align(8)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)
    .undefinedstack : {
        . = . + __UNDEFINED_STACK_SIZE;
    } align(8)
    RUN_START(__UNDEFINED_STACK_START)
    RUN_END(__UNDEFINED_STACK_END)
    } > MSRAM  

    .bss.icss_emac_pktbuf_mem (NOLOAD) : {
    } > ICSS_PKT_BUF_MEM    

    .bss.user_shared_mem (NOLOAD) : {
    } > USER_SHM_MEM    

    .bss.log_shared_mem (NOLOAD) : {
    } > LOG_SHM_MEM    

    .bss.ipc_vring_mem  : {
    } > RTOS_NORTOS_IPC_SHM_MEM    

    .sysmem  : {
    } > HEAP_MEM    

    .boot_pkt_mem (NOLOAD) : {
    } > BOOT_PKT_MEM    


}


MEMORY
{
    R5F_VECS   : ORIGIN = 0x0 , LENGTH = 0x40 
    R5F_TCMA   : ORIGIN = 0x40 , LENGTH = 0x7FC0 
    R5F_TCMB0   : ORIGIN = 0x41010000 , LENGTH = 0x8000 
    ICSS_PKT_BUF_MEM   : ORIGIN = 0x70000000 , LENGTH = 0x10000 
    HEAP_MEM   : ORIGIN = 0x70010000 , LENGTH = 0x80000 
    MSRAM   : ORIGIN = 0x70090000 , LENGTH = 0x144000 
    USER_SHM_MEM   : ORIGIN = 0x701D4000 , LENGTH = 0x4000 
    LOG_SHM_MEM   : ORIGIN = 0x701D8000 , LENGTH = 0x4000 
    RTOS_NORTOS_IPC_SHM_MEM   : ORIGIN = 0x701DC000 , LENGTH = 0x4000 
    CODE_MEM   : ORIGIN = 0x80000000 , LENGTH = 0x1D0000 
    BOOT_PKT_MEM   : ORIGIN = 0x80200000 , LENGTH = 0x200000 
    DATA_MEM   : ORIGIN = 0x80400000 , LENGTH = 0x200000 

    /* For memory Regions not defined in this core but shared by other cores with the current core */


}
