#include "SFGA_Hero_Downed.h"

#include "AbilitySystem/Attributes/Hero/SFCombatSet_Hero.h"
#include "AbilitySystem/Attributes/Hero/SFPrimarySet_Hero.h"
#include "AbilitySystem/GameplayEvent/SFGameplayEventTags.h"
#include "Character/SFCharacterGameplayTags.h"
#include "Character/Hero/SFHero.h"
#include "Libraries/SFAbilitySystemLibrary.h"
#include "Player/Components/SFPlayerCombatStateComponent.h"

USFGA_Hero_Downed::USFGA_Hero_Downed(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	ActivationPolicy = ESFAbilityActivationPolicy::Manual;
	
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;

	// 다운 상태 태그 부여
	ActivationOwnedTags.AddTag(SFGameplayTags::Character_State_Downed);
	ActivationBlockedTags.AddTag(SFGameplayTags::Character_State_Downed);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		FAbilityTriggerData TriggerData;
		TriggerData.TriggerTag = SFGameplayTags::GameplayEvent_Downed;
		TriggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
		AbilityTriggers.Add(TriggerData);
	}
}

void USFGA_Hero_Downed::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HasAuthority(&ActivationInfo))
	{
		return;
	}

	CachedDownedHero = Cast<ASFHero>(GetAvatarActorFromActorInfo());
	if (!CachedDownedHero.IsValid())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	CachedCombatStateComponent = USFPlayerCombatStateComponent::FindPlayerCombatStateComponent(GetAvatarActorFromActorInfo());
	if (!CachedCombatStateComponent.IsValid())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}

	float InitialGauge = CachedCombatStateComponent->GetInitialReviveGauge();
	
	// 즉시 사망 체크
	if (InitialGauge <= 0.f)
	{
		HandleDeath();
		return;
	}

	// DownCount 감소
	CachedCombatStateComponent->DecrementDownCount();

	// ReviveGauge 초기값 설정
	SetReviveGauge(InitialGauge);

	// 게이지 틱 타이머 시작
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			GaugeTickTimerHandle,
			this,
			&ThisClass::UpdateReviveGauge,
			UpdateInterval,
			true  // 반복
		);
	}

	// TODO: 다운 애니메이션, 이동 제한 등 추가 처리
}

void USFGA_Hero_Downed::UpdateReviveGauge()
{
	if (!HasAuthority(&CurrentActivationInfo))
	{
		return;
	}

	if (!CachedDownedHero.IsValid())
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
		return;
	}
	
	float CurrentGauge = ASC->GetNumericAttribute(USFCombatSet_Hero::GetReviveGaugeAttribute());
	int32 ReviverCount = CachedDownedHero->GetActiveInteractorCount();

	// 게이지 변화량 계산 공식(TODO : 추후 상호작용 관련 버프 존재시 해당 로직 수정)
	float FillAmount = FillRatePerReviver * ReviverCount;
	float DrainAmount = DrainRatePerSecond;
	float DeltaGauge = (FillAmount - DrainAmount) * UpdateInterval;
	float NewGauge = FMath::Clamp(CurrentGauge + DeltaGauge, 0.f, MaxReviveGauge);

	SetReviveGauge(NewGauge);

	// 사망/부활 체크
	if (NewGauge <= 0.f)
	{
		HandleDeath();
	}
	else if (NewGauge >= MaxReviveGauge)
	{
		HandleRevive();
	}
}

void USFGA_Hero_Downed::SetReviveGauge(float NewValue)
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->SetNumericAttributeBase(USFCombatSet_Hero::GetReviveGaugeAttribute(), NewValue);
	}
}

void USFGA_Hero_Downed::HandleDeath()
{
	// 타이머 정리
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(GaugeTickTimerHandle);
	}

	SetReviveGauge(0.f);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		USFAbilitySystemLibrary::SendDeathEvent(ASC);
	}
	
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USFGA_Hero_Downed::HandleRevive()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(GaugeTickTimerHandle);
	}

	// Health 회복
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		float MaxHealth = ASC->GetNumericAttribute(USFPrimarySet_Hero::GetMaxHealthAttribute());
		float ReviveHealth = MaxHealth * ReviveHealthPercent;
		ASC->SetNumericAttributeBase(USFPrimarySet_Hero::GetHealthAttribute(), ReviveHealth);
	}

	// ReviveGauge 리셋
	SetReviveGauge(0.f);

	// 부활자들에게 이벤트 발송
	if (CachedDownedHero.IsValid())
	{
		FGameplayEventData Payload;
		Payload.EventTag = SFGameplayTags::GameplayEvent_Revived;
		Payload.Instigator = CachedDownedHero.Get();

		TArray<TWeakObjectPtr<AActor>> ReviversCopy = CachedDownedHero->GetCachedRevivers();
		for (const TWeakObjectPtr<AActor>& Reviver : ReviversCopy)
		{
			if (Reviver.IsValid())
			{
				if (UAbilitySystemComponent* ReviverASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Reviver.Get()))
				{
					ReviverASC->HandleGameplayEvent(SFGameplayTags::GameplayEvent_Revived, &Payload);
				}
			}
		}
	}
	
	// TODO: 부활 애니메이션(단발성 GameplayCue 몽타주, 이동 복구 등 추가 처리)

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USFGA_Hero_Downed::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(GaugeTickTimerHandle);
	}

	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}