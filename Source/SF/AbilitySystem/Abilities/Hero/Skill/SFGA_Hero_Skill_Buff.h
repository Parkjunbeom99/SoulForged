#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SFGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "SFGA_Hero_Skill_Buff.generated.h"

class UGameplayEffect;
class UAnimMontage;

UCLASS(Abstract, Blueprintable)
class SF_API USFGA_Hero_Skill_Buff : public USFGameplayAbility
{
	GENERATED_BODY()

public:
	USFGA_Hero_Skill_Buff(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

//=========================Ability Data=========================
//스킬 공통 정보(몽타주/버프/레벨)
	UPROPERTY(EditDefaultsOnly, Category="SF|Animation")
	UAnimMontage* BuffMontage;

	UPROPERTY(EditDefaultsOnly, Category="SF|BuffEffect")
	TSubclassOf<UGameplayEffect> BuffEffectClass;

	UPROPERTY(EditDefaultsOnly, Category="SF|BuffEffect")
	int32 BuffLevel = 1;
//==============================================================


//=========================GameplayCue Tags=========================
//Skill Event → GroundCue → AuraCue 로 이어지는 구조
	UPROPERTY(EditDefaultsOnly, Category="SF|Cue")
	FGameplayTag StartEventTag;

	UPROPERTY(EditDefaultsOnly, Category="SF|Cue")
	FGameplayTag GroundCueTag;

	UPROPERTY(EditDefaultsOnly, Category="SF|Cue")
	FGameplayTag AuraCueTag;
//=================================================================


//=========================Aura Tracking=========================
//Aura가 적용된 Actor 목록 및 GE Handle 저장
	UPROPERTY()
	TMap<AActor*, FActiveGameplayEffectHandle> ActiveAuraEffects;
//================================================================


//==========================Skill Event==========================
//GameplayEvent를 수신하면 GroundCue 실행 후 로직 호출
	UFUNCTION()
	void OnReceivedSkillEvent(FGameplayEventData Payload);

	UFUNCTION(BlueprintNativeEvent)
	void OnSkillEventTriggered();
	virtual void OnSkillEventTriggered_Implementation();
//================================================================


//=======================Aura Process============================
//Player 태그가 있는 Actor만 Aura 및 GE 적용
	virtual void ApplyAura(AActor* Target);
	virtual void RemoveAura(AActor* Target);
//================================================================


public:

//=======================Ability Lifecycle=======================
//활성화 & 종료(몽타주 재생/큐 제거/Aura 제거 처리)
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;
//===============================================================
};
