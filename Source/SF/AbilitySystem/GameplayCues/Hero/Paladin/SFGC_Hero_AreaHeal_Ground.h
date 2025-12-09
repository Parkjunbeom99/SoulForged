#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "SFGC_Hero_AreaHeal_Ground.generated.h"

class UParticleSystem;
class UParticleSystemComponent;
class USoundBase;
class UAudioComponent;
class UNiagaraSystem;
class UNiagaraComponent;

/**
 * AreaHeal Ground GameplayCue
 * - OnActive : 바닥 위치에 Particle/Niagara 중 존재하는 FX 스폰 + 사운드 재생
 * - OnRemove : 컴포넌트 정리
 */
UCLASS()
class SF_API ASFGC_Hero_AreaHeal_Ground : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:
	ASFGC_Hero_AreaHeal_Ground();

protected:

	/* Casscade Particle */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SF|AreaHeal|VFX")
	UParticleSystem* GroundCascade;

	/* Niagara System */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SF|AreaHeal|VFX")
	UNiagaraSystem* GroundNiagara;

	/* Sound */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="SF|AreaHeal|Audio")
	USoundBase* GroundSound;

	/* Runtime spawned components */
	UPROPERTY(Transient)
	UParticleSystemComponent* SpawnedCascadeComp;

	UPROPERTY(Transient)
	UNiagaraComponent* SpawnedNiagaraComp;

	UPROPERTY(Transient)
	UAudioComponent* SpawnedAudioComp;

public:
	virtual bool OnActive_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(
		AActor* MyTarget,
		const FGameplayCueParameters& Parameters) override;
};
