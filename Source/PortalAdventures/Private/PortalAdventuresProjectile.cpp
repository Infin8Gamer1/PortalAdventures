// Copyright Epic Games, Inc. All Rights Reserved.

#include "PortalAdventuresProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"

APortalAdventuresProjectile::APortalAdventuresProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &APortalAdventuresProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void APortalAdventuresProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Only add impulse and destroy projectile if we hit a physics
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());

		Destroy();
	}
}

void APortalAdventuresProjectile::GetTrajectory_Implementation(FVector& out_Location, FRotator& out_Rotation, FRotator& out_ControlRotation, FVector& out_LinearVelocity, FVector& out_AngularVelocity)
{
	out_Location = GetActorLocation();
	out_Rotation = GetActorRotation();
	out_ControlRotation = FRotator::ZeroRotator;
	out_LinearVelocity = GetProjectileMovement()->Velocity;
	out_AngularVelocity = FVector::ZeroVector;
}

void APortalAdventuresProjectile::SetTrajectory_Implementation(const FVector& Location, const FRotator& Rotation, const FRotator& ControlRotation, const FVector& LinearVelocity, const FVector& AngularVelocity)
{
	FHitResult HitResult;
	SetActorLocationAndRotation(Location, Rotation, false, &HitResult, ETeleportType::TeleportPhysics);

	GetProjectileMovement()->SetVelocityInLocalSpace(UKismetMathLibrary::InverseTransformDirection(GetTransform(), LinearVelocity));
}

void APortalAdventuresProjectile::SetCollisionResponse_Implementation(ECollisionChannel Channel, ECollisionResponse Response)
{
	CollisionComp->SetCollisionResponseToChannel(Channel, Response);
}