#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"

class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAttributeComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	// Current Health
	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health = 100.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Studs;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Hearts = 0;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 MaxHearts = 4;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float AddHealthByAmount = 25.f;

public:
	void ReceiveDamage(float Damage);
	void ResetHealth();
	bool IsAlive();
	float GetHealth();
	void AddHealth();
	void AddStuds(int32 AmountOfStuds);
	void AddHearts(int32 NumberOfHearts);
	void ReduceHearts(int32 NumberOfHearts);
	void EmptyOrFillAllHearts(int32 NumberOfHearts);

	FORCEINLINE int32 GetStuds() const { return Studs; }
	FORCEINLINE int32 GetHearts() const { return Hearts; }
};
