// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NormalEnemy_BaseState.h"
#include "SFIdleState.generated.h"

/**
 * Idle State - 대기 상태 (엘든링 스타일)
 * 스폰 위치에서 가만히 대기하는 상태
 *
 * 전환 조건:
 * - 타겟 발견 → Combat State
 * - 순찰 몬스터인 경우 → Patrol State (TODO)
 */
UCLASS()
class SF_API USFIdleState : public UNormalEnemy_BaseState
{
	GENERATED_BODY()

public:
	virtual void OnUpdate_Implementation(float DeltaTime) override;

protected:
	/** Combat State로 전환 */
	void TransitionToCombat();

	/** Patrol State로 전환 (순찰 몬스터용) */
	void TransitionToPatrol();

	// 순찰 몬스터인지 체크 
	bool ShouldPatrol() const;
};