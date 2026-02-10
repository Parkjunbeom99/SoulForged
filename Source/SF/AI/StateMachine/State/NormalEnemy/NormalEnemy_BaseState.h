// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/StateMachine/State/SFState.h"
#include "NormalEnemy_BaseState.generated.h"

class ASFBaseAIController;
class USFCombatComponentBase;


UCLASS()
class SF_API UNormalEnemy_BaseState : public USFState
{
	GENERATED_BODY()

public:
	virtual void OnEnter_Implementation() override;

protected:
	
	ASFBaseAIController* GetAIController() const;

	
	USFCombatComponentBase* GetCombatComponent() const;
	
	bool HasTarget() const;

private:
	/** 이 State가 시작될 때 실행할 BehaviorTree의 태그 */
	UPROPERTY(EditDefaultsOnly, Category = "BehaviorTag", meta=(AllowPrivateAccess = "true"))
	FGameplayTag BehaviourTag;
};
