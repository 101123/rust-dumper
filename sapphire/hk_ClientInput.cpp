#include "hooks.hpp"

static bool has_initialized{ false };

auto impl::hooks::hk_BP_ClientInput( void* base_player, std::uintptr_t* input_state ) -> void
{
	if ( uint64_t localEnt = ( uint64_t )( base_player ) )
	{
		// 	public BasePlayer.%9c4fb218064495f81b6ea883a7f6e466e7d03e06<ulong> %06bb44d28ea1f09db28b3506e83548eb01824171; // 0xAD0
	}

	if ( auto clientEntities = rust::classes::BaseNetworkable::clientEntities( ) )
	{
		if ( auto entityList = clientEntities->entityList( ) )
		{
			for ( uint32_t i = 0; i < entityList->Vals->Count; i++ )
			{
				rust::classes::BaseNetworkable* networkable = entityList->Vals->Get( i );
				if ( !networkable )
					continue;

				auto base_object = *reinterpret_cast< std::uintptr_t* >( networkable + 0x10 );
				if ( !base_object )
					continue;

				auto object = *reinterpret_cast< std::uintptr_t* >( base_object + 0x30 );
				if ( !object )
					continue;

				const auto tag = *reinterpret_cast< std::uint16_t* >( object + 0x54 );
				if ( tag != 6 )
					continue;

				const auto ent = *reinterpret_cast< std::uintptr_t* >( base_object + 0x28 );
				if ( !ent )
					continue;

				const auto player = reinterpret_cast< rust::classes::BasePlayer* >( ent );
				if ( !ent )
					continue;

				const auto eyes = player->eyes( );

				if ( i == 0 )
				{
					printf( "ent[%d]: 0x%p\n", i, player );
					printf( "eyes: 0x%p\n", eyes );
					printf( "userId: %llu\n", player->userId( ) );

					if ( const auto name = player->_displayName( ) )
					{
						if ( name->buffer )
							printf( "name: %ls\n", name->buffer );
					}

					if ( eyes )
					{
						math::vector_t headAngles = eyes->ViewOffset( );
						printf( "%d %d %d\n", ( int )headAngles.x, ( int )headAngles.y, ( int )headAngles.z );
					}

					if ( auto inventory = player->inventory( ) )
					{
						printf( "inventory: 0x%p\n", inventory );

						uint64_t activeItem = player->clActiveItem( );
						printf( "activeItem: %llu\n", activeItem );

						if ( auto belt = inventory->GetBelt( ) )
						{
							printf( "belt: 0x%p\n", belt );
						
							if ( auto items = belt->itemList( ) )
							{

								printf( "items: 0x%p\n", items );

								for ( int j = 0; j < items->Count( ); j++ )
								{
									auto item = items->Get( j );
									if ( !item )
										continue;

									printf( "%d: uid: %llu\n", j, item->uid( ) );

									if ( item->uid( ) == activeItem )
										printf( "dec works\n" );
								}
							}
						}
					}
				}
			}
		}
	}

	impl::hooks::BP_ClientInput.GetOriginal< decltype( &impl::hooks::hk_BP_ClientInput ) >( )( base_player, input_state );
}