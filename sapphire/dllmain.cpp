#include "dumper.hpp"

bool __stdcall DllMain( void*, std::uint32_t call_reason, void* )
{
	if ( call_reason == DLL_PROCESS_ATTACH ) {
		AllocConsole( );
		freopen_s( reinterpret_cast< FILE** >( stdout ), "CONOUT$", "w", stdout );

		il2cpp::init( );
		hook_manager::init();
		dumper::produce( );

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