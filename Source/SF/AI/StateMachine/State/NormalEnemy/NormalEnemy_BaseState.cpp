// Fill out your copyright notice in the Description page of Project Settings.


#include "NormalEnemy_BaseState.h"
#include "AI/Controller/SFBaseAIController.h"
#include "AI/Controller/SFCombatComponentBase.h"
#include "AI/StateMachine/SFStateMachine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NormalEnemy_BaseState)

void UNormalEnemy_BaseState::OnEnter_Implementation()
{
	Super::OnEnter_Implementation();

	if (StateMachine)
	{
		if (StateMachine->OnChangeTreeDelegate.IsBound())
		{
			if (BehaviourTag.IsValid())
			{
				StateMachine->OnChangeTreeDelegate.Broadcast(BehaviourTag);
			}
		}
	}
}

ASFBaseAIController* UNormalEnemy_BaseState::GetAIController() const
{
	return Cast<ASFBaseAIController>(OwnerController);
}

USFCombatComponentBase* UNormalEnemy_BaseState::GetCombatComponent() const
{
	if (ASFBaseAIController* AIC = GetAIController())
	{
		return AIC->GetCombatComponent();
	}
	return nullptr;
}

bool UNormalEnemy_BaseState::HasTarget() const
{
	if (USFCombatComponentBase* CombatComp = GetCombatComponent())
	{
		return CombatComp->GetCurrentTarget() != nullptr;
	}
	return false;
}
