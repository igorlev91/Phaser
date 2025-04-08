


#include "Definitions/RPrimaryDataAsset.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RPrimaryDataAsset)


const URPrimaryDataAsset* URPrimaryDataAsset::GetOrLoadData(const TSoftObjectPtr<const URPrimaryDataAsset>& DataAssetPtr)
{
	if (!ensureMsgf(!DataAssetPtr.IsNull(), TEXT("ASSERT: [%i] %hs:\n'DataAssetPtr' doesnt set up in editor!"), __LINE__, __FUNCTION__))
	{
		return nullptr;
	}

	if (const UObject* Asset = DataAssetPtr.Get())
	{
		return Cast<URPrimaryDataAsset>(Asset);
	}

	URPrimaryDataAsset* LoadedDataAsset = Cast<URPrimaryDataAsset>(DataAssetPtr.ToSoftObjectPath().TryLoad());
	if (!ensureMsgf(LoadedDataAsset, TEXT("ASSERT: [%i] %hs:\n'%s' failed to load!"), __LINE__, __FUNCTION__, *DataAssetPtr.ToString()))
	{
		return nullptr;
	}

	LoadedDataAsset->AddToRoot();

	return LoadedDataAsset;
}