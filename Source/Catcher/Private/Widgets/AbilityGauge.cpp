// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/AbilityGauge.h"

#include "Components/TextBlock.h"

void UAbilityGauge::NativeOnListItemObjectSet(UObject* ListItemObject)
{
	IUserObjectListEntry::NativeOnListItemObjectSet(ListItemObject);
	UE_LOG(LogTemp, Warning, TEXT("Ability Gauge Created for ability: %s"), *(ListItemObject->GetName()));
}
