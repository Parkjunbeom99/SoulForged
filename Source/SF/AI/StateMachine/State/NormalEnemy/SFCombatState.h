// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NormalEnemy_BaseState.h"
#include "SFCombatState.generated.h"

/**
 * Combat State - 전투 상태 (엘든링 스타일)
 * 타겟을 추적하고 공격하는 상태
 *
 * 전환 조건:
 * - 타겟 상실 + 유예시간 경과 → Return State
 * - 스폰 위치에서 너무 멀어짐 → Return State
 * - 타겟이 너무 멀어짐 → Return State
 */
UCLASS()
class SF_API USFCombatState : public UNormalEnemy_BaseState
{
	GENERATED_BODY()

public:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;

protected:
	/** 타겟 상실 후 유지할 시간 (초) */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Transition")
	float TargetLostGracePeriod = 3.0f;

	/** 타겟 상실 유예 시간 타이머 */
	float TargetLostTimer = 0.0f;

	/** 마지막으로 타겟을 본 시간 */
	float LastSeenTargetTime = 0.0f;

	/** 타겟과의 최대 추적 거리 - 이 거리를 넘으면 포기 */
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Distance")
	float MaxChaseDistance = 3000.0f;

	/** 타겟이 너무 멀어졌는지 체크 */
	bool IsTargetTooFar() const;

	/** 스폰 위치에서 너무 멀어졌는지 체크 (Leash 거리) */
	bool IsTooFarFromHome() const;

	/** Return State로 전환 (타겟 클리어 후 복귀) */
	void TransitionToReturn();
};

