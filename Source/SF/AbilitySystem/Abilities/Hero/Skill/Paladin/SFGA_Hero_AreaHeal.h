#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Hero/Skill/SFGA_Hero_Skill_Buff.h"
#include "SFGA_Hero_AreaHeal.generated.h"

UCLASS()
class SF_API USFGA_Hero_AreaHeal : public USFGA_Hero_Skill_Buff
{
	GENERATED_BODY()

public:
	USFGA_Hero_AreaHeal(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	//=========================Heal Config=========================
	//범위, 지속시간, 틱 간격
	UPROPERTY(EditDefaultsOnly, Category="SF|Heal")
	float HealRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, Category="SF|Heal")
	float AreaDuration = 7.f;

	UPROPERTY(EditDefaultsOnly, Category="SF|Heal")
	float Interval = 0.5f;
	//=============================================================


	//=========================Runtime Data=========================
	//스킬 발동 위치(고정) & Timer 핸들
	UPROPERTY()
	FVector Origin;

	FTimerHandle TickHandle;
	FTimerHandle DurationHandle;
	//=============================================================


	//======================스킬 로직 Override=====================
	//부모 Event 수신 후 실제 AreaHeal 발동
	virtual void OnSkillEventTriggered_Implementation() override;

	void TickHeal(); //영역 안 캐릭터 스캔 + Aura & 힐 로직 처리
	void EndDuration(); //지속시간 종료시 수행 로직
	//==============================================================

public:

	//=======================Ability Lifecycle=======================
	//Ability 종료 시 Timer 정리 및 부모 처리
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,bool bWasCancelled) override;
	//==============================================================
};
