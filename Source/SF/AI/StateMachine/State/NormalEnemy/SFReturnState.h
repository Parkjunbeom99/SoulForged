// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NormalEnemy_BaseState.h"
#include "SFReturnState.generated.h"

/**
 * Return State - 복귀 상태 (엘든링 스타일)
 * 전투 종료 후 스폰 위치로 복귀하는 상태
 *
 * 전환 조건:
 * - 타겟 재발견 → Combat State
 * - 원위치 도착 → Idle/Patrol State
 */
UCLASS()
class SF_API USFReturnState : public UNormalEnemy_BaseState
{
	GENERATED_BODY()

public:
	virtual void OnEnter_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;

protected:
	void TransitionToIdle();
	void TransitionToPatrol();
	void TransitionToCombat();

	bool HasReachedHome() const;
	bool ShouldPatrol() const;
	
	UPROPERTY(EditDefaultsOnly, Category = "Return")
	float ArrivalThreshold = 150.0f;
};

