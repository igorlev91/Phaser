#pragma once

#include "CoreMinimal.h"
#include "Definitions/RPrimaryDataAsset.h"
#include "RMatchDataAsset.generated.h"

UCLASS()
class URMatchDataAsset : public URPrimaryDataAsset
{
	GENERATED_BODY()
public:

	static const URMatchDataAsset& Get();
	static const URMatchDataAsset* GetMatchData();

	UFUNCTION(BlueprintPure, Category = "Match Countdown")
	FORCEINLINE int32 GetMatchSecondsCountdown() const { return InMatchCountdown; }

	UFUNCTION(BlueprintPure, Category = "PreMatch Countdown")
	FORCEINLINE int32 GetPreMatchSecondsCountdown() const { return InPreMatchCountdown; }

	UFUNCTION(BlueprintPure, Category = "MatchTick")
	FORCEINLINE float GetMatchTickInterval() const { return TickInternal; }

	UFUNCTION(BlueprintCallable, Category = "PreMatch Countdown")
	void SetInMatchCountdown(int32 NewInMatchCountdown) { InMatchCountdown = NewInMatchCountdown; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BlueprintProtected, DisplayName = "TimerTick", ShowOnlyInnerProperties))
	float TickInternal = 0.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintProtected, DisplayName = "PreMatch Countdown"))
	int32 InPreMatchCountdown = 3; //second

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (BlueprintProtected, DisplayName = "Match Countdown"))
	int32 InMatchCountdown = 10; //minutes
};
