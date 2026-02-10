// Fill out your copyright notice in the Description page of Project Settings.

#include "SFBTTask_MoveToAbilityRange.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Controller/SFCombatComponentBase.h"
#include "Interface/SFAIControllerInterface.h"

USFBTTask_MoveToAbilityRange::USFBTTask_MoveToAbilityRange()
{
	NodeName = "Move To Ability Range";
	bNotifyTick = true;
	bCreateNodeInstance = true;

	MinRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(USFBTTask_MoveToAbilityRange, MinRangeKey));
	MaxRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(USFBTTask_MoveToAbilityRange, MaxRangeKey));
}

EBTNodeResult::Type USFBTTask_MoveToAbilityRange::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	ISFAIControllerInterface* AI = Cast<ISFAIControllerInterface>(AIController);
	if (!AI) return EBTNodeResult::Failed;

	USFCombatComponentBase* Combat = AI->GetCombatComponent();
	if (!Combat) return EBTNodeResult::Failed;

	AActor* Target = Combat->GetCurrentTarget();
	if (!Target) return EBTNodeResult::Failed;

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	CachedMinRange = BB->GetValueAsFloat(MinRangeKey.SelectedKeyName);
	CachedMaxRange = BB->GetValueAsFloat(MaxRangeKey.SelectedKeyName);

	if (CachedMaxRange <= 0.0f)
	{
		CachedMaxRange = 200.0f;
	}

	float CurrentDistance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());

	if (CurrentDistance <= CachedMaxRange * RangeMultiplier && CurrentDistance >= CachedMinRange)
	{
		return EBTNodeResult::Succeeded;
	}

	CachedOwnerComp = &OwnerComp;
	CachedTarget = Target;
	ElapsedTime = 0.0f;
	bIsMoving = true;

	float TargetRange = CachedMaxRange * RangeMultiplier;
	AIController->MoveToActor(Target, TargetRange - AcceptanceRadius);

	return EBTNodeResult::InProgress;
}

void USFBTTask_MoveToAbilityRange::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	if (!bIsMoving) return;

	ElapsedTime += DeltaSeconds;

	if (ElapsedTime > MaxDuration)
	{
		AAIController* AIController = OwnerComp.GetAIOwner();
		if (AIController) AIController->StopMovement();

		bIsMoving = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController)
	{
		bIsMoving = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	APawn* Pawn = AIController->GetPawn();
	AActor* Target = CachedTarget.Get();

	if (!Pawn || !Target)
	{
		AIController->StopMovement();
		bIsMoving = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	float CurrentDistance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());

	if (CurrentDistance <= CachedMaxRange * RangeMultiplier && CurrentDistance >= CachedMinRange)
	{
		AIController->StopMovement();
		bIsMoving = false;
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}
}

EBTNodeResult::Type USFBTTask_MoveToAbilityRange::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController && bIsMoving)
	{
		AIController->StopMovement();
	}

	bIsMoving = false;
	CachedOwnerComp.Reset();
	CachedTarget.Reset();

	return Super::AbortTask(OwnerComp, NodeMemory);
}

void USFBTTask_MoveToAbilityRange::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (AIController && bIsMoving)
	{
		AIController->StopMovement();
	}

	bIsMoving = false;
	ElapsedTime = 0.0f;
	CachedMaxRange = 0.0f;
	CachedMinRange = 0.0f;
	CachedOwnerComp.Reset();
	CachedTarget.Reset();

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}
