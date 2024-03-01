#include "uni_shell_osal.h"

#include "uni_shell.h"
#include "cmsis_os.h"

#if ASYNC_LOG

TaskHandle_t shell_hadle = NULL;

#else
	
#endif

__weak uint8_t is_shell_thread(void)
{

#if ASYNC_LOG
	if (shell_hadle != NULL && shell_hadle == xTaskGetCurrentTaskHandle()) {
		return TRUE;
	}
	return FALSE;
	
#else
	
	return TRUE;
#endif
}

void current_as_shell_thread(void)
{
#if ASYNC_LOG
	shell_hadle = xTaskGetCurrentTaskHandle();
	
#else
	
#endif
}