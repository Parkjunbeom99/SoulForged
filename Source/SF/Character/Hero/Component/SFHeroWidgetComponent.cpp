#include "SFHeroWidgetComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/Hero/SFCombatSet_Hero.h"
#include "Character/SFCharacterGameplayTags.h"
#include "Components/WidgetComponent.h"
#include "UI/InGame/SFHeroOverheadWidget.h"

USFHeroWidgetComponent::USFHeroWidgetComponent(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USFHeroWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    UnbindTagEvents();
    UnbindReviveGaugeAttribute();
    Super::EndPlay(EndPlayReason);
}

void USFHeroWidgetComponent::SetOverheadWidgetComponent(UWidgetComponent* InWidgetComponent)
{
    OverheadWidgetComponent = InWidgetComponent;
}

void USFHeroWidgetComponent::SetPlayerName(const FString& Name)
{
    if (USFHeroOverheadWidget* Widget = GetOverheadWidget())
    {
        Widget->SetPlayerName(Name);
    }
}

void USFHeroWidgetComponent::InitializeWithASC(UAbilitySystemComponent* ASC)
{
    if (!ASC)
    {
        return;
    }

    CachedASC = ASC;
    BindTagEvents();

    if (USFHeroOverheadWidget* Widget = GetOverheadWidget())
    {
        Widget->SetReviveGaugeVisible(false);
    }

    // 이미 Downed 상태인지 확인
    if (ASC->HasMatchingGameplayTag(SFGameplayTags::Character_State_Downed))
    {
        OnDownedStateBegin();
    }
}

USFHeroOverheadWidget* USFHeroWidgetComponent::GetOverheadWidget() const
{
    if (UWidgetComponent* WidgetComp = OverheadWidgetComponent.Get())
    {
        return Cast<USFHeroOverheadWidget>(WidgetComp->GetWidget());
    }
    return nullptr;
}

void USFHeroWidgetComponent::BindTagEvents()
{
    UAbilitySystemComponent* ASC = CachedASC.Get();
    if (!ASC)
    {
        return;
    }

    DownedTagDelegateHandle = ASC->RegisterGameplayTagEvent(SFGameplayTags::Character_State_Downed, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::OnDownedTagChanged);
}

void USFHeroWidgetComponent::UnbindTagEvents()
{
    UAbilitySystemComponent* ASC = CachedASC.Get();
    if (!ASC)
    {
        return;
    }

    if (DownedTagDelegateHandle.IsValid())
    {
        ASC->RegisterGameplayTagEvent(SFGameplayTags::Character_State_Downed,EGameplayTagEventType::NewOrRemoved).Remove(DownedTagDelegateHandle);
        DownedTagDelegateHandle.Reset();
    }
}

void USFHeroWidgetComponent::OnDownedTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    if (NewCount > 0)
    {
        OnDownedStateBegin();
    }
    else
    {
        OnDownedStateEnd();
    }
}

void USFHeroWidgetComponent::BindReviveGaugeAttribute()
{
    UAbilitySystemComponent* ASC = CachedASC.Get();
    if (!ASC)
    {
        return;
    }

    // 이미 바인딩되어 있으면 스킵
    if (ReviveGaugeDelegateHandle.IsValid())
    {
        return;
    }

    ReviveGaugeDelegateHandle = ASC->GetGameplayAttributeValueChangeDelegate(
        USFCombatSet_Hero::GetReviveGaugeAttribute()
    ).AddUObject(this, &ThisClass::OnReviveGaugeChanged);
}

void USFHeroWidgetComponent::UnbindReviveGaugeAttribute()
{
    if (UAbilitySystemComponent* ASC = CachedASC.Get())
    {
        if (ReviveGaugeDelegateHandle.IsValid())
        {
            ASC->GetGameplayAttributeValueChangeDelegate(
                USFCombatSet_Hero::GetReviveGaugeAttribute()
            ).Remove(ReviveGaugeDelegateHandle);
            ReviveGaugeDelegateHandle.Reset();
        }
    }
}

void USFHeroWidgetComponent::OnReviveGaugeChanged(const FOnAttributeChangeData& Data)
{
    if (USFHeroOverheadWidget* Widget = GetOverheadWidget())
    {
        Widget->SetReviveGaugePercent(Data.NewValue / 100.f);
    }
}

void USFHeroWidgetComponent::OnDownedStateBegin()
{
    // Attribute 바인딩 시작
    BindReviveGaugeAttribute();

    // UI 표시
    if (USFHeroOverheadWidget* Widget = GetOverheadWidget())
    {
        Widget->SetReviveGaugeVisible(true);

        // 초기값 설정
        if (UAbilitySystemComponent* ASC = CachedASC.Get())
        {
            float CurrentValue = ASC->GetNumericAttribute(USFCombatSet_Hero::GetReviveGaugeAttribute());
            Widget->SetReviveGaugePercent(CurrentValue / 100.f);
        }
    }
}

void USFHeroWidgetComponent::OnDownedStateEnd()
{
    // Attribute 바인딩 해제
    UnbindReviveGaugeAttribute();

    // UI 숨김
    if (USFHeroOverheadWidget* Widget = GetOverheadWidget())
    {
        Widget->SetReviveGaugeVisible(false);
    }
}

