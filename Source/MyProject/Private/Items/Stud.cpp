#include "Items/Stud.h"
#include "Interfaces/PickupInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"


AStud::AStud()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AStud::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator NewRotation = GetActorRotation() + FRotator(0.0f, 180.0f * DeltaTime, 0.0f);
	SetActorRotation(NewRotation);
}

void AStud::SpawnPickupSound()
{
	if (PickupSound)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			PickupSound,
			GetActorLocation()
		);
	}
}

void AStud::CollisionSphereOverlap(AActor* OtherActor)
{
	IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor);

	if (!PickupInterface) { return; }

	PickupInterface->AddStuds(this, *GetName());

	SpawnPickupSound();
	Destroy();
}

int32 AStud::IncremenetFlashCount(int32 Count)
{
	FlashCounter += Count;
	return FlashCounter;
}

void AStud::ResetFlashCount()
{
	FlashCounter = 0;
}


