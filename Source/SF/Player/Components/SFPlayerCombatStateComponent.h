#pragma once

#include "CoreMinimal.h"
#include "Components/PlayerStateComponent.h"
#include "SFPlayerCombatStateComponent.generated.h"

/**
 * 플레이어의 전투 관련 상태를 관리하는 컴포넌트
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SF_API USFPlayerCombatStateComponent : public UPlayerStateComponent
{
	GENERATED_BODY()

public:
	USFPlayerCombatStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintPure, Category = "SF|Combat")
	static USFPlayerCombatStateComponent* FindPlayerCombatStateComponent(const AActor* Actor);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

public:
	//~=============================================================================
	// 다운/부활 관련
	//~=============================================================================

	UFUNCTION(BlueprintCallable, Category = "SF|Combat")
	float GetInitialReviveGauge() const;
	
	UFUNCTION(BlueprintCallable, Category = "SF|Combat")
	int32 GetRemainingDownCount() const { return RemainingDownCount; }

	UFUNCTION(BlueprintCallable, Category = "SF|Combat")
	void DecrementDownCount();

	UFUNCTION(BlueprintCallable, Category = "SF|Combat")
	void ResetDownCount();

	UFUNCTION(BlueprintCallable, Category = "SF|Combat")
	int32 GetReviveCount() const { return ReviveCount; }

	UFUNCTION(BlueprintCallable, Category = "SF|Combat")
	void IncrementReviveCount();

protected:

	// RemainingDownCount별 초기 ReviveGauge 값 (index 0 = 첫 다운)
	UPROPERTY(EditDefaultsOnly, Category = "SF|Combat")
	TArray<float> InitialReviveGaugeByDownCount = { 80.f, 60.f, 30.f };

	// 초기 다운 가능 횟수
	UPROPERTY(VisibleDefaultsOnly, Category = "SF|Combat")
	int32 InitialDownCount = 3;
	
	// 현재 스테이지 세트에서 다운된 횟수 (서브 스테이지 간 유지, 새 스테이지 진입 시 리셋)
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "SF|Combat")
	int32 RemainingDownCount = 3;

	// 다른 플레이어를 부활시킨 횟수 
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "SF|Combat")
	int32 ReviveCount = 0;
};
