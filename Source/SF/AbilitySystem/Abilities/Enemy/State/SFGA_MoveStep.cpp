#include "SFGA_MoveStep.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "AbilitySystem/Abilities/SFGameplayAbilityTags.h"
#include "AbilitySystem/GameplayEvent/SFGameplayEventTags.h"
#include "Character/SFCharacterGameplayTags.h"

USFGA_MoveStep::USFGA_MoveStep()
{
    InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

    AbilityTags.AddTag(SFGameplayTags::Ability_Enemy_Movement_Step);
    AbilityTags.AddTag(SFGameplayTags::Character_State_UsingAbility);

    ActivationOwnedTags.AddTag(SFGameplayTags::Ability_Enemy_Movement_Step);
    ActivationOwnedTags.AddTag(SFGameplayTags::Character_State_UsingAbility);

    ActivationBlockedTags.AddTag(SFGameplayTags::Character_State_Dead);
    ActivationBlockedTags.AddTag(SFGameplayTags::Character_State_Stunned);
    ActivationBlockedTags.AddTag(SFGameplayTags::Character_State_Hit);
}

void USFGA_MoveStep::ActivateAbility(
    const FGameplayAbilitySpecHandle Handle, 
    const FGameplayAbilityActorInfo* ActorInfo, 
    const FGameplayAbilityActivationInfo ActivationInfo, 
    const FGameplayEventData* TriggerEventData)
{
    if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }

    ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    if (!Character) 
    {
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }


    float MoveDirectionInput = TriggerEventData ? TriggerEventData->EventMagnitude : 1.0f;
    if (FMath::IsNearlyZero(MoveDirectionInput)) MoveDirectionInput = 1.0f;

    FVector ForwardVector = Character->GetActorForwardVector();
    FVector MoveDirection = ForwardVector * MoveDirectionInput;


    UAbilityTask_ApplyRootMotionConstantForce* MoveTask = 
        UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(
            this,
            TEXT("MoveStepRootMotion"),
            MoveDirection,   
            StepIntensity,  
            StepDuration,    
            false,           
            StepCurve,       
            ERootMotionFinishVelocityMode::SetVelocity, 
            FVector::ZeroVector,
            0.0f,            
            false          
        );

  
    if (MoveTask)
    {
        MoveTask->OnFinish.AddDynamic(this, &USFGA_MoveStep::OnMoveStepFinished);
        MoveTask->ReadyForActivation();
    }
    else
    {
      
        EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
        return;
    }


    UAnimMontage* SelectedAnim = (MoveDirectionInput > 0) ? ForwardAnim : BackwardAnim;
    if (SelectedAnim)
    {
        Character->PlayAnimMontage(SelectedAnim, 1.0f);
    }
}

void USFGA_MoveStep::OnMoveStepFinished()
{
 

    ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo());
    if (Character)
    {
        Character->StopAnimMontage();
        if (UCharacterMovementComponent* MC = Character->GetCharacterMovement())
        {
            MC->StopMovementImmediately();
        }
    }

    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}

float USFGA_MoveStep::CalcAIScore(const FEnemyAbilitySelectContext& Context) const
{
    if (!Context.Self || !Context.Target)
        return 0.f;

    const float Dist = Context.DistanceToTarget;

    // 거리별 차등 점수
    if (Dist < 150.f)
    {
        return 10000.f;  
    }
    else if (Dist < MaxStepRange)
    {
        return 5000.f;   
    }

    return 0.f;
}

float USFGA_MoveStep::CalcScoreModifier(const FEnemyAbilitySelectContext& Context) const
{
    return 0.f;
}