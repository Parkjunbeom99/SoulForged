// Fill out your copyright notice in the Description page of Project Settings.

#include "SFCombatComponentBase.h"
#include "AIController.h"
#include "AbilitySystemGlobals.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystem/SFAbilitySystemComponent.h"
#include "AI/SFAIGameplayTags.h"
#include "AI/Controller/SFBaseAIController.h"
#include "Character/SFCharacterBase.h"
#include "Interface/SFEnemyAbilityInterface.h"

//SelectAbility 호출
//     ↓
// PrepareAbilityContext (거리/각도 계산)
//     ↓
// FilterValidAbilities (조건 검사 + 점수 계산)
//     ↓
// SelectWeightedAbility (가중치 랜덤 선택)
//     ↓
// 선택된 능력의 Tag 반환

USFCombatComponentBase::USFCombatComponentBase(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void USFCombatComponentBase::InitializeCombatComponent()
{
    AAIController* AIC = GetController<AAIController>();
    if (!AIC) return;

    APawn* Pawn = AIC->GetPawn();
    if (!Pawn) return;

    CachedASC = Cast<USFAbilitySystemComponent>(
        UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn));

}

//목적: Context에 부족한 정보(거리, 각도)를 계산해서 채워줌
FEnemyAbilitySelectContext USFCombatComponentBase::PrepareAbilityContext(const FEnemyAbilitySelectContext& Context) const
{
    FEnemyAbilitySelectContext PreparedContext = Context;

    if (!Context.Self || !Context.Target)
    {
        return PreparedContext;
    }

    // 이미 Distance가 있으면  패스 
    if (Context.DistanceToTarget == 0.f)
    {
        PreparedContext.DistanceToTarget = Context.Self->GetDistanceTo(Context.Target);
    }
    
    if (Context.AngleToTarget == 0.f)
    {
        if (ASFCharacterBase* Owner = Cast<ASFCharacterBase>(Context.Self))
        {
            const FVector ToTarget = (Context.Target->GetActorLocation() - Owner->GetActorLocation()).GetSafeNormal();
            const float Dot = FVector::DotProduct(Owner->GetActorForwardVector(), ToTarget);
            PreparedContext.AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(Dot, -1.f, 1.f)));
        }
    }

    return PreparedContext;
}
//목적: 능력에서 고유한 태그를 추출 (우선순위 순서대로)
FGameplayTag USFCombatComponentBase::ExtractAbilityTag(
    const FGameplayTagContainer& AllTags,
    const FGameplayTagContainer& SearchTags,
    const UGameplayAbility* Ability
) const
{
    
    for (const FGameplayTag& Tag : AllTags)
    {
        if (SearchTags.HasTagExact(Tag))
        {
            return Tag;
        }
    }

    
    if (Ability->AbilityTags.Num() > 0)
    {
        return Ability->AbilityTags.First();
    }

    if (Ability->GetAssetTags().Num() > 0)
    {
        return Ability->GetAssetTags().First();
    }

    return FGameplayTag();
}

