#include "SFItemHoverWidget.h"
#include "SFItemHoverEntryWidget.h"

USFItemHoverWidget::USFItemHoverWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USFItemHoverWidget::RefreshUI(const USFItemInstance* ItemInstance)
{
	if (Widget_HoverEntry)
	{
		Widget_HoverEntry->RefreshUI(ItemInstance);
	}
}