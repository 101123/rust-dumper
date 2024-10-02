#include "dumper.hpp"

HMODULE dumper_handle;

void main_thread() {
	il2cpp::init();
	hook_manager::init();
	dumper::produce();

	FreeLibraryAndExitThread( dumper_handle, 0 );
}

bool DllMain( HINSTANCE handle, uint32_t call_reason, void* ) {
	if ( call_reason == DLL_PROCESS_ATTACH ) {
		AllocConsole( );
		freopen_s( reinterpret_cast< FILE** >( stdout ), "CONOUT$", "w", stdout );

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