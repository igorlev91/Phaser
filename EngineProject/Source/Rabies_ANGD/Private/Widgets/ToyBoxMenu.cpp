// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ToyBoxMenu.h"

#include "Kismet/GameplayStatics.h"
#include "Widgets/RButton.h"

#include "Engine/World.h"

#include "Player/RMainMenuController.h"

#include "Framework/EOSGameInstance.h"
#include "Components/EditableText.h"
#include "Components/ScrollBox.h"
#include "Components/Button.h"
#include "Widgets/OpenLobby.h"
#include "Framework/EOSActionGameState.h"
#include "Components/TextBlock.h"
#include "Actors/ItemPickup.h"

#include "Framework/RItemDataAsset.h"

#include "Sound/SoundCue.h"

void UToyBoxMenu::NativeConstruct()
{
    Super::NativeConstruct();

    if (Item1)
    {
        Item1->OnToyBoxClicked.AddDynamic(this, &UToyBoxMenu::ClickOption);
        Item1->OnItemHovered.AddDynamic(this, &UToyBoxMenu::HoveredOption);
    }
    if (Item2)
    {
        Item2->OnToyBoxClicked.AddDynamic(this, &UToyBoxMenu::ClickOption);
        Item2->OnItemHovered.AddDynamic(this, &UToyBoxMenu::HoveredOption);
    }
    if (Item3)
    {
        Item3->OnToyBoxClicked.AddDynamic(this, &UToyBoxMenu::ClickOption);
        Item3->OnItemHovered.AddDynamic(this, &UToyBoxMenu::HoveredOption);
    }
    if (Item4)
    {
        Item4->OnToyBoxClicked.AddDynamic(this, &UToyBoxMenu::ClickOption);
        Item4->OnItemHovered.AddDynamic(this, &UToyBoxMenu::HoveredOption);
    }
    if (Item5)
    {
        Item5->OnToyBoxClicked.AddDynamic(this, &UToyBoxMenu::ClickOption);
        Item5->OnItemHovered.AddDynamic(this, &UToyBoxMenu::HoveredOption);
    }
    if (Item6)
    {
        Item6->OnToyBoxClicked.AddDynamic(this, &UToyBoxMenu::ClickOption);
        Item6->OnItemHovered.AddDynamic(this, &UToyBoxMenu::HoveredOption);
    }
    if (Item7)
    {
        Item7->OnToyBoxClicked.AddDynamic(this, &UToyBoxMenu::ClickOption);
        Item7->OnItemHovered.AddDynamic(this, &UToyBoxMenu::HoveredOption);
    }
    if (Item8)
    {
        Item8->OnToyBoxClicked.AddDynamic(this, &UToyBoxMenu::ClickOption);
        Item8->OnItemHovered.AddDynamic(this, &UToyBoxMenu::HoveredOption);
    }
}

void UToyBoxMenu::SetUpItemButtons(AItemPickup* itemToChange, FString itemSelection)
{
    ChoosingItem = itemToChange;
    itemTooltip->SetText(FText::FromString("Choose an item"));
    itemName->SetText(FText::FromString(""));

    if (itemSelection == "CommonToyBox")
    {
        SetButtonItem(Item1, CommonLibrary[0]);
        SetButtonItem(Item2, CommonLibrary[1]);
        SetButtonItem(Item3, CommonLibrary[2]);
        SetButtonItem(Item4, CommonLibrary[3]);
        SetButtonItem(Item5, CommonLibrary[4]);
        SetButtonItem(Item6, CommonLibrary[5]);
        SetButtonItem(Item7, CommonLibrary[6]);
        SetButtonItem(Item8, CommonLibrary[7]);
    }
    else if (itemSelection == "UncommonToyBox")
    {
        SetButtonItem(Item1, UncommonLibrary[0]);
        SetButtonItem(Item2, UncommonLibrary[1]);
        SetButtonItem(Item3, UncommonLibrary[2]);
        SetButtonItem(Item4, UncommonLibrary[3]);
        SetButtonItem(Item5, UncommonLibrary[4]);
        SetButtonItem(Item6, UncommonLibrary[5]);
        SetButtonItem(Item7, UncommonLibrary[6]);
        SetButtonItem(Item8, UncommonLibrary[7]);
    }
    else if (itemSelection == "RareToyBox")
    {
        SetButtonItem(Item1, RareLibrary[0]);
        SetButtonItem(Item2, RareLibrary[1]);
        SetButtonItem(Item3, RareLibrary[2]);
        SetButtonItem(Item4, RareLibrary[3]);
        SetButtonItem(Item5, nullptr);
        SetButtonItem(Item6, RareLibrary[4]);
        SetButtonItem(Item7, RareLibrary[5]);
        SetButtonItem(Item8, nullptr);
    }
}

void UToyBoxMenu::SetButtonItem(URButton* buttonEditting, URItemDataAsset* item)
{
    if (buttonEditting == nullptr)
        return;

    if (item == nullptr)
    {
        FSlateBrush InvisBrush;
        InvisBrush.TintColor = FSlateColor(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));

        FButtonStyle NewInvisStyle = buttonEditting->RabiesButton->GetStyle();
        NewInvisStyle.Pressed = InvisBrush;
        NewInvisStyle.Hovered = InvisBrush;
        NewInvisStyle.Normal = InvisBrush;
        buttonEditting->RabiesButton->SetStyle(NewInvisStyle);
        return;
    }

    buttonEditting->GivenItem = item;

    FSlateBrush Brush;
    Brush.SetResourceObject(item->ItemIcon);
    Brush.DrawAs = ESlateBrushDrawType::Image;
    //Brush.ImageSize = FVector2D(64, 64); // Set desired size

    FButtonStyle NewStyle = buttonEditting->RabiesButton->GetStyle();
    NewStyle.Pressed = Brush;
    NewStyle.Hovered = Brush;

    Brush.TintColor = FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f, 0.7f)); // Slightly gray with full opacity

    NewStyle.Normal = Brush;

    buttonEditting->RabiesButton->SetStyle(NewStyle);
}

void UToyBoxMenu::ClickOption(URItemDataAsset* chosenItem)
{
    if (chosenItem && ChoosingItem)
    {
        UGameplayStatics::PlaySound2D(this, ClickAudio);
        OnDecision.Broadcast(ChoosingItem, chosenItem);

        /*AEOSActionGameState* gameState = GetWorld()->GetGameState<AEOSActionGameState>();
        if (gameState)
        {
            OnDecision.Broadcast(ChoosingItem, chosenItem);
            gameState->Server_RequestChangeItem(ChoosingItem, chosenItem);
        }*/
        //ChoosingItem->SetupItem(chosenItem, FVector(0,0,0));
    }
}

void UToyBoxMenu::HoveredOption(URItemDataAsset* chosenItem)
{
    if (chosenItem == nullptr)
        return;

    UGameplayStatics::PlaySound2D(this, HoverAudio);

    itemTooltip->SetText(FText::FromString(chosenItem->ItemTooltip));

    itemName->SetText(FText::FromName(chosenItem->ItemName));
}
