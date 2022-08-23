// Jacob Holyfield - © Infinite Gaming Productions 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PortableObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPortableObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PORTALADVENTURES_API IPortableObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void GetTrajectory(FVector& out_Location, FRotator& out_Rotation, FRotator& out_ControlRotation, FVector& out_LinearVelocity, FVector& out_AngularVelocity) = 0;

	virtual void SetTrajectory(const FVector& Location, const FRotator& Rotation, const FRotator& ControlRotation, const FVector& LinearVelocity, const FVector& AngularVelocity) = 0;

	virtual void SetCollisionResponse(ECollisionChannel Channel, ECollisionResponse Response) = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EnteredPortal();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ExitedPortal();
};
