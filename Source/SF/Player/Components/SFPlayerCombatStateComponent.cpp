#include "SFPlayerCombatStateComponent.h"

#include "Net/UnrealNetwork.h"

USFPlayerCombatStateComponent::USFPlayerCombatStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

USFPlayerCombatStateComponent* USFPlayerCombatStateComponent::FindPlayerCombatStateComponent(const AActor* Actor)
{
	// PlayerState에서 직접 찾기
	if (USFPlayerCombatStateComponent* CombatStateComponent = Actor ? Actor->FindComponentByClass<USFPlayerCombatStateComponent>() : nullptr)
	{
		return CombatStateComponent;
	}

	// Pawn인 경우 PlayerState에서 찾기
	if (const APawn* Pawn = Cast<APawn>(Actor))
	{
		if (const APlayerState* PS = Pawn->GetPlayerState())
		{
			return PS->FindComponentByClass<USFPlayerCombatStateComponent>();
		}
	}

	return nullptr;
}

void USFPlayerCombatStateComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RemainingDownCount);
	DOREPLIFETIME(ThisClass, ReviveCount);
}

void USFPlayerCombatStateComponent::BeginPlay()
{
	Super::BeginPlay();
	
	InitialDownCount = InitialReviveGaugeByDownCount.Num();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		RemainingDownCount = InitialDownCount;
	}
}

float USFPlayerCombatStateComponent::GetInitialReviveGauge() const
{
	// 현재 사용할 인덱스 = 총 다운 횟수 - 남은 횟수
	const int32 DownIndex = InitialDownCount - RemainingDownCount;
	
	if (InitialReviveGaugeByDownCount.IsValidIndex(DownIndex))
	{
		return InitialReviveGaugeByDownCount[DownIndex];
	}

	// 배열 범위 초과 시 0 (즉시 사망)
	return 0.f;
}

void USFPlayerCombatStateComponent::DecrementDownCount()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		RemainingDownCount = RemainingDownCount > 0 ? RemainingDownCount - 1 : 0;
	}
}

void USFPlayerCombatStateComponent::ResetDownCount()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		RemainingDownCount = InitialDownCount; 
	}
}

void USFPlayerCombatStateComponent::IncrementReviveCount()
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ReviveCount++;
	}
}

#if WITH_EDITOR
void USFPlayerCombatStateComponent::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// 변경된 속성이 있는지 확인
	if (PropertyChangedEvent.Property)
	{
		const FName PropertyName = PropertyChangedEvent.Property->GetFName();

		// 배열(InitialReviveGaugeByDownCount)변경 확인
		if (PropertyName == GET_MEMBER_NAME_CHECKED(USFPlayerCombatStateComponent, InitialReviveGaugeByDownCount))
		{
			// InitialDownCount를 배열 크기로 업데이트
			InitialDownCount = InitialReviveGaugeByDownCount.Num();
		}
	}
}
#endif
