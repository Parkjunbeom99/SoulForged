#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "SFTargetDataTypes.generated.h"

USTRUCT(BlueprintType)
struct SF_API FSFGameplayAbilityTargetData_ChargePhase : public FGameplayAbilityTargetData
{
	GENERATED_BODY()

public:
	FSFGameplayAbilityTargetData_ChargePhase()
		: PhaseIndex(0)
		, RushTargetLocation(FVector::ZeroVector)
	{
	}

	FSFGameplayAbilityTargetData_ChargePhase(const int32 InPhaseIndex, const FVector& InRushTargetLocation)
		: PhaseIndex(InPhaseIndex)
		, RushTargetLocation(InRushTargetLocation)
	{
	}

	UPROPERTY()
	int32 PhaseIndex;

	UPROPERTY()
	FVector RushTargetLocation;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FSFGameplayAbilityTargetData_ChargePhase::StaticStruct();
	}

	bool NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
	{
		Ar << PhaseIndex;
		RushTargetLocation.NetSerialize(Ar, Map, bOutSuccess);
		bOutSuccess = true;
		return true;
	}
};

template<>
struct TStructOpsTypeTraits<FSFGameplayAbilityTargetData_ChargePhase> : public TStructOpsTypeTraitsBase2<FSFGameplayAbilityTargetData_ChargePhase>
{
	enum
	{
		WithNetSerializer = true
	};
};