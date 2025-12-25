#include "SFCameraMode_Spectator.h"

#include "Pawn/SFSpectatorPawn.h"

USFCameraMode_Spectator::USFCameraMode_Spectator()
{
	
}

void USFCameraMode_Spectator::UpdatePreventPenetration(float DeltaTime)
{
    if (!bPreventPenetration)
    {
        return;
    }

    AActor* TargetActor = GetTargetActor();
    if (!TargetActor)
    {
        return;
    }

    float TargetHalfHeight = TargetActor->GetSimpleCollisionHalfHeight();
    float TargetRadius = TargetActor->GetSimpleCollisionRadius();
    bool bUseManualCapsuleLogic = false;
    
    // 충돌 레이캐스트(Trace)에서 무시할 액터 (기본은 관전자 본인)
    AActor* ActorToIgnore = TargetActor;

    // 관전 대상(FollowTarget)의 캡슐 정보를 가져옴
    if (ASFSpectatorPawn* Spectator = Cast<ASFSpectatorPawn>(TargetActor))
    {
        if (AActor* FollowTarget = Spectator->GetFollowTarget())
        {
            TargetHalfHeight = FollowTarget->GetSimpleCollisionHalfHeight();
            TargetRadius = FollowTarget->GetSimpleCollisionRadius();
            bUseManualCapsuleLogic = true;

            // 카메라가 '관전 대상(Hero)'의 몸체에 부딪히지 않도록 무시 목록 교체
            ActorToIgnore = FollowTarget;
        }
    }

    // '안전 위치(SafeLocation)'를 캡슐 내부로 설정
    FVector ClosestPointOnLineToCapsuleCenter;
    FVector SafeLocation = TargetActor->GetActorLocation();
    FMath::PointDistToLine(SafeLocation, View.Rotation.Vector(), View.Location, ClosestPointOnLineToCapsuleCenter);

    // '안전 위치'가 캐릭터 캡슐 높이 범위 내에 있도록 조정
    float const PushInDistance = PenetrationAvoidanceFeelers[0].Extent + CollisionPushOutDistance;
    float const MaxHalfHeight = TargetHalfHeight - PushInDistance;
    
    SafeLocation.Z = FMath::Clamp(ClosestPointOnLineToCapsuleCenter.Z, SafeLocation.Z - MaxHalfHeight, SafeLocation.Z + MaxHalfHeight);

    // 캡슐 표면 계산
    if (bUseManualCapsuleLogic)
    {
        // 관전 중: 실제 대상의 Radius를 사용하여 가상의 표면 위치 계산
        FVector ToPoint = ClosestPointOnLineToCapsuleCenter - SafeLocation;
        ToPoint.Z = 0.f;

        if (!ToPoint.IsNearlyZero())
        {
            SafeLocation += ToPoint.GetSafeNormal() * TargetRadius;
        }
        else
        {
            SafeLocation += TargetActor->GetActorForwardVector() * TargetRadius;
        }
    }
    else if (const UPrimitiveComponent* TargetRoot = Cast<UPrimitiveComponent>(TargetActor->GetRootComponent()))
    {
        float DistanceSqr;
        TargetRoot->GetSquaredDistanceToCollision(ClosestPointOnLineToCapsuleCenter, DistanceSqr, SafeLocation);
    }

    // '안전 위치'를 캡슐 안쪽으로 살짝 밀어넣기
    if (PenetrationAvoidanceFeelers.Num() > 0)
    {
        SafeLocation += (SafeLocation - ClosestPointOnLineToCapsuleCenter).GetSafeNormal() * PushInDistance;
    }

    // ActorToIgnore(Hero)를 첫 번째 인자로 전달하여 캐릭터 충돌 무시
    bool const bSingleRayPenetrationCheck = !bDoPredictiveAvoidance;
    PreventCameraPenetration(*ActorToIgnore, SafeLocation, View.Location, DeltaTime, AimLineToDesiredPosBlockedPct, bSingleRayPenetrationCheck);
}
