#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Hero/Skill/SFGA_Hero_Skill_Buff.h"
#include "SFGA_Hero_AreaHeal_A.generated.h"

UCLASS()
class SF_API USFGA_Hero_AreaHeal_A : public USFGA_Hero_Skill_Buff
{
	GENERATED_BODY()

public:
	USFGA_Hero_AreaHeal_A(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	//=========================Buff Config=========================
	//범위/지속시간/틱 간격
	UPROPERTY(EditDefaultsOnly, Category="SF|Buff")
	float BuffRadius = 600.f;

	UPROPERTY(EditDefaultsOnly, Category="SF|Buff")
	float BuffDuration = 7.f;

	UPROPERTY(EditDefaultsOnly, Category="SF|Buff")
	float TickInterval = 0.5f;
	//=============================================================


	//=========================Runtime Data========================
	//시전 위치/타이머 핸들
	UPROPERTY()
	FVector Origin;

	FTimerHandle TickHandle;
	FTimerHandle DurationHandle;
	//=============================================================


	//======================Skill Logic Override===================
	//부모에서 Event 수신 후 실제 버프 영역 시작
	virtual void OnSkillEventTriggered_Implementation() override;

	void TickBuff();      //범위 내 Player 태그 Actor에게 버프 적용/해제
	void EndDuration();   //지속시간 종료 → Ability 종료 호출
	//=============================================================

public:

	//=======================Ability Lifecycle=====================
	//Ability 종료 시 타이머 정리 후 부모 처리 호출
	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,bool bWasCancelled) override;
	//=============================================================
};
