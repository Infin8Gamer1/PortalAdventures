// Jacob Holyfield - © Infinite Gaming Productions 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TriggerableObjectInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UTriggerableObjectInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PORTALADVENTURES_API ITriggerableObjectInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnTrigger(AActor* TriggeringActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void OnUntrigger(AActor* TriggeringActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool IsTriggered();
};
