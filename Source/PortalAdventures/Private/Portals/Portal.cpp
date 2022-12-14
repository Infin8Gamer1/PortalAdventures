// Jacob Holyfield - © Infinite Gaming Productions 2022
// Based On https://github.com/zachdavids/PortalSystem

#include "Portals/Portal.h"
#include "Portals/PortalMathLibrary.h"
#include "Portals/PortableObjectInterface.h"
#include "PortalAdventuresCharacter.h"
#include "PortalAdventurePlayerController.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

DEFINE_LOG_CATEGORY(LogPortal);

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(RootComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(FName("MeshComponent"));
	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeshComponent->SetupAttachment(RootComponent);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(FName("BoxComponent"));
	BoxComponent->SetBoxExtent(FVector(100, 100, 175));
	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &APortal::OnOverlapBegin);
	BoxComponent->OnComponentEndOverlap.AddDynamic(this, &APortal::OnOverlapEnd);
	BoxComponent->SetupAttachment(RootComponent);

	ParticleComponent = CreateDefaultSubobject<UNiagaraComponent>(FName("PortalRingParticles"));
	ParticleComponent->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
	
	RenderTarget = NewObject<UTextureRenderTarget2D>(this, UTextureRenderTarget2D::StaticClass(), FName("RenderTarget"));
	RenderTarget->InitAutoFormat(1920, 1080);
	RenderTarget->AddressX = TextureAddress::TA_Wrap;
	RenderTarget->AddressY = TextureAddress::TA_Wrap;
	RenderTarget->bAutoGenerateMips = false;
	RenderTarget->RenderTargetFormat = ETextureRenderTargetFormat::RTF_RGBA16f;
	RenderTarget->Filter = TextureFilter::TF_Bilinear;

	MaterialInstance = UMaterialInstanceDynamic::Create(Material, this);
	MaterialInstance->SetTextureParameterValue(FName("RenderTargetTexture"), RenderTarget);
	
	MeshComponent->SetMaterial(0, UnlinkedMaterial);
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	//Remove Actors that have left the trigger
	for (AActor* ActorToRemove : ActorsToRemoveFromTrigger)
	{
		ActorsInTrigger.Remove(ActorToRemove);
	}
	//Clear Actors to remove array
	ActorsToRemoveFromTrigger.Empty();

	//Check if actors need to be teleported
	for (auto& Elem : ActorsInTrigger)
	{
		if (UPortalMathLibrary::CheckIsCrossing(
			Elem.Key->GetActorLocation(), 
			GetActorLocation(), 
			GetActorForwardVector(), 
			Elem.Value.bLastInFront, 
			Elem.Value.LastPosition))
		{
			TeleportActor(Elem.Key);
		}
	}
}

void APortal::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ResetPortalSurfaceCollision();

	Target = nullptr;

	UpdateOverlappingActorsCollision();
}

void APortal::UpdateOverlappingActorsCollision()
{
	//set collision response for actors in the trigger still back to block
	for (auto& Elem : ActorsInTrigger)
	{
		if (!Elem.Key->Implements<UPortableObjectInterface>())
			break;

		if (Target != nullptr)
		{
			IPortableObjectInterface::Execute_SetCollisionResponse(Elem.Key, AuctualPortalSurfaceObjectType, ECollisionResponse::ECR_Ignore);
		}
		else
		{
			IPortableObjectInterface::Execute_SetCollisionResponse(Elem.Key, AuctualPortalSurfaceObjectType, ECollisionResponse::ECR_Block);
		}
	}
}

void APortal::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//dont do anything if the portal isn't linked or if other actor doesn't implement the interface
	if (Target == nullptr || !OtherActor->Implements<UPortableObjectInterface>())
		return;

	//Set Collision To Ignore Portal Surfaces
	IPortableObjectInterface::Execute_SetCollisionResponse(OtherActor, AuctualPortalSurfaceObjectType, ECollisionResponse::ECR_Ignore);
	IPortableObjectInterface::Execute_EnteredPortal(OtherActor);

	//Add Actor to actors in trigger map
	FTeleportPos Pos;
	Pos.bLastInFront = false;
	Pos.LastPosition = OtherActor->GetActorLocation();
	ActorsInTrigger.Add(OtherActor, Pos);
}

void APortal::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	//dont do anything if the portal isn't linked or if other actor doesn't implement the interface
	if (Target == nullptr || !OtherActor->Implements<UPortableObjectInterface>())
		return;

	//Set Collision To Respect Portal Surfaces
	IPortableObjectInterface::Execute_SetCollisionResponse(OtherActor, AuctualPortalSurfaceObjectType, ECollisionResponse::ECR_Block);
	IPortableObjectInterface::Execute_ExitedPortal(OtherActor);

	//Remove From the Actors In Trigger Map
	ActorsToRemoveFromTrigger.Add(OtherActor);
}

