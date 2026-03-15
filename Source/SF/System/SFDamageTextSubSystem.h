// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Subsystems/WorldSubsystem.h"
#include "SFDamageTextSubSystem.generated.h"


class USFDamageWidget;
class UWidgetComponent;
/**
 * 
 */
UCLASS()
class SF_API USFDamageTextSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	void ShowDamage(float DamageAmount, AActor* TargetActor, FVector HitLocation, bool bIsCritical);

	// 통계 출력 함수 (public)
	void ShowStats();
	void ResetStats();

private:
	UWidgetComponent* GetFromPool();
	void ReturnToPool(UWidgetComponent* Component);
	void PrewarmPool(int32 PoolSize);
	FVector CalcDamageTextLocation(AActor* TargetActor, FVector HitLocation);

	UFUNCTION()
	void OnWidgetAnimationFinished(UUserWidget* Widget);

	UPROPERTY()
	TSubclassOf<USFDamageWidget> DamageWidgetClass;

	UPROPERTY()
	TObjectPtr<AActor> PoolOwnerActor;

	UPROPERTY()
	TArray<TObjectPtr<UWidgetComponent>> AvailablePool;

	UPROPERTY()
	TMap<TObjectPtr<UUserWidget>, TObjectPtr<UWidgetComponent>> ActiveWidgetMap;

	// 통계 추적 변수
	int32 TotalCreatedCount = 0;      // 총 생성된 Widget 개수
	int32 PoolHitCount = 0;           // Pool에서 재사용한 횟수
	int32 PoolMissCount = 0;          // Pool이 비어서 새로 생성한 횟수
	float TotalCreationTime = 0.f;    // 총 생성 시간 (ms)
};
