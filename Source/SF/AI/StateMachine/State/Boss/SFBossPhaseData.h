// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SFBossPhaseData.generated.h"

class USFGameplayAbility;
class UGameplayEffect;


 //Phase에서 부여할 어빌리티 설정
 
USTRUCT(BlueprintType)
struct FSFPhaseAbilityConfig
{
	GENERATED_BODY()

	// 부여할 어빌리티 클래스 
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	TSubclassOf<USFGameplayAbility> AbilityClass;

	// Phase 진입 시 즉시 활성화할지 여부 
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bActivateOnEnter = false;

	//Phase 종료 시 제거할지 여부 
	UPROPERTY(EditDefaultsOnly, Category = "Ability")
	bool bClearOnExit = true;
};

/**
 * Phase 전환 조건
 */
USTRUCT(BlueprintType)
struct FSFPhaseTransitionCondition
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float HealthThreshold = 1.0f;
	
	bool IsMet(float CurrentHealthPercent) const
	{
		return CurrentHealthPercent <= HealthThreshold;
	}
};

USTRUCT(BlueprintType)
struct FSFBossPhaseConfig
{
	GENERATED_BODY()

	// Phase 번호 (1, 2, 3...) 
	UPROPERTY(EditDefaultsOnly, Category = "Phase")
	int32 PhaseNumber = 1;

	// 이 Phase에서 사용할 BehaviorTree 태그 
	UPROPERTY(EditDefaultsOnly, Category = "Phase|BehaviorTree")
	FGameplayTag BehaviourTag;

	// 이 Phase로 전환되는 조건 
	UPROPERTY(EditDefaultsOnly, Category = "Phase|Transition")
	FSFPhaseTransitionCondition TransitionCondition;

	// /** 이 Phase에서 부여할 어빌리티 목록 */
	UPROPERTY(EditDefaultsOnly, Category = "Phase|Abilities")
	TArray<FSFPhaseAbilityConfig> PhaseAbilities;

	/** Phase 전환 시 적용할 GameplayEffect (스탯 변경 등) */
	UPROPERTY(EditDefaultsOnly, Category = "Phase|Effects")
	TSubclassOf<UGameplayEffect> PhaseEffect;

	/** Phase 전환 시 재생할 몽타주 */
	UPROPERTY(EditDefaultsOnly, Category = "Phase|Animation")
	TObjectPtr<UAnimMontage> PhaseTransitionMontage;
};

/**
 * 보스의 전체 Phase 데이터를 담는 DataAsset
 * 
 * 사용법:
 * 1. Content Browser에서 우클릭 → Miscellaneous → Data Asset
 * 2. SFBossPhaseDataAsset 선택
 * 3. Phase 설정 후 BossCombatState에서 참조
 */
UCLASS(BlueprintType)
class SF_API USFBossPhaseDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	/** 모든 Phase 설정 (Phase 1, 2, 3...) */
	UPROPERTY(EditDefaultsOnly, Category = "Boss|Phases")
	TArray<FSFBossPhaseConfig> Phases;

	/** 특정 Phase 번호의 설정 가져오기 */
	const FSFBossPhaseConfig* GetPhaseConfig(int32 PhaseNumber) const
	{
		for (const FSFBossPhaseConfig& Config : Phases)
		{
			if (Config.PhaseNumber == PhaseNumber)
			{
				return &Config;
			}
		}
		return nullptr;
	}

	/** HP 비율에 따라 전환해야 할 Phase 번호 반환 (없으면 -1) */
	int32 GetTargetPhaseForHealth(float HealthPercent, int32 CurrentPhase) const
	{
		// 현재 Phase보다 높은 Phase 중 조건을 만족하는 가장 높은 Phase 찾기
		int32 TargetPhase = -1;
		
		for (const FSFBossPhaseConfig& Config : Phases)
		{
			if (Config.PhaseNumber > CurrentPhase && 
				Config.TransitionCondition.IsMet(HealthPercent))
			{
				if (TargetPhase < 0 || Config.PhaseNumber > TargetPhase)
				{
					TargetPhase = Config.PhaseNumber;
				}
			}
		}
		
		return TargetPhase;
	}

	/** 초기 Phase 번호 (보통 1) */
	int32 GetInitialPhase() const
	{
		if (Phases.Num() > 0)
		{
			int32 MinPhase = Phases[0].PhaseNumber;
			for (const FSFBossPhaseConfig& Config : Phases)
			{
				MinPhase = FMath::Min(MinPhase, Config.PhaseNumber);
			}
			return MinPhase;
		}
		return 1;
	}
};

