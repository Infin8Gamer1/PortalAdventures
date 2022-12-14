// Jacob Holyfield - © Infinite Gaming Productions 2022


#include "PuzzleElements/Door.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ADoor::ADoor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bIsDoorOpen = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(FName("RootComponent"));
	SetRootComponent(RootComponent);

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);

	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>(FName("DoorTimelineComponent"));
}

// Called when the game starts or when spawned
void ADoor::BeginPlay()
{
	Super::BeginPlay();
	
	DoorClosedPosition = DoorMesh->GetRelativeTransform();

	//Setup Door Timeline
	UpdateFunctionFloat.BindDynamic(this, &ADoor::UpdateTimelineComp);

	if (DoorTimelineFloatCurve)
	{
		DoorTimeline->AddInterpFloat(DoorTimelineFloatCurve, UpdateFunctionFloat);
	}
}

void ADoor::OpenDoor()
{
	DoorTimeline->Play();
	bIsDoorOpen = true;
}

void ADoor::CloseDoor()
{
	DoorTimeline->Reverse();
	bIsDoorOpen = false;
}

void ADoor::OnTrigger_Implementation(AActor* TriggeringActor)
{
	OpenDoor();
}

void ADoor::OnUntrigger_Implementation(AActor* TriggeringActor)
{
	CloseDoor();
}

bool ADoor::IsTriggered_Implementation()
{
	return bIsDoorOpen;
}

void ADoor::UpdateTimelineComp(float Output)
{
	DoorMesh->SetRelativeTransform(UKismetMathLibrary::TLerp(DoorClosedPosition, DoorOpenPosition, Output));
}

