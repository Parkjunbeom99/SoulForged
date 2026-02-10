// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "SFBTTask_LookAround.generated.h"

/**
 * 
 */
UCLASS() 
class SF_API USFBTTask_LookAround : public UBTTaskNode
{
	GENERATED_BODY()

public:
	USFBTTask_LookAround();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	/** 회전할 각도 (-180 ~ 180) */
	UPROPERTY(EditAnywhere, Category = "LookAround", meta=(ClampMin="-180", ClampMax="180"))
	float RotationAngle = 90.0f;

	/** 랜덤 회전 여부 (true면 좌우 랜덤) */
	UPROPERTY(EditAnywhere, Category = "LookAround")
	bool bRandomRotation = true;
};