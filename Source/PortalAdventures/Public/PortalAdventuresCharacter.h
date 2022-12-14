// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Portals/PortableObjectInterface.h"
#include "PortalAdventuresCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;
class UObjectPickupComponent;
class UPhysicsConstraintComponent;

// Declaration of the delegate that will be called when the Primary Action is triggered
// It is declared as dynamic so it can be accessed also in Blueprints
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUseItem);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireRedPortal);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFireBluePortal);

UCLASS(config=Game)
class APortalAdventuresCharacter : public ACharacter, public IPortableObjectInterface
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* HeldObjectSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UPhysicsConstraintComponent* GrabConstraint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UObjectPickupComponent* ObjectPickupComponent;

public:
	APortalAdventuresCharacter();

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float TurnRateGamepad;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnUseItem OnUseItem;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFireBluePortal OnFireBluePortal;

	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnFireRedPortal OnFireRedPortal;

	

protected:
	//Shooting Inputs
	/** Fires The Blue Portal*/
	void OnBlueFire();
	/** Fires The Red Portal*/
	void OnRedFire();
	/** Fires a projectile. */
	void OnFire();

	//Movement Inputs
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles strafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	void GetTrajectory_Implementation(FVector& out_Location, FRotator& out_Rotation, FRotator& out_ControlRotation, FVector& out_LinearVelocity, FVector& out_AngularVelocity) override;

	void SetTrajectory_Implementation(const FVector& Location, const FRotator& Rotation, const FRotator& ControlRotation, const FVector& LinearVelocity, const FVector& AngularVelocity) override;

	void SetCollisionResponse_Implementation(ECollisionChannel Channel, ECollisionResponse Response) override;
};

