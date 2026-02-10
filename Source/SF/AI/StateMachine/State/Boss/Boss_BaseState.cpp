// Fill out your copyright notice in the Description page of Project Settings.

#include "Boss_BaseState.h"
#include "AbilitySystemGlobals.h"
#include "AI/Controller/SFBaseAIController.h"
#include "AI/Controller/SFCombatComponentBase.h"
#include "AI/StateMachine/SFStateMachine.h"
#include "AbilitySystem/SFAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/Enemy/SFPrimarySet_Enemy.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(Boss_BaseState)

void UBoss_BaseState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	// BehaviourTag가 유효하면 BT 교체 요청
	if (StateMachine && BehaviourTag.IsValid())
	{
		if (StateMachine->OnChangeTreeDelegate.IsBound())
		{
			StateMachine->OnChangeTreeDelegate.Broadcast(BehaviourTag);
		}
	}
}

ASFBaseAIController* UBoss_BaseState::GetAIController() const
{
	return Cast<ASFBaseAIController>(OwnerController);
}

USFCombatComponentBase* UBoss_BaseState::GetCombatComponent() const
{
	if (ASFBaseAIController* AIC = GetAIController())
	{
		return AIC->GetCombatComponent();
	}
	return nullptr;
}

USFAbilitySystemComponent* UBoss_BaseState::GetASC() const
{
	if (!OwnerActor)
	{
		return nullptr;
	}
	
	return Cast<USFAbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor));
}

bool UBoss_BaseState::HasTarget() const
{
	if (USFCombatComponentBase* CombatComp = GetCombatComponent())
	{
		return CombatComp->GetCurrentTarget() != nullptr;
	}
	return false;
}

float UBoss_BaseState::GetHealthPercent() const
{
	USFAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return 1.0f;
	}

	float CurrentHP = ASC->GetNumericAttribute(USFPrimarySet_Enemy::GetHealthAttribute());
	float MaxHP = ASC->GetNumericAttribute(USFPrimarySet_Enemy::GetMaxHealthAttribute());

	if (MaxHP <= 0.0f)
	{
		return 1.0f;
	}

	return FMath::Clamp(CurrentHP / MaxHP, 0.0f, 1.0f);
}

