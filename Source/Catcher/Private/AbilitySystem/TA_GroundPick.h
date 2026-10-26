// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Abilities/GameplayAbilityTargetActor.h"
#include "TA_GroundPick.generated.h"

/**
 * 
 */
UCLASS()
class ATA_GroundPick : public AGameplayAbilityTargetActor
{
	GENERATED_BODY()
	
public:
	ATA_GroundPick();
	virtual void ConfirmTargetingAndContinue() override;
	void SetTargetAreaRadius(float NewTargetAreaRadius);
	void SetTargetTraceRange(float NewTargetTraceRange) {TargetTraceRange = NewTargetTraceRange; };
	void SetShouldDrawDebug(bool bNewShouldDrawDebug) { bShouldDrawDebug = bNewShouldDrawDebug; };
	void AddTargetTeamAttitude(ETeamAttitude::Type NewTeamAttitude) {TargetTeamAttitutes.Add(NewTeamAttitude); };
	
protected:
	virtual void Tick(float DeltaSeconds) override;
	
private:
	
	UPROPERTY(VisibleAnywhere, Category="Visual")
	class UDecalComponent* DecalComponent;
	
	FVector GetTargetPoint() const;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetTraceRange = 2000.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	float TargetAreaRadius = 200.0f;
	
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bShouldDrawDebug = true;
	
	UPROPERTY(EditDefaultsOnly, Category="Targeting")
	TSet<TEnumAsByte<ETeamAttitude::Type>> TargetTeamAttitutes;
	
	
};
