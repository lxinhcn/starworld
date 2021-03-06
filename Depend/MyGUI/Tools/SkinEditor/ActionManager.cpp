/*!
	@file
	@author		Albert Semenov
	@date		08/2010
*/
#include "precompiled.h"
#include "ActionManager.h"

template <> tools::ActionManager* MyGUI::Singleton<tools::ActionManager>::msInstance = nullptr;
template <> const char* MyGUI::Singleton<tools::ActionManager>::mClassTypeName("ActionManager");

namespace tools
{

	ActionManager::ActionManager() :
		mChanges(false)
	{
	}

	ActionManager::~ActionManager()
	{
	}

	void ActionManager::initialise()
	{
	}

	void ActionManager::shutdown()
	{
	}

	bool ActionManager::getChanges()
	{
		return mChanges;
	}

	void ActionManager::setChanges(bool _value)
	{
		if (mChanges != _value)
		{
			mChanges = _value;
			eventChanges(mChanges);
		}
	}

} // namespace tools
