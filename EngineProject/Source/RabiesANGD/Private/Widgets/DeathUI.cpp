// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/DeathUI.h"
#include "DeathUI.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UDeathUI::NativeConstruct()
{
	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UDeathUI::OnQuitClick);
	}
	if (MenuButton)
	{
		MenuButton->OnClicked.AddDynamic(this, &UDeathUI::OnMenuClick);
	}
}

void UDeathUI::OnQuitClick()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
	}
}

void UDeathUI::OnMenuClick()
{
	UWorld* World = GetWorld();

	if (World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("CharacterSelect"));
	}
}
