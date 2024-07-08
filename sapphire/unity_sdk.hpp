#pragma once
#include "il2cpp_lib.hpp"
#include "include.hpp"

namespace unity
{
	namespace Internal
	{
		//#define DO_ASSERT( name, size ) static_assert( sizeof( name ) == size, "sizeof " #name " is not "
		//#size );
		struct ScriptingObjectPtr { };

		struct ScriptingGCHandle
		{
			uint64_t m_Handle;
			uint64_t m_Weakness;
			uint64_t m_Object; // ScriptingObjectPtr
		};
		// DO_ASSERT( ScriptingGCHandle, 0x18 );

		struct Object
		{
			uint64_t __vftable;
			int m_InstanceID;
			int _bf_c;
			uint64_t m_EventIndex;
			ScriptingGCHandle m_MonoReference;
		};
		// DO_ASSERT( Object, 0x30 );

		struct EditorExtension : Object { };

		struct GameObject_ComponentPair
		{
			int typeindex;
			char pad_00[ 0x4 ];
			struct _Component* component;
		};
		// DO_ASSERT( GameObject_ComponentPair, 0x10 );

		using ConstantString = const char*;

		struct _GameObject : EditorExtension
		{
			GameObject_ComponentPair* m_Component;
			char pad_00[ 0x50 - 0x38 ];
			int m_Layer;
			uint16_t m_Tag;
			bool m_IsActive;
			bool m_IsActiveCached;
			int m_ActivationState;
			int m_SupportedMessages;
			ConstantString m_Name;
			// ListNode<GameObject> m_ActiveGONode;
			char pad_01[ 0x80 - 0x68 ];
		};

		// DO_ASSERT( _GameObject, 0x80 );

		struct _Component : EditorExtension
		{
			_GameObject* m_GameObject;
		};

		// DO_ASSERT( _Component, 0x38 );

		struct _Behaviour : _Component
		{
			bool m_Enabled;
			bool m_IsAdded;
		};
	}
	
	template < typename KeyType, typename ValueType >
	class Dictionary
	{
	private:
		uintptr_t Entries( )
		{
			return *( uintptr_t* )( this + 0x18 );
		}

	public:
		uint32_t Count( )
		{
			return *( uint32_t* )( Entries( ) + 0x18 );
		}

		ValueType Value( uint32_t idx )
		{
			auto entries = Entries( );
			if ( !entries )
				return {};

			auto offset = entries + 0x10;

			return *( ValueType* )( offset + ( idx * 0x18 ) + 0x20 );
		}

		KeyType Key( uint32_t idx )
		{
			auto entries = Entries( );
			if ( !entries )
				return {};

			auto offset = entries + 0x10;

			return *( KeyType* )( offset + ( idx * 0x18 ) + 0x18 );
		}
	};

	enum class key_code // todo; clean this mess up. maybe put both of these in enums.hpp
	{
		Backspace = 8, Delete = 127, Tab = 9, Clear = 12,
		Return = 13, Pause = 19, Escape = 27, Space = 32,
		Keypad0 = 256, Keypad1 = 257, Keypad2 = 258, Keypad3 = 259,
		Keypad4 = 260, Keypad5 = 261, Keypad6 = 262, Keypad7 = 263,
		Keypad8 = 264, Keypad9 = 265, KeypadPeriod = 266, KeypadDivide = 267,
		KeypadMultiply = 268, KeypadMinus = 269, KeypadPlus = 270, KeypadEnter = 271,
		KeypadEquals = 272, UpArrow = 273, DownArrow = 274, RightArrow = 275,
		LeftArrow = 276, Insert = 277, Home = 278, End = 279,
		PageUp = 280, PageDown = 281, F1 = 282, F2 = 283,
		F3 = 284, F4 = 285, F5 = 286, F6 = 287,
		F7 = 288, F8 = 289, F9 = 290, F10 = 291,
		F11 = 292, F12 = 293, F13 = 294, F14 = 295,
		F15 = 296, Alpha0 = 48, Alpha1 = 49, Alpha2 = 50,
		Alpha3 = 51, Alpha4 = 52, Alpha5 = 53, Alpha6 = 54,
		Alpha7 = 55, Alpha8 = 56, Alpha9 = 57, Exclaim = 33,
		DoubleQuote = 34, Hash = 35, Dollar = 36, Percent = 37,
		Ampersand = 38, Quote = 39, LeftParen = 40, RightParen = 41,
		Asterisk = 42, Plus = 43, Comma = 44, Minus = 45,
		Period = 46, Slash = 47, Colon = 58, Semicolon = 59,
		Less = 60, Equals = 61, Greater = 62, Question = 63,
		At = 64, LeftBracket = 91, Backslash = 92, RightBracket = 93,
		Caret = 94, Underscore = 95, BackQuote = 96, A = 97,
		B = 98, C = 99, D = 100, E = 101,
		F = 102, G = 103, H = 104, I = 105,
		J = 106, K = 107, L = 108, M = 109,
		N = 110, O = 111, P = 112, Q = 113,
		R = 114, S = 115, T = 116, U = 117,
		V = 118, W = 119, X = 120, Y = 121,
		Z = 122, LeftCurlyBracket = 123, Pipe = 124, RightCurlyBracket = 125, Tilde = 126,
		Numlock = 300, CapsLock = 301, ScrollLock = 302, RightShift = 303,
		LeftShift = 304, RightControl = 305, LeftControl = 306, RightAlt = 307,
		LeftAlt = 308, LeftCommand = 310, LeftApple = 310, LeftWindows = 311,
		RightCommand = 309, RightApple = 309, RightWindows = 312, AltGr = 313,
		Help = 315, Pr = 316, SysReq = 317, Break = 318,
		Menu = 319, Mouse0 = 323, Mouse1 = 324, Mouse2 = 325,
		Mouse3 = 326, Mouse4 = 327, Mouse5 = 328, Mouse6 = 329
	};

