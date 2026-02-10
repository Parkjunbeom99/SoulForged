// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NormalEnemy_BaseState.h"
#include "SFPatrolState.generated.h"

/**
 * Patrol State - 순찰 상태 (엘든링 스타일)
 * 정해진 경로를 순찰하는 상태
 *
 * 전환 조건:
 * - 타겟 발견 → Combat State
 * - 의심스러운 상황 → Alert State
 * - 순찰 완료 후 대기 → Idle State (TODO)
 */
UCLASS()
class SF_API USFPatrolState : public UNormalEnemy_BaseState
{
	GENERATED_BODY()

public:
	virtual void OnUpdate_Implementation(float DeltaTime) override;

protected:
	void TransitionToCombat();
};

