#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "SFUIData.generated.h"

class USFQuickbarEntryWidget;
class USFQuickbarSlotWidget;
class USFInventoryEntryWidget;
class USFInventorySlotWidget;
class USFItemDragWidget;
/**
 * 
 */
UCLASS()
class SF_API USFUIData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	static const USFUIData& Get();

	//~ 슬롯 크기
	UPROPERTY(EditDefaultsOnly, Category = "Slot")
	FIntPoint SlotSize = FIntPoint(80, 80);

	//~ 인벤토리 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<USFInventorySlotWidget> InventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "Inventory")
	TSubclassOf<USFInventoryEntryWidget> InventoryEntryWidgetClass;

	//~ 퀵바 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Quickbar")
	TSubclassOf<USFQuickbarSlotWidget> QuickbarSlotWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Quickbar")
	TSubclassOf<USFQuickbarEntryWidget> QuickbarEntryWidgetClass;

	//~ 공통 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "Common")
	TSubclassOf<USFItemDragWidget> ItemDragWidgetClass;
};
