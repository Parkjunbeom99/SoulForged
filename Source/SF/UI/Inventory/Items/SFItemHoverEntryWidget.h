#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/SFItemManagerComponent.h"

#include "SFItemHoverEntryWidget.generated.h"

class UVerticalBox;
class UImage;
class UTextBlock;
class URichTextBlock;
class USFItemInstance;

/**
 * 
 */
UCLASS()
class SF_API USFItemHoverEntryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	USFItemHoverEntryWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	void RefreshUI(const USFItemInstance* ItemInstance, ESFItemSlotType SlotType);

protected:
	// 아이템 이름 (등급 색상 적용)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_DisplayName;

	// 등급 배경 이미지
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_DisplayName_Background;

	// 등급 텍스트 (Common, Rare 등)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ItemRarity;

	// 아이템 타입 (Potion, Equipment 등)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_ItemType;

	// 스탯 목록 (AttackPower 10, Defense 5 등)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_AttributeModifiers;

	// 아이템 설명
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<URichTextBlock> Text_Description;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Description_1;

	// 페이드인 애니메이션
	UPROPERTY(meta = (BindWidgetAnim), Transient)
	TObjectPtr<UWidgetAnimation> Animation_FadeIn;
};