//목적: 사용 가능한 능력들을 필터링하고 점수 계산
void USFCombatComponentBase::FilterValidAbilities(
    UAbilitySystemComponent* ASC,
    const FGameplayTagContainer& SearchTags,
    const FEnemyAbilitySelectContext& Context,
    TArray<FAbilityCandidate>& OutCandidates
) const
{
    if (!ASC) return;

    const FGameplayAbilityActorInfo* ActorInfo = ASC->AbilityActorInfo.Get();

    for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        UGameplayAbility* Ability = Spec.Ability;
        if (!Ability) continue;


        FGameplayTagContainer AllTags;
        AllTags.AppendTags(Ability->AbilityTags);
        AllTags.AppendTags(Ability->GetAssetTags());

        if (!AllTags.HasAny(SearchTags)) continue;

        // Check cooldown
        if (!Ability->CheckCooldown(Spec.Handle, ActorInfo)) continue;

        // Check AI interface
        ISFEnemyAbilityInterface* AIInterface = Cast<ISFEnemyAbilityInterface>(Ability);
        if (!AIInterface) continue;

        // Calculate score
        FEnemyAbilitySelectContext ContextWithSpec = Context;
        ContextWithSpec.AbilitySpec = &Spec;

        const float Score = AIInterface->CalcAIScore(ContextWithSpec);
        if (Score <= 0.f) continue;
        
        FGameplayTag UniqueTag = ExtractAbilityTag(AllTags, SearchTags, Ability);
        if (!UniqueTag.IsValid()) continue;
        
        FAbilityCandidate Candidate;
        Candidate.Tag = UniqueTag;
        Candidate.Score = Score;
        Candidate.Spec = &Spec;
        OutCandidates.Add(Candidate);
    }
}
//목적: 가중치 기반 랜덤 선택 (룰렛 휠 알고리즘)
bool USFCombatComponentBase::SelectWeightedAbility(
    const TArray<FAbilityCandidate>& Candidates,
    FGameplayTag& OutSelectedTag
) const
{
    if (Candidates.Num() == 0)
    {
        return false;
    }
    
    float TotalWeight = 0.f;
    for (const FAbilityCandidate& Candidate : Candidates)
    {
        TotalWeight += Candidate.Score;
    }

    // 여기서 랜덤 가중치를 줘야 AI가 고정되지 않고 한다  
    float RandomValue = FMath::FRandRange(0.f, TotalWeight);

    for (const FAbilityCandidate& Candidate : Candidates)
    {
        if (RandomValue <= Candidate.Score)
        {
            OutSelectedTag = Candidate.Tag;
            return true;
        }
        RandomValue -= Candidate.Score;
    }
    
    OutSelectedTag = Candidates.Last().Tag;
    return true;
}

void USFCombatComponentBase::UpdateTargetActor(AActor* NewTarget)
{
    if (CurrentTarget == NewTarget) return;

    AAIController* AIC = GetController<AAIController>();
    if (!AIC) return;

    bool bWasInCombat = (CurrentTarget != nullptr);
    CurrentTarget = NewTarget;
    if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
    {
        BB->SetValueAsObject("TargetActor", NewTarget);
        BB->SetValueAsBool("bHasTarget", NewTarget != nullptr);
    }
    
    if (ASFBaseAIController* SFAIC = Cast<ASFBaseAIController>(AIC))
    {
        SFAIC->TargetActor = NewTarget;

        if (NewTarget)
        {
            SFAIC->SetFocus(NewTarget, EAIFocusPriority::Gameplay);
        }
        else
        {
            SFAIC->ClearFocus(EAIFocusPriority::Gameplay);
        }
    }

    bool bNowInCombat = (NewTarget != nullptr);
    SetGameplayTagStatus(SFGameplayTags::AI_State_Combat, bNowInCombat);
    
    if (bWasInCombat != bNowInCombat)
    {
        OnCombatStateChanged.Broadcast(bNowInCombat);
    }
}

bool USFCombatComponentBase::SelectAbility(
    const FEnemyAbilitySelectContext& Context,
    const FGameplayTagContainer& SearchTags,
    FGameplayTag& OutSelectedTag)
{
    OutSelectedTag = FGameplayTag();
    
    UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Context.Self);
    if (!ASC || SearchTags.IsEmpty())
    {
        return false;
    }
    
    const FEnemyAbilitySelectContext PreparedContext = PrepareAbilityContext(Context);
    
    TArray<FAbilityCandidate> Candidates;
    FilterValidAbilities(ASC, SearchTags, PreparedContext, Candidates);
    
    return SelectWeightedAbility(Candidates, OutSelectedTag);
}

void USFCombatComponentBase::SetGameplayTagStatus(const FGameplayTag& Tag, bool bActive)
{
    if (!CachedASC || !Tag.IsValid()) return;

    if (bActive)
    {
        if (!CachedASC->HasMatchingGameplayTag(Tag))
            CachedASC->AddLooseGameplayTag(Tag);
    }
    else
    {
        if (CachedASC->HasMatchingGameplayTag(Tag))
            CachedASC->RemoveLooseGameplayTag(Tag);
    }
}

APawn* USFCombatComponentBase::GetOwnerPawn() const
{
    if (AAIController* AIC = GetController<AAIController>())
        return AIC->GetPawn();
    return nullptr;
}

void USFCombatComponentBase::ClearTarget()
{
    UpdateTargetActor(nullptr);
}

