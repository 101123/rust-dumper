#include "hooks.hpp"

void impl::hooks::hk_perf_ui_update( std::uintptr_t rcx )
{
	printf( "hello update\n" );
}