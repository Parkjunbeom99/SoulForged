#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SFGameplayAbility.h"
#include "USFGA_Dodge.generated.h"

/**
 * 도지 방향 정의 (네트워크 동기화용)
 */
UENUM(BlueprintType)
enum class ESFDodgeDirection : uint8
{
	Forward = 0,
	Backward,
	Left,
	Right,
	Backstep
};

/**
 * 4방향 회피 몽타주 구조체
 */
USTRUCT(BlueprintType)
struct FDodgeMontageSet
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> ForwardMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> BackwardMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> LeftMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> RightMontage;
};

/**
 * 소울류 구르기 (클라이언트 예측 + 서버 동기화)
 */
UCLASS()
class SF_API USFGA_Dodge : public USFGameplayAbility
{
	GENERATED_BODY()

public:
	USFGA_Dodge(FObjectInitializer const& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	// 몽타주 종료 콜백
	UFUNCTION()
	void OnMontageFinished();

	// -------------------------------------------------------------------------
	// Core Logic
	// -------------------------------------------------------------------------

	/** * [계산] 현재 입력과 락온 상태를 기반으로 타겟 위치, 회전, 방향을 결정 
	 * OutDirection: 결정된 구르기 방향 (이걸 서버로 보냄)
	 */
	void CalculateDodgeParameters(bool bIsLockedOn, FVector& OutLocation, FRotator& OutRotation, ESFDodgeDirection& OutDirection) const;

	/** [실행] 결정된 데이터로 구르기 실제 수행 */
	void ApplyDodge(const FVector& TargetLocation, const FRotator& TargetRotation, ESFDodgeDirection Direction);

	/** [서버] 클라이언트 타겟 데이터 수신 핸들러 */
	void OnServerTargetDataReceived(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
	
	/** Motion Warping 설정 헬퍼 */
	void SetupMotionWarping(const FVector& TargetLocation, const FRotator& TargetRotation);

	/** 방향 Enum을 몽타주로 변환하는 헬퍼 */
	UAnimMontage* GetMontageFromDirection(ESFDodgeDirection Direction) const;

protected:
	// =========================================================
	//  Configuration
	// =========================================================

	// 4방향 구르기 몽타주 세트 (기존 단일 DodgeMontage 대체)
	UPROPERTY(EditDefaultsOnly, Category = "SF|Animation")
	FDodgeMontageSet DirectionalDodgeMontages;

	// 백스텝 몽타주 (입력 없이 사용 시)
	UPROPERTY(EditDefaultsOnly, Category = "SF|Animation")
	TObjectPtr<UAnimMontage> BackstepMontage;
	
	// 구르기 이동 거리
	UPROPERTY(EditDefaultsOnly, Category = "SF|Dodge")
	float DodgeDistance = 500.f;

	// Motion Warping 타겟 이름 (몽타주 내 NotifyState와 일치해야 함)
	UPROPERTY(EditDefaultsOnly, Category = "SF|Dodge")
	FName WarpTargetName = FName("DodgeTarget");

private:
	FDelegateHandle ServerTargetDataDelegateHandle;
	
	bool bSavedOrientRotationToMovement;
	bool bSavedUseControllerRotationYaw;
};