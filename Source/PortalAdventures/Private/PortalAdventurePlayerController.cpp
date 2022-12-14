// Jacob Holyfield - © Infinite Gaming Productions 2022


#include "PortalAdventurePlayerController.h"
#include "Portals/PortalManager.h"

APortalManager* APortalAdventurePlayerController::GetPortalManager()
{
	return PortalManager;
}

void APortalAdventurePlayerController::BeginPlay()
{
	PortalManager = GetWorld()->SpawnActor<APortalManager>(APortalManager::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
	PortalManager->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
	PortalManager->SetControllerOwner(this);
}

FMatrix APortalAdventurePlayerController::GetCameraProjectionMatrix()
{
	FMatrix ProjectionMatrix;

	if (GetLocalPlayer() != nullptr)
	{
		FSceneViewProjectionData PlayerProjectionData;
		GetLocalPlayer()->GetProjectionData(
			GetLocalPlayer()->ViewportClient->Viewport,
			PlayerProjectionData
		);

		ProjectionMatrix = PlayerProjectionData.ProjectionMatrix;
	}

	return ProjectionMatrix;
}
