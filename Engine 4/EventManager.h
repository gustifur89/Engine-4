#pragma once
#include "Header.h"
#include "GameObject.h"

class Event
{
public:
	enum TYPE
	{
		SPAWN,
		UPDATE
	};

	std::string data;
};

class EventManager
{
public:
	std::queue<Event> eventStack;
	
};

