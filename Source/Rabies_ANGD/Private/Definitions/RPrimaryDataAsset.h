

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "RPrimaryDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class URPrimaryDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const URPrimaryDataAsset* GetOrLoadData(const TSoftObjectPtr<const URPrimaryDataAsset>& DataAssetPtr);


	template <typename T>
	static FORCEINLINE const T* GetOrLoadData(const TSoftObjectPtr<const T>& DataAssetPtr) { return Cast<T>(GetOrLoadData(TSoftObjectPtr<const URPrimaryDataAsset>(DataAssetPtr))); }
};
