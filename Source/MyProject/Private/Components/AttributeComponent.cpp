#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0.f, MaxHealth);
}

void UAttributeComponent::ResetHealth()
{
	Health = MaxHealth;
}

bool UAttributeComponent::IsAlive()
{
	return Health > 0.f;
}

float UAttributeComponent::GetHealth()
{
	return Health;
}

void UAttributeComponent::AddHealth()
{
	Health = FMath::Clamp(Health + AddHealthByAmount, 0.f, MaxHealth);
}

void UAttributeComponent::AddStuds(int32 AmountOfStuds)
{
	Studs += AmountOfStuds;
}

void UAttributeComponent::AddHearts(int32 NumberOfHearts)
{
	Hearts += NumberOfHearts;
}

void UAttributeComponent::ReduceHearts(int32 NumberOfHearts)
{
	Hearts = FMath::Clamp(Hearts - NumberOfHearts, 0, MaxHearts);
}

void UAttributeComponent::EmptyOrFillAllHearts(int32 NumberOfHearts)
{
	if (NumberOfHearts == 0)
	{
		Hearts = 0;
	}
	if (NumberOfHearts == 4)
	{
		Hearts = MaxHearts;
	}
}

