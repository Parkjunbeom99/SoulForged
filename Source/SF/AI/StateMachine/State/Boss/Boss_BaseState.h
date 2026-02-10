// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/StateMachine/State/SFState.h"
#include "Boss_BaseState.generated.h"

class ASFBaseAIController;
class USFCombatComponentBase;
class USFAbilitySystemComponent;

/**
 * Boss 전용 Base State
 * 모든 Boss State가 상속받는 기본 클래스
 * 
 * 주요 기능:
 * - BehaviourTag 기반 BT 자동 교체
 * - AIController, CombatComponent, ASC 접근 헬퍼
 * - HP 비율 체크 헬퍼
 */
UCLASS(Abstract)
class SF_API UBoss_BaseState : public USFState
{
	GENERATED_BODY()

public:
	virtual void OnEnter_Implementation() override;

protected:
	// ===== Helper Functions =====
	
	/** AIController 캐스팅 헬퍼 */
	ASFBaseAIController* GetAIController() const;

	/** CombatComponent 접근 헬퍼 */
	USFCombatComponentBase* GetCombatComponent() const;

	/** AbilitySystemComponent 접근 헬퍼 */
	USFAbilitySystemComponent* GetASC() const;

	/** 타겟 유무 체크 */
	bool HasTarget() const;

	/** 현재 HP 비율 (0.0 ~ 1.0) */
	float GetHealthPercent() const;

protected:
	/** 이 State가 시작될 때 실행할 BehaviorTree의 태그 */
	UPROPERTY(EditDefaultsOnly, Category = "Boss|BehaviorTree")
	FGameplayTag BehaviourTag;
};

