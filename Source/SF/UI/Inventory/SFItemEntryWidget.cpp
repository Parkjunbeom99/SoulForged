#include "SFItemEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Item/SFItemInstance.h"
#include "Item/SFItemDefinition.h"
#include "Item/SFItemData.h"
#include "Item/SFItemRarityConfig.h"
#include "UI/SFUIData.h"   
#include "Items/SFItemHoverWidget.h"

USFItemEntryWidget::USFItemEntryWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void USFItemEntryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Text_Count->SetText(FText::GetEmpty());
}

void USFItemEntryWidget::NativeConstruct()
{
	Super::NativeConstruct();

	Image_Hover->SetVisibility(ESlateVisibility::Hidden);
}

void USFItemEntryWidget::NativeDestruct()
{
	HideItemHoverWidget();
	
	Super::NativeDestruct();
}

void USFItemEntryWidget::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	Image_Hover->SetVisibility(ESlateVisibility::Visible);
	
	ShowItemHoverWidget(InMouseEvent);
}

void USFItemEntryWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	Image_Hover->SetVisibility(ESlateVisibility::Hidden);
	
	HideItemHoverWidget();
}

FReply USFItemEntryWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseMove(InGeometry, InMouseEvent);

	if (ItemHoverWidget)
	{
		ItemHoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
	}

	return Reply;
}

FReply USFItemEntryWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FReply Reply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);

	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		Reply = Reply.DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}

	return Reply;
}

void USFItemEntryWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);

	SetDragOpacity(true);

	HideItemHoverWidget();
}

void USFItemEntryWidget::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	SetDragOpacity(false);
}

void USFItemEntryWidget::SetDragOpacity(bool bIsDragging)
{
	SetRenderOpacity(bIsDragging ? 0.5f : 1.0f);
}

void USFItemEntryWidget::RefreshUI(USFItemInstance* InItemInstance, int32 InItemCount)
{
	if (InItemInstance == nullptr || InItemCount < 1)
	{
		return;
	}
	
	ItemInstance = InItemInstance;
	ItemCount = InItemCount;

	// 아이템 정의에서 아이콘 가져오기
	const USFItemDefinition* ItemDef = USFItemData::Get().FindDefinitionById(InItemInstance->GetItemID());
	if (ItemDef && ItemDef->Icon)
	{
		Image_Icon->SetBrushFromTexture(ItemDef->Icon, true);
	}

	// 등급 색상 설정
	const FGameplayTag& RarityTag = InItemInstance->GetItemRarityTag();
	if (const USFItemRarityConfig* RarityConfig = USFItemData::Get().FindRarityByTag(RarityTag))
	{
		Image_RarityBG->SetColorAndOpacity(RarityConfig->Color);
	}

	// 수량 표시 (2개 이상일 때만)
	Text_Count->SetText(ItemCount > 1 ? FText::AsNumber(ItemCount) : FText::GetEmpty());
}

void USFItemEntryWidget::RefreshItemCount(int32 InItemCount)
{
	if (InItemCount < 1)
		return;

	ItemCount = InItemCount;
	Text_Count->SetText(ItemCount > 1 ? FText::AsNumber(ItemCount) : FText::GetEmpty());
}

void USFItemEntryWidget::ShowItemHoverWidget(const FPointerEvent& InMouseEvent)
{
	if (!ItemInstance)
	{
		return;
	}

	if (!ItemHoverWidget)
	{
		TSubclassOf<USFItemHoverWidget> HoverWidgetClass = USFUIData::Get().ItemHoverWidgetClass;
		if (HoverWidgetClass)
		{
			ItemHoverWidget = CreateWidget<USFItemHoverWidget>(GetOwningPlayer(), HoverWidgetClass);
		}
	}

	if (ItemHoverWidget)
	{
		ItemHoverWidget->RefreshUI(ItemInstance, GetSlotType());
		ItemHoverWidget->AddToViewport(100);
		ItemHoverWidget->SetPosition(InMouseEvent.GetScreenSpacePosition());
	}
}

void USFItemEntryWidget::HideItemHoverWidget()
{
	if (ItemHoverWidget)
	{
		ItemHoverWidget->RemoveFromParent();
		ItemHoverWidget = nullptr;
	}
}
