// Jacob Holyfield - © Infinite Gaming Productions 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PortableObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
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
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void GetTrajectory(FVector& out_Location, FRotator& out_Rotation, FRotator& out_ControlRotation, FVector& out_LinearVelocity, FVector& out_AngularVelocity);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetTrajectory(const FVector& Location, const FRotator& Rotation, const FRotator& ControlRotation, const FVector& LinearVelocity, const FVector& AngularVelocity);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCollisionResponse(ECollisionChannel Channel, ECollisionResponse Response);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void EnteredPortal();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void ExitedPortal();
};
