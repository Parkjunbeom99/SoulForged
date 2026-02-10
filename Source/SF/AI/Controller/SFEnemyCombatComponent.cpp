// Fill out your copyright notice in the Description page of Project Settings.

#include "SFEnemyCombatComponent.h"
#include "TimerManager.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "SFBaseAIController.h"
#include "AbilitySystem/SFAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/Enemy/SFCombatSet_Enemy.h"
#include "AbilitySystem/Attributes/SFPrimarySet.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISense_Sight.h"
#include "AI/SFAIGameplayTags.h"
#include "Character/SFCharacterBase.h"
#include "Character/SFCharacterGameplayTags.h"
#include "Character/Enemy/SFEnemyGameplayTags.h"

USFEnemyCombatComponent::USFEnemyCombatComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void USFEnemyCombatComponent::BeginDestroy()
{
    StopTargetEvaluationTimer();
    Super::BeginDestroy();
}

void USFEnemyCombatComponent::InitializeCombatComponent()
{
    Super::InitializeCombatComponent();

    if (!CachedASC)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeCombatComponent: CachedASC is null"), *GetNameSafe(GetOwner()));
        return;
    }
    
    const UAttributeSet* Set = CachedASC->GetAttributeSet(USFCombatSet_Enemy::StaticClass());
    CachedCombatSet = const_cast<USFCombatSet_Enemy*>(Cast<USFCombatSet_Enemy>(Set));

    if (!CachedCombatSet)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeCombatComponent: CachedCombatSet is null"), *GetNameSafe(GetOwner()));
    }
    else
    {
        UpdatePerceptionConfig();
    }
    
    AAIController* AIC = GetController<AAIController>();
    if (!AIC)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeCombatComponent: AIController is null"), *GetNameSafe(GetOwner()));
        return;
    }

    UAIPerceptionComponent* PerceptionComp = AIC->GetPerceptionComponent();
    if (!PerceptionComp)
    {
        UE_LOG(LogTemp, Warning, TEXT("[%s] InitializeCombatComponent: PerceptionComponent is null"), *GetNameSafe(GetOwner()));
    }
    else
    {
        PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &USFEnemyCombatComponent::OnTargetPerceptionUpdated);
        PerceptionComp->OnPerceptionUpdated.AddDynamic(this, &USFEnemyCombatComponent::OnPerceptionUpdated);
    }
    
    StartTargetEvaluationTimer();
}

void USFEnemyCombatComponent::StartTargetEvaluationTimer()
{
    AAIController* AIC = GetController<AAIController>();
    if (!AIC) return;

    UWorld* World = AIC->GetWorld();
    if (!World) return;

    if (TargetEvaluationTimerHandle.IsValid())
    {
        World->GetTimerManager().ClearTimer(TargetEvaluationTimerHandle);
    }

    World->GetTimerManager().SetTimer(
        TargetEvaluationTimerHandle,
        this,
        &USFEnemyCombatComponent::OnTargetEvaluationTimer,
        TargetEvaluationInterval,
        true
    );
}

void USFEnemyCombatComponent::StopTargetEvaluationTimer()
{
    AAIController* AIC = GetController<AAIController>();
    if (!AIC) return;

    UWorld* World = AIC->GetWorld();
    if (!World) return;

    if (TargetEvaluationTimerHandle.IsValid())
    {
        World->GetTimerManager().ClearTimer(TargetEvaluationTimerHandle);
        TargetEvaluationTimerHandle.Invalidate();
    }
}

void USFEnemyCombatComponent::OnTargetEvaluationTimer()
{
    EvaluateTarget();
    UpdateCombatRangeTags();
}

void USFEnemyCombatComponent::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!Actor || !Actor->ActorHasTag(TargetActorTag)) return;

    if (!IsValidTarget(Actor))
    {
        if (CurrentTarget == Actor)
        {
            EvaluateTarget();
        }
        return;
    }

    AAIController* AIC = GetController<AAIController>();
    if (!AIC) return;
    
    if (CurrentTarget && CurrentTarget != Actor)
    {
        APawn* MyPawn = AIC->GetPawn();
        if (MyPawn)
        {
            if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MyPawn))
            {
                if (ASC->HasMatchingGameplayTag(SFGameplayTags::Character_State_UsingAbility))
                {
                    return;
                }
            }
        }
    }

    if (Stimulus.WasSuccessfullySensed())
    {
        EvaluateTarget();
    }
    else if (CurrentTarget == Actor)
    {
        EvaluateTarget();
    }
}

void USFEnemyCombatComponent::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
    EvaluateTarget();
}

void USFEnemyCombatComponent::EvaluateTarget()
{
    AAIController* AIC = GetController<AAIController>();
    if (!AIC) return;

    UAIPerceptionComponent* PerceptionComp = AIC->GetPerceptionComponent();
    if (!PerceptionComp) return;

    TArray<AActor*> PerceivedActors;
    PerceptionComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

    AActor* BestTarget = nullptr;
    float BestScore = -1.f;

    for (AActor* Actor : PerceivedActors)
    {
        if (!Actor->ActorHasTag(TargetActorTag)) continue;

        if (!IsValidTarget(Actor)) continue;

        float Score = CalculateTargetScore(Actor);
        if (Score > BestScore)
        {
            BestScore = Score;
            BestTarget = Actor;
        }
    }

    UpdateTargetActor(BestTarget);
}

