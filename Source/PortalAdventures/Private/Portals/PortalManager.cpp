// Jacob Holyfield - © Infinite Gaming Productions 2022
// Based On https://github.com/zachdavids/PortalSystem

#include "Portals/PortalManager.h"
#include "Portals/Portal.h"
#include "Portals/PortalMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "PortalAdventuresCharacter.h"
#include "Components/ArrowComponent.h"
#include "PortalAdventurePlayerController.h"
#include "Components/SceneCaptureComponent2D.h"
#include "EngineUtils.h"

// Sets default values
APortalManager::APortalManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = ETickingGroup::TG_PostUpdateWork;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(RootComponent);

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(FName("CaptureComponent"));
	CaptureComponent->SetupAttachment(RootComponent);

	FPostProcessSettings CaptureSettings;
	//CaptureSettings.bOverride_DynamicGlobalIlluminationMethod = true;
	//CaptureSettings.DynamicGlobalIlluminationMethod = EDynamicGlobalIlluminationMethod::ScreenSpace;
	CaptureSettings.bOverride_AmbientOcclusionQuality = true;
	CaptureSettings.bOverride_MotionBlurAmount = true;
	CaptureSettings.bOverride_SceneFringeIntensity = true;
	//CaptureSettings.bOverride_GrainIntensity = true;
	CaptureSettings.bOverride_FilmGrainIntensity = true;
	CaptureSettings.bOverride_ScreenSpaceReflectionQuality = true;
	//CaptureSettings.bOverride_ScreenPercentage = true;
	CaptureSettings.AmbientOcclusionQuality = 0.0f;
	CaptureSettings.MotionBlurAmount = 0.0f;
	CaptureSettings.SceneFringeIntensity = 0.0f;
	//CaptureSettings.GrainIntensity = 0.0f; --> Depreciated replaced with FilmGrainIntensity
	CaptureSettings.FilmGrainIntensity = 0.0f;
	CaptureSettings.ScreenSpaceReflectionQuality = 0.0f;
	//CaptureSettings.ScreenPercentage = 100.0f; --> Depreciated

	CaptureComponent->bCaptureEveryFrame = false;
	CaptureComponent->bCaptureOnMovement = false;
	CaptureComponent->LODDistanceFactor = 3;
	CaptureComponent->TextureTarget = nullptr;
	CaptureComponent->bEnableClipPlane = true;
	CaptureComponent->bUseCustomProjectionMatrix = true;
	CaptureComponent->CaptureSource = ESceneCaptureSource::SCS_SceneColorSceneDepth;
	CaptureComponent->PostProcessSettings = CaptureSettings;

}

// Called when the game starts or when spawned
void APortalManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APortalManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdatePortals();
}


// Called every frame
void APortalManager::UpdatePortals()
{
	if (OwningController != nullptr)
	{
		APlayerCameraManager* CameraManager = OwningController->PlayerCameraManager;
		if (CameraManager != nullptr)
		{
			for (TActorIterator<APortal> It(GetWorld()); It; ++It)
			{
				APortal* Portal = *It;
				APortal* Target = Portal->GetTarget();
				if (Portal != nullptr && Target != nullptr)
				{
					FVector Position = UPortalMathLibrary::ConvertLocation(CameraManager->GetCameraLocation(), Portal, Target);
					FRotator Rotation = UPortalMathLibrary::ConvertRotation(CameraManager->GetCameraRotation(), Portal, Target);
					CaptureComponent->SetWorldLocationAndRotation(Position, Rotation);
					CaptureComponent->ClipPlaneNormal = Target->GetActorForwardVector();
					CaptureComponent->ClipPlaneBase = Target->GetActorLocation() + (CaptureComponent->ClipPlaneNormal * -1.5f);;
					CaptureComponent->CustomProjectionMatrix = OwningController->GetCameraProjectionMatrix();
					CaptureComponent->TextureTarget = Portal->GetRenderTarget();
					CaptureComponent->CaptureScene();
				}
			}
		}
	}
}

void APortalManager::SetControllerOwner(APortalAdventurePlayerController* NewOwner)
{
	OwningController = NewOwner;
}

void APortalManager::SetPortalClass(TSubclassOf<class APortal> Class)
{
	PortalClass = Class;
}

