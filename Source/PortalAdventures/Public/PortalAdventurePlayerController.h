// Jacob Holyfield - © Infinite Gaming Productions 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PortalAdventurePlayerController.generated.h"

class APortalManager;
class UMaterialInterface;

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

	/** Portal class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Portal)
	TSubclassOf<class APortal> PortalClass;

	UPROPERTY(EditDefaultsOnly, Category = Portal)
	TArray<UMaterialInterface*> MaterialsToSpawnPortalsOn;

protected:
	virtual void BeginPlay() override;

	APortalManager* PortalManager;
};
