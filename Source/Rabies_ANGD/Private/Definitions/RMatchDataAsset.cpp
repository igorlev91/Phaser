

#include "Definitions/RMatchDataAsset.h"
#include "Definitions/RGameSettings.h"



const URMatchDataAsset& URMatchDataAsset::Get()
{
	const URMatchDataAsset* RMatchDataAsset = URGameSettings::GetMatchDataAsset();
	checkf(RMatchDataAsset, TEXT("XRMatchDataAsset is not valid"));
	return *RMatchDataAsset;
}


const URMatchDataAsset* URMatchDataAsset::GetMatchData()
{
	const URMatchDataAsset* RMatchDataAsset = URGameSettings::GetMatchDataAsset();
	checkf(RMatchDataAsset, TEXT("UXRMatchDataAsset is not valid in UXRFootballSettings!"));
	return RMatchDataAsset;
}