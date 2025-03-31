// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/ItemPopupUI.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Framework/RItemDataAsset.h"
#include "Components/Image.h"

void UItemPopupUI::SetItem(URItemDataAsset* data)
{

	FSlateBrush brush;
	brush.SetResourceObject(data->ItemIcon);
	itemIcon->SetBrush(brush);

	itemDescription->SetText(FText::FromString(data->ItemDescription));
	itemTooltip->SetText(FText::FromString(data->ItemTooltip));

	TickOpacity(true);
	GetWorld()->GetTimerManager().SetTimer(HideTimerHandle, this, &UItemPopupUI::SetHiddenState, 5.0f, false);

}

void UItemPopupUI::TickOpacity(bool bShow)
{
	opacity = FMath::Lerp(opacity, (bShow) ? 1: 0, 3 * GetWorld()->GetDeltaSeconds());

	if (opacity >= 0.9f && bShow)
	{
		SetRenderOpacity(1);
		return;
	}
	if (opacity <= 0.1f && !bShow)
	{
		SetRenderOpacity(0);
		return;
	}
	SetRenderOpacity(opacity);

	TickTimerHandle = GetWorld()->GetTimerManager().SetTimerForNextTick(FTimerDelegate::CreateUObject(this, &UItemPopupUI::TickOpacity, bShow));
}

void UItemPopupUI::SetHiddenState()
{
	TickOpacity(false);
}

void UItemPopupUI::NativeConstruct()
{
	Super::NativeConstruct();

	SetRenderOpacity(0);
}
