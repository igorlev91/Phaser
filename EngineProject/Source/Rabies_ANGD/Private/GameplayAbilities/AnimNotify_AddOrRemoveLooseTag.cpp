// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/AnimNotify_AddOrRemoveLooseTag.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/RAbilitySystemComponent.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameplayTagsManager.h"

void UAnimNotify_AddOrRemoveLooseTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp->GetOwner()) return;

	UAbilitySystemComponent* OwnerASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner());
	if (!OwnerASC) return;

	if (bAddTag)
	{
		OwnerASC->AddLooseGameplayTag(EventTag);
	}
	else
	{
		OwnerASC->RemoveLooseGameplayTag(EventTag);
	}
}


FString UAnimNotify_AddOrRemoveLooseTag::GetNotifyName_Implementation() const
{
	if (EventTag.IsValid())
	{
		TArray<FName> TagNames;
		UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
		if (TagNames.Num() > 0)
		{
			return TagNames.Last().ToString();
		}
	}

	return FString("None");
}
