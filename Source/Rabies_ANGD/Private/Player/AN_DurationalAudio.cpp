// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AN_DurationalAudio.h"
#include "AbilitySystemBlueprintLibrary.h"

void UAN_DurationalAudio::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp || !MeshComp->GetOwner() || !UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner()))
		return;

	FGameplayTag EventTag;
	if (bShouldPlay)
	{

	}
	else
	{
	
	}

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
}
