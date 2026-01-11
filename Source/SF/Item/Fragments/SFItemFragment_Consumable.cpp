#include "SFItemFragment_Consumable.h"

#include "GameplayEffect.h"
#include "Item/SFItemInstance.h"

void USFItemFragment_Consumable::OnInstanceCreated(USFItemInstance* Instance) const
{
	if (!Instance)
	{
		return;
	}

	const FGameplayTag& RarityTag = Instance->GetItemRarityTag();

	for (const FSFTaggedRarityValues& Data : SetByCallerDatas)
	{
		if (Data.ValueTag.IsValid())
		{
			const float Value = Data.GetFixedValueForRarity(RarityTag);
			if (Value > 0.f)
			{
				Instance->AddOrRemoveStatTagStack(Data.ValueTag, FMath::RoundToInt(Value));
			}
		}
	}
}

void USFItemFragment_Consumable::ApplySetByCallersToSpec(FGameplayEffectSpec* Spec, const FGameplayTag& RarityTag) const
{
	if (!Spec)
	{
		return;
	}

	for (const FSFTaggedRarityValues& Data : SetByCallerDatas)
	{
		if (Data.ValueTag.IsValid())
		{
			const float Magnitude = Data.GetFixedValueForRarity(RarityTag);
			Spec->SetSetByCallerMagnitude(Data.ValueTag, Magnitude);
		}
	}
}
