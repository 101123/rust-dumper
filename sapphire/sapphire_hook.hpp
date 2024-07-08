#pragma once
#include "il2cpp_lib.hpp"

#include <string>

namespace impl
{
	// note - chloe; we cannot call the original of these functions however most of them OnEnable/Start/Update/OnGUI are empty so we do not need to add saving of original.
	class swap_ptr_c
	{
	private:
		std::uintptr_t m_original{ 0 };
	public:

		auto setup( il2cpp::MethodInfo* method, void* our_function, int arg_count = -1 ) -> void
		{
			m_original = *reinterpret_cast< std::uintptr_t* >( method );
			*reinterpret_cast< void** >( method ) = our_function;
		}

		auto restore( void* our_function ) -> void
		{
			*reinterpret_cast< void** >( our_function ) = &m_original;
		}
	};

	// note - chloe; virtual functions inside class will still have same index between restarts. Maybe i will add hook by specific index.
	class virt_hook_c
	{
	private:
		std::uintptr_t m_original{ 0 };
		std::uintptr_t m_idx{ 0 };
	public:
		auto setup( il2cpp::MethodInfo* methodInfo, void* ourFunction ) -> void
		{
			uint64_t classBase = ( uint64_t )methodInfo->Class( );
			uint64_t searchMethod = *( uint64_t* )methodInfo;

			if ( searchMethod == uint64_t( ourFunction ) )
				return;

			for ( auto idx{ classBase }; idx <= classBase + 0x1500; idx += 0x1 )
			{
				const auto addr = *reinterpret_cast< std::uintptr_t* >( idx );
				if ( addr == searchMethod )
				{
					m_original = searchMethod;
					m_idx = idx;
					*reinterpret_cast< std::uintptr_t* >( idx ) = std::uintptr_t( ourFunction );
				}
			}
		}

		template< typename type >
		auto GetOriginal( ) -> type { return reinterpret_cast< type >( m_original ); }

		auto restore( void* our_function ) -> void
		{
			*reinterpret_cast< uint64_t* >( m_idx ) = m_original;
		}
	};
}