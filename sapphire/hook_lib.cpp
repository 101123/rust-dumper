#include "hook_lib.hpp"
#include "minhook/minhook.h"

bool hook_t::create() {
	return MH_CreateHook( this->_target, this->_hook, this->_original ) == MH_OK;
}

bool hook_t::remove() {
	return MH_RemoveHook( this->_target ) == MH_OK;
}

bool hook_t::enable() {
	return MH_EnableHook( this->_target ) == MH_OK;
}

bool hook_t::disable() {
	return MH_DisableHook( this->_target ) == MH_OK;
}

bool hook_manager::init() {
	return MH_Initialize() == MH_OK;
}

bool hook_manager::uninit() {
	return MH_Uninitialize() == MH_OK;
}

