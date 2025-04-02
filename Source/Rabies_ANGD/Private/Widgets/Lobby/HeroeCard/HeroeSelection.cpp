// Fill out your copyright notice in the Description page of Project Settings.


#include "HeroeSelection.h"
#include "Components/HorizontalBoxSlot.h"
#include "../../../Common/Library/CandyChaosLoobyBlueprintLibrary.h"
#include "../../../PlayerController/LobbyPlayerController.h"

bool UHeroeSelection::Initialize() {

    if (!Super::Initialize())
        return false;

    this->CandyChaosLobbyGameInstance = UCandyChaosLoobyBlueprintLibrary::GetCandyChaosLobbyGameInstance(this);

    if (BackButton)
        BackButton->OnClicked().AddUObject(this, &UHeroeSelection::OnBackButtonClicked);

    this->FillContainer();

    return true;
}

void UHeroeSelection::OnBackButtonClicked()
{
    RemoveFromParent();

 
}

void UHeroeSelection::FillContainer()
{
    //Fill the container of heroes with the card of each heroes and thier stats
    this->Container->ClearChildren();

    if (IsValid(this->CandyChaosLobbyGameInstance))
    {
        for (FHeroes Heroes : this->CandyChaosLobbyGameInstance->Heroes)
        {
            UHeroeCard* InHeroeItem = CreateWidget<UHeroeCard>(this, this->HeroeCard);

            InHeroeItem->SetPadding(FMargin{ 8,0,8,5 });
            InHeroeItem->SetHeroeName(Heroes.Name);
            InHeroeItem->SetHeroeIcon(Heroes.Icon);
            InHeroeItem->SetAttributeStats(Heroes);

            UHorizontalBoxSlot* HorizontalBoxSlot = this->Container->AddChildToHorizontalBox(InHeroeItem);

            HorizontalBoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Bottom);

            InHeroeItem->SetIsEnabled(this->IsHeroeCardEnabled(Heroes));

            this->SetHeroeCard(InHeroeItem);
        }
    }
}


TArray<UHeroeCard*> UHeroeSelection::GetHeroeCards()
{
    return this->HeroeCards;
}

bool UHeroeSelection::IsHeroeCardEnabled(FHeroes Heroes)
{
    return true;
}

void UHeroeSelection::SetHeroeCard(UHeroeCard* InHeroeCard)
{
    this->HeroeCards.Add(InHeroeCard);
}
