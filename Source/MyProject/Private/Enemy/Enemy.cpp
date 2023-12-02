#include "Enemy/Enemy.h"
#include "AIController.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Heart.h"
#include "Containers/UnrealString.h"

/** Flash Damage */
#include "UObject/ConstructorHelpers.h"
#include "Components/StaticMeshComponent.h"

AEnemy::AEnemy()
{
 	
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	/** Flash Damage */
	FindAndSetNoghriMeshMatObjects();

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}

}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);
	CombatTarget = EventInstigator->GetPawn();
		
	if (IsInsideAttackRadius())
	{
		EnemyState = EEnemyState::EES_Attacking;
	}
	else if (IsOutsideAttackRadius())
	{
		ChaseTarget();
	}
	
	return DamageAmount;
}

void AEnemy::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit(ImpactPoint, Hitter);

	ClearPatrolTimer();
	ClearAttackTimer();
	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);

	if (IsInsideAttackRadius())
	{
		if (!IsDead()) StartAttackTimer();
	}
}

void AEnemy::HandleFlashDamage(const FName& HitActorName, AActor* ActorOwner)
{

	StaticMesh = Cast<USkeletalMeshComponent>(ActorOwner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	FString NameOfActorHitStr = HitActorName.ToString();

	if (IsValidEnemy(NameOfActorHitStr) && StaticMesh) {

		CreateNoghriMeshDynMatInstances(this);
		SetHitEnemyMeshEmissiveToRed();
		SetHitEnemyMeshEmissiveToBlack(NameOfActorHitStr);

	}
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

void AEnemy::Die()
{
	Super::Die();

	if (EquippedWeapon)
	{
		EquippedWeapon->Destroy();
	}

	EnemyState = EEnemyState::EES_Dead;
	ClearAttackTimer();
	DisableCapsule();
	GetCharacterMovement()->bOrientRotationToMovement = false;
	LegoFigureDeathEvent();
	SpawnHeart();
}

void AEnemy::SpawnHeart()
{
	UWorld* World = GetWorld();
	if (World && HeartClass && Attributes)
	{
		const FVector SpawnLocation = GetActorLocation() + FVector(0.f, 0.f, 125.f);
		AHeart* SpawnedHeart = World->SpawnActor<AHeart>(HeartClass, SpawnLocation, GetActorRotation());
		if (SpawnedHeart)
		{
			SpawnedHeart->SetHearts(Attributes->GetHearts());
			SpawnedHeart->SetOwner(this);
		}
	}
}

void AEnemy::Attack()
{
	Super::Attack();

	if (CombatTarget == nullptr)
	{
		return;
	}

	EnemyState = EEnemyState::EES_Engaged;
	PlayAttackMontage();
}

bool AEnemy::CanAttack()
{
	bool bCanAttack =
		IsInsideAttackRadius() &&
		!IsAttacking() &&
		!IsEngaged() &&
		!IsDead();

	return bCanAttack;
}

void AEnemy::AttackEnd()
{
	EnemyState = EEnemyState::EES_NoState;
	CheckCombatTarget();
}

void AEnemy::HandleDamage(float DamageAmount)
{
	Super::HandleDamage(DamageAmount);
}

void AEnemy::SetLegoFigureMeshEmissive
(
	USkeletalMeshComponent* FigureMesh, UMaterialInstanceDynamic* DynamicMaterialHead, UMaterialInstanceDynamic* DynamicMaterialHeadPiece,
	UMaterialInstanceDynamic* DynamicMaterialLeftArm, UMaterialInstanceDynamic* DynamicMaterialRightArm, UMaterialInstanceDynamic* DynamicMaterialLeftHand,
	UMaterialInstanceDynamic* DynamicMaterialRightHand, UMaterialInstanceDynamic* DynamicMaterialLeftLeg, UMaterialInstanceDynamic* DynamicMaterialRightLeg,
	UMaterialInstanceDynamic* DynamicMaterialTorso, UMaterialInstanceDynamic* DynamicMaterialHips, FColor Color, float MultiplierValue, bool ClearTimerHandle,
	int32 HeadElement, int32 HeadPieceElement, int32 LeftArmElement, int32 RightArmElement, int32 LeftHandElement, int32 RightHandElement, int32 LeftLegElement,
	int32 RightLegElement, int32 TorsoElement, int32 HipsElement
)
{
	Super::SetLegoFigureMeshEmissive
	(
		FigureMesh,
		DynamicMaterialHead,
		DynamicMaterialHeadPiece,
		DynamicMaterialLeftArm,
		DynamicMaterialRightArm,
		DynamicMaterialLeftHand,
		DynamicMaterialRightHand,
		DynamicMaterialLeftLeg,
		DynamicMaterialRightLeg,
		DynamicMaterialTorso,
		DynamicMaterialHips,
		Color,
		MultiplierValue,
		ClearTimerHandle,
		HeadElement,
		HeadPieceElement,
		LeftArmElement,
		RightArmElement,
		LeftHandElement,
		RightHandElement,
		LeftLegElement,
		RightLegElement,
		TorsoElement,
		HipsElement
	);

	if (!ClearTimerHandle) return;

	GetWorldTimerManager().ClearTimer(EnemyTimerHandle);
}

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	SpawnDefaultWeapon();
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();
		const float WaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);
		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, WaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius())
	{
		ClearAttackTimer();

		// Outside combat radius, lose interest
		CombatTarget = nullptr;

		if (!IsEngaged())
		{
			StartPatrolling();
		}

	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();

		if (!IsEngaged())
		{
			ChaseTarget();
		}
	}
	else if (CanAttack())
	{
		StartAttackTimer();
	}

}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;
	GetCharacterMovement()->MaxWalkSpeed = PatrollingSpeed;
	MoveToTarget(PatrolTarget);
}

