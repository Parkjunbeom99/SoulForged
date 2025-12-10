// Fill out your copyright notice in the Description page of Project Settings.


#include "SFPrimarySet.h"

#include "AbilitySystem/SFAbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include "GameplayEffectExtension.h"
#include "AbilitySystem/GameplayEvent/SFGameplayEventTags.h"
#include "Character/SFCharacterGameplayTags.h"

USFPrimarySet::USFPrimarySet()
{
}

void USFPrimarySet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(ThisClass, MoveSpeed, COND_None, REPNOTIFY_Always);
}

bool USFPrimarySet::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	return Super::PreGameplayEffectExecute(Data);
}

void USFPrimarySet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// 변경된 Attribute가 'Damage'인지 확인
	// 라인 36-49
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		const float DamageDone = GetDamage();
		SetDamage(0.0f);

		if (DamageDone > 0.0f && GetHealth() > 0.0f)
		{// 4. 'Damage'를 'Health'에 적용
			SetHealth(GetHealth() - DamageDone);
		}
	}
	
}

void USFPrimarySet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void USFPrimarySet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void USFPrimarySet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	if (Attribute == GetMaxHealthAttribute())
	{
		if (GetHealth() > NewValue)
		{
			USFAbilitySystemComponent* SFASC = GetSFAbilitySystemComponent();
			check(SFASC);
			SFASC->ApplyModToAttribute(GetHealthAttribute(), EGameplayModOp::Override, NewValue);
		}
	}
	if (Attribute == GetHealthAttribute())
	{
		if (GetHealth() <= 0.1f)
		{
			if (USFAbilitySystemComponent* SFASC = GetSFAbilitySystemComponent())
			{
				if (AActor* OwnerActor = GetOwningActor())
				{
					if (OwnerActor->HasAuthority())  
					{
						if (!SFASC->HasMatchingGameplayTag(SFGameplayTags::Character_State_Dead))
						{    
							FGameplayEventData Payload;
							SFASC->HandleGameplayEvent(SFGameplayTags::GameplayEvent_Death, &Payload);
						}
					}
				}
			}
		}
	}
}

void USFPrimarySet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		NewValue = FMath::Max(NewValue, 1.0f);
	}
}

void USFPrimarySet::OnRep_Health(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, Health, OldValue);
	
	
}

void USFPrimarySet::OnRep_MaxHealth(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MaxHealth, OldValue);
}

void USFPrimarySet::OnRep_MoveSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(ThisClass, MoveSpeed, OldValue);
}
