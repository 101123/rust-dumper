#pragma once

#include <vector>

struct hook_t {
	hook_t( void* target, void* hook, void** original ) :
		_target( target ), _original( original ), _hook( hook ) {};

	bool create();
	bool remove();
	bool enable();
	bool disable();

	void* _target;
	void* _hook;
	void** _original;
};

namespace hook_manager {
	bool init();
	bool uninit();
}