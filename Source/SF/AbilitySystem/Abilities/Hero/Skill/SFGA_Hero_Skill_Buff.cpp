#include "SFGA_Hero_Skill_Buff.h"

#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimInstance.h"

#include "Character/SFCharacterBase.h"
#include "AbilitySystem/SFAbilitySystemComponent.h"

USFGA_Hero_Skill_Buff::USFGA_Hero_Skill_Buff(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}


//=========================ActivateAbility=========================
//스킬 시작 → 몽타주 재생 → GameplayEvent 대기
void USFGA_Hero_Skill_Buff::ActivateAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle,ActorInfo,ActivationInfo,TriggerEventData);

	//활성 조건 확인
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle,ActorInfo,ActivationInfo,true,true);
		return;
	}

	//몽타주를 AbilityTask 방식으로 재생
	if (BuffMontage && ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		auto* Task = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,NAME_None,BuffMontage,1.f,NAME_None,false,1.f);

		if (Task)
			Task->ReadyForActivation();
	}

	//GameplayEvent 대기(Notify 신호 받고 실행)
	if (StartEventTag.IsValid())
	{
		auto* Wait = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, StartEventTag);
		Wait->EventReceived.AddDynamic(this,&USFGA_Hero_Skill_Buff::OnReceivedSkillEvent);
		Wait->ReadyForActivation();
	}
}
//================================================================

//=======================Event Received===========================
//Notify → GameplayEvent → GroundCue 실행 후 자식 스킬 로직 호출
void USFGA_Hero_Skill_Buff::OnReceivedSkillEvent(FGameplayEventData Payload)
{
	auto* ASC = CurrentActorInfo ? CurrentActorInfo->AbilitySystemComponent.Get() : nullptr;
	if (!ASC) return;

	if (GroundCueTag.IsValid())
		ASC->AddGameplayCue(GroundCueTag);

	OnSkillEventTriggered();
}

void USFGA_Hero_Skill_Buff::OnSkillEventTriggered_Implementation()
{
	//파생 클래스에서 로직 구현
}
//================================================================

//==========================ApplyAura=============================
//Aura 적용
void USFGA_Hero_Skill_Buff::ApplyAura(AActor* Target)
{
	if (!AuraCueTag.IsValid()) return;
	if (!Target->ActorHasTag("Player")) return; //핵심 필터

	auto* Char = Cast<ASFCharacterBase>(Target);
	if (!Char) return;

	auto* ASC = Cast<USFAbilitySystemComponent>(Char->GetAbilitySystemComponent());
	if (!ASC) return;

	auto Ctx = ASC->MakeEffectContext();
	Ctx.AddSourceObject(this);

	auto Spec = ASC->MakeOutgoingSpec(BuffEffectClass,BuffLevel,Ctx);
	if (!Spec.IsValid()) return;

	auto Handle = ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	if (!Handle.IsValid()) return;

	ASC->AddGameplayCue(AuraCueTag);
	ActiveAuraEffects.Add(Target, Handle);
}
//================================================================

//==========================RemoveAura============================
//Aura Cue 제거
void USFGA_Hero_Skill_Buff::RemoveAura(AActor* Target)
{
	if (!ActiveAuraEffects.Contains(Target)) return;

	auto* Char = Cast<ASFCharacterBase>(Target);
	if (!Char) return;

	auto* ASC = Cast<USFAbilitySystemComponent>(Char->GetAbilitySystemComponent());
	if (ASC)
	{
		const auto Handle = ActiveAuraEffects[Target];
		if (Handle.IsValid()) ASC->RemoveActiveGameplayEffect(Handle);
		if (AuraCueTag.IsValid()) ASC->RemoveGameplayCue(AuraCueTag);
	}

	ActiveAuraEffects.Remove(Target);
}
//================================================================

//==========================EndAbility============================
//FX 정리
void USFGA_Hero_Skill_Buff::EndAbility(
	const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled)
{
	auto* ASC = ActorInfo ? ActorInfo->AbilitySystemComponent.Get() : nullptr;

	if (ASC && GroundCueTag.IsValid())
		ASC->RemoveGameplayCue(GroundCueTag);

	TArray<AActor*> Keys;
	ActiveAuraEffects.GetKeys(Keys);
	for (auto* T : Keys)
		RemoveAura(T);

	ActiveAuraEffects.Empty();

	Super::EndAbility(Handle,ActorInfo,ActivationInfo,bReplicateEndAbility,bWasCancelled);
}
//================================================================
