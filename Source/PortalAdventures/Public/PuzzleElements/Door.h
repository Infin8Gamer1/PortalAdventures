// Jacob Holyfield - © Infinite Gaming Productions 2022

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PuzzleElements/TriggerableObjectInterface.h"
#include "Components/TimelineComponent.h"
#include "Door.generated.h"

class UStaticMeshComponent;

UCLASS()
class PORTALADVENTURES_API ADoor : public AActor, public ITriggerableObjectInterface
{
	GENERATED_BODY()


/* VARS */
public:
	/*The position the Door Mesh should be when the door is open*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (MakeEditWidget))
	FTransform DoorOpenPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCurveFloat* DoorTimelineFloatCurve;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* DoorMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTimelineComponent* DoorTimeline;

private:
	/*The position the door mesh should be when the door is closed --
	Currently gets set to Door Mesh Relative position at begin play*/
	FTransform DoorClosedPosition;

	/*Is the door set to be open?*/
	bool bIsDoorOpen;

/* FUNCTIONS */
public:	
	// Sets default values for this actor's properties
	ADoor();

	UFUNCTION(BlueprintCallable)
	void OpenDoor();

	UFUNCTION(BlueprintCallable)
	void CloseDoor();

	void OnTrigger_Implementation(AActor* TriggeringActor) override;

	void OnUntrigger_Implementation(AActor* TriggeringActor) override;

	bool IsTriggered_Implementation() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	FOnTimelineFloat UpdateFunctionFloat;

	UFUNCTION()
	void UpdateTimelineComp(float Output);

};
