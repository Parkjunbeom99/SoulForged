#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Enemy/Combat/SFGA_Enemy_BaseAttack.h"
#include "SFGA_Dragon_FlameBreath_Line.generated.h"

class UAbilityTask_PlayMontageAndWait;

UCLASS()
class SF_API USFGA_Dragon_FlameBreath_Line : public USFGA_Enemy_BaseAttack
{
    GENERATED_BODY()

public:
    USFGA_Dragon_FlameBreath_Line();

    virtual float CalcScoreModifier(const FEnemyAbilitySelectContext& Context) const override;

    virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
    virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

protected:
    UFUNCTION()
    void OnMontageInterrupted();

    UFUNCTION()
    void OnMontageCancelled();

    UFUNCTION()
    void OnChargeStartCompleted();

    UFUNCTION()
    void OnBreathEndCompleted();

    void StartCharging();
    void TransitionToBreath();
    void StopBreath();
    
    void ApplyBreathDamage();
    AActor* FindPrimaryTarget();
    
    void UpdateRotationToTarget(); 

    void OnDamageReceivedDuringCharge(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);
    void InterruptBreath();

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Montage")
    TObjectPtr<UAnimMontage> BreathMontage;  
    
    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Timing")
    float ChargeDuration = 2.5f; 

    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Timing")
    float BreathDuration = 3.0f; 

    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Timing")
    float BreathEndDuration = 2.0f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Breath")
    float BreathRange = 1500.f;

    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Breath")
    float BreathWidth = 150.f;

    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Breath")
    float BreathDamagePerTick = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Breath")
    float BreathTickRate = 1.f;

    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Phase")
    TMap<int32, float> PhaseDamageMultipliers;
    
    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Interrupt")
    float InterruptThreshold = 100.f;

    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Interrupt")
    TSubclassOf<UGameplayEffect> InterruptStaggerEffect;
    
    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Interrupt")
    float InterruptStaggerDamage = 10.f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Dragon|Debug")
    bool bIsDebug = false;

private:
    UPROPERTY()
    TWeakObjectPtr<AActor> PrimaryTarget;

    UPROPERTY()
    TSet<TWeakObjectPtr<AActor>> HitActors;

    UPROPERTY()
    UAbilityTask_PlayMontageAndWait* ChargeStartMontageTask;

    UPROPERTY()
    UAbilityTask_PlayMontageAndWait* BreathEndMontageTask;

    FTimerHandle ChargeTimerHandle;
    FTimerHandle BreathTickTimer;
    FTimerHandle BreathDurationTimer;
    FTimerHandle RotationTimerHandle;

    float AccumulatedInterruptDamage = 0.f;
    FDelegateHandle OnDamageReceivedHandle;

    float CurrentBreathDamage = 0.f;
};