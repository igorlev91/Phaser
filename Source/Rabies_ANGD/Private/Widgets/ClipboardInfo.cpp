// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ClipboardInfo.h"
#include "Framework/RCharacterDefination.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UClipboardInfo::SetCharacterInfo(URCharacterDefination* Character)
{
	FSlateBrush brush;

	brush.SetResourceObject(Character->CharacterIcon);
	characterIcon->SetBrush(brush);

	brush.SetResourceObject(Character->MeleeIcon);
	meleeIcon->SetBrush(brush);

	brush.SetResourceObject(Character->RangedIcon);
	rangedIcon->SetBrush(brush);

	brush.SetResourceObject(Character->SpecialIcon);
	specialIcon->SetBrush(brush);

	brush.SetResourceObject(Character->UltimateIcon);
	ultimateIcon->SetBrush(brush);

	brush.SetResourceObject(Character->PassiveIcon);
	passiveIcon->SetBrush(brush);

	characterName->SetText(FText::FromName(Character->CharacterName));
	meleeText->SetText(Character->MeleeText);
	rangedText->SetText(Character->RangedText);
	specialText->SetText(Character->SpecialText);
	ultimateText->SetText(Character->UltimateText);
	passiveText->SetText(Character->PassiveText);

}
