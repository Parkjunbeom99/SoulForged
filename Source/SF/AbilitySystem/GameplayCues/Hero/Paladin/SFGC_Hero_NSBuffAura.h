#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "SFGC_Hero_NSBuffAura.generated.h"

class UNiagaraSystem;
class UNiagaraComponent;
class USoundBase;
class UAudioComponent;

/**
 * Aura Heal GameplayCue (각 플레이어에 적용되는 지속형 Buff Aura)
 * - OnActive  : 최초 1회 FX 생성
 * - WhileActive : 유지 (Tick 아님 → 성능 부담 없음)
 * - OnRemove : FX/Sound 제거 및 Cleanup
 */
UCLASS()
class SF_API ASFGC_Hero_NSBuffAura : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	ASFGC_Hero_NSBuffAura();

protected:

	/*================ Aura Niagara FX ================*/
	UPROPERTY(EditAnywhere, Category="Aura|VFX")
	UNiagaraSystem* AuraNiagaraFX;

	UPROPERTY(Transient)
	UNiagaraComponent* AuraComponent;


	/*================ Optional Loop Sound ================*/
	UPROPERTY(EditAnywhere, Category="Aura|Sound")
	USoundBase* AuraLoopSound;

	UPROPERTY(Transient)
	UAudioComponent* AuraAudioComp;


public:

	virtual bool OnActive_Implementation(
		AActor* Target,
		const FGameplayCueParameters& Parameters) override;

	//🔥 WhileActive 추가 (Aura 유지 목적)
	virtual bool WhileActive_Implementation(
		AActor* Target,
		const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(
		AActor* Target,
		const FGameplayCueParameters& Parameters) override;

};
