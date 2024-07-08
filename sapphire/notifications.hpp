#pragma once
#include <vector>
#include <memory>

#include "include.hpp"
#include "unity_gui.hpp"

namespace sapphire
{
	namespace features
	{
		class c_notify_text
		{
		public:
			String m_text;
			clr_t m_color;
			float m_time;

		public:
			c_notify_text( const String text, clr_t color, float time ) : m_text( text ), m_color( color ), m_time( time ) { }
		};

		class c_notify
		{
		private:
			std::vector< std::shared_ptr< sapphire::features::c_notify_text > > m_notify_text;
		public:
			c_notify( ) : m_notify_text{} {}

			auto push( String text, float timestamp, clr_t color = clr_t( 255, 255, 255, 255 ) ) -> void {
				m_notify_text.push_back( std::make_shared< sapphire::features::c_notify_text >( text, color, timestamp ) );
			}

			auto run( ) -> void {
				
			}
		};

		namespace notifcations
		{
			inline c_notify object;
		}
	}
}