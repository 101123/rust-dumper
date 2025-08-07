#include "dumper.hpp"

HMODULE dumper_handle;

void main_thread() {
#ifdef HOOK
	HANDLE exception_handler = AddVectoredExceptionHandler( 1, dumper::exception_handler );
#endif

	il2cpp::init();
	hook_manager::init();
	dumper::produce();

#ifdef HOOK
	RemoveVectoredExceptionHandler( exception_handler );
#endif

	FreeLibraryAndExitThread( dumper_handle, 0 );
}

bool DllMain( HINSTANCE handle, uint32_t call_reason, void* ) {
	if ( call_reason == DLL_PROCESS_ATTACH ) {
		AllocConsole( );
		freopen( "CONOUT$", "w", stdout );

		dumper_handle = handle;
		CreateThread( NULL, NULL, ( LPTHREAD_START_ROUTINE )main_thread, NULL, NULL, NULL );

		return true;
	}

	else if ( call_reason == DLL_PROCESS_DETACH ) {
		FreeConsole();
		fclose( stdout );

		hook_manager::uninit();

		return true;
	}

	return false;
}