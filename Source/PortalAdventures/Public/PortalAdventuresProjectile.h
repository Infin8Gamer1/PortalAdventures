// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portals/PortableObjectInterface.h"
#include "PortalAdventuresProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class APortalAdventuresProjectile : public AActor, public IPortableObjectInterface
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

public:
	APortalAdventuresProjectile();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	void GetTrajectory_Implementation(FVector& out_Location, FRotator& out_Rotation, FRotator& out_ControlRotation, FVector& out_LinearVelocity, FVector& out_AngularVelocity) override;

	void SetTrajectory_Implementation(const FVector& Location, const FRotator& Rotation, const FRotator& ControlRotation, const FVector& LinearVelocity, const FVector& AngularVelocity) override;

	void SetCollisionResponse_Implementation(ECollisionChannel Channel, ECollisionResponse Response) override;
};

