#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "SFGC_SkillPhaseFX.generated.h"

class USFDA_SkillPhaseFX;

//================== 스킬 페이즈 FX GameplayCue ==================
// - 하나의 Cue 안에서 여러 페이즈(CastStart/Loop/Activate)를 처리
// - AnimNotify에서 Phase를 넘겨주면
//   DataAsset에서 해당 페이즈 FX 세트를 꺼내서 재생
//===============================================================
UCLASS()
class SF_API USFGC_SkillPhaseFX : public UGameplayCueNotify_Static
{
	GENERATED_BODY()

public:
	//GameplayCue 진입 지점
	virtual void HandleGameplayCue(AActor* Target,
								   EGameplayCueEvent::Type EventType,
								   const FGameplayCueParameters& Parameters) override;

protected:
	//================== 내부 헬퍼: 스폰 위치 계산 ==================
	// - 캐릭터 발 위치 기준으로 아래로 라인트레이스
	// - 막힌 곳이 있으면 ImpactPoint 사용
	// - 없으면 캐릭터 위치 사용
	//============================================================
	FVector GetFloorLocationForActor(AActor* Target) const;
};
