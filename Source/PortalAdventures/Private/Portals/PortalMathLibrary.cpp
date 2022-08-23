// Fill out your copyright notice in the Description page of Project Settings.


#include "Portals/PortalMathLibrary.h"

#include "Portals/Portal.h"
#include "GameFramework/Actor.h"

FVector UPortalMathLibrary::ConvertLocation(FVector const& Location, AActor* Portal, AActor* Target)
{
	FVector Direction = Location - Portal->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FVector Dots;
	Dots.X = FVector::DotProduct(Direction, Portal->GetActorForwardVector());
	Dots.Y = FVector::DotProduct(Direction, Portal->GetActorRightVector());
	Dots.Z = FVector::DotProduct(Direction, Portal->GetActorUpVector());

	FVector NewDirection = Dots.X * -Target->GetActorForwardVector()
		+ Dots.Y * -Target->GetActorRightVector()
		+ Dots.Z * Target->GetActorUpVector();

	return TargetLocation + NewDirection;
}

FRotator UPortalMathLibrary::ConvertRotation(FRotator const& Rotation, AActor* Portal, AActor* Target)
{
	FVector RotationAdjustment(0.f, 0.f, -180.f);
	FVector LocalAdjustment = FVector::ZeroVector;

	if (FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) > KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = 180.f;
		RotationAdjustment.Z += LocalAdjustment.X;
	}
	else if (FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) < -KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = -180.f;
		RotationAdjustment.Z -= LocalAdjustment.X;
	}

	FQuat QuatRotation = FQuat::MakeFromEuler(RotationAdjustment) * FQuat(Rotation);
	FQuat LocalQuat = FQuat::MakeFromEuler(LocalAdjustment) * Portal->GetActorTransform().GetRotation().Inverse() * QuatRotation;
	FQuat NewWorldQuat = Target->GetActorTransform().GetRotation() * LocalQuat;
	return NewWorldQuat.Rotator();
}

FVector UPortalMathLibrary::ConvertVelocity(FVector const& Velocity, AActor* Portal, AActor* Target)
{
	// Compute new velocity

	/*
	FVector Dots;
	Dots.X = FVector::DotProduct(Velocity, Portal->GetActorForwardVector());
	Dots.Y = FVector::DotProduct(Velocity, Portal->GetActorRightVector());
	Dots.Z = FVector::DotProduct(Velocity, Portal->GetActorUpVector());

	FVector NewVelocity = Dots.X * Target->GetActorForwardVector()
		+ Dots.Y * Target->GetActorRightVector()
		+ Dots.Z * Target->GetActorUpVector();
	*/

	FVector NewVelocity =
		FVector::DotProduct(Velocity, Portal->GetActorForwardVector()) * Target->GetActorForwardVector() +
		FVector::DotProduct(Velocity, Portal->GetActorRightVector()) * Target->GetActorRightVector() +
		FVector::DotProduct(Velocity, Portal->GetActorUpVector()) * Target->GetActorUpVector();

	return -NewVelocity;
}

FVector UPortalMathLibrary::ConvertLocation2(FVector const& Location, AActor* Portal, AActor* Target)
{
	if (Portal == nullptr || Target == nullptr)
	{
		return FVector::ZeroVector;
	}

	FVector Direction = Location - Portal->GetActorLocation();
	FVector TargetLocation = Target->GetActorLocation();

	FVector Dots;
	Dots.X = FVector::DotProduct(Direction, Portal->GetActorForwardVector());
	Dots.Y = FVector::DotProduct(Direction, Portal->GetActorRightVector());
	Dots.Z = FVector::DotProduct(Direction, Portal->GetActorUpVector());

	FVector NewDirection = Dots.X * Target->GetActorForwardVector()
		+ Dots.Y * Target->GetActorRightVector()
		+ Dots.Z * Target->GetActorUpVector();

	return TargetLocation + NewDirection;
}

FRotator UPortalMathLibrary::ConvertRotation2(FRotator const& Rotation, AActor* Portal, AActor* Target)
{
	if (Portal == nullptr || Target == nullptr)
	{
		return FRotator::ZeroRotator;
	}

	FTransform PortalTransform = Portal->GetActorTransform();
	FTransform TargetTransform = Target->GetActorTransform();
	FQuat QuatRotation = FQuat(Rotation);

	FQuat LocalQuat = PortalTransform.GetRotation().Inverse() * QuatRotation;
	FQuat NewWorldQuat = TargetTransform.GetRotation() * LocalQuat;

	return NewWorldQuat.Rotator();

	/*
	FVector RotationAdjustment(0.f, 0.f, -180.f);
	FVector LocalAdjustment = FVector::ZeroVector;

	if (FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) > KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = 180.f;
		RotationAdjustment.Z += LocalAdjustment.X;
	}
	else if (FVector::DotProduct(Portal->GetActorForwardVector(), FVector::UpVector) < -KINDA_SMALL_NUMBER)
	{
		LocalAdjustment.X = FMath::UnwindDegrees(Portal->GetTransform().GetRotation().Euler().X);
		LocalAdjustment.Y = -180.f;
		RotationAdjustment.Z -= LocalAdjustment.X;
	}

	FQuat QuatRotation = FQuat::MakeFromEuler(RotationAdjustment) * FQuat(Rotation);
	FQuat LocalQuat = FQuat::MakeFromEuler(LocalAdjustment) * Portal->GetActorTransform().GetRotation().Inverse() * QuatRotation;
	FQuat NewWorldQuat = Target->GetActorTransform().GetRotation() * LocalQuat;
	return NewWorldQuat.Rotator();
	*/
}

bool UPortalMathLibrary::CheckIsInFront(FVector const& Point, FVector const& PortalLocation, FVector const& PortalNormal)
{
	FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);

	return PortalPlane.PlaneDot(Point) >= 0;
}

bool UPortalMathLibrary::CheckIsCrossing(FVector const& Point, FVector const& PortalLocation, FVector const& PortalNormal, bool& out_LastInFront, FVector& out_LastPosition)
{
	FPlane PortalPlane = FPlane(PortalLocation, PortalNormal);
	FVector IntersectionPoint;
	bool IsIntersecting = FMath::SegmentPlaneIntersection(out_LastPosition, Point, PortalPlane, IntersectionPoint);
	bool IsInFront = CheckIsInFront(Point, PortalLocation, PortalNormal);
	bool IsCrossing = false;

	if (IsIntersecting && !IsInFront && out_LastInFront)
	{
		IsCrossing = true;
	}

	out_LastInFront = IsInFront;
	out_LastPosition = Point;

	return IsCrossing;
}