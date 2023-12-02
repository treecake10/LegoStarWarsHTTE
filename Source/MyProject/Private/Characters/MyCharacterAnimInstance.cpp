#include "Characters/MyCharacterAnimInstance.h"
#include "Characters/MyProjectCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UMyCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	MyProjectCharacter = Cast<AMyProjectCharacter>(TryGetPawnOwner());

	if (MyProjectCharacter)
	{
		MyProjectCharacterMovement = MyProjectCharacter->GetCharacterMovement();
	}
}

void UMyCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (MyProjectCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(MyProjectCharacterMovement->Velocity);
		IsFalling = MyProjectCharacterMovement->IsFalling();
		CharacterState = MyProjectCharacter->GetCharacterState();
	}
}

