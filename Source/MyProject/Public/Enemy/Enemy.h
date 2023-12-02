#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"
#include <Characters/CharacterTypes.h>

class UPawnSensingComponent;

UCLASS()
class MYPROJECT_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void HandleFlashDamage(const FName& HitActorName, AActor* ActorOwner) override;
	/** <IHitInterface> */

protected:

	/** <AActor> */
	virtual void BeginPlay() override;
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die() override;
	void SpawnHeart();
	virtual void Attack() override;
	virtual bool CanAttack() override;
	virtual void AttackEnd() override;
	virtual void HandleDamage(float DamageAmount) override;

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

	/** </ABaseCharacter> */

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:

	/** AI Behavior */
	void InitializeEnemy();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void PatrolTimerFinished();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); // Callback for OnPawnSeen in UPawnSensingComponent

	void CreateNoghriMeshDynMatInstances(UObject* InOuter);
	void FindAndSetNoghriMeshMatObjects();
	bool IsValidEnemy(const FString& NameOfActorHit);
	void SetHitEnemyMeshEmissiveToRed();
	void SetHitEnemyMeshEmissiveToBlack(FString& NameOfActorHitStr);

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClass;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class AHeart> HeartClass;

	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	UPROPERTY()
	class AAIController* EnemyController;

	// Current patrol target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 125.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 300.f;


	/** Flash Damage for Enemy Mesh */

	FTimerHandle EnemyTimerHandle;
	USkeletalMeshComponent* StaticMesh;

	UMaterialInstanceDynamic* DynMaterialHead;
	UMaterialInstanceDynamic* DynMaterialHeadPiece;
	UMaterialInstanceDynamic* DynMaterialLeftArm;
	UMaterialInstanceDynamic* DynMaterialRightArm;
	UMaterialInstanceDynamic* DynMaterialLeftHand;
	UMaterialInstanceDynamic* DynMaterialRightHand;
	UMaterialInstanceDynamic* DynMaterialLeftLeg;
	UMaterialInstanceDynamic* DynMaterialRightLeg;
	UMaterialInstanceDynamic* DynMaterialTorso;
	UMaterialInstanceDynamic* DynMaterialHips;

	UPROPERTY(EditAnywhere)
	UMaterialInterface* NoghriHeadFlashDamageMaterial;
	UMaterialInterface* NoghriHoodFlashDamageMaterial;
	UMaterialInterface* NoghriArmsAndHandsFlashDamageMaterial;
	UMaterialInterface* NoghriLegsFlashDamageMaterial;
	UMaterialInterface* NoghriTorsoFlashDamageMaterial;
	UMaterialInterface* NoghriHipsFlashDamageMaterial;

};
