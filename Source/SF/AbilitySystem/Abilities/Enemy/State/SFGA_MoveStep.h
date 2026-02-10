#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/SFGameplayAbility.h"
#include "Interface/SFEnemyAbilityInterface.h"
#include "SFGA_MoveStep.generated.h"

class UCurveFloat;

UCLASS()
class SF_API USFGA_MoveStep : public USFGameplayAbility, public ISFEnemyAbilityInterface
{
	GENERATED_BODY()

public:
	USFGA_MoveStep();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual float CalcAIScore(const FEnemyAbilitySelectContext& Context) const override;
	virtual float CalcScoreModifier(const FEnemyAbilitySelectContext& Context) const override;

protected:

	UPROPERTY(EditAnywhere, Category = "Movement")
	TObjectPtr<UAnimMontage> ForwardAnim;

	UPROPERTY(EditAnywhere, Category = "Movement")
	TObjectPtr<UAnimMontage> BackwardAnim;

	UPROPERTY(EditAnywhere, Category = "Movement")
	TObjectPtr<UCurveFloat> StepCurve;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float StepIntensity = 1500.f;

	UPROPERTY(EditAnywhere, Category = "Movement")
	float StepDuration = 1.0f;

	UPROPERTY(EditAnywhere, Category = "AI")
	float MaxStepRange = 300.f;

private:
	UFUNCTION()
	void OnMoveStepFinished();
};