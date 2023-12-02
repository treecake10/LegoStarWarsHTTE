#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Stud.generated.h"

class UNiagaraComponent;

UCLASS()
class MYPROJECT_API AStud : public AActor
{
	GENERATED_BODY()
	
public:	
	AStud();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void SpawnPickupSound();

	UFUNCTION(BlueprintCallable)
	void CollisionSphereOverlap(AActor* OtherActor);

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;

	UFUNCTION(BlueprintCallable)
	int32 IncremenetFlashCount(int32 Count);

	UFUNCTION(BlueprintCallable)
	void ResetFlashCount();

private:
	int32 FlashCounter = 0;

	UPROPERTY(EditAnywhere, Category = "Studs Properties")
	int32 Studs;

public:
	FORCEINLINE int32 GetStuds() const { return Studs; }
};
