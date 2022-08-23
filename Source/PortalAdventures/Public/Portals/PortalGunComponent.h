// Jacob Holyfield - © Infinite Gaming Productions 2022

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PortalGunComponent.generated.h"

class APortalAdventuresCharacter;

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

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

private:
	/** The Character holding this weapon*/
	APortalAdventuresCharacter* Character;	
};
