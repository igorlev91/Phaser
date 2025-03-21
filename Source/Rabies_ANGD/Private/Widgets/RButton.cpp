// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/RButton.h"
#include "Components/Button.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/SizeBoxSlot.h"
#include "Components/SlateWrapperTypes.h"
#include "Components/TextBlock.h"
#include "Engine/Engine.h"

void URButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (RabiesText)
	{
		RabiesText->SetText(ButtonsText);
	}

	if (RabiesButton)
	{
		RabiesCanvasSlot = Cast<UCanvasPanelSlot>(RabiesButton->Slot);

		RabiesButton->OnHovered.AddDynamic(this, &URButton::OnHoverButton);
		RabiesButton->OnUnhovered.AddDynamic(this, &URButton::OnUnhoverButton);

		FButtonStyle rabbiesStyle = RabiesButton->WidgetStyle;
		rabbiesStyle.Normal.TintColor = NormalColor;
		rabbiesStyle.Hovered.TintColor = HoveredColor;
		rabbiesStyle.Pressed.TintColor = PressedColor;

		rabbiesStyle.Normal.OutlineSettings.Color = BorderColor;
		rabbiesStyle.Hovered.OutlineSettings.Color = BorderColor;
		rabbiesStyle.Pressed.OutlineSettings.Color = BorderColor;

		RabiesButton->SetStyle(rabbiesStyle);
		
		OnUnhoverButton();
	}
}

void URButton::OnHoverButton()
{
	if (!RabiesCanvasSlot)
		return;

	RabiesCanvasSlot->SetSize(DefaultScale * HoverScale);

	float rotationRadius = FMath::DegreesToRadians(HoverRotation);
	RabiesButton->RenderTransform.Angle = rotationRadius;
}

void URButton::OnUnhoverButton()
{
	if (!RabiesCanvasSlot)
		return;

	RabiesCanvasSlot->SetSize(DefaultScale);

	float rotationRadius = FMath::DegreesToRadians(DefaultRotation);
	RabiesButton->RenderTransform.Angle = rotationRadius;
}
