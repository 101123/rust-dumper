#pragma once
#include "sapphire_hook.hpp"
#include "include.hpp"
#include "globals.hpp"

#define SAPPHIRE_HOOK( type, name, args ) inline impl::virt_hook_c name; type hk_##name args;
#define SAPPHIRE_SWAP( type, name, args ) inline impl::swap_ptr_c name; type hk_##name args;
#define HOOK_CUSTOM( addr, orig, repl ) orig = *addr; *addr = reinterpret_cast< std::uintptr_t >( repl ); 

namespace impl 
{
	namespace hooks
	{
		SAPPHIRE_HOOK( void, BP_ClientInput, ( void* base_player, std::uintptr_t* input_state ) );
		SAPPHIRE_HOOK( void, on_attacked, ( void* instance, void* hit_info ) );
		SAPPHIRE_HOOK( void, launch_projectile, ( void* instance ) );
		SAPPHIRE_SWAP( void, ddraw_ongui, ( std::uintptr_t rcx ) );
		SAPPHIRE_SWAP( void, perf_ui_update, ( std::uintptr_t rcx ) );

		inline std::uintptr_t o_projectile_shoot_rpc;
		auto hk_projectile_shoot_rpc( void* _rcx, void* _rdx, void* a1, std::uint64_t projectile_shoot, void* a3 ) -> void;
	}
}