#include "SFLockOnComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "SF/Character/SFCharacterBase.h" 
#include "SF/Interface/EnemyActorComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "SF/Character/SFCharacterGameplayTags.h" 

USFLockOnComponent::USFLockOnComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	
	// 기본값 설정
	TargetTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Type.Enemy"))); 
}

void USFLockOnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CurrentTarget)
	{
		if (!IsTargetValid(CurrentTarget))
		{
			EndLockOn();
		}
		else
		{
			// TODO: 여기서 컨트롤러 회전 업데이트 등을 수행할 수도 있음 (GAS 단계에서 처리 예정)
		}
	}
}

bool USFLockOnComponent::TryLockOn()
{
	// 1. 이미 락온 중이라면 해제 (Toggle 방식)
	if (CurrentTarget)
	{
		EndLockOn();
		return false; // 해제됨
	}

	// 2. 새로운 타겟 탐색
	AActor* NewTarget = FindBestTarget();
	if (NewTarget)
	{
		CurrentTarget = NewTarget;
		
		// 락온 상태 태그 적용
		if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerPawn))
			{
				ASC->AddLooseGameplayTag(SFGameplayTags::Character_State_LockedOn);
			}
		}

		// TODO: 락온 성공 사운드 재생
		return true; // 락온 성공
	}

	return false; // 타겟 없음 (이때 Controller에서 카메라 리셋 호출)
}

void USFLockOnComponent::EndLockOn()
{
	CurrentTarget = nullptr;

	// 락온 상태 태그 제거
	if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerPawn))
		{
			ASC->RemoveLooseGameplayTag(SFGameplayTags::Character_State_LockedOn);
		}
	}

	// TODO: 락온 해제 사운드
}

AActor* USFLockOnComponent::FindBestTarget()
{
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return nullptr;

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return nullptr;

	FVector CameraLocation;
	FRotator CameraRotation;
	PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
	FVector CameraForward = CameraRotation.Vector();

	// 1. 구체 충돌로 주변 액터 수집
	TArray<AActor*> OverlappedActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn)); // Pawn 채널 검색

	UKismetSystemLibrary::SphereOverlapActors(
		this,
		OwnerPawn->GetActorLocation(),
		LockOnDistance,
		ObjectTypes,
		ASFCharacterBase::StaticClass(), // 검색 클래스 필터
		{ OwnerPawn }, // 자기 자신 제외
		OverlappedActors
	);

	AActor* BestTarget = nullptr;
	float BestScore = -1.0f; // 점수가 높을수록 좋음

	for (AActor* Actor : OverlappedActors)
	{
		if (!IsTargetValid(Actor)) continue;

		// 2. 점수 계산 (Dot Product)
		FVector DirectionToTarget = (Actor->GetActorLocation() - CameraLocation).GetSafeNormal();
		float DotResult = FVector::DotProduct(CameraForward, DirectionToTarget);
		
		// 화면 뒤에 있거나 시야각을 벗어나면 제외 (예: 45도 = 0.707)
		if (DotResult < ScreenCenterWeight) continue;

		// 거리 가중치 (가까울수록 점수 +)
		float Distance = FVector::Dist(OwnerPawn->GetActorLocation(), Actor->GetActorLocation());
		float DistanceScore = 1.0f - (Distance / LockOnDistance); // 0~1

		// 최종 점수: 내적(중앙 정렬) 비중을 높게 설정
		float FinalScore = (DotResult * 2.0f) + DistanceScore;

		if (FinalScore > BestScore)
		{
			BestScore = FinalScore;
			BestTarget = Actor;
		}
	}

	return BestTarget;
}

bool USFLockOnComponent::IsTargetValid(AActor* TargetActor) const
{
	if (!TargetActor || !TargetActor->IsValidLowLevel()) return false;

	// 1. 사망 여부 확인 (인터페이스나 태그 사용)
	// 예시: if (TargetActor->Implements<USFHealthInterface>() && ISFHealthInterface::Execute_IsDead(TargetActor)) return false;
	
	// 2. 거리 확인
	float Distance = FVector::Dist(GetOwner()->GetActorLocation(), TargetActor->GetActorLocation());
	if (Distance > LockOnBreakDistance) return false;

	// 3. 시야(LineTrace) 확인 - 장애물에 가려졌는지
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(TargetActor);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GetOwner()->GetActorLocation() + FVector(0,0,50), // 발 밑 말고 약간 위에서 시작
		TargetActor->GetActorLocation(),
		ECC_Visibility, // 시야 채널
		QueryParams
	);

	// 무언가에 막혔다면 타겟 아님 (잠깐 가려지는 허용 시간 로직은 추후 추가)
	if (bHit) return false;

	return true;
}

void USFLockOnComponent::SwitchTarget(FVector2D InputDirection)
{
	// Phase 3에서 구현 (타겟 스위칭)
}