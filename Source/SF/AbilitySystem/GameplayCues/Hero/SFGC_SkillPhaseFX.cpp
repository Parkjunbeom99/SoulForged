#include "SFGC_SkillPhaseFX.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "SFSkillFXTypes.h"
#include "AbilitySystem/GamePlayCues/Hero/SFSkillFXTypes.h"

//================== 바닥 위치 계산 ==================
FVector USFGC_SkillPhaseFX::GetFloorLocationForActor(AActor* Target) const
{
	if (!Target)
	{
		return FVector::ZeroVector;
	}

	UWorld* World = Target->GetWorld();
	if (!World)
	{
		return Target->GetActorLocation();
	}

	const FVector ActorLocation = Target->GetActorLocation();
	const FVector Start = ActorLocation + FVector(0.f, 0.f, 50.f);
	const FVector End   = ActorLocation - FVector(0.f, 0.f, 1000.f);

	FHitResult HitResult;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(SFGC_SkillPhaseFX_FloorTrace), false, Target);

	if (World->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		return HitResult.ImpactPoint;
	}

	//바닥을 못 찾으면 캐릭터 위치 사용
	return ActorLocation;
}

//================== GameplayCue 처리 ==================
// - EventType: 주로 Executed를 사용(AnimNotify에서 호출할 때)
// - Parameters.SourceObject: USFDA_SkillPhaseFX*
// - Parameters.RawMagnitude: ESFSkillFXPhase를 float로 변환한 값
//======================================================
void USFGC_SkillPhaseFX::HandleGameplayCue(AActor* Target,
                                           EGameplayCueEvent::Type EventType,
                                           const FGameplayCueParameters& Parameters)
{
	//AnimNotify에서 주로 Executed로 들어올 것을 예상
	if (EventType != EGameplayCueEvent::Executed &&
	    EventType != EGameplayCueEvent::OnActive &&
	    EventType != EGameplayCueEvent::WhileActive)
	{
		return;
	}

	if (!Target)
	{
		return;
	}

	//FX DataAsset 가져오기
	const USFDA_SkillPhaseFX* FXData = Cast<USFDA_SkillPhaseFX>(Parameters.SourceObject);
	if (!FXData)
	{
		return;
	}

	//Phase 복원
	const int32 PhaseIndex = FMath::RoundToInt(Parameters.RawMagnitude);
	ESFSkillFXPhase Phase = ESFSkillFXPhase::CastStart;

	switch (PhaseIndex)
	{
	case static_cast<int32>(ESFSkillFXPhase::CastStart):
		Phase = ESFSkillFXPhase::CastStart;
		break;
	case static_cast<int32>(ESFSkillFXPhase::CastLoop):
		Phase = ESFSkillFXPhase::CastLoop;
		break;
	case static_cast<int32>(ESFSkillFXPhase::Activate):
	default:
		Phase = ESFSkillFXPhase::Activate;
		break;
	}

	//해당 페이즈 FX 세트 가져오기
	const FSFSkillPhaseFX& PhaseFX = FXData->GetFXForPhase(Phase);

	UWorld* World = Target->GetWorld();
	if (!World)
	{
		return;
	}

	//스폰 위치: 캐릭터 발밑 바닥
	const FVector SpawnLocation = GetFloorLocationForActor(Target);
	const FRotator SpawnRotation = Target->GetActorRotation();

	//나이아가라 FX
	if (PhaseFX.NiagaraSystem)
	{
		auto NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			World,
			PhaseFX.NiagaraSystem,
			SpawnLocation,
			Target->GetActorRotation(),
			PhaseFX.FXScale             // << 🔥 크기 적용
		);
	}

	//캐스케이드 파티클 FX
	if (PhaseFX.CascadeSystem)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			World,
			PhaseFX.CascadeSystem,
			SpawnLocation,
			Target->GetActorRotation(),
			PhaseFX.FXScale             // << 🔥 크기 적용
		);
	}

	//사운드 FX
	if (PhaseFX.Sound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			World,
			PhaseFX.Sound,
			SpawnLocation,
			PhaseFX.SoundVolume         // << 🔥 볼륨 적용
		);
	}
}
