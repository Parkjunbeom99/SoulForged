#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "SFCommonUpgradeFragment.generated.h"

class UGameplayEffect;

/**
 * 업그레이드의 개별 효과를 정의하는 기본 Fragment 클래스
 * DefaultToInstanced, EditInlineNew: 에디터 내 인스턴스 생성 지원
 */
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class SF_API USFCommonUpgradeFragment : public UObject
{
	GENERATED_BODY()
};

/**
 * [Fragment] 스탯 강화
 * GAS의 GameplayEffect와 SetByCaller를 사용하여 스탯을 동적으로 상승 시킴
 */
UCLASS(DisplayName = "Fragment: Stat Boost")
class SF_API USFCommonUpgradeFragment_StatBoost : public USFCommonUpgradeFragment
{
	GENERATED_BODY()

public:
	// 적용할 GameplayEffect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	TSubclassOf<UGameplayEffect> EffectClass;

	// SetByCaller로 넘겨줄 태그 (예: Data.Stat.AttackPower)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GAS")
	FGameplayTag AttributeTag;

	// 기본 적용 수치 (희귀도 배율 적용 전)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float BaseMagnitude = 10.0f;
};

/**
 * [Fragment] 스킬 레벨업
 * 특정 카테고리의 스킬 레벨을 올려줌
 */
UCLASS(DisplayName = "Fragment: Skill Level")
class SF_API USFCommonUpgradeFragment_SkillLevel : public USFCommonUpgradeFragment
{
	GENERATED_BODY()

public:
	// 레벨을 올릴 대상 스킬 태그 (예: InputTag.Primary)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	FGameplayTag TargetSkillInputTag;

	// 증가시킬 레벨 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
	int32 LevelIncrement = 1;
};