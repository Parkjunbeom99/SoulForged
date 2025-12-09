#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "NiagaraSystem.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundBase.h"
#include "SFSkillFXTypes.generated.h"

//================== 스킬 FX 페이즈 ==================
// - 한 스킬 내에서 FX 타이밍을 구분하기 위한 단계
// - AnimNotify에서 이 값을 지정해서 Cue로 전달
//====================================================
UENUM(BlueprintType)
enum class ESFSkillFXPhase : uint8
{
	CastStart  UMETA(DisplayName="Cast Start"),	 //캐스팅 시작
	CastLoop   UMETA(DisplayName="Cast Loop"),	 //캐스팅 유지(루프)
	Activate   UMETA(DisplayName="Activate")	 //실제 발동/임팩트
};

//================== 페이즈별 FX 세트 ==================
// - Niagara / Cascade / Sound 세 가지를 하나의 묶음으로 관리
// - 필요없는 것은 비워두면 됨(Null이면 해당 FX 미사용)
//======================================================
USTRUCT(BlueprintType)
struct FSFSkillPhaseFX
{
	GENERATED_BODY()

	//나이아가라 FX
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	TObjectPtr<UNiagaraSystem> NiagaraSystem = nullptr;

	//캐스케이드 파티클 FX
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	TObjectPtr<UParticleSystem> CascadeSystem = nullptr;

	//사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	TObjectPtr<USoundBase> Sound = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX|Control")
	FVector FXScale = FVector(1.f,1.f,1.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX|Control")
	float SoundVolume = 1.f;
};

//================== 스킬 페이즈 FX 데이터 에셋 ==================
// - 한 스킬의 CastStart / CastLoop / Activate FX 구성을 모두 보관
// - AnimNotify에서 이 DataAsset을 지정해서 전달
//===============================================================
UCLASS(BlueprintType)
class SF_API USFDA_SkillPhaseFX : public UDataAsset
{
	GENERATED_BODY()

public:
	//CastStart 페이즈 FX 세트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	FSFSkillPhaseFX CastStartFX;

	//CastLoop 페이즈 FX 세트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	FSFSkillPhaseFX CastLoopFX;

	//Activate 페이즈 FX 세트
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FX")
	FSFSkillPhaseFX ActivateFX;

public:
	//================== 페이즈별 FX 조회 헬퍼 ==================
	// - 외부에서는 Phase만 넘겨주면 알아서 해당 세트 반환
	//==========================================================
	UFUNCTION(BlueprintCallable, Category="FX")
	const FSFSkillPhaseFX& GetFXForPhase(ESFSkillFXPhase Phase) const
	{
		switch (Phase)
		{
		case ESFSkillFXPhase::CastStart:
			return CastStartFX;
		case ESFSkillFXPhase::CastLoop:
			return CastLoopFX;
		case ESFSkillFXPhase::Activate:
		default:
			return ActivateFX;
		}
	}
};
