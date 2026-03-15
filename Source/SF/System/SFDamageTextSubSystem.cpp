#include "System/SFDamageTextSubSystem.h"
#include "SFDamageSettings.h"
#include "Components/WidgetComponent.h"
#include "UI/InGame/SFDamageWidget.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

void USFDamageTextSubSystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (const USFDamageSettings* Settings = GetDefault<USFDamageSettings>())
    {
        DamageWidgetClass = Settings->DamageWidgetClass.LoadSynchronous();

        if (DamageWidgetClass)
        {
            PrewarmPool(10);
        }
    }
}

void USFDamageTextSubSystem::Deinitialize()
{
    // 게임 종료 시 통계 출력
    ShowStats();

    for (auto& Pair : ActiveWidgetMap)
    {
        if (IsValid(Pair.Value))
        {
            Pair.Value->DestroyComponent();
        }
    }
    for (auto& Comp  : AvailablePool)
    {
        if (IsValid(Comp))
        {
            Comp->DestroyComponent();
        }
    }

    if (PoolOwnerActor)
    {
        PoolOwnerActor->Destroy();
    }

    Super::Deinitialize();
}

void USFDamageTextSubSystem::ShowDamage(float DamageAmount, AActor* TargetActor, FVector HitLocation, bool bIsCritical)
{
    if (!TargetActor || !DamageWidgetClass)
    {
        return;
    }

    UWidgetComponent* WidgetComp = GetFromPool();
    if (!WidgetComp)
    {
        return;
    }

    FVector SpawnLocation = CalcDamageTextLocation(TargetActor, HitLocation);
    SpawnLocation += FMath::VRand() * 50.f;

    WidgetComp->SetWorldLocation(SpawnLocation);
    WidgetComp->SetVisibility(true);

    if (!WidgetComp->GetUserWidgetObject())
    {
        WidgetComp->InitWidget();
    }

    if (USFDamageWidget* DamageWidget = Cast<USFDamageWidget>(WidgetComp->GetUserWidgetObject()))
    {
        ActiveWidgetMap.Emplace(DamageWidget, WidgetComp);
        DamageWidget->OnFinished.RemoveAll(this);
        DamageWidget->OnFinished.AddDynamic(this, &ThisClass::OnWidgetAnimationFinished);
        DamageWidget->PlayDamageEffect(DamageAmount, bIsCritical);
    }
    else
    {
        ReturnToPool(WidgetComp);
    }
}

UWidgetComponent* USFDamageTextSubSystem::GetFromPool()
{
    if (AvailablePool.Num() > 0)
    {
        PoolHitCount++;
        return AvailablePool.Pop();
    }

    PoolMissCount++;

    double StartTime = FPlatformTime::Seconds();

    if (!PoolOwnerActor)
    {
        PoolOwnerActor = GetWorld()->SpawnActor<AActor>();
    }

    UWidgetComponent* NewComp = NewObject<UWidgetComponent>(PoolOwnerActor);
    NewComp->SetWidgetClass(DamageWidgetClass);
    NewComp->SetWidgetSpace(EWidgetSpace::Screen);
    NewComp->SetDrawAtDesiredSize(true);
    NewComp->SetVisibility(false);
    NewComp->RegisterComponent();

    double EndTime = FPlatformTime::Seconds();
    float CreationTime = (EndTime - StartTime) * 1000.f;
    TotalCreationTime += CreationTime;
    TotalCreatedCount++;

    return NewComp;
}

void USFDamageTextSubSystem::OnWidgetAnimationFinished(UUserWidget* Widget)
{
    if (auto* FoundComp = ActiveWidgetMap.Find(Widget))
    {
        ReturnToPool(*FoundComp);
        ActiveWidgetMap.Remove(Widget);
    }
}

void USFDamageTextSubSystem::ReturnToPool(UWidgetComponent* Component)
{
    if (Component)
    {
        if (USFDamageWidget* Widget = Cast<USFDamageWidget>(Component->GetUserWidgetObject()))
        {
            if (UWorld* World = Widget->GetWorld())
            {
                World->GetTimerManager().ClearAllTimersForObject(Widget);
            }

            Widget->StopAllAnimations();
        }

        Component->SetVisibility(false);
        AvailablePool.Add(Component);
    }
}

