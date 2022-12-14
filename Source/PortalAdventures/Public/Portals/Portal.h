// Jacob Holyfield - © Infinite Gaming Productions 2022
// Based On https://github.com/zachdavids/PortalSystem

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class UMaterial;
class UBoxComponent;
class USceneComponent;
class UNiagaraComponent;
class UStaticMeshComponent;
class UTextureRenderTarget2D;

DECLARE_LOG_CATEGORY_EXTERN(LogPortal, Log, All);

USTRUCT(BlueprintType)
struct PORTALADVENTURES_API FTeleportPos
{
	GENERATED_BODY()

public:

	UPROPERTY()
	FVector LastPosition;

	UPROPERTY()
	bool bLastInFront;
};


UCLASS()
class PORTALADVENTURES_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	void ResetPortalSurfaceCollision();

	void SetupPortalSurfaceCollision();

	void TeleportActor(AActor* Actor);

	void SetTarget(APortal* NewTarget);

	void SetPortalSurface(AActor* Surface);

	void SetPortalSurfaceCollision(const ECollisionChannel& CollisionChannel);

	ECollisionChannel GetPortalSurfaceCollision();

	void SetColor(FLinearColor Color);

	void SetPreferedPortalSurfaceObjectType(ECollisionChannel PreferedObjectType);

	AActor* GetPortalSurface() const;

	APortal* GetTarget() const;

	UTextureRenderTarget2D* GetRenderTarget();

	void UpdateOverlappingActorsCollision();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Portal)
	UStaticMeshComponent* MeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Portal)
	UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Portal)
	UNiagaraComponent* ParticleComponent;

	UPROPERTY(EditAnywhere, Category = Capture)
	UTextureRenderTarget2D* RenderTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capture)
	UMaterial* Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capture)
	UMaterial* UnlinkedMaterial;

	UPROPERTY(EditAnywhere, Category = Capture)
	UMaterialInstanceDynamic* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Capture)
	APortal* Target;

	ECollisionChannel PreferedPortalSurfaceObjectType = ECollisionChannel::ECC_GameTraceChannel2;
	ECollisionChannel AuctualPortalSurfaceObjectType = ECollisionChannel::ECC_GameTraceChannel2;
	ECollisionChannel PortalSurfaceOldObjectType = ECollisionChannel::ECC_WorldStatic;

private:

	AActor* PortalSurface;

	TMap<AActor*, FTeleportPos> ActorsInTrigger;

	TArray<AActor*> ActorsToRemoveFromTrigger;

	FLinearColor PortalColor;
};
