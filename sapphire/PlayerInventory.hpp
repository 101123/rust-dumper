#pragma once

class Item
{
public:
	uint64_t uid( )
	{
		return *( uint64_t* )( ( uint64_t )this + 0x70 );
	}
};

class ItemContainer
{
public:
	List<Item*>* itemList( )
	{
		return *( List<Item*>** )( ( uint64_t )this + 0x38 );
	}
};

class rust::classes::PlayerInventory
{
public:
	ItemContainer* GetBelt( )
	{
		return *( ItemContainer** )( ( uint64_t )this + 0x30 );
	}
};