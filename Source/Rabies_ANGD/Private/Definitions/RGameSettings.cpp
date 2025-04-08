
#include "Definitions/RGameSettings.h"
#include "Definitions/RMatchDataAsset.h"


const URMatchDataAsset* URGameSettings::GetMatchDataAsset()
{
	return URMatchDataAsset::GetOrLoadData<URMatchDataAsset>(Get().MatchDataAssetInternal);
}

