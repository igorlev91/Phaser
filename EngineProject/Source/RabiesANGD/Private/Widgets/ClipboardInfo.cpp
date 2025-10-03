// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ClipboardInfo.h"
#include "Framework/RCharacterDefination.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Widgets/SelectAbilityIcon.h"
#include "IMediaModule.h"
#include "FileMediaSource.h"
#include "MediaPlayer.h"
#include "Components/Image.h"
#include "Framework/RSaveGame.h"
#include "Kismet/GameplayStatics.h"

void UClipboardInfo::SetCharacterInfo(URCharacterDefination* Character)
{
	FSlateBrush brush;
	brush.SetResourceObject(Character->CharacterIcon);
	characterIcon->SetBrush(brush);

	meleeIcon->SetIcon(Character->MeleeIcon, Character->MeleeMediaSource);
	rangedIcon->SetIcon(Character->RangedIcon, Character->RangedMediaSource);
	specialIcon->SetIcon(Character->SpecialIcon, Character->SpecialMediaSource);
	ultimateIcon->SetIcon(Character->UltimateIcon, Character->UltimateMediaSource);
	passiveIcon->SetIcon(Character->PassiveIcon, Character->PassiveMediaSource);

	characterName->SetText(FText::FromName(Character->CharacterName));
	/*meleeText->SetText(Character->MeleeText);
	rangedText->SetText(Character->RangedText);
	specialText->SetText(Character->SpecialText);
	ultimateText->SetText(Character->UltimateText);*/
	passiveText->SetText(Character->PassiveText);

	bool bHideIt = false;
	ShowStar(false);

	USaveGame* baseSave = UGameplayStatics::LoadGameFromSlot(TEXT("RabiesSaveData"), 0);
	if (baseSave)
	{
		URSaveGame* LoadedGame = Cast<URSaveGame>(baseSave);
		if (LoadedGame)
		{
			bHideIt = !LoadedGame->bItemSuggestions;

			if (Character->CharacterName == "Chester")
			{
				ShowStar(LoadedGame->bWonAsChester);
			}
			else if (Character->CharacterName == "Toni")
			{
				ShowStar(LoadedGame->bWonAsToni);
			}
			else if (Character->CharacterName == "Tex")
			{
				ShowStar(LoadedGame->bWonAsTex);
			}
			else if (Character->CharacterName == "Dot")
			{
				ShowStar(LoadedGame->bWonAsDot);
			}
			else if (Character->CharacterName == "BoltHead")
			{
				ShowStar(LoadedGame->bWonAsBoltHead);
			}
		}
	}

	if (bHideIt)
	{
		BestItemsText->SetText(FText::FromString(""));
	}
	else
	{
		BestItemsText->SetText(FText::FromString("Best Items"));
	}


	SetBestItem(BigStickImage, Character->BigStick, bHideIt);

	SetBestItem(SpareBatteriesImage, Character->SpareBatteries, bHideIt);

	SetBestItem(AmmoBoxImage, Character->AmmoBox, bHideIt);

	SetBestItem(LaserSightImage, Character->LaserSight, bHideIt);

	SetBestItem(OldStopwatchImage, Character->OldStopwatch, bHideIt);

	SetBestItem(SpeedPillsImage, Character->SpeedPills, bHideIt);

	SetBestItem(EnergyCellImage, Character->Energycell, bHideIt);

	SetBestItem(ElectronicChargerImage, Character->ElectronicCharger, bHideIt);

	SetBestItem(CannedFoodImage, Character->CannedFood, bHideIt);

	SetBestItem(IVBagImage, Character->IVBag, bHideIt);

	SetBestItem(HardHatImage, Character->HardHat, bHideIt);

	SetBestItem(UsedArmorImage, Character->UsedArmor, bHideIt);

	SetBestItem(AntidoteImage, Character->Antidote, bHideIt);

	SetBestItem(SprayBottleImage, Character->SprayBottle, bHideIt);

	SetBestItem(BloodBagImage, Character->BloodBag, bHideIt);

	SetBestItem(FriendshipBraceletImage, Character->Friendshipbracelet, bHideIt);

	SetBestItem(TaserImage, Character->Taser, bHideIt);

	SetBestItem(BagOfNailsImage, Character->BagofNails, bHideIt);

	SetBestItem(RadioImage, Character->Radio, bHideIt);

	SetBestItem(FuelAndCoolantImage, Character->FuelAndCoolant, bHideIt);

	SetBestItem(HeaterImage, Character->Heater, bHideIt);

	SetBestItem(RodOfUraniumImage, Character->RodofUranium, bHideIt);
}

void UClipboardInfo::PlayMoive(int index)
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Hit Chest: %f"), (float)index));

	switch (index)
	{
	case 0:
		meleeIcon->PlayMoive();
		break;

	case 1:
		rangedIcon->PlayMoive();
		break;

	case 2:
		specialIcon->PlayMoive();
		break;

	case 3:
		ultimateIcon->PlayMoive();
		break;

	case 4:
		passiveIcon->PlayMoive();
		break;

	default:
		break;
	}
}

void UClipboardInfo::SetBestItem(UImage* iconToChange, bool isBest, bool hideIt)
{
	if (iconToChange == nullptr)
		return;

	float alpha = (isBest) ? 1 : 0.725f;
	if (hideIt)
		alpha = 0.0f;

	FSlateBrush brush;
	brush = iconToChange->GetBrush();
	brush.SetResourceObject(brush.GetResourceObject());
	brush.ImageSize = 70.0f;
	brush.TintColor = (isBest) ? FSlateColor(FLinearColor(1,1,1,alpha)) : FSlateColor(FLinearColor(0.088542f, 0.088542f, 0.088542f, alpha));
	iconToChange->SetBrush(brush);
}

void UClipboardInfo::ShowStar(bool hasStar)
{
	UE_LOG(LogTemp, Log, TEXT("b has star: %s"), hasStar ? TEXT("true") : TEXT("false"));
	StarImage->SetRenderOpacity((hasStar) ? 1.0f : 0.0f );
}
