// Jacob Holyfield - © Infinite Gaming Productions 2022


#include "Portals/PortalGunComponent.h"
#include "Portals/PortalManager.h"
#include "PortalAdventuresCharacter.h"
#include "PortalAdventurePlayerController.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UPortalGunComponent::UPortalGunComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	BluePortalColor = FLinearColor(0.0f, 30.0f, 50.0f, 1.0f);
	RedPortalColor = FLinearColor(50.0f, 8.0f, 0.0f, 1.0f);
	PortalFailParticles = nullptr;
	PortalFailSound = nullptr;
	PortalFireSound = nullptr;
	ShootParticle = nullptr;
	FireAnimation = nullptr;
	PortalClass = nullptr;
	PortalManager = nullptr;
}

void UPortalGunComponent::FireBluePortal()
{
	if (Character == nullptr || PortalManager == nullptr)
	{
		return;
	}

	bool sucess = false;
	FHitResult OutHit;

	if (PortalManager != nullptr)
	{
		const FVector Start = Character->GetFirstPersonCameraComponent()->GetComponentLocation();
		const FVector End = Start + Character->GetFirstPersonCameraComponent()->GetForwardVector() * 10000.f;
		sucess = PortalManager->SpawnBluePortal(Start, End, BluePortalColor, OutHit);
	}

	PlayEffects(BluePortalColor, OutHit, sucess);
}

void UPortalGunComponent::FireRedPortal()
{
	if (Character == nullptr || PortalManager == nullptr)
	{
		return;
	}

	bool sucess = false;
	FHitResult OutHit;

	const FVector Start = Character->GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector End = Start + Character->GetFirstPersonCameraComponent()->GetForwardVector() * 10000.f;
	sucess = PortalManager->SpawnRedPortal(Start, End, RedPortalColor, OutHit);

	PlayEffects(RedPortalColor, OutHit, sucess);
}

void UPortalGunComponent::PlayEffects(const FLinearColor PortalColor, const FHitResult& OutHit, bool Sucess)
{
	if (Character == nullptr)
	{
		return;
	}

	// Try and play the sound if specified
	if (PortalFireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, PortalFireSound, Character->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	if (ShootParticle != nullptr)
	{
		USkeletalMeshComponent* WeaponMesh = Cast<USkeletalMeshComponent>(GetOwner()->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		if (WeaponMesh != nullptr)
		{
			FVector Location = WeaponMesh->GetSocketLocation(FName("Muzzle"));
			FRotator Rotation = UKismetMathLibrary::FindLookAtRotation(Location, 
				(OutHit.IsValidBlockingHit()) ? OutHit.ImpactPoint : OutHit.TraceEnd);

			UNiagaraComponent* Particle = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ShootParticle, Location, Rotation);
			Particle->SetNiagaraVariableLinearColor(FString("ParticleColor"), PortalColor);
		}
	}

	//If portal Failed to spawn then play fail particles at hit location
	if (!Sucess && PortalFailParticles != nullptr)
	{
		UNiagaraComponent* NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), PortalFailParticles, OutHit.ImpactPoint, OutHit.ImpactNormal.Rotation());
		NiagaraComp->SetNiagaraVariableLinearColor(FString("ParticleColor"), PortalColor);
	}

	//If Portal Failed to spawn play fail sound at hit location
	if (!Sucess && PortalFailSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), PortalFailSound, OutHit.ImpactPoint);
	}
}

void UPortalGunComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Character != nullptr)
	{
		// Unregister from the OnUseItem Event
		Character->OnFireRedPortal.RemoveDynamic(this, &UPortalGunComponent::FireRedPortal);
		Character->OnFireBluePortal.RemoveDynamic(this, &UPortalGunComponent::FireBluePortal);
	}
}

void UPortalGunComponent::AttachWeapon(APortalAdventuresCharacter* TargetCharacter, bool AttachOwner)
{
	Character = TargetCharacter;
	if (Character != nullptr)
	{
		if (AttachOwner)
		{
			// Attach the weapon to the First Person Character
			FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
			GetOwner()->AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));
		}

		APortalAdventurePlayerController* Controller = Cast<APortalAdventurePlayerController>(Character->GetController());
		if (Controller != nullptr)
		{
			PortalManager = Controller->GetPortalManager();
			if (PortalManager != nullptr)
			{
				PortalManager->SetPortalClass(PortalClass);
				PortalManager->SetPortalableMaterials(MaterialsToSpawnPortalsOn);
			}
		}
		

		// Register so that Fire is called every time the character tries to use the item being held
		Character->OnFireRedPortal.AddDynamic(this, &UPortalGunComponent::FireRedPortal);
		Character->OnFireBluePortal.AddDynamic(this, &UPortalGunComponent::FireBluePortal);
	}
}

