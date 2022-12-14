// Jacob Holyfield - © Infinite Gaming Productions 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PortalAdventurePlayerController.generated.h"

class APortalManager;

/**
 * 
 */
UCLASS()
class PORTALADVENTURES_API APortalAdventurePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	APortalManager* GetPortalManager();

	FMatrix GetCameraProjectionMatrix();

protected:
	virtual void BeginPlay() override;

	APortalManager* PortalManager;
};