void APortalManager::SetPortalableMaterials(TArray<UMaterialInterface*> PortableMats)
{
	MaterialsToSpawnPortalsOn = PortableMats;
}

bool APortalManager::SpawnBluePortal(const FVector& Start, const FVector& End, const FLinearColor Color, FHitResult& OutHit)
{
	APortal* NewPortal = SpawnPortal(RedPortal, Color, Start, End, ECollisionChannel::ECC_GameTraceChannel3, OutHit);

	if (NewPortal != nullptr)
	{
		if (BluePortal != nullptr)
		{
			BluePortal->Destroy();
		}

		NewPortal->SetupPortalSurfaceCollision();

		BluePortal = NewPortal;

		return true;
	}

	return false;
}

bool APortalManager::SpawnRedPortal(const FVector& Start, const FVector& End, const FLinearColor Color, FHitResult& OutHit)
{
	APortal* NewPortal = SpawnPortal(BluePortal, Color, Start, End, ECollisionChannel::ECC_GameTraceChannel2, OutHit);
	
	if (NewPortal != nullptr)
	{
		if (RedPortal != nullptr)
		{
			RedPortal->Destroy();
		}

		NewPortal->SetupPortalSurfaceCollision();

		RedPortal = NewPortal;

		return true;
	}

	return false;
}

APortal* APortalManager::SpawnPortal(APortal* Target, const FLinearColor Color, const FVector& Start, const FVector& End, const ECollisionChannel& PreferedSurfaceObjectType, FHitResult& OutHit)
{
	APortal* Portal = nullptr;

	UWorld* const World = GetWorld();
	if (World != nullptr && PortalClass != nullptr)
	{
		FCollisionQueryParams TraceParams;
		TraceParams.bTraceComplex = true;

		if (RedPortal != nullptr)
		{
			TraceParams.AddIgnoredActor(RedPortal);
		}

		if (BluePortal != nullptr)
		{
			TraceParams.AddIgnoredActor(BluePortal);
		}

		if (OwningController->GetPawn() != nullptr)
		{
			TraceParams.AddIgnoredActor(OwningController->GetPawn());
		}

		if (GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, TraceParams))
		{
			UStaticMeshComponent* mesh = Cast<UStaticMeshComponent>(OutHit.GetComponent());
			if (mesh != nullptr)
			{
				bool CanSpawnOnSurface = false;
				TArray<UMaterialInterface*> MaterialsHit = mesh->GetMaterials();
				for (UMaterialInterface* hitMat : MaterialsHit)
				{
					if (MaterialsToSpawnPortalsOn.Contains(hitMat))
					{
						CanSpawnOnSurface = true;
						break;
					}

					UE_LOG(LogPortal, Warning, TEXT("Portal can't spawn on %s material"), *hitMat->GetName());
				}

				if (CanSpawnOnSurface)
				{
					FActorSpawnParameters ActorSpawnParams;
					ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

					FVector Origin = OutHit.Location + OutHit.ImpactNormal;
					FRotator Rotation = OutHit.ImpactNormal.Rotation();
					if (FVector::DotProduct(OutHit.ImpactNormal, FVector::UpVector) > KINDA_SMALL_NUMBER)
					{
						Rotation.Roll -= GetActorRotation().Yaw;
					}
					else if (FVector::DotProduct(OutHit.ImpactNormal, FVector::UpVector) < -KINDA_SMALL_NUMBER)
					{
						Rotation.Roll += GetActorRotation().Yaw;
					}

					Portal = World->SpawnActor<APortal>(PortalClass, Origin, Rotation, ActorSpawnParams);

					if (VerifyPortalPlacement(Portal, Origin, OutHit.GetActor(), Target))
					{
						Portal->SetActorLocation(Origin);

						Portal->SetTarget(Target);

						if (Target != nullptr)
						{
							Target->SetTarget(Portal);
						}

						Portal->SetColor(Color);

						Portal->SetPreferedPortalSurfaceObjectType(PreferedSurfaceObjectType);
						Portal->SetPortalSurface(OutHit.GetActor());
					}
					else
					{
						Portal->Destroy();
						Portal = nullptr;
					}
				}
			}
		}
	}

	return Portal;
}