void APortal::TeleportActor(AActor* ActorToTeleport)
{
	// Check that actor is valid to teleport and we are linked to another portal
	if (ActorToTeleport == nullptr || !ActorToTeleport->Implements<UPortableObjectInterface>() || Target == nullptr)
	{
		return;
	}

	//Get Objects Current Trajectory
	FVector SavedLocation;
	FRotator SavedRotation;
	FRotator SavedControlRotation;
	FVector SavedLinearVelocity;
	FVector SavedAngularVelocity;

	IPortableObjectInterface::Execute_GetTrajectory(ActorToTeleport, SavedLocation, SavedRotation, SavedControlRotation, SavedLinearVelocity, SavedAngularVelocity);

	//Calculate new Trajectory
	FVector NewLocation = UPortalMathLibrary::ConvertLocation(SavedLocation, this, Target);
	FRotator NewRotation = UPortalMathLibrary::ConvertRotation(SavedRotation, this, Target);
	FRotator NewControlRotation = UPortalMathLibrary::ConvertRotation(SavedControlRotation, this, Target);
	FVector NewLinearVelocity = UPortalMathLibrary::ConvertVelocity(SavedLinearVelocity, this, Target);
	//TODO: Figure out if we need to adjust angular velocity when teleporting actors
	FVector NewAngularVelocity = SavedAngularVelocity;

	IPortableObjectInterface::Execute_SetTrajectory(ActorToTeleport, NewLocation, NewRotation, NewControlRotation, NewLinearVelocity, NewAngularVelocity);

	UE_LOG(LogPortal, Log, TEXT("Teleported %s to %s"), *ActorToTeleport->GetName(), *Target->GetName());
}

void APortal::SetColor(FLinearColor Color)
{
	PortalColor = Color;
	ParticleComponent->SetNiagaraVariableLinearColor(FString("ParticleColor"), Color);
	//MaterialInstance->SetVectorParameterValue(FName("Color"), Color);
}

void APortal::SetTarget(APortal* NewTarget)
{
	//set the correct material for the portal
	if (NewTarget == nullptr)
	{
		MeshComponent->SetMaterial(0, UnlinkedMaterial);
	} else {
		MeshComponent->SetMaterial(0, MaterialInstance);
	}

	Target = NewTarget;

	UpdateOverlappingActorsCollision();
}

void APortal::SetPreferedPortalSurfaceObjectType(ECollisionChannel PreferedObjectType)
{
	PreferedPortalSurfaceObjectType = PreferedObjectType;
}

void APortal::SetPortalSurfaceCollision(const ECollisionChannel& CollisionChannel)
{
	if (GetPortalSurface() != nullptr)
	{
		UPrimitiveComponent* X = Cast<UPrimitiveComponent>(GetPortalSurface()->GetRootComponent());
		if (X != nullptr)
		{
			UE_LOG(LogPortal, Log, TEXT("Set %s collision to %i, was %i"), *GetPortalSurface()->GetName(), CollisionChannel, X->GetCollisionObjectType());
			X->SetCollisionObjectType(CollisionChannel);
		}
	}
}

ECollisionChannel APortal::GetPortalSurfaceCollision()
{
	if (GetPortalSurface() != nullptr)
	{
		UPrimitiveComponent* X = Cast<UPrimitiveComponent>(GetPortalSurface()->GetRootComponent());
		if (X != nullptr)
			return X->GetCollisionObjectType();
	}

	return ECollisionChannel::ECC_WorldStatic;
}

void APortal::ResetPortalSurfaceCollision()
{
	//Set portal surface back to normal collision as long as linked portal is not on the same surface
	bool IsSameSurfaceAsLinkedPortal = (Target != nullptr && Target->GetPortalSurface() == GetPortalSurface());

	if (!IsSameSurfaceAsLinkedPortal)
	{
		SetPortalSurfaceCollision(PortalSurfaceOldObjectType);
	}
}

void APortal::SetupPortalSurfaceCollision()
{
	bool IsSameSurfaceAsLinkedPortal = (Target != nullptr && Target->GetPortalSurface() == GetPortalSurface());

	if (IsSameSurfaceAsLinkedPortal)
	{
		AuctualPortalSurfaceObjectType = Target->AuctualPortalSurfaceObjectType;
		PortalSurfaceOldObjectType = Target->PortalSurfaceOldObjectType;
	}
	else
	{
		AuctualPortalSurfaceObjectType = PreferedPortalSurfaceObjectType;
		PortalSurfaceOldObjectType = GetPortalSurfaceCollision();
	}

	SetPortalSurfaceCollision(AuctualPortalSurfaceObjectType);
}

void APortal::SetPortalSurface(AActor* NewSurface)
{
	PortalSurface = NewSurface;

	UE_LOG(LogPortal, Warning, TEXT("Set %s Portal surface to %s"), *GetName(), *PortalSurface->GetName());
}

AActor* APortal::GetPortalSurface() const
{
	return PortalSurface;
}

APortal* APortal::GetTarget() const
{
	return Target;
}

UTextureRenderTarget2D* APortal::GetRenderTarget()
{
	return RenderTarget;
}