	enum class e_engine_layers
	{
		terrain = 8388608,
		world = 65536,
		ragdolls = 512,
		construction = 2097152,
		construction_socket = 4194304,
		craters = 1,
		game_trace = 16384,
		trigger = 262144,
		vehicles_detailed = 8192,
		rain_fall = 1101070337,
		deploy = 1235288065,
		default_deploy_volume_check = 537001984,
		build_line_of_sight_check = 2097152,
		projectile_line_of_sight_check = 2162688,
		projectile_line_of_sight_check_terrain = 10551296,
		meele_line_of_sight_check = 2162688,
		eye_line_of_sight_check = 2162688,
		entity_line_of_sight_check = 1218519041,
		player_buildings = 18874624,
		planner_placement = 161546496,
		solid = 1218652417,
		vis_culling = 10551297,
		altitude_check = 1218511105,
		hab_ground_effect = 1218511105,
		ai_line_of_sight = 1218519297,
		dismount_check = 1486946561,
		ai_placement = 278986753,
		wheel_ray = 1235321089,
	};

	enum class event_type : int
	{
		mouse_down = 0,
		mouse_up = 1,
		mouse_drag = 3,
		key_down = 4,
		key_up = 5,
		repaint = 7
	};

	struct rect_t
	{
		float x, y, w, h;
		rect_t( float _x, float _y, float _w, float _h ) : x( _x ), y( _y ), w( _w ), h( _h ) { }
		rect_t( ) : x( 0 ), y( 0 ), w( 0 ), h( 0 ) { }

		auto contains( math::vector_2d_t point ) -> bool
		{
			return point.x >= x && point.x < ( x + w ) && point.y >= y && point.y < ( y + h );
		}
	};

	//inline auto get_key( unity::key_code key ) -> bool
	//{
	//	SAPPHIRE_METHOD( get_key_fn, "UnityEngine::Input.GetKeyInt()", 1, "", -1, bool( * )( unity::key_code ) );
	//	return get_key_fn( key );
	//}

	//inline auto line_of_sight( math::vector_t source, math::vector_t destination ) -> bool
	//{
	//	SAPPHIRE_METHOD( line_of_sight_fn, "GamePhysics.LineOfSight()", -1, "", -1, bool( * )( math::vector_t, math::vector_t, unity::e_engine_layers, float ) );
	//	return line_of_sight_fn( source, destination, unity::e_engine_layers::projectile_line_of_sight_check_terrain, 0 );
	//}

	//inline auto view_matrix( ) -> math::mat4x4_t
	//{
	//	SAPPHIRE_ICALL( get_main_camera_fn, "UnityEngine.Camera::get_main()", std::uintptr_t( * )( ) );

	//	if ( !camera ) {
	//		camera = get_main_camera_fn( );
	//	}

	//	auto camera_ = *reinterpret_cast< std::uintptr_t* >( camera + 0x10 );
	//	if ( !camera_ ) {
	//		return {};
	//	}

	//	auto matrix = *reinterpret_cast< math::mat4x4_t* >( camera_ + 0x2e4 );
	//	if ( !matrix.m ) {
	//		return {};
	//	}

	//	return matrix;
	//}

	//inline auto camera_pos( ) -> math::vector_t
	//{
	//	SAPPHIRE_ICALL( get_main_camera_fn, "UnityEngine.Camera::get_main()", std::uintptr_t( * )( ) );

	//	if ( !camera )
	//		camera = get_main_camera_fn( );

	//	auto camera_ = *reinterpret_cast< std::uintptr_t* >( camera + 0x10 );
	//	if ( !camera_ ) {
	//		return {};
	//	}

	//	auto position = *reinterpret_cast< math::vector_t* >( camera_ + 0x42c );
	//	if ( !position.x || !position.y ) {
	//		return {};
	//	}

	//	return position;
	//}
}