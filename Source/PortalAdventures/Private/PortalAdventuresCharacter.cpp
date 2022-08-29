// Copyright Epic Games, Inc. All Rights Reserved.

#include "PortalAdventuresCharacter.h"

#include "Portals/Portal.h"
#include "Portals/PortalManager.h"

#include "PortalAdventuresProjectile.h"
#include "PortalAdventurePlayerController.h"

#include "PuzzleElements/ObjectPickupComponent.h"

#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/StaticMeshComponent.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// APortalAdventuresCharacter

APortalAdventuresCharacter::APortalAdventuresCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	TurnRateGamepad = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	HeldObjectSlot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HeldObjectSlot"));
	HeldObjectSlot->SetupAttachment(FirstPersonCameraComponent);

	GrabConstraint = CreateDefaultSubobject<UPhysicsConstraintComponent>(TEXT("GrabConstraint"));
	GrabConstraint->SetupAttachment(HeldObjectSlot);
	GrabConstraint->SetLinearXLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
	GrabConstraint->SetLinearYLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
	GrabConstraint->SetLinearZLimit(ELinearConstraintMotion::LCM_Free, 0.0f);
	GrabConstraint->SetLinearPositionDrive(true, true, true);
	GrabConstraint->SetLinearVelocityDrive(true, true, true);
	GrabConstraint->SetLinearDriveParams(1000.0f, 100.0f, 0.0f);
	GrabConstraint->SetAngularDriveMode(EAngularDriveMode::TwistAndSwing);
	GrabConstraint->SetAngularOrientationDrive(true, true);
	GrabConstraint->SetAngularVelocityDrive(true, true);
	GrabConstraint->SetAngularDriveParams(1000.0f, 100.0f, 0.0f);

	ObjectPickupComponent = CreateDefaultSubobject<UObjectPickupComponent>(TEXT("ObjectPickupComponent"));
	ObjectPickupComponent->SetHeldObjectSlot(HeldObjectSlot);
	ObjectPickupComponent->SetGrabConstraint(GrabConstraint);
}

void APortalAdventuresCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

}

//////////////////////////////////////////////////////////////////////////// Input

void APortalAdventuresCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire events
	PlayerInputComponent->BindAction("PrimaryAction", IE_Pressed, this, &APortalAdventuresCharacter::OnFire);
	PlayerInputComponent->BindAction("FireBlue", IE_Pressed, this, &APortalAdventuresCharacter::OnBlueFire);
	PlayerInputComponent->BindAction("FireRed", IE_Pressed, this, &APortalAdventuresCharacter::OnRedFire);

	// Bind grab event
	PlayerInputComponent->BindAction("Grab", IE_Pressed, ObjectPickupComponent, &UObjectPickupComponent::GrabPressed);
	//PlayerInputComponent->BindAction("Grab", IE_Released, this, &APortalAdventuresCharacter::Drop);

	// Bind movement events
	PlayerInputComponent->BindAxis("Move Forward / Backward", this, &APortalAdventuresCharacter::MoveForward);
	PlayerInputComponent->BindAxis("Move Right / Left", this, &APortalAdventuresCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "Mouse" versions handle devices that provide an absolute delta, such as a mouse.
	// "Gamepad" versions are for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &APortalAdventuresCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &APortalAdventuresCharacter::LookUpAtRate);
}

void APortalAdventuresCharacter::OnBlueFire()
{
	OnFireBluePortal.Broadcast();
}

void APortalAdventuresCharacter::OnRedFire()
{
	OnFireRedPortal.Broadcast();
}

void APortalAdventuresCharacter::OnFire()
{
	// Trigger the OnItemUsed Event
	OnUseItem.Broadcast();
}

void APortalAdventuresCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APortalAdventuresCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APortalAdventuresCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APortalAdventuresCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void APortalAdventuresCharacter::GetTrajectory_Implementation(FVector& out_Location, FRotator& out_Rotation, FRotator& out_ControlRotation, FVector& out_LinearVelocity, FVector& out_AngularVelocity)
{
	out_Location = GetActorLocation();
	out_Rotation = GetActorRotation();
	out_ControlRotation = GetController()->GetControlRotation();
	out_LinearVelocity = GetCharacterMovement()->Velocity;
	out_AngularVelocity = FVector::ZeroVector;
}

void APortalAdventuresCharacter::SetTrajectory_Implementation(const FVector& Location, const FRotator& Rotation, const FRotator& ControlRotation, const FVector& LinearVelocity, const FVector& AngularVelocity)
{
	FHitResult HitResult;
	SetActorLocationAndRotation(Location, Rotation, false, &HitResult, ETeleportType::TeleportPhysics);
	GetController()->SetControlRotation(ControlRotation);
	//GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->Velocity = LinearVelocity;
}

void APortalAdventuresCharacter::SetCollisionResponse_Implementation(ECollisionChannel Channel, ECollisionResponse Response)
{
	GetCapsuleComponent()->SetCollisionResponseToChannel(Channel, Response);
}