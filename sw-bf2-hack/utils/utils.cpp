#include "../includes.h"

void utils::raise_error(DWORD64 check_var, const char* error_msg, const char* success_message)
{
	if (!check_var)
	{
		MessageBoxA(NULL, error_msg, "alternative hack", MB_OK | MB_ICONERROR);
		FreeLibraryAndExitThread(global_vars::dll_module, 1);
	}
#ifdef ENABLE_DEBUG_CONSOLE
	else
	{
		std::cout << success_message << "0x" << std::hex << (DWORD)check_var << std::endl;
	}
#endif // DEBUG_
}