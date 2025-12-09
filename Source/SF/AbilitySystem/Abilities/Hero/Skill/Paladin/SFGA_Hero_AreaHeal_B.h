#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Hero/Skill/SFGA_Hero_Skill_Buff.h"
#include "SFGA_Hero_AreaHeal_B.generated.h"

UCLASS()
class SF_API USFGA_Hero_AreaHeal_B : public USFGA_Hero_Skill_Buff
{
	GENERATED_BODY()

public:
	USFGA_Hero_AreaHeal_B(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	//=========================Invincible Config=========================
	//무적 장판 범위, 지속시간, 체크 간격
	UPROPERTY(EditDefaultsOnly, Category="SF|Invincible")
	float InvincibleRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, Category="SF|Invincible")
	float AreaDuration = 7.f;

	UPROPERTY(EditDefaultsOnly, Category="SF|Invincible")
	float Interval = 0.5f;
	//==================================================================


	//=========================Runtime Data=========================
	//스킬 발동 기준 위치와 타이머 핸들
	UPROPERTY()
	FVector Origin;

	FTimerHandle TickHandle;
	FTimerHandle DurationHandle;
	//==============================================================


	//======================스킬 로직 Override======================
	//GameplayEvent 수신 후 실제 무적 장판 발동
	virtual void OnSkillEventTriggered_Implementation() override;

	void TickInvincible(); //범위 내 Player에게 무적 GE 적용 or 해제
	void EndDuration(); //지속시간 종료 시 호출
	//==============================================================

public:

	//=======================Ability Lifecycle======================
	//Ability 종료 시 타이머 정리 후 부모 처리 호출
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,bool bWasCancelled) override;
	//==============================================================
};
