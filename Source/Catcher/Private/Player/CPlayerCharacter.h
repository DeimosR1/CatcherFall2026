// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CCharacter.h"
#include "CPlayerCharacter.generated.h"

/**
 * 
 */
UCLASS()
class ACPlayerCharacter : public ACCharacter
{
	GENERATED_BODY()

public:
	ACPlayerCharacter();
	virtual void PawnClientRestart() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
private:
	UPROPERTY(VisibleDefaultsOnly, Category = "Camera")
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleDefaultsOnly, Category = "Camera")
	class UCameraComponent* ViewCam;

	//--------------------------------------------------------------------//
	//                               Input                                //
	//--------------------------------------------------------------------//

private:

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputMappingContext* GameplayMappingContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* JumpInputAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* LookInputAction;
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	class UInputAction* MoveInputAction;

	void HandleLookInput(const struct FInputActionValue& InputActionValue); //Adding the & at the end of FInputActionValue makes us reference the value in the memory and not create a copy.
	
	void HandleMoveInput(const struct FInputActionValue& InputActionValue);

	FVector GetRightDirection() const; //When you have const here in the function you are saying that anything in the class is not supposed to be changed. It doesn't let you modified the value.
	FVector GetLookFwdDirection() const;
	FVector GetMoveFwdDirection() const;

};
