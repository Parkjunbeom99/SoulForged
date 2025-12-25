#pragma once

#include "CoreMinimal.h"
#include "SFCameraMode_ThirdPerson.h"
#include "SFCameraMode_Spectator.generated.h"

/**
 * USFCameraMode_Spectator
 * 관전 모드 전용 카메라
 * 관전자(SpectatorPawn) 자체가 아닌, 관전 대상(FollowTarget)의 캡슐 크기를 기준으로
 * 벽 충돌(Penetration)을 계산하도록 오버라이드
 */
UCLASS()
class SF_API USFCameraMode_Spectator : public USFCameraMode_ThirdPerson
{
	GENERATED_BODY()

public:
	USFCameraMode_Spectator();

protected:
	// 벽 뚫림 방지 로직 재정의
	virtual void UpdatePreventPenetration(float DeltaTime) override;
};
