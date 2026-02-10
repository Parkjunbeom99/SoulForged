// Fill out your copyright notice in the Description page of Project Settings.

#include "SFBossCombatState.h"
#include "SFBossPhaseData.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/SFAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/SFGameplayAbility.h"
#include "AbilitySystem/Abilities/Enemy/SFEnemyAbilityInitializer.h"
#include "AI/StateMachine/SFStateMachine.h"
#include "System/SFGameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SFBossCombatState)

void USFBossCombatState::OnEnter_Implementation()
{
	// BaseState의 OnEnter는 호출하지 않음 (BT 교체를 Phase 기반으로 처리)
	USFState::OnEnter_Implementation();

	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	// 초기 Phase 설정
	if (PhaseDataAsset)
	{
		CurrentPhase = PhaseDataAsset->GetInitialPhase();
		
		if (const FSFBossPhaseConfig* Config = PhaseDataAsset->GetPhaseConfig(CurrentPhase))
		{
			GivePhaseAbilities(*Config);
			ApplyPhaseEffect(*Config);
			StartCurrentPhaseBehavior();
		}
	}

	bIsTransitioning = false;

	UE_LOG(LogTemp, Log, TEXT("[%s] Boss Combat State Entered - Phase %d"), 
		*GetNameSafe(OwnerActor), CurrentPhase);
}

void USFBossCombatState::OnExit_Implementation()
{
	Super::OnExit_Implementation();

	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	ClearPhaseAbilities();

	// Phase Effect 제거
	if (ActivePhaseEffectHandle.IsValid())
	{
		if (USFAbilitySystemComponent* ASC = GetASC())
		{
			ASC->RemoveActiveGameplayEffect(ActivePhaseEffectHandle);
		}
		ActivePhaseEffectHandle.Invalidate();
	}
}

void USFBossCombatState::OnUpdate_Implementation(float DeltaTime)
{
	Super::OnUpdate_Implementation(DeltaTime);

	if (!OwnerActor || !OwnerActor->HasAuthority())
	{
		return;
	}

	// Phase 전환 중이 아닐 때만 체크
	if (!bIsTransitioning)
	{
		CheckPhaseTransition();
	}
}

void USFBossCombatState::OnResume_Implementation()
{
	Super::OnResume_Implementation();

	// Groggy에서 복귀 시 현재 Phase의 BT 재시작
	StartCurrentPhaseBehavior();

}

void USFBossCombatState::ForceTransitionToPhase(int32 NewPhase)
{
	if (NewPhase != CurrentPhase)
	{
		TransitionToPhase(NewPhase);
	}
}

void USFBossCombatState::CheckPhaseTransition()
{
	if (!PhaseDataAsset)
	{
		return;
	}

	float HealthPercent = GetHealthPercent();
	int32 TargetPhase = PhaseDataAsset->GetTargetPhaseForHealth(HealthPercent, CurrentPhase);

	if (TargetPhase > 0 && TargetPhase != CurrentPhase)
	{
		TransitionToPhase(TargetPhase);
	}
}

void USFBossCombatState::TransitionToPhase(int32 NewPhase)
{
	if (!PhaseDataAsset)
	{
		return;
	}

	const FSFBossPhaseConfig* NewConfig = PhaseDataAsset->GetPhaseConfig(NewPhase);
	if (!NewConfig)
	{
		UE_LOG(LogTemp, Warning, TEXT("[%s] Phase %d config not found!"), 
			*GetNameSafe(OwnerActor), NewPhase);
		return;
	}

	bIsTransitioning = true;

	UE_LOG(LogTemp, Log, TEXT("[%s] Phase Transition: %d -> %d"), 
		*GetNameSafe(OwnerActor), CurrentPhase, NewPhase);

	// 이전 Phase 어빌리티 정리
	ClearPhaseAbilities();

	// Phase 업데이트
	int32 OldPhase = CurrentPhase;
	CurrentPhase = NewPhase;

	// 새 Phase 어빌리티 부여
	GivePhaseAbilities(*NewConfig);

	// Phase Effect 적용
	ApplyPhaseEffect(*NewConfig);

	// 새 Phase BT 시작
	StartCurrentPhaseBehavior();

	bIsTransitioning = false;
}

