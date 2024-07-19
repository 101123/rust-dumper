#pragma once

namespace rust {
	class item_container {
	public:
		enum e_item_container_flag {
			is_player = 1,
			clothing = 2,
			belt = 4,
			single_type = 8,
			is_locked = 16,
			show_slots_on_icon = 32,
			no_broken_items = 64,
			no_item_input = 128,
			contents_hidden = 256
		};
	};
}