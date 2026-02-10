// SFBTTask_LookAround.cpp
#include "SFBTTask_LookAround.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "BehaviorTree/BlackboardComponent.h"

USFBTTask_LookAround::USFBTTask_LookAround()
{
	NodeName = "Look Around (Normal Enemy)";
	bNotifyTick = true;  
	bNotifyTaskFinished = true;
	
	RotationAngle = 90.0f;
	bRandomRotation = true;
}

EBTNodeResult::Type USFBTTask_LookAround::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn)
	{
		return EBTNodeResult::Failed;
	}
	
	FRotator CurrentRotation = AIController->GetControlRotation();
	
	float YawOffset = RotationAngle;
	if (bRandomRotation)
	{
		float Direction = FMath::RandBool() ? 1.f : -1.f; // 우 or 좌
		float Angle = FMath::RandRange(10.f, RotationAngle); // 최소 각도 보장

		YawOffset = Direction * Angle;
	}

	// 목표 회전 계산
	FRotator TargetRotation = CurrentRotation;
	TargetRotation.Yaw += YawOffset;
	TargetRotation.Normalize();

	// Controller Rotation 설정 
	AIController->SetControlRotation(TargetRotation);
	
	return EBTNodeResult::Succeeded;
}

