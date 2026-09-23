// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/GC_CameraShake.h"

#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

bool UGC_CameraShake::OnExecute_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters) const
{
	APlayerController* PlayerController = Cast<APlayerController>(MyTarget->GetOwner());
	if (PlayerController)
	{
		PlayerController->ClientStartCameraShake(CameraShake ,CameraShakeScale,ECameraShakePlaySpace::CameraLocal, MyTarget->GetActorRotation());
	}
	
	return true;
}
