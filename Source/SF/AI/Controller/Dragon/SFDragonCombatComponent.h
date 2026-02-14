// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "AI/Controller/SFCombatComponentBase.h"
#include "SFDragonCombatComponent.generated.h"

UENUM(BlueprintType)
enum class EBossAttackZone : uint8
{
    None        UMETA(DisplayName = "None"),
    Melee       UMETA(DisplayName = "Melee Range"),
    Mid         UMETA(DisplayName = "Mid Range"),
    Long        UMETA(DisplayName = "Long Range"),
    OutOfRange  UMETA(DisplayName = "Out of Range")
};


UENUM(BlueprintType)
enum class EBossTargetState : uint8
{
    None         UMETA(DisplayName = "None"),
    Locked       UMETA(DisplayName = "Locked"),      
    Grace        UMETA(DisplayName = "Grace"),       
    ForceRelease UMETA(DisplayName = "Force Release") 
};


UCLASS()
class SF_API USFDragonCombatComponent : public USFCombatComponentBase
{
    GENERATED_BODY()

public:
    USFDragonCombatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginDestroy() override;

    virtual void InitializeCombatComponent() override;
    
    UFUNCTION()
    void AddThreat(float ThreatValue, AActor* Actor);

    AActor* GetHighestThreatActor();
    void CleanupThreatMap();


    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    EBossAttackZone GetTargetLocationZone() const { return CurrentZone; }

    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    float GetDistanceToTarget() const { return CachedDistance; }

    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    float GetAngleToTarget() const { return CachedAngle; }

    UFUNCTION(BlueprintCallable, Category = "AI|Combat")
    float GetPlayerHealthPercent() const { return PlayerHealthPercent; }

    UFUNCTION(BlueprintCallable, Category = "AI|Phase")
    void SetCurrentPhase(int32 NewPhase) { CurrentPhase = NewPhase; }

    UFUNCTION(BlueprintPure, Category = "AI|Phase")
    int32 GetCurrentPhase() const { return CurrentPhase; }

    virtual bool SelectAbility(const FEnemyAbilitySelectContext& Context, const FGameplayTagContainer& SearchTags, FGameplayTag& OutSelectedTag) override;

protected:

    virtual void EvaluateTarget() override;
    
    void UpdateSpatialData();

 
    void MonitorTargetState();


    void StartSpatialUpdateTimer();
    void StopSpatialUpdateTimer();
    void StartStateMonitorTimer();
    void StopStateMonitorTimer();
    void StartThreatUpdateTimer();
    void StopThreatUpdateTimer();
    
    bool IsValidTarget(AActor* Target) const;
    bool ShouldForceReleaseTarget(AActor* Target) const;

protected:
    // Target State Machine
    UPROPERTY()
    EBossTargetState CurrentTargetState = EBossTargetState::None;

    UPROPERTY()
    float LastValidTargetTime = 0.f;

    UPROPERTY(EditAnywhere, Category = "AI|Target Hold")
    float TargetGraceDuration = 0.5f;

    UPROPERTY(EditAnywhere, Category = "AI|Target Hold")
    float MaxCombatRange = 5000.f;


    UPROPERTY()
    TMap<AActor*, float> ThreatMap;

    // Cached Spatial Data
    UPROPERTY()
    EBossAttackZone CurrentZone = EBossAttackZone::None;

    UPROPERTY()
    float CachedDistance = 0.f;

    UPROPERTY()
    float CachedAngle = 0.f;

    // Zone Ranges
    UPROPERTY(EditAnywhere, Category = "AI|Zone")
    float MeleeRange = 1500.f;

    UPROPERTY(EditAnywhere, Category = "AI|Zone")
    float MidRange = 2500.f;

    UPROPERTY(EditAnywhere, Category = "AI|Zone")
    float LongRange = 3500.f;

    // Player state
    UPROPERTY()
    float PlayerHealthPercent = 1.0f;

    UPROPERTY(VisibleAnywhere, Category = "AI|Phase")
    int32 CurrentPhase = 1;

    // Update Intervals
    UPROPERTY(EditAnywhere, Category = "AI|Update")
    float SpatialUpdateInterval = 0.1f;

    UPROPERTY(EditAnywhere, Category = "AI|Update")
    float StateMonitorInterval = 0.2f;

    UPROPERTY(EditAnywhere, Category = "AI|Update")
    float ThreatUpdateInterval = 0.5f;

    // Timers
    FTimerHandle SpatialUpdateTimerHandle;
    FTimerHandle StateMonitorTimerHandle;
    FTimerHandle ThreatUpdateTimerHandle;

  
    FGameplayTag LastSelectedAbilityTag;

    UPROPERTY()
    TArray<FGameplayTag> RecentAbilityHistory;

    UPROPERTY(EditAnywhere, Category = "AI")
    int32 MaxHistorySize = 2;

    // Ability selection weights
    UPROPERTY(EditAnywhere, Category = "AI|Ability Selection")
    float RecentAbilityPenalty = 0.3f;

    UPROPERTY(EditAnywhere, Category = "AI|Ability Selection")
    float RandomVarianceMin = 0.8f;

    UPROPERTY(EditAnywhere, Category = "AI|Ability Selection")
    float RandomVarianceMax = 1.2f;

    UPROPERTY(EditAnywhere, Category = "AI|Ability Selection")
    float EliteScoreThreshold = 0.6f;

};