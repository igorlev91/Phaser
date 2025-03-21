// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/PauseUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RPlayerBase.h"

void UPauseUI::NativeConstruct()
{
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UPauseUI::OnQuitClick);
	}
	if (ReturnButton)
	{
		ReturnButton->OnClicked.AddDynamic(this, &UPauseUI::OnReturnClick);
	}
	if (MenuButton)
	{
		MenuButton->OnClicked.AddDynamic(this, &UPauseUI::OnMenuClick);
	}

}

void UPauseUI::OnQuitClick()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
	}
}

void UPauseUI::OnReturnClick()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = false;
	ARPlayerBase* CurrentPlayer = Cast<ARPlayerBase>(UGameplayStatics::GetPlayerCharacter(this, 0));
	CurrentPlayer->SetPausetoFalse();
	RemoveFromParent();
}

void UPauseUI::OnMenuClick()
{
	UWorld* World = GetWorld();

	if (World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("CharacterSelect"));
	}
}