void USFBossCombatState::StartCurrentPhaseBehavior()
{
	if (!PhaseDataAsset || !StateMachine)
	{
		return;
	}

	const FSFBossPhaseConfig* Config = PhaseDataAsset->GetPhaseConfig(CurrentPhase);
	if (!Config)
	{
		return;
	}

	if (Config->BehaviourTag.IsValid() && StateMachine->OnChangeTreeDelegate.IsBound())
	{
		StateMachine->OnChangeTreeDelegate.Broadcast(Config->BehaviourTag);
	}
}

void USFBossCombatState::GivePhaseAbilities(const FSFBossPhaseConfig& PhaseConfig)
{
	USFAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return;
	}

	USFGameInstance* GI = nullptr;
	if (OwnerActor && OwnerActor->GetWorld())
	{
		GI = Cast<USFGameInstance>(OwnerActor->GetWorld()->GetGameInstance());
	}

	for (const FSFPhaseAbilityConfig& AbilityConfig : PhaseConfig.PhaseAbilities)
	{
		if (!AbilityConfig.AbilityClass)
		{
			continue;
		}

		FGameplayAbilitySpec Spec(AbilityConfig.AbilityClass, 1, INDEX_NONE, OwnerActor);
		FGameplayAbilitySpecHandle Handle = ASC->GiveAbility(Spec);

		if (!Handle.IsValid())
		{
			continue;
		}

		// AbilityData 적용 
		if (GI)
		{
			if (USFGameplayAbility* CDO = Cast<USFGameplayAbility>(AbilityConfig.AbilityClass->GetDefaultObject()))
			{
				if (const FAbilityBaseData* Data = GI->FindAbilityData(CDO->GetAbilityID()))
				{
					if (FGameplayAbilitySpec* RealSpec = ASC->FindAbilitySpecFromHandle(Handle))
					{
						USFEnemyAbilityInitializer::ApplyAbilityData(*RealSpec, *Data);
					}
				}
			}
		}

		// 핸들 저장
		FGrantedBossAbility GrantedAbility;
		GrantedAbility.Handle = Handle;
		GrantedAbility.bClearOnExit = AbilityConfig.bClearOnExit;
		GrantedAbilityHandles.Add(GrantedAbility);

		// 즉시 활성화
		if (AbilityConfig.bActivateOnEnter)
		{
			ASC->TryActivateAbility(Handle);
		}
	}
}

void USFBossCombatState::ClearPhaseAbilities()
{
	USFAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return;
	}

	for (int32 i = GrantedAbilityHandles.Num() - 1; i >= 0; --i)
	{
		const FGrantedBossAbility& Granted = GrantedAbilityHandles[i];

		if (!Granted.bClearOnExit)
		{
			continue;
		}

		if (Granted.Handle.IsValid())
		{
			ASC->CancelAbilityHandle(Granted.Handle);
			ASC->ClearAbility(Granted.Handle);
		}

		GrantedAbilityHandles.RemoveAt(i);
	}
}

void USFBossCombatState::ApplyPhaseEffect(const FSFBossPhaseConfig& PhaseConfig)
{
	if (!PhaseConfig.PhaseEffect)
	{
		return;
	}

	USFAbilitySystemComponent* ASC = GetASC();
	if (!ASC)
	{
		return;
	}

	// 이전 Phase Effect 제거
	if (ActivePhaseEffectHandle.IsValid())
	{
		ASC->RemoveActiveGameplayEffect(ActivePhaseEffectHandle);
		ActivePhaseEffectHandle.Invalidate();
	}

	// 새 Phase Effect 적용
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	ContextHandle.AddSourceObject(OwnerActor);

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(
		PhaseConfig.PhaseEffect, 1.0f, ContextHandle);

	if (SpecHandle.IsValid())
	{
		ActivePhaseEffectHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	}
}

