// Fill out your copyright notice in the Description page of Project Settings.


#include "CandyChaosLoobyBlueprintLibrary.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"




UCandyChaosLoobyBlueprintLibrary::UCandyChaosLoobyBlueprintLibrary(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{ }

void UCandyChaosLoobyBlueprintLibrary::ShowLoadingScreen(const UObject* WorldContextObject, bool bPlayUntilStopped, float PlayTime)
{
   
}

void UCandyChaosLoobyBlueprintLibrary::HideLoadingScreen(const UObject* WorldContextObject)
{
   
}

UCandyChaosLobbyGameInstance* UCandyChaosLoobyBlueprintLibrary::GetCandyChaosLobbyGameInstance(const UObject* WorldContextObject)
{
    UCandyChaosLobbyGameInstance* CandyChaosLobbyGameInstance = nullptr;

    if (!WorldContextObject)    
        return nullptr;    

    UWorld* World = WorldContextObject->GetWorld();

    if (!World)    
        return nullptr;
    
    if (IsValid(World->GetGameInstance()))
        CandyChaosLobbyGameInstance = Cast<UCandyChaosLobbyGameInstance>(World->GetGameInstance());

    return CandyChaosLobbyGameInstance;
}

UUserWidget* UCandyChaosLoobyBlueprintLibrary::CreateAndShowWidget(UObject* WorldContextObject, TSubclassOf<UUserWidget> WidgetClass, bool bShowMouseCursor, bool bOnlyCreate)
{
    if (!WorldContextObject || !WidgetClass)
        return nullptr;    

    UWorld* World = WorldContextObject->GetWorld();

    if (!World)
        return nullptr;    

    APlayerController* PlayerController = World->GetFirstPlayerController();

    if (!PlayerController)
        return nullptr;    

    UUserWidget* Widget = CreateWidget<UUserWidget>(PlayerController, WidgetClass);

    if (!Widget)
        return nullptr;    

    Widget->AddToViewport();

    if (!bOnlyCreate)
    {
        FInputModeUIOnly InputModeData;
        InputModeData.SetWidgetToFocus(Widget->TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

        PlayerController->SetInputMode(InputModeData);
        PlayerController->bShowMouseCursor = bShowMouseCursor;
    }

    return Widget;
}

bool UCandyChaosLoobyBlueprintLibrary::IsServer(UObject* WorldContextObject)
{
    UWorld* World = WorldContextObject->GetWorld();

    return true;
}

ALobbyGameMode* UCandyChaosLoobyBlueprintLibrary::GetLobbyGameMode(const UObject* WorldContextObject)
{
    ALobbyGameMode* LobbyGameMode = nullptr;

    if (!WorldContextObject)
        return nullptr;

    UWorld* World = WorldContextObject->GetWorld();

    if (!World)
        return nullptr;

    AGameModeBase* GameMode = UGameplayStatics::GetGameMode(World);

    if(IsValid(GameMode))
        LobbyGameMode = Cast<ALobbyGameMode>(GameMode);

    return LobbyGameMode;
}

AActor* UCandyChaosLoobyBlueprintLibrary::GetActorByName(const UObject* WorldContextObject, FString InActorName) {

    if (!WorldContextObject)
        return nullptr;

    TArray<AActor*> ActorsInScene{};

    UGameplayStatics::GetAllActorsOfClass(WorldContextObject->GetWorld(), AActor::StaticClass(), ActorsInScene);

    if (ActorsInScene.Num() > 0)
    {
        for (AActor* actor : ActorsInScene)
        {
            if (actor == nullptr)
                return nullptr;

            if (actor->GetActorNameOrLabel() == InActorName)
                return actor;
        }
    }

    return nullptr;
}