void USFDamageTextSubSystem::PrewarmPool(int32 PoolSize)
{
    if (!DamageWidgetClass)
    {
        return;
    }

    if (!PoolOwnerActor)
    {
        PoolOwnerActor = GetWorld()->SpawnActor<AActor>();
    }

    for (int32 i = 0; i < PoolSize; i++)
    {
        UWidgetComponent* NewComp = NewObject<UWidgetComponent>(PoolOwnerActor);
        NewComp->SetWidgetClass(DamageWidgetClass);
        NewComp->SetWidgetSpace(EWidgetSpace::Screen);
        NewComp->SetDrawAtDesiredSize(true);
        NewComp->SetVisibility(false);
        NewComp->RegisterComponent();

        AvailablePool.Add(NewComp);
        TotalCreatedCount++;
    }
}

void USFDamageTextSubSystem::ShowStats()
{
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("   Damage Widget Pool Statistics"));
    UE_LOG(LogTemp, Warning, TEXT("========================================"));
    UE_LOG(LogTemp, Warning, TEXT("Total Created:     %d widgets"), TotalCreatedCount);
    UE_LOG(LogTemp, Warning, TEXT("Current Pool Size: %d widgets"), AvailablePool.Num());
    UE_LOG(LogTemp, Warning, TEXT("Active Widgets:    %d widgets"), ActiveWidgetMap.Num());
    UE_LOG(LogTemp, Warning, TEXT("----------------------------------------"));
    UE_LOG(LogTemp, Warning, TEXT("Pool Hits:         %d (재사용)"), PoolHitCount);
    UE_LOG(LogTemp, Warning, TEXT("Pool Misses:       %d (새로 생성)"), PoolMissCount);

    int32 TotalRequests = PoolHitCount + PoolMissCount;
    float HitRate = TotalRequests > 0 ? (float)PoolHitCount / TotalRequests * 100.f : 0.f;
    UE_LOG(LogTemp, Warning, TEXT("Pool Hit Rate:     %.1f%%"), HitRate);

    float AvgCreationTime = TotalCreatedCount > 0 ? TotalCreationTime / TotalCreatedCount : 0.f;
    UE_LOG(LogTemp, Warning, TEXT("Avg Creation Time: %.3f ms"), AvgCreationTime);
    UE_LOG(LogTemp, Warning, TEXT("========================================"));

    // 화면에도 출력
    if (GEngine)
    {
        FString StatsMessage = FString::Printf(
            TEXT("Pool Stats: %d created | %.1f%% hit rate | %d/%d active"),
            TotalCreatedCount, HitRate, ActiveWidgetMap.Num(), AvailablePool.Num());
        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Green, StatsMessage);
    }
}

void USFDamageTextSubSystem::ResetStats()
{
    TotalCreatedCount = 0;
    PoolHitCount = 0;
    PoolMissCount = 0;
    TotalCreationTime = 0.f;

    UE_LOG(LogTemp, Warning, TEXT("[Pool] Statistics reset"));

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Pool statistics reset!"));
    }
}

FVector USFDamageTextSubSystem::CalcDamageTextLocation(AActor* TargetActor, FVector HitLocation)
{

    if (USkeletalMeshComponent* Mesh = TargetActor->FindComponentByClass<USkeletalMeshComponent>())
    {
        if (Mesh->DoesSocketExist(TEXT("DamageText")))
        {
            return Mesh->GetSocketLocation(TEXT("DamageText"));
        }
    }
    if (!HitLocation.IsNearlyZero())
    {
        return HitLocation;
    }

    if (ACharacter* Char = Cast<ACharacter>(TargetActor))
    {
        return Char->GetActorLocation() + FVector(0.f, 0.f, Char->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
    }

    return TargetActor->GetActorLocation();
}