// Fill out your copyright notice in the Description page of Project Settings.


#include "SFBTTask_SelectAbilityByTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "AbilitySystem/Abilities/Enemy/Combat/SFGA_Enemy_BaseAttack.h"
#include "AbilitySystem/GameplayEvent/SFGameplayEventTags.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Float.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Name.h"
#include "Character/SFCharacterGameplayTags.h"
#include "Interface/SFAIControllerInterface.h"
#include "Interface/SFEnemyAbilityInterface.h"

USFBTTask_SelectAbilityByTag::USFBTTask_SelectAbilityByTag()
{
	NodeName = TEXT("Select Ability(Task)");

	// BlackboardKey는 Name 타입 (FGameplayTag를 FName으로 저장)
	BlackboardKey.AddNameFilter(this, GET_MEMBER_NAME_CHECKED(USFBTTask_SelectAbilityByTag, BlackboardKey));
	MinRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(USFBTTask_SelectAbilityByTag, MinRangeKey));
	MaxRangeKey.AddFloatFilter(this, GET_MEMBER_NAME_CHECKED(USFBTTask_SelectAbilityByTag, MaxRangeKey));

}

EBTNodeResult::Type USFBTTask_SelectAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	APawn* Pawn = OwnerComp.GetAIOwner() ? OwnerComp.GetAIOwner()->GetPawn() : nullptr;
	if (!Pawn) return EBTNodeResult::Failed;

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn);
	if (!ASC) return EBTNodeResult::Failed;

	// 이미 어빌리티 사용 중이면 실패
	if (ASC->HasMatchingGameplayTag(SFGameplayTags::Character_State_UsingAbility))
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB) return EBTNodeResult::Failed;

	// 이미 선택된 어빌리티가 있으면 성공 (FName으로 저장된 GameplayTag 확인)
	const FName SelectedTagName = BB->GetValueAsName(BlackboardKey.SelectedKeyName);
	if (!SelectedTagName.IsNone())
	{
		FGameplayTag SelectedTag = FGameplayTag::RequestGameplayTag(SelectedTagName, false);
		if (SelectedTag.IsValid())
		{
			return EBTNodeResult::Succeeded;
		}
	}

	// CombatComponent를 통해 어빌리티 선택
	if (ISFAIControllerInterface* AI = Cast<ISFAIControllerInterface>(OwnerComp.GetAIOwner()))
	{
		USFCombatComponentBase* Combat = AI->GetCombatComponent();
		if (!Combat) return EBTNodeResult::Failed;

		FEnemyAbilitySelectContext Context;
		Context.Self = Pawn;
		Context.Target = Combat->GetCurrentTarget();

		FGameplayTag OutSelectedTag;
		if (Combat->SelectAbility(Context, AbilitySearchTags, OutSelectedTag))
		{
			// FGameplayTag를 FName으로 변환해서 Blackboard에 저장
			BB->SetValueAsName(BlackboardKey.SelectedKeyName, OutSelectedTag.GetTagName());

			// 선택된 어빌리티의 Range 정보를 Blackboard에 저장
			for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
			{
				if (Spec.Ability && Spec.Ability->AbilityTags.HasTagExact(OutSelectedTag))
				{
					// SetByCallerTagMagnitudes에서 Range 정보 추출
					const float* MinValPtr = Spec.SetByCallerTagMagnitudes.Find(SFGameplayTags::Data_EnemyAbility_MinAttackRange);
					const float* MaxValPtr = Spec.SetByCallerTagMagnitudes.Find(SFGameplayTags::Data_EnemyAbility_AttackRange);

					float MinRange = MinValPtr ? *MinValPtr : 0.f;
					float MaxRange = MaxValPtr ? *MaxValPtr : 200.f;

					// MaxRange가 0 이하면 무한대로 설정
					if (MaxRange <= 0.f) MaxRange = 999999.f;

					// Range 정보를 Blackboard에 저장
					BB->SetValueAsFloat(MinRangeKey.SelectedKeyName, MinRange);
					BB->SetValueAsFloat(MaxRangeKey.SelectedKeyName, MaxRange);
					break;
				}
			}
			return EBTNodeResult::Succeeded;
		}
		else
		{
			BB->ClearValue(BlackboardKey.SelectedKeyName);
		}
	}

	return EBTNodeResult::Failed;
}

FString USFBTTask_SelectAbilityByTag::GetStaticDescription() const
{
	FString Description = Super::GetStaticDescription();
	Description += FString::Printf(TEXT("\nSearch Tags: %s"), *AbilitySearchTags.ToStringSimple());
	Description += FString::Printf(TEXT("\nSelected Ability Key: %s"), *BlackboardKey.SelectedKeyName.ToString());
	return Description;
}