bool APortalManager::VerifyPortalPlacement(const APortal* Portal, FVector& Origin, const AActor* Surface, const APortal* Target)
{
	FVector OriginalOrigin = Origin;

	FVector Forward = Portal->GetActorForwardVector();
	FVector Right = Portal->GetActorRightVector();
	FVector Up = Portal->GetActorUpVector();

	//Check if surface is moving
	//const AActor* Surface = Portal->GetPortalSurface();
	if (!Surface->GetVelocity().IsNearlyZero())
	{
		UE_LOG(LogPortal, Warning, TEXT("Portal surface moving"));
		return false;
	}

	//Check if portal is overlapping linked portal
	//const APortal* Target = Portal->GetTarget();
	if (Target != nullptr)
	{
		FitPortalAroundTargetPortal(Portal, Target, Forward, Right, Up, Origin);
	}

	//Check if portal fits on surface
	FVector PortalExtent = Portal->CalculateComponentsBoundingBoxInLocalSpace().GetExtent();
	const FVector TopEdge = Up * PortalExtent.Z;
	const FVector BottomEdge = -TopEdge;
	const FVector RightEdge = Right * PortalExtent.Y;
	const FVector LeftEdge = -RightEdge;

	if (!FitPortalOnSurface(Portal, Surface, Forward, Right, Up, TopEdge, BottomEdge, RightEdge, LeftEdge, Origin))
	{
		UE_LOG(LogPortal, Warning, TEXT("Portal does not fit on surface"));
		return false;
	}

	return true;
}

void APortalManager::FitPortalAroundTargetPortal(const APortal* Portal, const APortal* Target, const FVector& Forward, const FVector& Right, const FVector& Up, FVector& Origin)
{
	FVector TargetForward = Target->GetActorForwardVector();

	//Reposition if portals are on the same face
	if (FVector::DotProduct(Forward, TargetForward) > 1.f - KINDA_SMALL_NUMBER)
	{
		FVector Distance = Origin - Target->GetActorLocation();
		FVector RightProjection = FVector::DotProduct(Distance, Right) * Right;
		FVector UpProjection = FVector::DotProduct(Distance, Up) * Up;

		float RightProjectionLength = RightProjection.Size();
		float UpProjectionLength = UpProjection.Size();

		if (RightProjectionLength < 1.0f)
		{
			RightProjection = Right;
		}

		FVector Size = Portal->CalculateComponentsBoundingBoxInLocalSpace().GetSize();
		if (UpProjectionLength < Size.Z && RightProjectionLength < Size.Y)
		{
			RightProjection.Normalize();
			Origin += RightProjection * (Size.Y - RightProjectionLength + 1.0f);
		}
	}
}

bool APortalManager::FitPortalOnSurface(const APortal* Portal, const AActor* Surface, const FVector& Forward, const FVector& Right, const FVector& Up, const FVector& TopEdge, const FVector& BottomEdge, const FVector& RightEdge, const FVector& LeftEdge, FVector& Origin, int RecursionCount)
{
	if (RecursionCount == 0)
	{
		return false;
	}

	TArray<FVector> Corners;
	Corners.SetNumUninitialized(4);
	Corners[0] = Origin + TopEdge + LeftEdge;
	Corners[1] = Origin + TopEdge + RightEdge;
	Corners[2] = Origin + BottomEdge + LeftEdge;
	Corners[3] = Origin + BottomEdge + RightEdge;

	FHitResult HitResult;
	for (int i = 0; i < 4; ++i)
	{
		if (TraceCorner(Portal, Surface, Origin, Corners[i], Forward, Right, Up, HitResult))
		{
			float Distance = FMath::PointDistToLine(Corners[i], FVector::CrossProduct(HitResult.Normal, Forward), HitResult.Location) + 5.0f;
			Origin += HitResult.Normal * Distance;

			return FitPortalOnSurface(Portal, Surface, Forward, Right, Up, TopEdge, BottomEdge, RightEdge, LeftEdge, Origin, RecursionCount - 1);
		}
	}

	return true;
}

