#include "SFGA_Hero_AreaHeal_B.h"

#include "Kismet/GameplayStatics.h"
#include "Character/SFCharacterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SFGA_Hero_AreaHeal_B)

USFGA_Hero_AreaHeal_B::USFGA_Hero_AreaHeal_B(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StartEventTag = FGameplayTag::RequestGameplayTag("Event.Skill.AreaHeal");
	GroundCueTag  = FGameplayTag::RequestGameplayTag("GameplayCue.Skill.AreaHeal.Ground.B");
	AuraCueTag    = FGameplayTag::RequestGameplayTag("GameplayCue.Buff.Invincible");
}



//===================OnSkillEventTriggered===================
//GameplayEvent 수신 후 실행
void USFGA_Hero_AreaHeal_B::OnSkillEventTriggered_Implementation()
{
	//스킬 기준 위치 설정
	if(CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid())
		Origin = CurrentActorInfo->AvatarActor->GetActorLocation();

	//즉시 1틱 실행
	TickInvincible();

	//주기적 Tick
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TickHandle,this,&USFGA_Hero_AreaHeal_B::TickInvincible,Interval,true);

		World->GetTimerManager().SetTimer(
			DurationHandle,this,&USFGA_Hero_AreaHeal_B::EndDuration,AreaDuration,false);
	}
}
//===========================================================



//===========================TickInvincible===========================
//범위 내 Actor 검색 → Player 태그 있는 대상만 무적 GE 유지
void USFGA_Hero_AreaHeal_B::TickInvincible()
{
	UWorld* World = GetWorld();
	if(!World) return;

	TArray<AActor*> All;
	UGameplayStatics::GetAllActorsOfClass(World,ASFCharacterBase::StaticClass(),All);

	TSet<AActor*> Inside;

	//범위 내 대상 수집
	const float RadiusSq = InvincibleRadius * InvincibleRadius;

	for(AActor* A : All)
	{
		if(!A) continue;

		if(FVector::DistSquared(Origin,A->GetActorLocation()) <= RadiusSq)
			Inside.Add(A);
	}

	//무적 Aura 적용
	for(AActor* Target : Inside)
	{
		//Player 태그 없는 Actor는 무시
		if(!Target->ActorHasTag("Player"))
			continue;

		//이미 효과가 적용되어 있으면 스킵
		if(ActiveAuraEffects.Contains(Target))
			continue;
		
		ApplyAura(Target);
	}

	//무적 Aura 제거
	TArray<AActor*> RemoveList;
	for(auto& Pair : ActiveAuraEffects)
	{
		AActor* Target = Pair.Key;
		if(!Target) continue;

		//범위 밖이면 제거
		if(!Inside.Contains(Target))
			RemoveList.Add(Target);
	}

	for(AActor* Target : RemoveList)
	{
		RemoveAura(Target);
	}
}
//===================================================================



//============================EndDuration============================
//지속시간 만료 → Ability 종료
void USFGA_Hero_AreaHeal_B::EndDuration()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
//===================================================================



//==============================EndAbility===========================
//타이머 정리 + Aura/GE/GameplayCue 정리
void USFGA_Hero_AreaHeal_B::EndAbility(
	const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,bool bWasCancelled)
{
	if(UWorld* World = GetWorld())
		World->GetTimerManager().ClearAllTimersForObject(this);

	Super::EndAbility(Handle,ActorInfo,ActivationInfo,bReplicateEndAbility,bWasCancelled);
}
//===================================================================
