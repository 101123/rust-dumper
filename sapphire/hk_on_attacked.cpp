#include <windows.h>
#include <string>

#include "hooks.hpp"

auto impl::hooks::hk_on_attacked( void* instance, void* hit_info ) -> void
{
	impl::hooks::on_attacked.GetOriginal< decltype( &impl::hooks::hk_on_attacked ) >( )( instance, hit_info );
}