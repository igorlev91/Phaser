// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbilities/GA_AbilityBase.h"
#include "GameplayAbilities/RAbilityGenericTags.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "Particles/ParticleSystem.h"
#include "Perception/AISense_Damage.h"

#include "Components/AudioComponent.h"

UGA_AbilityBase::UGA_AbilityBase()
{
	ActivationBlockedTags.AddTag(URAbilityGenericTags::GetDeadTag());
	ActivationBlockedTags.AddTag(URAbilityGenericTags::GetUnActionableTag());
}

void UGA_AbilityBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	StopDurationAudioEffect();
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_AbilityBase::SignalDamageStimuliEvent(FGameplayAbilityTargetDataHandle TargetHandle)
{
	TArray<AActor*> targets = UAbilitySystemBlueprintLibrary::GetAllActorsFromTargetData(TargetHandle);

	for (AActor* target : targets)
	{
		UAISense_Damage::ReportDamageEvent(this, target, GetOwningActorFromActorInfo(), 1, target->GetActorLocation(), target->GetActorLocation());
	}
}

void UGA_AbilityBase::ExecuteSpawnVFXCue(UParticleSystem* VFXToSpawn, float Size, const FVector& Location)
{
	UE_LOG(LogTemp, Warning, TEXT("THIS IS A CODE COMMENT: There is no spawn vfx code yet"));
}

void UGA_AbilityBase::TriggerAudioCue()
{
	FGameplayCueParameters CueParams;
	CueParams.Location = GetAvatarActorFromActorInfo()->GetActorLocation();
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(AudioCueTag, CueParams);
}

void UGA_AbilityBase::StartDurationAudioEffect()
{
	if (!DurationAudioEffectHandle.IsValid() && DurationAudioEffect)
	{
		DurationAudioEffectHandle = BP_ApplyGameplayEffectToOwner(DurationAudioEffect);
	}
}

void UGA_AbilityBase::StopDurationAudioEffect()
{
	if (DurationAudioEffectHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("I SHOULD BE STOPPING!!!"));

		BP_RemoveGameplayEffectFromOwnerWithHandle(DurationAudioEffectHandle);
	}
}
