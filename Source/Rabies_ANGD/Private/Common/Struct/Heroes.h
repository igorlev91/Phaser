// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "HeroeAttribute.h"
#include "../../Character/RLobbyCharacter.h"
#include "Heroes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FHeroes
{
    GENERATED_BODY()

public:
    FHeroes(FString InName, FString InHeroeIcon, TSubclassOf<ARLobbyCharacter> InTargetClass, TArray<struct FHeroeAttribute> InAttributes);
    FHeroes();
    ~FHeroes();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heroe Attributes")
    FString Name;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heroe Attributes")
    TSubclassOf<ARLobbyCharacter> TargetClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heroe Attributes")
    FString Icon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Heroe Attributes")
    TArray<struct FHeroeAttribute> Attributes;
};
