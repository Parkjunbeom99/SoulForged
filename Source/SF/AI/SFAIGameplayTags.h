// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "NativeGameplayTags.h"


namespace SFGameplayTags
{
	// ===== AI State Tags =====
	// Normal Enemy States
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Idle);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Combat);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Alert);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Patrol);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Return);

	// Elite Enemy States
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Elite_Enraged);

	// Boss Enemy States
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Boss_Phase1);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Boss_Phase2);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Boss_Phase3);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_State_Boss_Groggy);

	// ===== AI Behaviour Tags =====
	// Normal Enemy Behaviours
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Normal_Default);

	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Idle);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Combat);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Patrol);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Return);
	
	// Elite Enemy Behaviours
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Elite_Default);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Elite_Enraged);

	// Boss Enemy Behaviours
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Boss_Phase1);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Boss_Phase2);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Boss_Phase3);
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Behaviour_Boss_Groggy);

	// ===== AI Range Tags =====
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Range_Melee);    // 근접 공격 범위 내
	SF_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(AI_Range_Guard);    // 경계/원거리 범위 내
}