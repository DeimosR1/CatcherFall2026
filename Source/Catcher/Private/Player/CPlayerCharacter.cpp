// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CPlayerCharacter.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Catcher/Catcher.h"
#include "EnhancedInputComponent.h"

ACPlayerCharacter::ACPlayerCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>("Camera Boom");
	CameraBoom->ProbeChannel = ECC_CAMERA_BOOM;
	CameraBoom->SetupAttachment(GetRootComponent());

	ViewCam = CreateDefaultSubobject<UCameraComponent>("View Cam");
	ViewCam->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	CameraBoom->bUsePawnControlRotation = true;
	bUseControllerRotationYaw = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(720.f);
}

void ACPlayerCharacter::PawnClientRestart()
{
	Super::PawnClientRestart();
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputLocalPlayerSubsystem = GetController<APlayerController>()->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (EnhancedInputLocalPlayerSubsystem) 
	{
		EnhancedInputLocalPlayerSubsystem->ClearAllMappings();
		EnhancedInputLocalPlayerSubsystem->AddMappingContext(GameplayMappingContext, 0);
	}
}

void ACPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) 
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::Jump);
		EnhancedInputComponent->BindAction(LookInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleLookInput);
		EnhancedInputComponent->BindAction(MoveInputAction, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleMoveInput);
		
		for (const TPair<ECAbilityInputID, UInputAction*>& InputActionPair : GameplayAbilityInputActions)
		{
			EnhancedInputComponent->BindAction(InputActionPair.Value, ETriggerEvent::Triggered, this, &ACPlayerCharacter::HandleAbilityInputAction, InputActionPair.Key);
		}
	}
}

void ACPlayerCharacter::HandleAbilityInputAction(const struct FInputActionValue& InputActionValue,
	ECAbilityInputID InputID)
{
	bool bPressed = InputActionValue.Get<bool>();
	
	if (bPressed)
	{
		GetAbilitySystemComponent()->AbilityLocalInputPressed((int32)InputID);
	}
	else
	{
		GetAbilitySystemComponent()->AbilityLocalInputReleased((int32)InputID);
	}
}

void ACPlayerCharacter::HandleLookInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputAction = InputActionValue.Get<FVector2D>();
	AddControllerYawInput(InputAction.X);
	AddControllerPitchInput(InputAction.Y);
}

void ACPlayerCharacter::HandleMoveInput(const FInputActionValue& InputActionValue)
{
	FVector2D InputAction = InputActionValue.Get<FVector2D>();
	InputAction.Normalize();

	AddMovementInput(GetMoveFwdDirection() * InputAction.Y + GetRightDirection() * InputAction.X);
}

FVector ACPlayerCharacter::GetRightDirection() const
{
	return ViewCam->GetRightVector();
}

FVector ACPlayerCharacter::GetLookFwdDirection() const
{
	return ViewCam->GetForwardVector();
}

FVector ACPlayerCharacter::GetMoveFwdDirection() const
{
	return FVector::CrossProduct(GetRightDirection(), FVector::UpVector);
}
