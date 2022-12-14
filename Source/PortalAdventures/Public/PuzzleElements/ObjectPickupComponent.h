// Jacob Holyfield - © Infinite Gaming Productions 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ObjectPickupComponent.generated.h"

class AActor;
class APortalAdventuresCharacter;
class UStaticMeshComponent;
class UPhysicsConstraintComponent;
class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTALADVENTURES_API UObjectPickupComponent : public UActorComponent
{
	GENERATED_BODY()

/* VARS */
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float ReachRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxThrowVelocity;

private:
	// The actor that is being held
	UPrimitiveComponent* HeldObject;
	FName HeldBoneName;

	UStaticMeshComponent* HeldObjectSlot;

	UPhysicsConstraintComponent* GrabConstraint;

/* FUNCTIONS */
public:
	// Sets default values for this component's properties
	UObjectPickupComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Do a trace and pickup a object
	UFUNCTION()
	void GrabPressed();

	void SetHeldObjectSlot(UStaticMeshComponent* mesh);
	void SetGrabConstraint(UPhysicsConstraintComponent* constraint);

private:
	void GrabObject(const FHitResult& HitResult);

	void DropObject();
};
