// Fill out your copyright notice in the Description page of Project Settings.

#include "SFReturnState.h"

#include "AI/SFAIGameplayTags.h"
#include "AI/Controller/SFBaseAIController.h"
#include "AI/Controller/SFCombatComponentBase.h"
#include "AI/StateMachine/SFStateMachine.h"

void USFReturnState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	UE_LOG(LogTemp, Log, TEXT("[%s] Return State Entered"), *GetNameSafe(OwnerActor));

	// Return 전용: 타겟 클리어
	if (USFCombatComponentBase* CombatComp = GetCombatComponent())
	{
		CombatComp->ClearTarget();
	}
}

void USFReturnState::OnUpdate_Implementation(float DeltaTime)
{
	Super::OnUpdate_Implementation(DeltaTime);

	// 복귀 중 타겟 재발견 시 다시 Combat으로
	if (HasTarget() && HasReachedHome())
	{
		TransitionToCombat();
		return;
	}

	// 원위치 도착 시 Idle 또는 Patrol로 전환
	if (HasReachedHome())
	{
		if (ShouldPatrol())
		{
			TransitionToPatrol();
		}
		else
		{
			TransitionToIdle();
		}
		return;
	}
}

void USFReturnState::TransitionToIdle()
{
	if (USFStateMachine* SM = GetStateMachine())
	{
		SM->ActivateStateByTag(SFGameplayTags::AI_State_Idle);
	}
}

void USFReturnState::TransitionToPatrol()
{
	if (USFStateMachine* SM = GetStateMachine())
	{
		SM->ActivateStateByTag(SFGameplayTags::AI_State_Patrol);
	}
}

void USFReturnState::TransitionToCombat()
{
	if (USFStateMachine* SM = GetStateMachine())
	{
		SM->ActivateStateByTag(SFGameplayTags::AI_State_Combat);
	}
}

bool USFReturnState::HasReachedHome() const
{
	ASFBaseAIController* AIC = GetAIController();
	if (!AIC)
	{
		return false;
	}

	float DistanceFromHome = AIC->GetDistanceFromHome();
	return DistanceFromHome <= ArrivalThreshold;
}

bool USFReturnState::ShouldPatrol() const
{
	if (USFStateMachine* SM = GetStateMachine())
	{
		return SM->HasStateWithTag(SFGameplayTags::AI_State_Patrol);
	}

	return false;
}