bool USFEnemyCombatComponent::IsValidTarget(AActor* Target) const
{
    if (!IsValid(Target)) return false;

    ASFCharacterBase* TargetChar = Cast<ASFCharacterBase>(Target);
    if (!TargetChar) return false;

    UAbilitySystemComponent* ASC = TargetChar->GetSFAbilitySystemComponent();
    if (ASC)
    {
        if (ASC->HasMatchingGameplayTag(SFGameplayTags::Character_State_Dead) ||
            ASC->HasMatchingGameplayTag(SFGameplayTags::Character_State_Downed))
        {
            return false;
        }
    }

    return true;
}

float USFEnemyCombatComponent::CalculateTargetScore(AActor* Target) const
{
    if (!IsValid(Target)) return -1.f;

    APawn* MyPawn = GetOwnerPawn();
    if (!IsValid(MyPawn)) return -1.f;

    float TotalScore = 0.f;

    // 1. 거리 점수 (가까울수록 높음) 
    const float Distance = FVector::Dist(
        MyPawn->GetActorLocation(), 
        Target->GetActorLocation()
    );
    
    const float DistanceScore = FMath::GetMappedRangeValueClamped(
        FVector2D(0.f, MaxTargetDistance),
        FVector2D(MaxDistanceScore, 0.f),
        Distance
    );
    TotalScore += DistanceScore * DistanceScoreWeight;  // 가중치 1.0 적용

    // 각도 점수 (정면에 있을수록 높음) 
    const FVector ToTarget = (Target->GetActorLocation() - MyPawn->GetActorLocation()).GetSafeNormal();
    const float Dot = FVector::DotProduct(MyPawn->GetActorForwardVector(), ToTarget);
    
    // Dot: -1(뒤) ~ 1(정면) → 0 ~ MaxAngleScore(100)
    const float AngleScore = ((Dot + 1.f) * 0.5f) * MaxAngleScore;
    TotalScore += AngleScore * AngleScoreWeight;  // 가중치 1.0 적용

    // 3. 체력 점수 (낮을수록 높음) 
    if (bConsiderTargetHealth)
    {
        if (ASFCharacterBase* TargetChar = Cast<ASFCharacterBase>(Target))
        {
            if (USFAbilitySystemComponent* ASC = TargetChar->GetSFAbilitySystemComponent())
            {
                if (const USFPrimarySet* PrimarySet = ASC->GetSet<USFPrimarySet>())
                {
                    const float HealthRatio = PrimarySet->GetHealth() / 
                        FMath::Max(PrimarySet->GetMaxHealth(), 1.f);
                    
                    // 체력 100% → 0점, 체력 0% → MaxHealthScore(200)점
                    const float HealthScore = (1.f - HealthRatio) * MaxHealthScore;
                    TotalScore += HealthScore * HealthScoreWeight;  // 가중치 0.5
                }
            }
        }
    }

    return TotalScore;
}

void USFEnemyCombatComponent::UpdateCombatRangeTags()
{
    if (!CachedASC || !CachedCombatSet || !CurrentTarget || !IsValidTarget(CurrentTarget))
    {
        SetGameplayTagStatus(SFGameplayTags::AI_Range_Melee, false);
        SetGameplayTagStatus(SFGameplayTags::AI_Range_Guard, false);
        return;
    }

    APawn* MyPawn = GetOwnerPawn();
    if (!MyPawn) return;

    float Distance = FVector::Dist(MyPawn->GetActorLocation(), CurrentTarget->GetActorLocation());

    SetGameplayTagStatus(SFGameplayTags::AI_Range_Melee, Distance <= CachedCombatSet->GetMeleeRange());
    SetGameplayTagStatus(SFGameplayTags::AI_Range_Guard, Distance <= CachedCombatSet->GetGuardRange());
}

void USFEnemyCombatComponent::UpdatePerceptionConfig()
{
    if (!CachedCombatSet) return;

    AAIController* AIC = Cast<AAIController>(GetOwner());
    if (!AIC || !AIC->GetPerceptionComponent()) return;

    UAIPerceptionComponent* Perception = AIC->GetPerceptionComponent();
    FAISenseID SightID = UAISense::GetSenseID<UAISense_Sight>();
    UAISenseConfig_Sight* SightConfig = Cast<UAISenseConfig_Sight>(Perception->GetSenseConfig(SightID));

    if (SightConfig)
    {
        SightConfig->SightRadius = CachedCombatSet->GetSightRadius();
        SightConfig->LoseSightRadius = CachedCombatSet->GetLoseSightRadius();
        Perception->ConfigureSense(*SightConfig);
    }
}