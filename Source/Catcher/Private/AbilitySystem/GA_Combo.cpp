// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GA_Combo.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "Abilities/Tasks/AbilityTask_WaitInputPress.h"
#include "AbilitySystem/CAbilitySystemNativeTags.h"

UGA_Combo::UGA_Combo()
{
	AbilityTags.AddTag(TAG_ABILITY_BASICATTACK);
	BlockAbilitiesWithTag.AddTag(TAG_ABILITY_BASICATTACK);
}

void UGA_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Casting Combo Ability"));
	
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		UAbilityTask_PlayMontageAndWait* PlayMontageAndWait = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, ComboMontage);
		
		PlayMontageAndWait->OnCompleted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayMontageAndWait->OnCancelled.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayMontageAndWait->OnInterrupted.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		PlayMontageAndWait->OnBlendOut.AddDynamic(this, &UGA_Combo::K2_EndAbility);
		
		PlayMontageAndWait->ReadyForActivation();
		
		UAbilityTask_WaitGameplayEvent* WaitComboChangeEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TAG_ABILITY_COMBO_CHANGE, nullptr, false, false);
		
		WaitComboChangeEvent->EventReceived.AddDynamic(this, &UGA_Combo::HandleComboChange);
		WaitComboChangeEvent->ReadyForActivation();
	}
	
	SetupWaitInputPress();
	
	if (K2_HasAuthority())
	{
		UAbilityTask_WaitGameplayEvent* WaitDamageEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, TAG_ABILITY_COMBO_DAMAGE);
		WaitDamageEvent->EventReceived.AddDynamic(this, &UGA_Combo::DoDamage);
		WaitDamageEvent->ReadyForActivation();
	}
}

void UGA_Combo::HandleComboChange(FGameplayEventData EventData)
{
	FGameplayTag EventTag = EventData.EventTag;
	
	if (EventTag == TAG_ABILITY_COMBO_CHANGE_End)
	{
		NextComboName = NAME_None;
		UE_LOG(LogTemp, Warning, TEXT("Next Combo is None"));
		return;
	}
	
	TArray<FName> TagNames;
	UGameplayTagsManager::Get().SplitGameplayTagFName(EventTag, TagNames);
	NextComboName = TagNames.Last();
	
	UE_LOG(LogTemp, Warning, TEXT("Next Combo Name changed to: %s"), *(NextComboName.ToString()));
}

void UGA_Combo::SetupWaitInputPress()
{
	UAbilityTask_WaitInputPress* WaitInputPress = UAbilityTask_WaitInputPress::WaitInputPress(this);
	WaitInputPress->OnPress.AddDynamic(this, &UGA_Combo::HandleComboInputPress);
	WaitInputPress->ReadyForActivation();
}

void UGA_Combo::HandleComboInputPress(float TimeWaited)
{
	SetupWaitInputPress();
	if (NextComboName == NAME_None)
	{
		return;
	}
	
	if (UAnimInstance* AnimInstance = GetCurrentActorInfo()->GetAnimInstance())
	{
		AnimInstance->Montage_SetNextSection(AnimInstance->Montage_GetCurrentSection(ComboMontage), NextComboName, ComboMontage);
	}
}

void UGA_Combo::DoDamage(FGameplayEventData EventData)
{
}
