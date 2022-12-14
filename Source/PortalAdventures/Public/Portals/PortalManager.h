// Jacob Holyfield - © Infinite Gaming Productions 2022
// Based On https://github.com/zachdavids/PortalSystem

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalManager.generated.h"

class APortal;
class USceneCaptureComponent2D;
class APortalAdventurePlayerController;
class UMaterialInterface;

UCLASS()
class PORTALADVENTURES_API APortalManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortalManager();

	void Tick(float DeltaTime) override;

	// Called by character every frame
	void UpdatePortals();

	void SetControllerOwner(APortalAdventurePlayerController* NewOwner);

	void SetPortalClass(TSubclassOf<class APortal> Class);

	void SetPortalableMaterials(TArray<UMaterialInterface*> PortableMats);

	// Tries to spawn a new blue portal, returns true if the portal was sucessfully spawned
	bool SpawnBluePortal(const FVector& Start, const FVector& End, const FLinearColor Color, FHitResult& OutHit);

	// Tries to spawn a new red portal, returns true if the portal was sucessfully spawned
	bool SpawnRedPortal(const FVector& Start, const FVector& End, const FLinearColor Color, FHitResult& OutHit);

	APortal* SpawnPortal(APortal* Target, const FLinearColor Color, const FVector& Start, const FVector& End, const ECollisionChannel& PreferedSurfaceOvbjectType, FHitResult& OutHit);

	bool TraceCorner(const APortal* Portal, const AActor* Surface, const FVector& Start, const FVector& End, const FVector& Forward, const FVector& Right, const FVector& Up, FHitResult& HitResult);

	bool VerifyPortalPlacement(const APortal* Portal, FVector& Origin, const AActor* Surface, const APortal* Target);

	void FitPortalAroundTargetPortal(const APortal* Portal, const APortal* Target, const FVector& Forward, const FVector& Right, const FVector& Up, FVector& Origin);

	bool FitPortalOnSurface(const APortal* Portal, const AActor* Surface, const FVector& Forward, const FVector& Right, const FVector& Up, const FVector& TopEdge, const FVector& BottomEdge, const FVector& RightEdge, const FVector& LeftEdge, FVector& Origin, int RecursionCount = 10);


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capture)
	USceneCaptureComponent2D* CaptureComponent;

private:

	APortal* BluePortal;

	APortal* RedPortal;

	UPROPERTY()
	APortalAdventurePlayerController* OwningController;

	/** Portal class to spawn */
	UPROPERTY()
	TSubclassOf<class APortal> PortalClass;

	UPROPERTY()
	TArray<UMaterialInterface*> MaterialsToSpawnPortalsOn;

};
