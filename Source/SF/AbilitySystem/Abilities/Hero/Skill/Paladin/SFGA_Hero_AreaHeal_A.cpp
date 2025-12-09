#include "SFGA_Hero_AreaHeal_A.h"

#include "Kismet/GameplayStatics.h"
#include "Character/SFCharacterBase.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SFGA_Hero_AreaHeal_A)

USFGA_Hero_AreaHeal_A::USFGA_Hero_AreaHeal_A(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StartEventTag = FGameplayTag::RequestGameplayTag("Event.Skill.AreaHeal");
	GroundCueTag  = FGameplayTag::RequestGameplayTag("GameplayCue.Skill.AreaHeal.Ground.A");
	AuraCueTag    = FGameplayTag::RequestGameplayTag("GameplayCue.Buff.CriticalChance");
}


//===================OnSkillEventTriggered===================
//애니 Notify → GameplayEvent 수신 후 버프 영역 시작
void USFGA_Hero_AreaHeal_A::OnSkillEventTriggered_Implementation()
{
	//시전 기준 위치 저장
	if(CurrentActorInfo && CurrentActorInfo->AvatarActor.IsValid())
		Origin = CurrentActorInfo->AvatarActor->GetActorLocation();

	//즉시 1틱 수행
	TickBuff();

	//주기적 Tick 시작
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			TickHandle,this,&USFGA_Hero_AreaHeal_A::TickBuff,
			TickInterval,true);

		World->GetTimerManager().SetTimer(
			DurationHandle,this,&USFGA_Hero_AreaHeal_A::EndDuration,
			BuffDuration,false);
	}
}
//===========================================================

//============================TickBuff========================
//범위 내 Player 태그 Actor만 버프 유지(벗어나면 해제)
void USFGA_Hero_AreaHeal_A::TickBuff()
{
	UWorld* World = GetWorld();
	if(!World) return;

	TArray<AActor*> AllActors;
	UGameplayStatics::GetAllActorsOfClass(World, ASFCharacterBase::StaticClass(), AllActors);

	const float RadiusSq = BuffRadius * BuffRadius;

	//범위 안에 있는 Actor들
	TSet<AActor*> InsideActors;

	for(AActor* Actor : AllActors)
	{
		if(!Actor) continue;

		//거리 체크
		if(FVector::DistSquared(Origin, Actor->GetActorLocation()) > RadiusSq)
			continue;

		InsideActors.Add(Actor);
	}

	//범위 안 대상 처리
	for(AActor* Target : InsideActors)
	{
		//Player 태그 없는 대상은 버프 적용 X
		if(!Target->ActorHasTag("Player"))
			continue;

		//이미 Aura(버프)가 적용된 대상이면 패스
		if(ActiveAuraEffects.Contains(Target))
			continue;

		//부모 공용 로직으로 버프+Aura 이펙트 적용
		ApplyAura(Target);
	}

	//범위 밖으로 나간 대상 처리
	TArray<AActor*> RemoveList;

	for(auto& Pair : ActiveAuraEffects)
	{
		AActor* Target = Pair.Key;
		if(!Target) continue;

		//이번 Tick 기준 범위 안에 없으면 제거 대상
		if(!InsideActors.Contains(Target))
			RemoveList.Add(Target);
	}

	for(AActor* Target : RemoveList)
	{
		RemoveAura(Target);
	}
}
//===========================================================

//============================EndDuration=====================
//버프 지속시간이 끝나면 Ability 종료
void USFGA_Hero_AreaHeal_A::EndDuration()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
//===========================================================

//==============================EndAbility====================
//타이머 정리 후 부모 EndAbility 로 Aura & FX 정리
void USFGA_Hero_AreaHeal_A::EndAbility(
	const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,bool bWasCancelled)
{
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(TickHandle);
		World->GetTimerManager().ClearTimer(DurationHandle);
	}

	Super::EndAbility(Handle,ActorInfo,ActivationInfo,bReplicateEndAbility,bWasCancelled);
}
//===========================================================