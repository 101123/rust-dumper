#include "hooks.hpp"

auto impl::hooks::hk_ddraw_ongui( std::uintptr_t rcx ) -> void
{
	sapphire::rendering::init( );

}