#pragma once

#include "CoreMinimal.h"
#include "UI/Common/SFHoverWidget.h"
#include "Item/SFItemManagerComponent.h"

#include "SFItemHoverWidget.generated.h"

class USFItemHoverEntryWidget;
class USFItemInstance;

/**
 * 
 */
UCLASS()
class SF_API USFItemHoverWidget : public USFHoverWidget
{
	GENERATED_BODY()

public:
	USFItemHoverWidget(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void RefreshUI(const USFItemInstance* ItemInstance, ESFItemSlotType SlotType);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USFItemHoverEntryWidget> Widget_HoverEntry;
};
