// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActiveGameplayEffectHandle.h"
#include "Boss_BaseState.h"
#include "GameplayAbilitySpecHandle.h"
#include "SFBossCombatState.generated.h"

class USFBossPhaseDataAsset;
struct FSFBossPhaseConfig;

/**
 * 부여된 어빌리티 핸들 추적용 구조체
 */
USTRUCT()
struct FGrantedBossAbility
{
	GENERATED_BODY()

	FGameplayAbilitySpecHandle Handle;
	bool bClearOnExit = true;
};

/**
 * Boss Combat State - 보스 전투 상태
 * 
 * Phase를 내부 데이터로 관리하는 단일 Combat State
 * 
 * 주요 기능:
 * - Phase 데이터 기반 BT 교체
 * - Phase별 어빌리티 부여/제거
 * - HP 기반 Phase 자동 전환
 * - Groggy State와 연동 (PushState/PopState)
 * 
 * 사용법:
 * 1. PhaseDataAsset에 Phase 설정
 * 2. 이 State를 보스의 StateMachine에 등록
 * 3. 전투 시작 시 이 State로 전환
 */
UCLASS()
class SF_API USFBossCombatState : public UBoss_BaseState
{
	GENERATED_BODY()

public:
	virtual void OnEnter_Implementation() override;
	virtual void OnExit_Implementation() override;
	virtual void OnUpdate_Implementation(float DeltaTime) override;
	virtual void OnResume_Implementation() override;

	/** 현재 Phase 번호 반환 */
	UFUNCTION(BlueprintPure, Category = "Boss|Phase")
	int32 GetCurrentPhase() const { return CurrentPhase; }

	/** 강제로 특정 Phase로 전환 */
	UFUNCTION(BlueprintCallable, Category = "Boss|Phase")
	void ForceTransitionToPhase(int32 NewPhase);

protected:
	// ===== Phase 관리 =====
	
	/** Phase 전환 조건 체크 */
	void CheckPhaseTransition();

	/** 특정 Phase로 전환 */
	void TransitionToPhase(int32 NewPhase);

	/** 현재 Phase의 BT 시작 */
	void StartCurrentPhaseBehavior();

	// ===== 어빌리티 관리 =====
	
	/** 현재 Phase의 어빌리티 부여 */
	void GivePhaseAbilities(const FSFBossPhaseConfig& PhaseConfig);

	/** 현재 Phase의 어빌리티 제거 */
	void ClearPhaseAbilities();

	/** Phase 전환 시 GameplayEffect 적용 */
	void ApplyPhaseEffect(const FSFBossPhaseConfig& PhaseConfig);

protected:
	/** Phase 데이터 에셋 */
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Phase")
	TObjectPtr<USFBossPhaseDataAsset> PhaseDataAsset;

	/** 현재 Phase 번호 */
	UPROPERTY(VisibleInstanceOnly, Category = "Boss|Phase")
	int32 CurrentPhase = 0;

	//Phase 전환 중인지 여부 
	UPROPERTY(VisibleInstanceOnly, Category = "Boss|Phase")
	bool bIsTransitioning = false;

	/** 부여된 어빌리티 핸들 목록 */
	UPROPERTY()
	TArray<FGrantedBossAbility> GrantedAbilityHandles;

	/** 적용된 Phase Effect 핸들 */
	UPROPERTY()
	FActiveGameplayEffectHandle ActivePhaseEffectHandle;
};

