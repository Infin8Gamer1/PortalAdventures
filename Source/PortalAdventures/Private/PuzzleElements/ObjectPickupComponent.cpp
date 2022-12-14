// Jacob Holyfield - © Infinite Gaming Productions 2022


#include "PuzzleElements/ObjectPickupComponent.h"
#include "PortalAdventuresCharacter.h"
#include "Camera/CameraComponent.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"
#include "Components/StaticMeshComponent.h"

// Sets default values for this component's properties
UObjectPickupComponent::UObjectPickupComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	ReachRange = 200.0f;
	MaxThrowVelocity = 300.0f;

	HeldObject = nullptr;
	HeldBoneName = FName("None");

	GrabConstraint = nullptr;
	HeldObjectSlot = nullptr;
}

void UObjectPickupComponent::GrabPressed()
{
	if (HeldObject == nullptr)
	{
		APortalAdventuresCharacter* Player = Cast<APortalAdventuresCharacter>(GetOwner());
		if (Player == nullptr)
			return;

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(GetOwner());

		FVector Start = Player->GetFirstPersonCameraComponent()->GetComponentLocation();
		FVector End = Start + (Player->GetFirstPersonCameraComponent()->GetForwardVector() * ReachRange);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_PhysicsBody, Params))
		{
			GrabObject(HitResult);
		}
	}
	else
	{
		DropObject();
	}
}

void UObjectPickupComponent::SetHeldObjectSlot(UStaticMeshComponent* mesh)
{
	HeldObjectSlot = mesh;
}

void UObjectPickupComponent::SetGrabConstraint(UPhysicsConstraintComponent* constraint)
{
	GrabConstraint = constraint;
}

void UObjectPickupComponent::GrabObject(const FHitResult& HitResult)
{
	HeldObjectSlot->SetWorldLocation(HitResult.ImpactPoint, false, nullptr, ETeleportType::TeleportPhysics);

	GrabConstraint->SetConstrainedComponents(HeldObjectSlot, FName("None"), HitResult.GetComponent(), HitResult.BoneName);

	HitResult.GetComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	HeldObject = HitResult.GetComponent();
	HeldBoneName = HitResult.BoneName;
}

void UObjectPickupComponent::DropObject()
{
	GrabConstraint->BreakConstraint();

	//Clamp Max Velocity
	FVector NewVelocity = HeldObject->GetPhysicsLinearVelocity(HeldBoneName).GetClampedToSize(-MaxThrowVelocity, MaxThrowVelocity);
	HeldObject->SetPhysicsLinearVelocity(NewVelocity, false, HeldBoneName);

	HeldObject->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);

	HeldObject = nullptr;
	HeldBoneName = FName("None");
}

// Called every frame
void UObjectPickupComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (HeldObject != nullptr)
	{
		HeldObject->WakeAllRigidBodies();
	}
}

