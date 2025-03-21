// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/IUserObjectListEntry.h"
#include "PlayerEntry.generated.h"

/**
 * 
 */
UCLASS()
class UPlayerEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()
	
protected:
	/** Follows the same pattern as the NativeOn[X] methods in UUserWidget - super calls are expected in order to route the event to BP. */
	virtual void NativeOnListItemObjectSet(UObject* ListItemObject);

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerName;
};
