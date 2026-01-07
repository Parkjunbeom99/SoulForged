#include "SFUIData.h"

#include "System/SFAssetManager.h"

const USFUIData& USFUIData::Get()
{
	return USFAssetManager::Get().GetUIData();
}
