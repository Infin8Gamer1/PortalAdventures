// Jacob Holyfield - © Infinite Gaming Productions 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PortalGunComponent.generated.h"

class APortalAdventuresCharacter;
class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTALADVENTURES_API UPortalGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPortalGunComponent();

	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachWeapon(APortalAdventuresCharacter* TargetCharacter, bool AttachOwner = true);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireBluePortal();

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireRedPortal();

private:
	void PlayEffects(const FLinearColor PortalColor, const FHitResult& OutHit, bool Sucess);

protected:
	/** Ends gameplay for this component. */
	UFUNCTION()
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FLinearColor BluePortalColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FLinearColor RedPortalColor;

	/** Sound to play when we fire a portal*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* PortalFireSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UNiagaraSystem* PortalFailParticles;

	/** Sound to play if the portal fails to spawn*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	USoundBase* PortalFailSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Particles to spawn when we shoot a portal*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UNiagaraSystem* ShootParticle;

private:
	/** The Character holding this weapon*/
	APortalAdventuresCharacter* Character;	
};