void AEnemy::ChaseTarget()
{
	EnemyState = EEnemyState::EES_Chasing;
	GetCharacterMovement()->MaxWalkSpeed = ChasingSpeed;
	MoveToTarget(CombatTarget);
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::StartAttackTimer()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float AttackTime = FMath::RandRange(AttackMin, AttackMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, AttackTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const double DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	
	return DistanceToTarget <= Radius;
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController == nullptr || Target == nullptr) return;

	FAIMoveRequest MoveRequest;
	MoveRequest.SetGoalActor(Target);
	MoveRequest.SetAcceptanceRadius(50.f);
	EnemyController->MoveTo(MoveRequest);
	
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;
	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget)
		{
			ValidTargets.AddUnique(Target);
		}
	}

	const int32 NumPatrolTargets = ValidTargets.Num();

	if (NumPatrolTargets > 0)
	{
		const int32 TargetSelection = FMath::RandRange(0, NumPatrolTargets - 1);
		return ValidTargets[TargetSelection];
	}

	return nullptr;
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	const bool bShouldChaseTarget =
		EnemyState != EEnemyState::EES_Dead &&
		EnemyState != EEnemyState::EES_Chasing &&
		EnemyState < EEnemyState::EES_Attacking &&
		SeenPawn->ActorHasTag(FName("EngageableTarget")) &&
		!SeenPawn->ActorHasTag(FName("Dead"));

	if (bShouldChaseTarget)
	{
		CombatTarget = SeenPawn;
		ClearPatrolTimer();
		ChaseTarget();
	}
}

void AEnemy::CreateNoghriMeshDynMatInstances(UObject* InOuter)
{
	DynMaterialHead = UMaterialInstanceDynamic::Create(NoghriHeadFlashDamageMaterial, InOuter);
	DynMaterialHeadPiece = UMaterialInstanceDynamic::Create(NoghriHoodFlashDamageMaterial, InOuter);
	DynMaterialLeftArm = UMaterialInstanceDynamic::Create(NoghriArmsAndHandsFlashDamageMaterial, InOuter);
	DynMaterialRightArm = UMaterialInstanceDynamic::Create(NoghriArmsAndHandsFlashDamageMaterial, InOuter);
	DynMaterialLeftHand = UMaterialInstanceDynamic::Create(NoghriArmsAndHandsFlashDamageMaterial, InOuter);
	DynMaterialRightHand = UMaterialInstanceDynamic::Create(NoghriArmsAndHandsFlashDamageMaterial, InOuter);
	DynMaterialLeftLeg = UMaterialInstanceDynamic::Create(NoghriLegsFlashDamageMaterial, InOuter);
	DynMaterialRightLeg = UMaterialInstanceDynamic::Create(NoghriLegsFlashDamageMaterial, InOuter);
	DynMaterialTorso = UMaterialInstanceDynamic::Create(NoghriTorsoFlashDamageMaterial, InOuter);
	DynMaterialHips = UMaterialInstanceDynamic::Create(NoghriHipsFlashDamageMaterial, InOuter);
}

