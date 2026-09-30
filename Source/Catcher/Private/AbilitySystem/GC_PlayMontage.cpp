// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GC_PlayMontage.h"
#include "Animation/CAnimInstance.h"
#include "GameFramework/Character.h"

bool UGC_PlayMontage::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	
	USkeletalMeshComponent* TargetSkeletalMeshComponent = MyTarget->GetComponentByClass<USkeletalMeshComponent>();
	
	if (TargetSkeletalMeshComponent)
	{
		UAnimInstance* TargetAnimInstance = TargetSkeletalMeshComponent->GetAnimInstance();
		if (TargetAnimInstance)
		{
			TargetAnimInstance->Montage_Play(MontageToPlay);
		}
	}
	return true;
}