bool APortalManager::TraceCorner(const APortal* Portal, const AActor* Surface, const FVector& Start, const FVector& End, const FVector& Forward, const FVector& Right, const FVector& Up, FHitResult& HitResult)
{
	bool bFoundHit = false;

	FCollisionQueryParams TraceParams;
	TraceParams.bFindInitialOverlaps = true;
	TraceParams.bTraceComplex = true;
	TraceParams.AddIgnoredActor(Portal);
	TraceParams.AddIgnoredActor(Surface);

	if (RedPortal != nullptr)
	{
		TraceParams.AddIgnoredActor(RedPortal);
	}

	if (BluePortal != nullptr)
	{
		TraceParams.AddIgnoredActor(BluePortal);
	}

	//Get all actors that implement PortableObject and Ignore them (will include our character)
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UPortableObjectInterface::StaticClass(), FoundActors);
	TraceParams.AddIgnoredActors(FoundActors);

	//Check inner surface for intersections
	FHitResult InnerHitResult;
	if (GetWorld()->LineTraceSingleByChannel(InnerHitResult, Start - Forward, End - Forward, ECC_Visibility, TraceParams))
	{
		HitResult = InnerHitResult;
		bFoundHit = true;
	}

	//Check outer surface for intersections
	FHitResult OuterHitResult;
	if (GetWorld()->LineTraceSingleByChannel(OuterHitResult, Start + Forward, End + Forward, ECC_Visibility, TraceParams))
	{
		HitResult = OuterHitResult;
		bFoundHit = true;
	}

	//Determine the closer intersection, if any
	if (InnerHitResult.bBlockingHit && OuterHitResult.bBlockingHit)
	{
		HitResult = (InnerHitResult.Distance <= OuterHitResult.Distance) ? InnerHitResult : OuterHitResult;
	}

	//Check if corner overlaps surface, if not we reached the end of surface and fake it as collision
	FCollisionQueryParams VerticalTraceParams;

	FHitResult OverlapHitResult;

	float Fraction = 0.f;
	FVector Direction = End - Start;
	FVector Location(0.f);
	while (Fraction <= 1.0f + KINDA_SMALL_NUMBER)
	{
		Location = Start + (Direction * Fraction);
		if (!GetWorld()->LineTraceSingleByChannel(OverlapHitResult, Location + (Forward * 2), Location - (Forward * 2), ECC_Visibility, VerticalTraceParams))
		{
			//Found an edge now determine its normal
			FVector RightProjection = Direction.ProjectOnToNormal(Right);
			FVector UpProjection = Direction.ProjectOnToNormal(Up);

			int32 Vertical = GetWorld()->LineTraceSingleByChannel
			(
				OverlapHitResult,
				Location + (RightProjection * 0.05) - (UpProjection * 0.05) + (Forward * 2),
				Location + (RightProjection * 0.05) - (UpProjection * 0.05) - (Forward * 2),
				ECC_Visibility,
				VerticalTraceParams
			);

			int32 Horizontal = GetWorld()->LineTraceSingleByChannel
			(
				OverlapHitResult,
				Location + (UpProjection * 0.05) - (RightProjection * 0.05) + (Forward * 2),
				Location + (UpProjection * 0.05) - (RightProjection * 0.05) - (Forward * 2),
				ECC_Visibility,
				VerticalTraceParams
			);

			if (Vertical)
			{
				OverlapHitResult.Normal = -UpProjection.GetClampedToSize(0, 1.0f);
			}

			if (Horizontal)
			{
				OverlapHitResult.Normal = -RightProjection.GetClampedToSize(0, 1.0f);
			}

			//Corner, choose one
			if (!Vertical && !Horizontal)
			{
				OverlapHitResult.Normal = -UpProjection.GetClampedToSize(0, 1.0f);
			}

			OverlapHitResult.bBlockingHit = true;
			OverlapHitResult.Location = Location;
			OverlapHitResult.Distance = FVector::Distance(Start, OverlapHitResult.Location);
			OverlapHitResult.ImpactNormal = FVector::CrossProduct(OverlapHitResult.Normal, Forward);

			if (bFoundHit)
			{
				if (OverlapHitResult.Distance <= HitResult.Distance)
				{
					HitResult = OverlapHitResult;
				}
			}
			else
			{
				HitResult = OverlapHitResult;
			}

			bFoundHit = true;
			break;
		}

		OverlapHitResult.Reset();
		Fraction += 0.05;
	}

	return bFoundHit;
}

