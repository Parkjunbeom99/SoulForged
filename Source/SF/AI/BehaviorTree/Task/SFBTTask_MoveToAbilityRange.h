// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "SFBTTask_MoveToAbilityRange.generated.h"

UCLASS()
class SF_API USFBTTask_MoveToAbilityRange : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	USFBTTask_MoveToAbilityRange();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;

protected:
	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MinRangeKey;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector MaxRangeKey;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RangeMultiplier = 0.9f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float MaxDuration = 5.0f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AcceptanceRadius = 50.0f;

private:
	TWeakObjectPtr<UBehaviorTreeComponent> CachedOwnerComp;
	TWeakObjectPtr<AActor> CachedTarget;
	float CachedMaxRange = 0.0f;
	float CachedMinRange = 0.0f;
	float ElapsedTime = 0.0f;
	bool bIsMoving = false;
};
