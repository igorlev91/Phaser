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
	//UE_LOG(LogTemp, Warning, TEXT("Trying to start duration audio effect"))
	if (bPlayingDurationAudioEffect)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Durational Effect Handle is Valid, should not fire again"))
		return;
	}

	if (DurationAudioEffect)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Started duration audio effect"))
		DurationAudioEffectHandle = BP_ApplyGameplayEffectToOwner(DurationAudioEffect);
		bPlayingDurationAudioEffect = true;
	}
}

void UGA_AbilityBase::StopDurationAudioEffect()
{
	//UE_LOG(LogTemp, Warning, TEXT("I SHOULD BE STOPPING!!!"));

	BP_RemoveGameplayEffectFromOwnerWithHandle(DurationAudioEffectHandle);
	bPlayingDurationAudioEffect = false;
	
}
