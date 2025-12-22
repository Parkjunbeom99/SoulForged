// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/SFCharacterBase.h"
#include "SFHero.generated.h"

class ASFPlayerController;
/**
 * 
 */
UCLASS()
class SF_API ASFHero : public ASFCharacterBase
{
	GENERATED_BODY()
public:
	ASFHero(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "SF|Hero")
	ASFPlayerController* GetSFPlayerController() const;
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override {}

	// ~ Begin ISFInteractable
	virtual FSFInteractionInfo GetPreInteractionInfo(const FSFInteractionQuery& InteractionQuery) const override;
	virtual bool CanInteraction(const FSFInteractionQuery& InteractionQuery) const override;
	virtual void OnInteractActiveStarted(AActor* Interactor) override;
	virtual void OnInteractActiveEnded(AActor* Interactor) override;
	virtual void OnInteractionSuccess(AActor* Interactor) override;
	virtual int32 GetActiveInteractorCount() const override;
	// ~ End ISFInteractable

	const TArray<TWeakObjectPtr<AActor>>& GetCachedRevivers() const { return CachedRevivers; }

protected:
	virtual void OnAbilitySystemInitialized() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "SF|Revive")
	FSFInteractionInfo ReviveInteractionInfo;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> CachedRevivers;
};
