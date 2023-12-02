#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterTypes.h"
#include "Interfaces/PickupInterface.h"
#include "MyProjectCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class AItem;
class AHeart;
class AStud;
class UAnimMontage;
class UMyProjectCharacterOverlay;

UCLASS()
class MYPROJECT_API AMyProjectCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:
	AMyProjectCharacter();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void BindActionsAndAxes(UInputComponent* PlayerInputComponent);
	void UnbindAllInputs();
	void RebindAllInputs();
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void GetHit(const FVector& ImpactPoint, AActor* Hitter) override;
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddHearts(AHeart* Heart) override;
	virtual void AddStuds(AStud* Stud, const FName& CurrentStudName) override;

	UPROPERTY(VisibleAnywhere)
    UStaticMeshComponent* LightsaberMesh;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* LightsaberGreenGlow;

	UPROPERTY(EditAnywhere)
	UMaterial* LightsaberInvisible;

	UPROPERTY(EditAnywhere)
	UMaterial* LightsaberChrome;

protected:

	virtual void BeginPlay() override;

	bool bSaberOn;
	bool bEmptyHealthFromHit;

	/** Callback for input */
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void ZKeyPressed();
	virtual void Attack() override;

	/** Combat */
	virtual void AttackEnd() override;
	virtual bool CanAttack() override;
	bool CanDisarm();
	bool CanArm();
	void Disarm();
	void Arm();
	void SetSaberMeshMatOn();
	void SetSaberMeshMatOff();
	void PlayEquipMontage(const FName& SectionName);
	virtual void Die() override;

	void CreatePlayerMeshDynMatInstances(UObject* InOuter);
	void FindAndSetPlayerMeshMatObjects();
	void FindAndSetPlayerSaberMeshMatObjects();

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

	UFUNCTION(BlueprintCallable)
	void SaberOff();

	UFUNCTION(BlueprintCallable)
	void SaberOn();

	UFUNCTION(BlueprintCallable)
	void SnapSaberHiltToHand();

	UFUNCTION(BlueprintCallable)
	void SnapSaberHiltToHips();

	UFUNCTION(BlueprintCallable)
	void FinishEquipping();

	UFUNCTION(BlueprintCallable)
	void EnablePlayer();

	UFUNCTION(BlueprintImplementableEvent)
	void HeartFillSequenceEvent();

	UFUNCTION(BlueprintImplementableEvent)
	void EnablePlayerEvent();

	FName PreviousStudName;

	UPROPERTY(BlueprintReadWrite)
	UMyProjectCharacterOverlay* MyProjectCharacterOverlay;

private:

	void InitializeMyProjectCharacterOverlay(bool ResetStudCount, bool SetHeartsTimer, bool InsideSetHUDHealth);
	void SetHUDHealth();
	void SpawnDefaultWeapon();

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class AWeapon> WeaponClass;

	/** Character components */

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* ViewCamera;
	
	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipMontage;

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EActionState ActionState = EActionState::EAS_Unoccupied;

	/** Flash Damage for Player Mesh */

	FTimerHandle PlayerFlashDamageTimerHandle;
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
	UMaterialInterface* PlayerHeadFlashDamageMaterial;
	UMaterialInterface* PlayerHairFlashDamageMaterial;
	UMaterialInterface* PlayerArmsLegsRightHandHipsFlashDamageMaterial;
	UMaterialInterface* PlayerLeftHandFlashDamageMaterial;
	UMaterialInterface* PlayerTorsoFlashDamageMaterial;

	/** Overlay */
	FTimerHandle ScaleHeartTimerHandle;

public:

	/** <IHitInterface> */
	virtual void HandleFlashDamage(const FName& HitActorName, AActor* ActorOwner) override;
	/** <IHitInterface> */

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
};
