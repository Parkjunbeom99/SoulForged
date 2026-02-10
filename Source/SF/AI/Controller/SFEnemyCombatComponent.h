// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SFCombatComponentBase.h"
#include "Perception/AIPerceptionTypes.h"
#include "SFEnemyCombatComponent.generated.h"

class USFCombatSet_Enemy;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SF_API USFEnemyCombatComponent : public USFCombatComponentBase
{
    GENERATED_BODY()

public:
    USFEnemyCombatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    virtual void BeginDestroy() override;

    UFUNCTION(BlueprintPure, Category = "SF|Combat")
    static USFEnemyCombatComponent* FindSFEnemyCombatComponent(const AController* Controller)
    {
        return (Controller ? Controller->FindComponentByClass<USFEnemyCombatComponent>() : nullptr);
    }


    virtual void InitializeCombatComponent() override;
    
    UFUNCTION()
    void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

    
    UFUNCTION()
    void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

protected:

    bool IsValidTarget(AActor* Target) const;
    
    virtual void EvaluateTarget() override;
    
    float CalculateTargetScore(AActor* Target) const;

    
    void UpdateCombatRangeTags();

    
    void UpdatePerceptionConfig();

    
    void StartTargetEvaluationTimer();

    
    void StopTargetEvaluationTimer();

    
    UFUNCTION()
    void OnTargetEvaluationTimer();

protected:

    UPROPERTY()
    TObjectPtr<USFCombatSet_Enemy> CachedCombatSet;

    
    UPROPERTY(EditAnywhere, Category = "Combat|Target")
    float TargetEvaluationInterval = 0.5f;
    
    FTimerHandle TargetEvaluationTimerHandle;
    
    UPROPERTY(EditDefaultsOnly, Category = "Combat|Target")
    FName TargetActorTag = FName("Player");
    
    UPROPERTY(EditDefaultsOnly, Category = "Combat|Scoring")
    float MaxTargetDistance = 10000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Scoring")
    float DistanceScoreWeight = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Scoring")
    float AngleScoreWeight = 1.0f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Scoring")
    float MaxDistanceScore = 1000.f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Scoring")
    float MaxAngleScore = 100.f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Combat|Scoring")
    bool bConsiderTargetHealth = false;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Scoring", meta=(EditCondition="bConsiderTargetHealth"))
    float HealthScoreWeight = 0.5f;

    UPROPERTY(EditDefaultsOnly, Category = "Combat|Scoring", meta=(EditCondition="bConsiderTargetHealth"))
    float MaxHealthScore = 200.f;
};