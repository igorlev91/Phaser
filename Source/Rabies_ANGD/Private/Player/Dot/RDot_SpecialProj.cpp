// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Dot/RDot_SpecialProj.h"
#include "GameplayAbilities/GA_AbilityBase.h"
#include "Character/RCharacterBase.h"
#include "Enemy/REnemyBase.h"

void ARDot_SpecialProj::HitCharacter(ARCharacterBase* hitCharacter, bool isEnemy, int hitNumber)
{
	if (isEnemy == false)
		return;

	
}

void ARDot_SpecialProj::Init(UGameplayEffect effectToApply)
{
	OnHitCharacter.AddUObject(this, &ARDot_SpecialProj::HitCharacter);
}
