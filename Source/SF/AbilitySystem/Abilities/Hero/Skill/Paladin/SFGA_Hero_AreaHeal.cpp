#include "SFGA_Hero_AreaHeal.h"
#include "Kismet/GameplayStatics.h"
#include "Character/SFCharacterBase.h"

USFGA_Hero_AreaHeal::USFGA_Hero_AreaHeal(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	StartEventTag = FGameplayTag::RequestGameplayTag("Event.Skill.AreaHeal");
	GroundCueTag = FGameplayTag::RequestGameplayTag("GameplayCue.Skill.AreaHeal.Ground");
	AuraCueTag   = FGameplayTag::RequestGameplayTag("GameplayCue.Buff.Heal");
}


//===================OnSkillEventTriggered===================
//GameplayEvent 수신 후 실행됨. AreaHeal 시작 구간
void USFGA_Hero_AreaHeal::OnSkillEventTriggered_Implementation()
{
	//스킬 위치 = 시작 기준 위치를 사용
	Origin = CurrentActorInfo->AvatarActor->GetActorLocation();

	//즉시 1틱 실행
	TickHeal();

	//주기적 Tick
	GetWorld()->GetTimerManager().SetTimer(
		TickHandle,this,&USFGA_Hero_AreaHeal::TickHeal,Interval,true);

	//지속시간 후 종료
	GetWorld()->GetTimerManager().SetTimer(
		DurationHandle,this,&USFGA_Hero_AreaHeal::EndDuration,AreaDuration,false);
}
//===========================================================

//============================TickHeal============================
//범위 내 Actor 검색 → Player 태그 있는 대상만 Aura & 힐 유지(ActorTag 기반)
void USFGA_Hero_AreaHeal::TickHeal()
{
	TArray<AActor*> All;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(),ASFCharacterBase::StaticClass(),All);

	TSet<AActor*> Inside;

	//범위 내 대상 수집
	for(auto* A : All)
		if(FVector::DistSquared(Origin,A->GetActorLocation()) <= HealRadius*HealRadius)
			Inside.Add(A);


	//Aura 적용
	for(auto* Target : Inside)
	{
		//Player 태그 없는 Actor는 무시
		if(!Target->ActorHasTag("Player"))
			continue;

		//Aura 없으면 적용
		if(!ActiveAuraEffects.Contains(Target))
			ApplyAura(Target);
	}

	//Aura 제거
	TArray<AActor*> RemoveList;
	for(auto& P : ActiveAuraEffects)
		if(!Inside.Contains(P.Key))
			RemoveList.Add(P.Key);

	for(auto* T : RemoveList)
		RemoveAura(T);
}
//===============================================================



//============================EndDuration=========================
//지속시간 만료 → Skill 종료
void USFGA_Hero_AreaHeal::EndDuration()
{
	EndAbility(CurrentSpecHandle,CurrentActorInfo,CurrentActivationInfo,true,false);
}
//===============================================================



//==============================EndAbility========================
//타이머 정리 + 부모 정리까지 수행
void USFGA_Hero_AreaHeal::EndAbility(
	const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,bool bWasCancelled)
{
	GetWorld()->GetTimerManager().ClearAllTimersForObject(this);
	Super::EndAbility(Handle,ActorInfo,ActivationInfo,bReplicateEndAbility,bWasCancelled);
}
//===============================================================
