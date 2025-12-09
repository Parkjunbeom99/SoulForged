#include "SFGC_Hero_NSBuffAura.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SFGC_Hero_NSBuffAura)

ASFGC_Hero_NSBuffAura::ASFGC_Hero_NSBuffAura()
{
	PrimaryActorTick.bCanEverTick = false;

	AuraComponent = nullptr;
	AuraAudioComp = nullptr;

	// WhileActive 사용 시 Recommended
	bAutoDestroyOnRemove = true;
}

bool ASFGC_Hero_NSBuffAura::OnActive_Implementation(
	AActor* Target,
	const FGameplayCueParameters& Parameters)
{
	if(!Target) return false;

	//======= 이미 Aura가 있을 경우 다시 생성 금지 (중복 방지 핵심) =======
	if(AuraComponent && AuraComponent->IsActive())
		return false; //WhileActive 유지중이므로 새로운 생성 X

	USceneComponent* AttachComp = nullptr;

	if(ACharacter* Char = Cast<ACharacter>(Target))
		AttachComp = Char->GetMesh();
	else
		AttachComp = Target->GetRootComponent();


	//============ Niagara Aura Spawn ============//
	if(AuraNiagaraFX)
	{
		AuraComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			AuraNiagaraFX,
			AttachComp,
			NAME_None,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			false
		);
	}

	//============ Optional Loop Sound ============//
	if(AuraLoopSound)
	{
		AuraAudioComp = UGameplayStatics::SpawnSoundAttached(
			AuraLoopSound,
			AttachComp
		);
		AuraAudioComp->bAutoDestroy = false;
	}

	return true;
}

bool ASFGC_Hero_NSBuffAura::WhileActive_Implementation(
	AActor* Target,
	const FGameplayCueParameters& Parameters)
{
	//🔥 아무 것도 안함 = 유지 목적
	//Tick 아님 → 성능 부담 없음
	return true;
}

bool ASFGC_Hero_NSBuffAura::OnRemove_Implementation(
	AActor* Target,
	const FGameplayCueParameters& Parameters)
{
	if(AuraComponent)
	{
		AuraComponent->Deactivate();
		AuraComponent->DestroyComponent();
		AuraComponent = nullptr;
	}

	if(AuraAudioComp)
	{
		AuraAudioComp->Stop();
		AuraAudioComp->DestroyComponent();
		AuraAudioComp = nullptr;
	}

	return true;
}
