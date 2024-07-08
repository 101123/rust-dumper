#include "sapphire_crt.hpp"
#include "sapphire_hook.hpp"
#include "sapphire_importer.hpp"
#include "hooks.hpp"
#include "dumper.hpp"

auto DllMain( void*, std::uint32_t call_reason, void* ) -> bool
{
	if ( call_reason != DLL_PROCESS_ATTACH )
		return false;

	AllocConsole( );
	freopen_s( reinterpret_cast< FILE** >( stdout ), "CONOUT$", "w", stdout );

	il2cpp::Init( );
	{
		il2cpp::Il2CppClass* inputMessageClass = il2cpp::GetClassByName( "InputMessage" );
		il2cpp::Il2CppClass* matchClasses[ 2 ] = { inputMessageClass, inputMessageClass };

		il2cpp::MethodInfo* clientInput = il2cpp::GetMethodByParamClass(
			il2cpp::GetClassByName( "BasePlayer" ),
			il2cpp::SearchForClassByFieldCount( matchClasses, _countof( matchClasses ), 3, 0 ),
			1,
			METHOD_ATTRIBUTE_VIRTUAL
		);

		printf( "ClientInput: 0x%p\n", clientInput );

		dumper::produce( );

		//impl::hooks::BP_ClientInput.setup( clientInput, &impl::hooks::hk_BP_ClientInput );

		// impl::hooks::perf_ui_update.setup(  )
		// impl::hooks::ddraw_ongui.setup( "UnityEngine::DDraw.OnGUI()", &impl::hooks::hk_ddraw_ongui, 0 ); // should work anywhere as long as we have hook to call it in.
		// impl::hooks::bp_client_input.setup( "BasePlayer.ClientInput()", &impl::hooks::hk_bp_client_input );
		// impl::hooks::launch_projectile.setup( "BaseProjectile.LaunchProjectile()", &impl::hooks::hk_launch_projectile );
		// impl::hooks::on_attacked.setup( "BasePlayer.OnAttacked()", &impl::hooks::hk_on_attacked );
	}

	return true;
}