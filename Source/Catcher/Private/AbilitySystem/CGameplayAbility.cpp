// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/CGameplayAbility.h"

#include "GameplayCueNotifyTypes.h"
#include "Kismet/KismetSystemLibrary.h"

TArray<FHitResult> UCGameplayAbility::GetHitResultsFromSweepLocationTargetData(
	const FGameplayAbilityTargetDataHandle& TargetDataHandle, float SphereSweepRadius, ETeamAttitude::Type TargetTeamAttitute, bool bDrawDebug,
	bool bIgnoreSelf) const
{
	TArray<FHitResult> OutResults; //TArray is Raw Array that knows when to grow
	
	TSet<AActor*> HitActors; //Same as set in Mathematics. Does not allow repetition of items in it.
	
	const IGenericTeamAgentInterface* OwnerTeamInterface = Cast<IGenericTeamAgentInterface>(GetAvatarActorFromActorInfo());
	
	for (const TSharedPtr<FGameplayAbilityTargetData> TargetData : TargetDataHandle.Data)
	{
		FVector StartLocation = TargetData->GetOrigin().GetTranslation();
		FVector EndLocation = TargetData->GetEndPoint();
		
		TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
		ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));
		
		TArray<AActor*> ActorsToIgnore;
		if (bIgnoreSelf){ActorsToIgnore.Add(GetAvatarActorFromActorInfo());}
		
		TArray<FHitResult> Results;
		
		UKismetSystemLibrary::SphereTraceMultiForObjects(this, StartLocation, EndLocation, SphereSweepRadius, ObjectTypes, false, ActorsToIgnore, bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None, Results, false);
		
		for (const FHitResult& Result : Results)
		{
			if (HitActors.Contains(Result.GetActor()))
			{
				continue;
			}
			
			if (OwnerTeamInterface)
			{
				if (OwnerTeamInterface->GetTeamAttitudeTowards(*Result.GetActor()) != TargetTeamAttitute)
				{
					continue;
				}
			}
			HitActors.Add(Result.GetActor());
			
			OutResults.Add(Result);
		}
	}
	return OutResults;
}
