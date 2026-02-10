// Fill out your copyright notice in the Description page of Project Settings.

#include "SFAIGameplayTags.h"

namespace SFGameplayTags
{
	// ===== AI State Tags =====
	// Normal Enemy States
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Idle, "AI.State.Idle", "AI Idle State");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Combat, "AI.State.Combat", "AI Combat State");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Alert, "AI.State.Alert", "AI Alert State");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Patrol, "AI.State.Patrol", "AI Patrol State");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Return, "AI.State.Return", "AI Return State");

	// Elite Enemy States
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Elite_Enraged, "AI.State.Elite.Enraged", "Elite Enraged State");

	// Boss Enemy States
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Boss_Phase1, "AI.State.Boss.Phase1", "Boss Phase 1 State");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Boss_Phase2, "AI.State.Boss.Phase2", "Boss Phase 2 State");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Boss_Phase3, "AI.State.Boss.Phase3", "Boss Phase 3 State");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_State_Boss_Groggy, "AI.State.Boss.Groggy", "Boss Groggy State");

	// ===== AI Behaviour Tags =====
	// Normal Enemy Behaviours
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Normal_Default, "AI.Behaviour.Normal.Default", "Normal Enemy Default Behaviour");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Idle, "AI.Behaviour.Idle", "Normal Enemy Idle Behaviour");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Combat, "AI.Behaviour.Combat", "Normal Enemy Combat Behaviour");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Patrol, "AI.Behaviour.Patrol", "Normal Enemy Patrol Behaviour");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Return, "AI.Behaviour.Return", "Normal Enemy Return Behaviour");
	

	// Elite Enemy Behaviours
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Elite_Default, "AI.Behaviour.Elite.Default", "Elite Enemy Default Behaviour");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Elite_Enraged, "AI.Behaviour.Elite.Enraged", "Elite Enemy Enraged Behaviour");

	// Boss Enemy Behaviours
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Boss_Phase1, "AI.Behaviour.Boss.Phase1", "Boss Phase 1 Behaviour");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Boss_Phase2, "AI.Behaviour.Boss.Phase2", "Boss Phase 2 Behaviour");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Boss_Phase3, "AI.Behaviour.Boss.Phase3", "Boss Phase 3 Behaviour");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Behaviour_Boss_Groggy, "AI.Behaviour.Boss.Groggy", "Boss Groggy Behaviour");

	// ===== AI Range Tags =====
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Range_Melee, "AI.Range.Melee", "Target is within melee range");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(AI_Range_Guard, "AI.Range.Guard", "Target is within guard range");
}