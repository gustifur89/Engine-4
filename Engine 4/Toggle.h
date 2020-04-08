#pragma once
class Toggle
{
private:
	bool internalToggle;
	bool internalState;
public:
	Toggle()
	{
		internalToggle = true;
		internalState = false;
	}

	bool toggle(bool condition)
	{
		if (condition)
		{
			if (internalToggle)
			{
				internalToggle = false;
				internalState = !internalState;
				return true;
			}
		}
		else
		{
			internalToggle = true;
		}
		return false;
	}

	bool getState()
	{
		return internalState;
	}
	
	bool setState(bool state)
	{
		internalState = state;
		return state;
	}

};
