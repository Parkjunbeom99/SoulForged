// Fill out your copyright notice in the Description page of Project Settings.

#include "SFPatrolState.h"

#include "AI/SFAIGameplayTags.h"
#include "AI/StateMachine/SFStateMachine.h"

void USFPatrolState::OnUpdate_Implementation(float DeltaTime)
{
	Super::OnUpdate_Implementation(DeltaTime);

	// 타겟 발견 시 즉시 Combat으로 전환
	if (HasTarget())
	{
		TransitionToCombat();
		return;
	}
}

void USFPatrolState::TransitionToCombat()
{
	if (USFStateMachine* SM = GetStateMachine())
	{
		SM->ActivateStateByTag(SFGameplayTags::AI_State_Combat);
	}
}



