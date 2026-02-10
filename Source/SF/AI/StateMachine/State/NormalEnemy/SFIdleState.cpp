// Fill out your copyright notice in the Description page of Project Settings.

#include "SFIdleState.h"

#include "AI/SFAIGameplayTags.h"
#include "AI/StateMachine/SFStateMachine.h"

void USFIdleState::OnUpdate_Implementation(float DeltaTime)
{
	Super::OnUpdate_Implementation(DeltaTime);

	// 타겟 발견 시 즉시 Combat으로 전환
	if (HasTarget())
	{
		TransitionToCombat();
		return;
	}
	
	if (ShouldPatrol())
	{
	    TransitionToPatrol();
	    return;
	}
}

void USFIdleState::TransitionToCombat()
{
	if (USFStateMachine* SM = GetStateMachine())
	{
		SM->ActivateStateByTag(SFGameplayTags::AI_State_Combat);
	}
}

void USFIdleState::TransitionToPatrol()
{
	if (USFStateMachine* SM = GetStateMachine())
	{
		SM->ActivateStateByTag(SFGameplayTags::AI_State_Patrol);
	}
}

bool USFIdleState::ShouldPatrol() const
{
	if (USFStateMachine* SM = GetStateMachine())
	{
		return SM->HasStateWithTag(SFGameplayTags::AI_State_Patrol);
	}
	return false;
}