// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/GameOverUI.h"
#include "Components/Image.h"
#include "TimerManager.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Player/RPlayerBase.h"
#include "Widgets/RButton.h"
#include "Components/CheckBox.h"
#include "Components/Overlay.h"
#include "Framework/RSaveGame.h"
#include "Components/BackgroundBlur.h"


void UGameOverUI::NativeConstruct()
{
    Super::NativeConstruct();

    CurrentBlurStrength = 0.0f;
    TargetBlurStrength = 10.0f; // You can expose this if needed
    BlurLerpSpeed = 1.0f;      // Adjust to control how fast the blur fades in

    if (BackgroundBlur)
    {
        PanelOverlay->SetRenderOpacity(0.0f);
        BackgroundBlur->SetBlurStrength(0.0f);
    }

    if (QuitButton)
    {
        QuitButton->RabiesButton->OnClicked.AddDynamic(this, &UGameOverUI::OnQuitClick);
    }
    if (LeaveButton)
    {
        LeaveButton->RabiesButton->OnClicked.AddDynamic(this, &UGameOverUI::OnLeaveClick);
    }
}

void UGameOverUI::ShowGameOverBlur()
{
    if (BackgroundBlur)
    {
        GetWorld()->GetTimerManager().SetTimer(BlurUpdateTimer, this, &UGameOverUI::UpdateBlurStrength, 0.016f, true);
    }
}

void UGameOverUI::OnQuitClick()
{
    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
    if (PlayerController)
    {
        UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, true);
    }
}

void UGameOverUI::OnLeaveClick()
{
    UWorld* World = GetWorld();

    if (World)
    {
        UGameplayStatics::OpenLevel(World, TEXT("RabiesMainMenu"));
    }
}

void UGameOverUI::UpdateBlurStrength()
{
    CurrentBlurStrength = FMath::FInterpTo(CurrentBlurStrength, TargetBlurStrength, 0.016f, BlurLerpSpeed);
    PanelOverlay->SetRenderOpacity(FMath::FInterpTo(PanelOverlay->GetRenderOpacity(), 1.0f, 0.016f, BlurLerpSpeed));

    BackgroundBlur->SetBlurStrength(CurrentBlurStrength);

    if (FMath::IsNearlyEqual(CurrentBlurStrength, TargetBlurStrength, 0.01f))
    {
        PanelOverlay->SetRenderOpacity(1.0f);
        BackgroundBlur->SetBlurStrength(TargetBlurStrength);
        GetWorld()->GetTimerManager().ClearTimer(BlurUpdateTimer);
    }
}