#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Heart.generated.h"

class UNiagaraComponent;

UCLASS()
class MYPROJECT_API AHeart : public AActor
{
	GENERATED_BODY()
	
public:	
	AHeart();
	virtual void Tick(float DeltaTime);

protected:
	virtual void BeginPlay();

	virtual void SpawnPickupSystem();
	virtual void SpawnPickupSound();

	UFUNCTION(BlueprintCallable)
	void CollisionSphereOverlap(AActor* OtherActor);

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;

private:
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* PickupEffect;

	UPROPERTY(EditAnywhere)
	float DriftRate = -100.f;

	UPROPERTY(EditAnywhere, Category = "Hearts Properties")
	int32 Hearts;

	double DesiredZ;

public:
	FORCEINLINE int32 GetHearts() const { return Hearts; }
	FORCEINLINE void SetHearts(int32 NumberOfHearts) { Hearts = NumberOfHearts; }

};
