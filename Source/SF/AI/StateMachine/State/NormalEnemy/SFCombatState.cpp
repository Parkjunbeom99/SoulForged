// Fill out your copyright notice in the Description page of Project Settings.

#include "SFCombatState.h"

#include "AI/SFAIGameplayTags.h"
#include "AI/Controller/SFBaseAIController.h"
#include "AI/Controller/SFCombatComponentBase.h"
#include "AI/StateMachine/SFStateMachine.h"
#include "GameFramework/Actor.h"


void USFCombatState::OnEnter_Implementation()
{
    Super::OnEnter_Implementation();

    // Combat 전용 초기화
    TargetLostTimer = 0.0f;
    LastSeenTargetTime = 0.0f;

    UE_LOG(LogTemp, Log, TEXT("[%s] Combat State Entered"), *GetNameSafe(OwnerActor));
}

void USFCombatState::OnUpdate_Implementation(float DeltaTime)
{
    Super::OnUpdate_Implementation(DeltaTime);

    if (!OwnerActor)
    {
        return;
    }

    // 스폰 위치에서 너무 멀어지면 즉시 복귀
    if (IsTooFarFromHome())
    {
        TransitionToReturn();
        return;
    }

    bool bHasValidTarget = HasTarget();

    if (bHasValidTarget)
    {
        TargetLostTimer = 0.0f;

        if (UWorld* World = GetWorld())
        {
            LastSeenTargetTime = World->GetTimeSeconds();
        }

        if (IsTargetTooFar())
        {
            TransitionToReturn();
            return;
        }
    }
    else
    {
        // 타겟이 없으면 유예시간을 주자
        TargetLostTimer += DeltaTime;

        if (TargetLostTimer >= TargetLostGracePeriod)
        {
            UE_LOG(LogTemp, Warning, TEXT("[%s] Target lost for %.1fs! Returning..."),
                *GetNameSafe(OwnerActor), TargetLostGracePeriod);
            TransitionToReturn();
            return;
        }
    }
}

void USFCombatState::TransitionToReturn()
{
    if (USFStateMachine* SM = GetStateMachine())
    {
        SM->ActivateStateByTag(SFGameplayTags::AI_State_Return);
    }
}

bool USFCombatState::IsTargetTooFar() const
{
    USFCombatComponentBase* CombatComp = GetCombatComponent();
    if (!CombatComp || !OwnerActor)
    {
        return false;
    }

    AActor* Target = CombatComp->GetCurrentTarget();
    if (!Target)
    {
        return false;
    }

    float Distance = FVector::Dist(
        OwnerActor->GetActorLocation(),
        Target->GetActorLocation()
    );

    return Distance > MaxChaseDistance;
}

bool USFCombatState::IsTooFarFromHome() const
{
    ASFBaseAIController* AIC = GetAIController();
    if (!AIC)
    {
        return false;
    }

    // AIController의 MaxLeashDistance와 비교
    float DistanceFromHome = AIC->GetDistanceFromHome();
    float MaxLeashDistance = AIC->GetMaxLeashDistance();

    return MaxLeashDistance > 0.0f && DistanceFromHome > MaxLeashDistance;
}



