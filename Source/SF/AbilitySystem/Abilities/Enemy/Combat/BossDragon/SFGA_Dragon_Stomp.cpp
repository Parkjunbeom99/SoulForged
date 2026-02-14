// Fill out your copyright notice in the Description page of Project Settings.


#include "SFGA_Dragon_Stomp.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystem/GameplayCues/SFGameplayCueTags.h"
#include "AbilitySystem/GameplayEffect/SFGameplayEffectContext.h"
#include "AbilitySystem/GameplayEvent/SFGameplayEventTags.h"
#include "Character/SFCharacterBase.h"
#include "Engine/OverlapResult.h"
#include "Character/Enemy/Component/Boss_Dragon/SFDragonGameplayTags.h"

USFGA_Dragon_Stomp::USFGA_Dragon_Stomp()
{
	AbilityID = FName("Dragon_Stomp");
	AttackType = EAttackType::Melee;
	
	AbilityTags.AddTag(SFGameplayTags::Ability_Dragon_Stomp);
	CoolDownTag = SFGameplayTags::Ability_Cooldown_Dragon_Stomp;
}

void USFGA_Dragon_Stomp::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	

	if (StompMontage)
	{

		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			StompMontage,
			1.f,
			NAME_None,
			true
		);


		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &USFGA_Dragon_Stomp::OnMontageCompleted);
			MontageTask->OnInterrupted.AddDynamic(this, &USFGA_Dragon_Stomp::OnMontageInterrupted);
			MontageTask->OnCancelled.AddDynamic(this, &USFGA_Dragon_Stomp::OnMontageCancelled);
			MontageTask->ReadyForActivation();
		}
	}

	if (!ActorInfo->IsNetAuthority())
	{
		return;
	}
	UAbilityTask_WaitGameplayEvent* WaitEventTask =
		UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			SFGameplayTags::GameplayEvent_Tracing,
			nullptr,
			false,
			true
			);

	if (WaitEventTask)
	{
		WaitEventTask->EventReceived.AddDynamic(this, &USFGA_Dragon_Stomp::EmitShockWave);
		WaitEventTask->ReadyForActivation();
	}

}


void USFGA_Dragon_Stomp::EmitShockWave(FGameplayEventData Payload)
{
	ASFCharacterBase* Dragon = GetSFCharacterFromActorInfo();
	if (!Dragon)
	{
		return;
	}
	
	const FHitResult* HitResult = Payload.ContextHandle.GetHitResult();
	if (!HitResult)
	{
		return;
	}

	FVector StompLoc = HitResult->Location;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetAvatarActorFromActorInfo());

	bool bHit = Dragon->GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		StompLoc,
		FQuat::Identity,
		ECollisionChannel::ECC_Pawn,
		FCollisionShape::MakeSphere(ShockwaveRadius),
		QueryParams
	);

	
	if (bIsDebug)
	{
		DrawDebugSphere(
			Dragon->GetWorld(),
			StompLoc,
			ShockwaveRadius,
			32,
			FColor::Red,
			false,
			3.0f,
			0,
			2.0f
		);
	}

	if (bHit)
	{

		FGameplayEffectContextHandle EffectContext =
			MakeEffectContext(CurrentSpecHandle, CurrentActorInfo);

		for (const FOverlapResult& Overlap : OverlapResults)
		{
			AActor* HitActor = Overlap.GetActor();

			if (HitActor && GetAttitudeTowards(HitActor) == ETeamAttitude::Hostile)
			{
				ApplyDamageToTarget(HitActor, EffectContext);
				ApplyKnockBackToTarget(HitActor, StompLoc);

				if (bIsDebug)
				{
					DrawDebugLine(
						Dragon->GetWorld(),
						StompLoc,
						HitActor->GetActorLocation(),
						FColor::Green,
						false,
						3.0f,
						0,
						2.0f
					);
				}
			}
		}
	}
	if (CurrentActorInfo->IsNetAuthority())
	{
		FGameplayCueParameters CueParams;
		CueParams.EffectCauser = GetAvatarActorFromActorInfo();
		CueParams.Location = StompLoc; 
		CueParams.RawMagnitude = ShockwaveRadius;
		CueParams.Instigator = GetAvatarActorFromActorInfo();
		
		FireGameplayCueWithCosmetic_Static(SFGameplayTags::GameplayCue_Dragon_Stomp, CueParams);
	}
}

void USFGA_Dragon_Stomp::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void USFGA_Dragon_Stomp::OnMontageInterrupted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void USFGA_Dragon_Stomp::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

float USFGA_Dragon_Stomp::CalcScoreModifier(const FEnemyAbilitySelectContext& Context) const
{
	float Modifier = 0.f;

	const FBossEnemyAbilitySelectContext* BossContext =
		static_cast<const FBossEnemyAbilitySelectContext*>(&Context);

	if (!BossContext) return Modifier;

	// 근접 거리 전용
	if (BossContext->Zone == EBossAttackZone::Melee)
	{
		Modifier += 1000.f;
	}
	else if (BossContext->Zone == EBossAttackZone::Mid)
	{
		Modifier += 300.f;
	}
	else
	{
		return -9999.f;
	}

	// 플레이어 체력이 높을 때 견제용
	if (BossContext->PlayerHealthPercentage > 0.6f)
	{
		Modifier += 200.f;
	}

	return Modifier;
}

