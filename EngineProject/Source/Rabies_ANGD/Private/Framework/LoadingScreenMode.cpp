// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LoadingScreenMode.h"

void ALoadingScreenMode::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Error, TEXT("%s,  Got to begin play"), *GetName());
}
