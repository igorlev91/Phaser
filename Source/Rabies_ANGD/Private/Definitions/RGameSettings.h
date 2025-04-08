

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "RGameSettings.generated.h"

 /**
 
  * Is set up in 'Project Settings' -> 'Game' -> 'DefaultGame'.
  */
UCLASS(Config = "Game", DefaultConfig, DisplayName = "URGameSettings")
class  URGameSettings final : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	static const URGameSettings& Get() { return *GetDefault<ThisClass>(); }
	
	UFUNCTION(BlueprintPure, Category = "URGameSettings", meta = (DisplayName = "GetRMatchDataAsset"))
	static const class URMatchDataAsset* GetMatchDataAsset();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Config, meta = (BlueprintProtected, DisplayName = "RMatchDataAsset", ShowOnlyInnerProperties))
	TSoftObjectPtr<const class URMatchDataAsset> MatchDataAssetInternal; 
};
