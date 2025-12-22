// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Hero/SFHero.h"

#include "AbilitySystemComponent.h"
#include "Character/SFCharacterGameplayTags.h"
#include "Component/SFHeroMovementComponent.h"
#include "Player/SFPlayerController.h"
#include "Player/SFPlayerState.h"
#include "Team/SFTeamTypes.h"

ASFHero::ASFHero(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<USFHeroMovementComponent>(CharacterMovementComponentName))
{
}

ASFPlayerController* ASFHero::GetSFPlayerController() const
{
	return Cast<ASFPlayerController>(Controller);
}

FGenericTeamId ASFHero::GetGenericTeamId() const
{
	if (const ASFPlayerState* PS = GetPlayerState<ASFPlayerState>())
	{
		return PS->GetGenericTeamId();
	}
	return FGenericTeamId(SFTeamID::Player);
}

FSFInteractionInfo ASFHero::GetPreInteractionInfo(const FSFInteractionQuery& InteractionQuery) const
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (ASC->HasMatchingGameplayTag(SFGameplayTags::Character_State_Downed))
		{
			return ReviveInteractionInfo;
		}
	}
	
	return FSFInteractionInfo();
}

bool ASFHero::CanInteraction(const FSFInteractionQuery& InteractionQuery) const
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !ASC->HasMatchingGameplayTag(SFGameplayTags::Character_State_Downed))
	{
		return false;
	}

	if (InteractionQuery.RequestingAvatar.Get() == this)
	{
		return false;
	}

	return true;
}

void ASFHero::OnInteractActiveStarted(AActor* Interactor)
{
	if (!HasAuthority() || !IsValid(Interactor))
	{
		return;
	}
	
	CachedRevivers.AddUnique(Interactor);
}

void ASFHero::OnInteractActiveEnded(AActor* Interactor)
{
	if (!HasAuthority() || !IsValid(Interactor))
	{
		return;
	}
	
	CachedRevivers.RemoveSingleSwap(Interactor);
}

void ASFHero::OnInteractionSuccess(AActor* Interactor)
{
	// 필요 시 추가 처리
}

int32 ASFHero::GetActiveInteractorCount() const
{
	int32 Count = 0;
	for (const TWeakObjectPtr<AActor>& Reviver : CachedRevivers)
	{
		if (Reviver.IsValid())
		{
			++Count;
		}
	}
	return Count;
}

void ASFHero::OnAbilitySystemInitialized()
{
	Super::OnAbilitySystemInitialized();

	if (ASFPlayerState* PS = GetPlayerState<ASFPlayerState>())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("[PermanentUpgrade] SFHero OnAbilitySystemInitialized -> Notify PlayerState")
		);

		PS->OnPawnReadyForPermanentUpgrade();
	}
}
