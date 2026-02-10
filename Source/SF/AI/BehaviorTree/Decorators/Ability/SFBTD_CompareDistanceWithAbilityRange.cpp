#include "SFBTD_CompareDistanceWithAbilityRange.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/GameplayEvent/SFGameplayEventTags.h"
#include "AI/Controller/SFBaseAIController.h"

USFBTD_CompareDistanceWithAbilityRange::USFBTD_CompareDistanceWithAbilityRange()
{
    NodeName = "Compare Distance With Ability Range";
    bNotifyTick = true;
    FlowAbortMode = EBTFlowAbortMode::Self;
    AbilityTagKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(USFBTD_CompareDistanceWithAbilityRange, AbilityTagKey));
}

uint16 USFBTD_CompareDistanceWithAbilityRange::GetInstanceMemorySize() const
{
    return sizeof(FBTDistanceCompareMemory);
}

void USFBTD_CompareDistanceWithAbilityRange::InitializeMemory(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    EBTMemoryInit::Type InitType) const
{
    FBTDistanceCompareMemory* Memory = CastInstanceNodeMemory<FBTDistanceCompareMemory>(NodeMemory);
    if (Memory)
    {
        Memory->bLastResult = false;
    }
}

bool USFBTD_CompareDistanceWithAbilityRange::CalculateRawConditionValue(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory) const
{
    return CheckCondition(OwnerComp);
}

void USFBTD_CompareDistanceWithAbilityRange::TickNode(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory,
    float DeltaSeconds)
{
    Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

    FBTDistanceCompareMemory* Memory = CastInstanceNodeMemory<FBTDistanceCompareMemory>(NodeMemory);
    if (!Memory) return;

    const bool bCurrent = CheckCondition(OwnerComp);
    if (Memory->bLastResult != bCurrent)
    {
        Memory->bLastResult = bCurrent;
        OwnerComp.RequestExecution(this);
    }
}

bool USFBTD_CompareDistanceWithAbilityRange::CheckCondition(
    UBehaviorTreeComponent& OwnerComp) const
{
    const UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
    if (!BB) return false;

    ASFBaseAIController* AIC = Cast<ASFBaseAIController>(OwnerComp.GetAIOwner());
    if (!AIC) return false;

    USFCombatComponentBase* CombatComp = AIC->GetCombatComponent();
    if (!CombatComp) return false;

    AActor* Target = CombatComp->GetCurrentTarget();
    APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    if (!Target || !Pawn) return false;

    float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());

    if (AbilityTagKey.IsNone())
        return false;

    const FName TagName = BB->GetValueAsName(AbilityTagKey.SelectedKeyName);
    if (TagName.IsNone())
        return false;

    const FGameplayTag AbilityTag = FGameplayTag::RequestGameplayTag(TagName);

    float MinRange = 0.f;
    float MaxRange = 0.f;
    GetAbilityAttackRange(OwnerComp, AbilityTag, MinRange, MaxRange);

    if (MaxRange <= 0.f)
        return false;

    return Distance > MinRange && Distance <= MaxRange;
}

void USFBTD_CompareDistanceWithAbilityRange::GetAbilityAttackRange(
    UBehaviorTreeComponent& OwnerComp,
    const FGameplayTag& AbilityTag,
    float& OutMinRange,
    float& OutMaxRange) const
{
    OutMinRange = 0.f;
    OutMaxRange = 0.f;

    APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
    if (!Pawn) return;

    UAbilitySystemComponent* ASC =
        UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn);

    if (!ASC) return;

    for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (!Spec.Ability)
            continue;

        if (Spec.Ability->AbilityTags.HasTagExact(AbilityTag) ||
            Spec.Ability->GetAssetTags().HasTagExact(AbilityTag))
        {
            const float* MaxPtr =
                Spec.SetByCallerTagMagnitudes.Find(
                    SFGameplayTags::Data_EnemyAbility_AttackRange);

            const float* MinPtr =
                Spec.SetByCallerTagMagnitudes.Find(
                    SFGameplayTags::Data_EnemyAbility_MinAttackRange);

            OutMinRange = MinPtr ? *MinPtr : 0.f;
            OutMaxRange = MaxPtr ? *MaxPtr : 0.f;
            break;
        }
    }
}