void AEnemy::FindAndSetNoghriMeshMatObjects()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> NoghriHeadMatInstanceAsset(TEXT("/Game/Mixamo/Noghri/Materials/135_ncl1_3_Mat_Instance_Flash_Damage"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> NoghriHoodMatInstanceAsset(TEXT("/Game/Mixamo/Noghri/Materials/192_ncl1_2_Mat_Instance_Flash_Damage"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> NoghriArmsAndHandsMatInstanceAsset(TEXT("/Game/Mixamo/Noghri/Materials/135_001_Mat_Instance_Flash_Damage"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> NoghriLegsMatInstanceAsset(TEXT("/Game/Mixamo/Noghri/Materials/199_2_Mat_Instance_Flash_Damage"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> NoghriTorsoMatInstanceAsset(TEXT("/Game/Mixamo/Noghri/Materials/192_ncl1_1_Mat_Instance_Flash_Damage"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> NoghriHipsMatInstanceAsset(TEXT("/Game/Mixamo/Noghri/Materials/192_2_Mat_Instance_Flash_Damage"));

	if (NoghriHeadMatInstanceAsset.Succeeded())
	{
		NoghriHeadFlashDamageMaterial = NoghriHeadMatInstanceAsset.Object;
	}

	if (NoghriHoodMatInstanceAsset.Succeeded())
	{
		NoghriHoodFlashDamageMaterial = NoghriHoodMatInstanceAsset.Object;
	}

	if (NoghriArmsAndHandsMatInstanceAsset.Succeeded())
	{
		NoghriArmsAndHandsFlashDamageMaterial = NoghriArmsAndHandsMatInstanceAsset.Object;
	}

	if (NoghriLegsMatInstanceAsset.Succeeded())
	{
		NoghriLegsFlashDamageMaterial = NoghriLegsMatInstanceAsset.Object;
	}

	if (NoghriTorsoMatInstanceAsset.Succeeded())
	{
		NoghriTorsoFlashDamageMaterial = NoghriTorsoMatInstanceAsset.Object;
	}

	if (NoghriHipsMatInstanceAsset.Succeeded())
	{
		NoghriHipsFlashDamageMaterial = NoghriHipsMatInstanceAsset.Object;
	}
}

bool AEnemy::IsValidEnemy(const FString& NameOfActorHit)
{
	bool EnemyIsValid =
		NameOfActorHit.StartsWith("BP_Noghri_Assassin_Staff_C") ||
		NameOfActorHit.StartsWith("BP_Noghri_Assassin_Dagger_C");

	return EnemyIsValid;
}

void AEnemy::SetHitEnemyMeshEmissiveToRed()
{
	SetLegoFigureMeshEmissive
	(
		StaticMesh,
		DynMaterialHead,
		DynMaterialHeadPiece,
		DynMaterialLeftArm,
		DynMaterialRightArm,
		DynMaterialLeftHand,
		DynMaterialRightHand,
		DynMaterialLeftLeg,
		DynMaterialRightLeg,
		DynMaterialTorso,
		DynMaterialHips,
		FColor::Red,
		0.5f,
		false,
		9, 3, 2, 6, 7, 8, 1, 4, 5, 0
	);
}

void AEnemy::SetHitEnemyMeshEmissiveToBlack(FString& NameOfActorHitStr)
{
	if (NameOfActorHitStr.StartsWith("BP_Noghri_Assassin_Staff_C")) {

		FTimerManager& TimerManagerNoghriStaffAssassin = GetWorldTimerManager();

		TimerManagerNoghriStaffAssassin.SetTimer(EnemyTimerHandle, [this]()
		{
			SetLegoFigureMeshEmissive
			(
				StaticMesh,
				DynMaterialHead,
				DynMaterialHeadPiece,
				DynMaterialLeftArm,
				DynMaterialRightArm,
				DynMaterialLeftHand,
				DynMaterialRightHand,
				DynMaterialLeftLeg,
				DynMaterialRightLeg,
				DynMaterialTorso,
				DynMaterialHips,
				FColor::Black,
				0.0f,
				true,
				9, 3, 2, 6, 7, 8, 1, 4, 5, 0
			);
		}, 0.2f, false);

	}
	else if (NameOfActorHitStr.StartsWith("BP_Noghri_Assassin_Dagger_C")) {

		FTimerManager& TimerManagerNoghriDaggerAssassin = GetWorldTimerManager();

		TimerManagerNoghriDaggerAssassin.SetTimer(EnemyTimerHandle, [this]()
		{
			SetLegoFigureMeshEmissive
			(
				StaticMesh,
				DynMaterialHead,
				DynMaterialHeadPiece,
				DynMaterialLeftArm,
				DynMaterialRightArm,
				DynMaterialLeftHand,
				DynMaterialRightHand,
				DynMaterialLeftLeg,
				DynMaterialRightLeg,
				DynMaterialTorso,
				DynMaterialHips,
				FColor::Black,
				0.0f,
				true,
				9, 3, 2, 6, 7, 8, 0, 4, 1, 5
			);
		}, 0.2f, false);
	}
}





