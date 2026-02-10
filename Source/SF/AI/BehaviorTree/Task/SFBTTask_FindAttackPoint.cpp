#include "AI/BehaviorTree/Task/SFBTTask_FindAttackPoint.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "EnvironmentQuery/EnvQueryManager.h"
#include "NavigationSystem.h"
#include "Character/SFCharacterBase.h"
#include "AbilitySystem/SFAbilitySystemComponent.h"
#include "AbilitySystem/GameplayEvent/SFGameplayEventTags.h"
#include "AI/Controller/SFBaseAIController.h"
#include "AI/Controller/SFCombatComponentBase.h"

USFBTTask_FindAttackPoint::USFBTTask_FindAttackPoint(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    NodeName = "Find Attack Point (EQS)";
    bCreateNodeInstance = true;
    ResultKeyName.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(USFBTTask_FindAttackPoint, ResultKeyName));
    MinRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(USFBTTask_FindAttackPoint, MinRangeKey));
    MaxRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(USFBTTask_FindAttackPoint, MaxRangeKey));
}

EBTNodeResult::Type USFBTTask_FindAttackPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* AIController = OwnerComp.GetAIOwner();
    ASFCharacterBase* Character = AIController ? Cast<ASFCharacterBase>(AIController->GetPawn()) : nullptr;
    UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Character || !Blackboard || !QueryTemplate) return EBTNodeResult::Failed;

    ASFBaseAIController* SFController = Cast<ASFBaseAIController>(AIController);
    USFCombatComponentBase* CombatComponent = SFController->GetCombatComponent();
    AActor* TargetActorPtr = nullptr;
    if (CombatComponent)
    {
        TargetActorPtr = CombatComponent->GetCurrentTarget();
    }
    if (!TargetActorPtr) return EBTNodeResult::Failed;

    float MinDist = Blackboard->GetValueAsFloat(MinRangeKey.SelectedKeyName);
    float MaxDist = Blackboard->GetValueAsFloat(MaxRangeKey.SelectedKeyName);

    if (MaxDist <= 0.f)
    {
        MaxDist = 1000.f;
    }

    if (bSkipIfInRange)
    {
        float DistSq = FVector::DistSquared2D(Character->GetActorLocation(), TargetActorPtr->GetActorLocation());
        if (DistSq >= (MinDist * MinDist) && DistSq <= (MaxDist * MaxDist * 0.81f))
        {
            return EBTNodeResult::Succeeded;
        }
    }

    CachedOwnerComp = &OwnerComp;
    FEnvQueryRequest QueryRequest(QueryTemplate, Character);

    QueryRequest.SetFloatParam(FName("MinDistance"), MinDist);
    QueryRequest.SetFloatParam(FName("MaxDistance"), MaxDist);
    QueryRequest.SetFloatParam(FName("OptimalDistance"), (MinDist + MaxDist) * 0.5f);

    QueryID = QueryRequest.Execute(RunMode, this, &USFBTTask_FindAttackPoint::OnQueryFinished);

    return (QueryID == INDEX_NONE) ? EBTNodeResult::Failed : EBTNodeResult::InProgress;
}

void USFBTTask_FindAttackPoint::OnQueryFinished(TSharedPtr<FEnvQueryResult> Result)
{
    if (!CachedOwnerComp.IsValid() || QueryID == INDEX_NONE || Result->QueryID != QueryID) return;

    QueryID = INDEX_NONE;

    if (Result->IsAborted())
    {
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Aborted);
        return;
    }

    if (Result->IsSuccessful())
    {
        FVector ResultLocation = Result->GetItemAsLocation(0);

        if (bProjectToNavMesh)
        {
            if (UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld()))
            {
                FNavLocation ProjectedLocation;
                FVector ProjectionExtent(NavMeshProjectionRadius, NavMeshProjectionRadius, NavMeshProjectionRadius);

                if (NavSys->ProjectPointToNavigation(ResultLocation, ProjectedLocation, ProjectionExtent))
                {
                    ResultLocation = ProjectedLocation.Location;
                }
            }
        }

        if (UBlackboardComponent* Blackboard = CachedOwnerComp->GetBlackboardComponent())
        {
            Blackboard->SetValueAsVector(ResultKeyName.SelectedKeyName, ResultLocation);
        }
        FinishLatentTask(*CachedOwnerComp, EBTNodeResult::Succeeded);
    }
    else
    {
        FinishLatentTask(*CachedOwnerComp, FailureResult);
    }
}

EBTNodeResult::Type USFBTTask_FindAttackPoint::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    if (QueryID != INDEX_NONE)
    {
        if (UEnvQueryManager* QueryManager = UEnvQueryManager::GetCurrent(GetWorld()))
        {
            QueryManager->AbortQuery(QueryID);
        }
        QueryID = INDEX_NONE;
    }
    return Super::AbortTask(OwnerComp, NodeMemory);
}

void USFBTTask_FindAttackPoint::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
    QueryID = INDEX_NONE;
    CachedOwnerComp.Reset();
    Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}