#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{

	PlayHitSound(ImpactPoint);

	if (!IsAlive() && Hitter)
	{
		Die();
	}

}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::Die()
{
	Tags.Add(FName("Dead"));

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::HandleFlashDamage(const FName& HitActorName, AActor* ActorOwner)
{
}

void ABaseCharacter::HandleDamage(float DamageAmount)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(DamageAmount);
	}
}

void ABaseCharacter::PlayHitSound(const FVector& ImpactPoint)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
		);
	}
}

void ABaseCharacter::DisableCapsule()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::SetLegoFigureMeshEmissive
(
	USkeletalMeshComponent* FigureMesh, UMaterialInstanceDynamic* DynamicMaterialHead, UMaterialInstanceDynamic* DynamicMaterialHeadPiece,
	UMaterialInstanceDynamic* DynamicMaterialLeftArm, UMaterialInstanceDynamic* DynamicMaterialRightArm, UMaterialInstanceDynamic* DynamicMaterialLeftHand,
	UMaterialInstanceDynamic* DynamicMaterialRightHand, UMaterialInstanceDynamic* DynamicMaterialLeftLeg, UMaterialInstanceDynamic* DynamicMaterialRightLeg,
	UMaterialInstanceDynamic* DynamicMaterialTorso, UMaterialInstanceDynamic* DynamicMaterialHips, FColor Color, float MultiplierValue, bool ClearTimerHandle,
	int32 HeadElement, int32 HeadPieceElement, int32 LeftArmElement, int32 RightArmElement, int32 LeftHandElement, int32 RightHandElement, int32 LeftLegElement,
	int32 RightLegElement, int32 TorsoElement, int32 HipsElement
)
{
	DynamicMaterialHead->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialHead->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(HeadElement, DynamicMaterialHead);

	DynamicMaterialHeadPiece->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialHeadPiece->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(HeadPieceElement, DynamicMaterialHeadPiece);

	DynamicMaterialLeftArm->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialLeftArm->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(LeftArmElement, DynamicMaterialLeftArm);

	DynamicMaterialRightArm->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialRightArm->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(RightArmElement, DynamicMaterialRightArm);

	DynamicMaterialLeftHand->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialLeftHand->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(LeftHandElement, DynamicMaterialLeftHand);

	DynamicMaterialRightHand->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialRightHand->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(RightHandElement, DynamicMaterialRightHand);

	DynamicMaterialLeftLeg->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialLeftLeg->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(LeftLegElement, DynamicMaterialLeftLeg);

	DynamicMaterialRightLeg->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialRightLeg->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(RightLegElement, DynamicMaterialRightLeg);

	DynamicMaterialTorso->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialTorso->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(TorsoElement, DynamicMaterialTorso);

	DynamicMaterialHips->SetVectorParameterValue("EmissiveColor", Color);
	DynamicMaterialHips->SetScalarParameterValue("EmissiveMultiplier", MultiplierValue);
	FigureMesh->SetMaterial(HipsElement, DynamicMaterialHips);
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomMontageSection(AttackMontage, AttackMontageSections);
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::AttackEnd()
{
}

void ABaseCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{

	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();
	}

}

void ABaseCharacter::PlayMontageSection(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames)
{
	if (SectionNames.Num() <= 0) return -1;
	const int32 MaxSectionIndex = SectionNames.Num() - 1;
	const int32 Selection = FMath::RandRange(0, MaxSectionIndex);
	PlayMontageSection(Montage, SectionNames[Selection]);
	return Selection;
}



