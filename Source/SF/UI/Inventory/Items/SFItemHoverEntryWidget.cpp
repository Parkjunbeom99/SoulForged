#include "SFItemHoverEntryWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/RichTextBlock.h"
#include "Item/SFItemInstance.h"
#include "Item/SFItemDefinition.h"
#include "Item/SFItemData.h"
#include "Item/SFItemRarityConfig.h"
#include "Item/Fragments/SFItemFragment_Consumable.h"
#include "Item/Fragments/SFItemFragment_Equippable.h"
#include "UI/SFUIData.h"


USFItemHoverEntryWidget::USFItemHoverEntryWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void USFItemHoverEntryWidget::RefreshUI(const USFItemInstance* ItemInstance)
{
    if (ItemInstance == nullptr)
    {
        SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    SetVisibility(ESlateVisibility::HitTestInvisible);

    if (Animation_FadeIn)
    {
        PlayAnimationForward(Animation_FadeIn);
    }

    const USFItemData& ItemData = USFItemData::Get();
    const USFItemDefinition* ItemDef = ItemData.FindDefinitionById(ItemInstance->GetItemID());
    if (!ItemDef)
    {
        return;
    }

    // ========== 등급 정보 ==========
    const FGameplayTag& RarityTag = ItemInstance->GetItemRarityTag();
    const USFItemRarityConfig* RarityConfig = ItemData.FindRarityByTag(RarityTag);
    
    FLinearColor RarityColor = FLinearColor::White;
    FText RarityDisplayName = FText::GetEmpty();
    
    if (RarityConfig)
    {
        RarityColor = RarityConfig->Color;
        RarityDisplayName = RarityConfig->DisplayName;
    }

    // ========== Display Name ==========
    Text_DisplayName->SetText(ItemDef->DisplayName);
    Text_DisplayName->SetColorAndOpacity(RarityColor);

    // 등급별 배경 (RarityConfig에 BackgroundBrush 사용 가능)
    if (RarityConfig)
    {
        Image_DisplayName_Background->SetBrush(RarityConfig->BackgroundBrush);
    }

    // ========== Item Rarity ==========
    Text_ItemRarity->SetText(RarityDisplayName);
    Text_ItemRarity->SetColorAndOpacity(RarityColor);

    // ========== Item Type ==========
    FText ItemTypeText = NSLOCTEXT("SF", "ItemType_Misc", "Miscellaneous");
    if (const USFItemFragment_Consumable* ConsumeFrag = ItemDef->FindFragment<USFItemFragment_Consumable>())
    {
        // ConsumeTypeTag 기반으로 타입 텍스트 결정
        if (ConsumeFrag->ConsumeTypeTag.MatchesTag(FGameplayTag::RequestGameplayTag(TEXT("Item.Consumable.Potion"))))
        {
            ItemTypeText = NSLOCTEXT("SF", "ItemType_Potion", "Potion");
        }
        // TODO : 추가 타입
    }
    else if (const USFItemFragment_Equippable* EquipFrag = ItemDef->FindFragment<USFItemFragment_Equippable>())
    {
        // EquipSlotTag 기반으로 타입 텍스트 결정
        const FGameplayTag& SlotTag = EquipFrag->EquipSlotTag;
        
        // TODO : Equipment.Slot.Weapon, Equipment.Slot.Armor.Head 등으로 분기
        const FSFUIInfo& UIInfo = USFUIData::Get().GetTagUIInfo(SlotTag);
        if (UIInfo.IsValid())
        {
            ItemTypeText = UIInfo.Title;
        }
    }
    
    Text_ItemType->SetText(ItemTypeText);

    // ========== Attribute Modifiers (스탯 목록) ==========
    FString AttributeString;
    
    const FSFGameplayTagStackContainer& StatContainer = ItemInstance->GetStatContainer();
    for (const FSFGameplayTagStack& Stack : StatContainer.GetStacks())
    {
        // 태그에서 마지막 부분 추출 (Stat.AttackPower → AttackPower)
        FString TagString = Stack.GetStackTag().ToString();
        FString StatName;
        TagString.Split(TEXT("."), nullptr, &StatName, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
        
        // UI 데이터에서 표시 이름 가져오기
        const FSFUIInfo& StatUIInfo = USFUIData::Get().GetTagUIInfo(Stack.GetStackTag());
        if (StatUIInfo.IsValid())
        {
            StatName = StatUIInfo.Title.ToString();
        }
        
        AttributeString.Append(FString::Printf(TEXT("%s +%d\n"), *StatName, Stack.GetStackCount()));
    }
    AttributeString.RemoveFromEnd(TEXT("\n"));

    if (AttributeString.IsEmpty())
    {
        Text_AttributeModifiers->SetVisibility(ESlateVisibility::Collapsed);
    }
    else
    {
        Text_AttributeModifiers->SetText(FText::FromString(AttributeString));
        Text_AttributeModifiers->SetVisibility(ESlateVisibility::Visible);
    }

    if (Text_Description)
    {
        if (ItemDef->Description.IsEmpty())
        {
            Text_Description->SetVisibility(ESlateVisibility::Collapsed);
        }
        else
        {
            Text_Description->SetText(ItemDef->Description);
            Text_Description->SetVisibility(ESlateVisibility::Visible);
        }
    }
}