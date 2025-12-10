// Fill out your copyright notice in the Description page of Project Settings.


#include "SFGA_CharacterDeath.h"

#include "AbilitySystem/GameplayEvent/SFGameplayEventTags.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/SFCharacterGameplayTags.h"

USFGA_CharacterDeath::USFGA_CharacterDeath()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	bServerRespectsRemoteAbilityCancellation = true;
	
	FAbilityTriggerData TriggerData;
	TriggerData.TriggerTag = SFGameplayTags::GameplayEvent_Death;
	TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(TriggerData);

	ActivationOwnedTags.AddTag(SFGameplayTags::Character_State_Dead);
}

void USFGA_CharacterDeath::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* Avatar = GetAvatarActorFromActorInfo();
	
	
	if (!Avatar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (Avatar->HasAuthority())
	{
		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &ThisClass::DeathEventAfterDelay);
		Avatar->GetWorldTimerManager().SetTimer(EventTimerHandle, TimerDel, EventTime, false);
	}
}

void USFGA_CharacterDeath::DeathEventAfterDelay()
{
	DeathTimerEvent();
}

void USFGA_CharacterDeath::DeathTimerEvent()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (Avatar && Avatar->HasAuthority())
	{
		Avatar->Destroy();
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}




