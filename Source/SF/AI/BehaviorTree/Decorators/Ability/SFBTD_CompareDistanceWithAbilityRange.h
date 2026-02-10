// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/Blackboard/BlackboardKeyEnums.h"
#include "SFBTD_CompareDistanceWithAbilityRange.generated.h"

/**
 * 데코레이터 내부에 상태를 저장하기 위한 메모리 구조체
 */
USTRUCT()
struct FBTDistanceCompareMemory
{
	GENERATED_BODY()

	bool bLastResult = false;
};

UCLASS()
class SF_API USFBTD_CompareDistanceWithAbilityRange : public UBTDecorator
{
	GENERATED_BODY()

public:
	USFBTD_CompareDistanceWithAbilityRange();

	UPROPERTY(EditAnywhere, Category = "Ability")
	FBlackboardKeySelector AbilityTagKey;


protected:
	virtual uint16 GetInstanceMemorySize() const override;
	virtual void InitializeMemory(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTMemoryInit::Type InitType) const override;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	bool CheckCondition(UBehaviorTreeComponent& OwnerComp) const;
	void GetAbilityAttackRange(UBehaviorTreeComponent& OwnerComp, const FGameplayTag& AbilityTag, float& OutMinRange,
	                           float& OutMaxRange) const;
};