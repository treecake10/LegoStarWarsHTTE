#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class MYPROJECT_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

	/** Combat */
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void Attack();

	UFUNCTION(BlueprintCallable)
	virtual void Die();

	virtual void HandleFlashDamage(const FName& HitActorName, AActor* ActorOwner) override;
	virtual void HandleDamage(float DamageAmount);
	void PlayHitSound(const FVector& ImpactPoint);
	void DisableCapsule();
	virtual bool CanAttack();
	bool IsAlive();

	virtual void SetLegoFigureMeshEmissive
	(
		USkeletalMeshComponent* FigureMesh, 
		UMaterialInstanceDynamic* DynamicMaterialHead, 
		UMaterialInstanceDynamic* DynamicMaterialHeadPiece,
		UMaterialInstanceDynamic* DynamicMaterialLeftArm,
		UMaterialInstanceDynamic* DynamicMaterialRightArm,
		UMaterialInstanceDynamic* DynamicMaterialLeftHand,
		UMaterialInstanceDynamic* DynamicMaterialRightHand,
		UMaterialInstanceDynamic* DynamicMaterialLeftLeg,
		UMaterialInstanceDynamic* DynamicMaterialRightLeg,
		UMaterialInstanceDynamic* DynamicMaterialTorso,
		UMaterialInstanceDynamic* DynamicMaterialHips,
		FColor Color,
		float MultiplierValue,
		bool ClearTimerHandle,
		int32 HeadElement, 
		int32 HeadPieceElement, 
		int32 LeftArmElement, 
		int32 RightArmElement,
		int32 LeftHandElement,
		int32 RightHandElement,
		int32 LeftLegElement,
		int32 RightLegElement,
		int32 TorsoElement,
		int32 HipsElement
	);
 
	virtual int32 PlayAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	UFUNCTION(BlueprintImplementableEvent)
	void LegoFigureDeathEvent();

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled);

	UPROPERTY(VisibleAnywhere, Category = Weapon)
	AWeapon* EquippedWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(BlueprintReadOnly, Category = Combat)
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere, Category = Combat)
	double WarpTargetDistance = 75.f;

private:
	void PlayMontageSection(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomMontageSection(UAnimMontage* Montage, const TArray<FName>& SectionNames);

	UPROPERTY(EditAnywhere, Category = Combat)
	USoundBase* HitSound;

	/**
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = Combat)
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	TArray<FName> AttackMontageSections;

};
