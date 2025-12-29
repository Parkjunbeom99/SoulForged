#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SFCommonRarityConfig.generated.h"

/**
 * 
 */
UCLASS()
class SF_API USFCommonRarityConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(GetCommonRarityConfigAssetType(), GetFName());
	}
	static FPrimaryAssetType GetCommonRarityConfigAssetType() { return FPrimaryAssetType(TEXT("CommonRarityConfig")); }

	// 희귀도 식별 태그 (ex: Rarity.Epic)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SF|Identity")
	FGameplayTag RarityTag;

	// 스탯 배율 (Common=1.0, Epic=2.5 등)
	// StatBoost Fragment의 BaseMagnitude에 곱해지는 수치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SF|Stats")
	float StatMultiplier = 1.0f;

	// 등급별 등장 확률 가중치 (LootTable 계산용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SF|Stats")
	float Weight = 1.0f;

	// 등급별 프레임/배경 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SF|UI", meta = (AssetBundles = "UI"))
	TSoftObjectPtr<UTexture2D> FrameTexture;

	// 등급별 색상
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SF|UI")
	FLinearColor FrameColor;
};
