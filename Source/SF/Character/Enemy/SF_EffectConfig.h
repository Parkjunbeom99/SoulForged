#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SF_EffectConfig.generated.h"

class UNiagaraSystem;
class USoundBase;
class USoundAttenuation;

UENUM(BlueprintType)
enum class EEffectSpawnType : uint8
{
	Location,  
	Attached   
};

USTRUCT(BlueprintType)
struct FEffectData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEffectSpawnType SpawnType = EEffectSpawnType::Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UNiagaraSystem> Effect_NiagaraSystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TSoftObjectPtr<USoundBase> Effect_SoundBase;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	TSoftObjectPtr<USoundAttenuation> Effect_Attenuation;

	// ===== Cosmetic 추가 =====
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cosmetic")
	FName AttachSocketName = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cosmetic")
	FVector LocationOffset = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cosmetic")
	FRotator RotationOffset = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cosmetic|Niagara")
	FName NiagaraScaleParam = NAME_None;
};

UCLASS()
class SF_API USF_EffectConfig : public UDataAsset
{
	GENERATED_BODY()
    
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TMap<FGameplayTag, FEffectData> EffectDataMap;
};