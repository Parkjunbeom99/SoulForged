#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SFCommonUpgradeDefinition.generated.h"

class USFCommonUpgradeFragment;

/**
 * 어떤 업그레이드 인지 정의하는 메인 데이터 에셋
 */
UCLASS()
class SF_API USFCommonUpgradeDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GetCommonUpgradeDefinitionAssetType(), GetFName());
	}
	static FPrimaryAssetType GetCommonUpgradeDefinitionAssetType() { return FPrimaryAssetType(TEXT("CommonUpgradeDefinition")); }
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SF|UI")
	FText DisplayName;

	// 예: "공격력이 {0} 증가합니다."
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SF|UI", meta = (MultiLine = "true"))
	FText DescriptionFormat;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SF|UI", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> Icon;

	// 분류 및 필터링 (ex: Upgrade.Stat.AttackPower)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTag IdentifierTag; 

	// 조립식 기능 정의 (Fragments)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category = "Behavior")
	TArray<TObjectPtr<USFCommonUpgradeFragment>> Fragments;

	template <typename ResultClass>
	const ResultClass* FindFragment() const
	{
		for (const USFCommonUpgradeFragment* Fragment : Fragments)
		{
			if (const ResultClass* Result = Cast<ResultClass>(Fragment))
			{
				return Result;
			}
		}
		return nullptr;
	}
};
