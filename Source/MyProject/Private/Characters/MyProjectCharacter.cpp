#include "Characters/MyProjectCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "HUD/MyProjectCharacterHUD.h"
#include "HUD/MyProjectCharacterOverlay.h"
#include "Components/CapsuleComponent.h"

/** Flash Damage */
#include "UObject/ConstructorHelpers.h"

AMyProjectCharacter::AMyProjectCharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	bEmptyHealthFromHit = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom);

	/** Lego Figure Player Mesh Materials */
	FindAndSetPlayerMeshMatObjects();

	/** Lightsaber Materials */
	FindAndSetPlayerSaberMeshMatObjects();

}

void AMyProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	BindActionsAndAxes(PlayerInputComponent);
}

void AMyProjectCharacter::BindActionsAndAxes(UInputComponent* PlayerInputComponent)
{
	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMyProjectCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AMyProjectCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AMyProjectCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AMyProjectCharacter::LookUp);

	PlayerInputComponent->BindAction(FName("Jump"), IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction(FName("ArmDisarm"), IE_Pressed, this, &AMyProjectCharacter::ZKeyPressed);
	PlayerInputComponent->BindAction(FName("Attack"), IE_Pressed, this, &AMyProjectCharacter::Attack);
}

void AMyProjectCharacter::UnbindAllInputs()
{
	UInputComponent* PlayerInputComponent = FindComponentByClass<UInputComponent>();
	if (PlayerInputComponent)
	{
		PlayerInputComponent->RemoveAxisBinding(FName("MoveForward"));
		PlayerInputComponent->RemoveAxisBinding(FName("MoveRight"));
		PlayerInputComponent->RemoveAxisBinding(FName("Turn"));
		PlayerInputComponent->RemoveAxisBinding(FName("LookUp"));

		PlayerInputComponent->RemoveActionBinding(FName("Jump"), IE_Pressed);
		PlayerInputComponent->RemoveActionBinding(FName("ArmDisarm"), IE_Pressed);
		PlayerInputComponent->RemoveActionBinding(FName("Attack"), IE_Pressed);
	}
}

void AMyProjectCharacter::RebindAllInputs()
{
	UInputComponent* PlayerInputComponent = FindComponentByClass<UInputComponent>();
	if (PlayerInputComponent)
	{
		BindActionsAndAxes(PlayerInputComponent);
	}
}

float AMyProjectCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	HandleDamage(DamageAmount);

	if (Attributes)
	{
		Attributes->ReduceHearts(1);
	}

	SetHUDHealth();
	return DamageAmount;
}

void AMyProjectCharacter::GetHit(const FVector& ImpactPoint, AActor* Hitter)
{
	Super::GetHit(ImpactPoint, Hitter);

	SetWeaponCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AMyProjectCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AMyProjectCharacter::AddHearts(AHeart* Heart)
{
	if (Attributes && MyProjectCharacterOverlay)
	{
		if (Attributes->GetHearts() < 4)
		{
			Attributes->AddHearts(1);
		}
		
		Attributes->AddHealth();

		if (Attributes->GetHearts() == 2) 
		{
			MyProjectCharacterOverlay->ShowHeartVisibilities(true, true, false, false, false, true, false, false);
		}
		else if (Attributes->GetHearts() == 3) 
		{
			MyProjectCharacterOverlay->ShowHeartVisibilities(true, true, false, false, false, false, true, false);
		}
		else if (Attributes->GetHearts() == 4)
		{
			MyProjectCharacterOverlay->ShowHeartVisibilities(true, true, true, false, false, false, false, true);
		}
	}
}

void AMyProjectCharacter::AddStuds(AStud* Stud, const FName& CurrentStudName)
{
	if (Attributes && MyProjectCharacterOverlay)
	{
		if (!PreviousStudName.IsNone() && CurrentStudName == PreviousStudName) { return; }
		
		if (PreviousStudName.IsNone()) {
			MyProjectCharacterOverlay->ShowStudCountVisibility(true);
		}

		FString StudNameStr = CurrentStudName.ToString();

		if (StudNameStr.StartsWith("BP_Silver"))
		{
			Attributes->AddStuds(10);
			MyProjectCharacterOverlay->ShowStudIconVisibilities(true, false, false, false);
		}
		else if (StudNameStr.StartsWith("BP_Gold"))
		{
			Attributes->AddStuds(100);
			MyProjectCharacterOverlay->ShowStudIconVisibilities(false, true, false, false);
		}
		else if (StudNameStr.StartsWith("BP_Blue"))
		{
			Attributes->AddStuds(1000);
			MyProjectCharacterOverlay->ShowStudIconVisibilities(false, false, true, false);
		}
		else if (StudNameStr.StartsWith("BP_Purple"))
		{
			Attributes->AddStuds(10000);
			MyProjectCharacterOverlay->ShowStudIconVisibilities(false, false, false, true);
		}

		MyProjectCharacterOverlay->SetStudCount(Attributes->GetStuds());
		PreviousStudName = CurrentStudName;
	}
}

void AMyProjectCharacter::BeginPlay()
{
	Super::BeginPlay();

	bSaberOn = false;
	Tags.Add(FName("EngageableTarget"));

	if (Attributes)
	{
		Attributes->EmptyOrFillAllHearts(4);
	}

	InitializeMyProjectCharacterOverlay(true, true, false);
	SpawnDefaultWeapon();
}

void AMyProjectCharacter::MoveForward(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
	if (Controller && (Value != 0.f)) {

		// determine which way is forward
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyProjectCharacter::MoveRight(float Value)
{
	if (ActionState != EActionState::EAS_Unoccupied) return;
	
	if (Controller && (Value != 0.f)) {

		// determine which way is right
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMyProjectCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AMyProjectCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMyProjectCharacter::ZKeyPressed()
{

	if (CanDisarm())
	{
		Disarm();
	}
	else if (CanArm())
	{
		Arm();
	}
}

void AMyProjectCharacter::Attack()
{

	Super::Attack();
	
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}

}

void AMyProjectCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

bool AMyProjectCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
	
}

bool AMyProjectCharacter::CanDisarm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool AMyProjectCharacter::CanArm()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped;
}

void AMyProjectCharacter::Disarm()
{
	PlayEquipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AMyProjectCharacter::Arm()
{
	PlayEquipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AMyProjectCharacter::SetSaberMeshMatOn()
{
	EquippedWeapon->ChangeItemMeshMaterial(1, LightsaberChrome);
	EquippedWeapon->ChangeItemMeshMaterial(3, LightsaberChrome);
	EquippedWeapon->ChangeItemMeshInstanceMaterial(0, LightsaberGreenGlow);
}

void AMyProjectCharacter::SetSaberMeshMatOff()
{
	EquippedWeapon->ChangeItemMeshMaterial(0, LightsaberInvisible);
	EquippedWeapon->ChangeItemMeshMaterial(1, LightsaberInvisible);
	EquippedWeapon->ChangeItemMeshMaterial(3, LightsaberInvisible);
}

void AMyProjectCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}

void AMyProjectCharacter::Die()
{
	Super::Die();

	ActionState = EActionState::EAS_Dead;
	
	UnbindAllInputs();
	
	if (bSaberOn)
	{
		SetSaberMeshMatOff();
	}
	
	DisableCapsule();
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetVisibility(false);

	if (Attributes)
	{
		Attributes->EmptyOrFillAllHearts(0);
	}

	if (MyProjectCharacterOverlay && !bEmptyHealthFromHit)
	{
		MyProjectCharacterOverlay->ShowHeartVisibilities(false, false, false, false, false, false, false, false);
	}

	bEmptyHealthFromHit = false;

	EnablePlayerEvent();
	HeartFillSequenceEvent();
}

void AMyProjectCharacter::CreatePlayerMeshDynMatInstances(UObject* InOuter)
{
	DynMaterialHead = UMaterialInstanceDynamic::Create(PlayerHeadFlashDamageMaterial, InOuter);
	DynMaterialHeadPiece = UMaterialInstanceDynamic::Create(PlayerHairFlashDamageMaterial, InOuter);
	DynMaterialLeftArm = UMaterialInstanceDynamic::Create(PlayerArmsLegsRightHandHipsFlashDamageMaterial, InOuter);
	DynMaterialRightArm = UMaterialInstanceDynamic::Create(PlayerArmsLegsRightHandHipsFlashDamageMaterial, InOuter);
	DynMaterialLeftLeg = UMaterialInstanceDynamic::Create(PlayerArmsLegsRightHandHipsFlashDamageMaterial, InOuter);
	DynMaterialRightLeg = UMaterialInstanceDynamic::Create(PlayerArmsLegsRightHandHipsFlashDamageMaterial, InOuter);
	DynMaterialRightHand = UMaterialInstanceDynamic::Create(PlayerArmsLegsRightHandHipsFlashDamageMaterial, InOuter);
	DynMaterialHips = UMaterialInstanceDynamic::Create(PlayerArmsLegsRightHandHipsFlashDamageMaterial, InOuter);
	DynMaterialLeftHand = UMaterialInstanceDynamic::Create(PlayerLeftHandFlashDamageMaterial, InOuter);
	DynMaterialTorso = UMaterialInstanceDynamic::Create(PlayerTorsoFlashDamageMaterial, InOuter);
}

void AMyProjectCharacter::FindAndSetPlayerMeshMatObjects()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PlayerHeadMatInstanceAsset(TEXT("/Game/Player/Materials/Luke_Minifigure/283_ncl1_1_Inst"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PlayerHairMatInstanceAsset(TEXT("/Game/Player/Materials/Luke_Minifigure/138_2_Inst"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PlayerArmsLegsRightHandHipsMatInstanceAsset(TEXT("/Game/Player/Materials/Luke_Minifigure/26_2_Inst"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PlayerLeftHandMatInstanceAsset(TEXT("/Game/Player/Materials/Luke_Minifigure/283_2_Inst"));
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PlayerTorsoMatInstanceAsset(TEXT("/Game/Player/Materials/Luke_Minifigure/26_ncl1_3_Inst"));

	if (PlayerHeadMatInstanceAsset.Succeeded())
	{
		PlayerHeadFlashDamageMaterial = PlayerHeadMatInstanceAsset.Object;
	}

	if (PlayerHairMatInstanceAsset.Succeeded())
	{
		PlayerHairFlashDamageMaterial = PlayerHairMatInstanceAsset.Object;
	}

	if (PlayerArmsLegsRightHandHipsMatInstanceAsset.Succeeded())
	{
		PlayerArmsLegsRightHandHipsFlashDamageMaterial = PlayerArmsLegsRightHandHipsMatInstanceAsset.Object;
	}

	if (PlayerLeftHandMatInstanceAsset.Succeeded())
	{
		PlayerLeftHandFlashDamageMaterial = PlayerLeftHandMatInstanceAsset.Object;
	}

	if (PlayerTorsoMatInstanceAsset.Succeeded())
	{
		PlayerTorsoFlashDamageMaterial = PlayerTorsoMatInstanceAsset.Object;
	}
}

void AMyProjectCharacter::FindAndSetPlayerSaberMeshMatObjects()
{
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> SaberGreenGlowAsset(TEXT("/Game/Player/Materials/Green_Lightsaber/greenglow"));
	static ConstructorHelpers::FObjectFinder<UMaterial> SaberInvisibleAsset(TEXT("/Game/Player/Materials/Green_Lightsaber/invisible"));
	static ConstructorHelpers::FObjectFinder<UMaterial> SaberHiltAsset(TEXT("/Game/Player/Materials/Green_Lightsaber/metalchrome"));

	if (SaberGreenGlowAsset.Succeeded())
	{
		LightsaberGreenGlow = SaberGreenGlowAsset.Object;
	}

	if (SaberInvisibleAsset.Succeeded())
	{
		LightsaberInvisible = SaberInvisibleAsset.Object;
	}

	if (SaberHiltAsset.Succeeded())
	{
		LightsaberChrome = SaberHiltAsset.Object;
	}
}

void AMyProjectCharacter::SetLegoFigureMeshEmissive
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

	GetWorldTimerManager().ClearTimer(PlayerFlashDamageTimerHandle);
}

void AMyProjectCharacter::SaberOff()
{
	bSaberOn = false;
	SetSaberMeshMatOff();
}

void AMyProjectCharacter::SaberOn()
{
	bSaberOn = true;
	SetSaberMeshMatOn();
}

void AMyProjectCharacter::SnapSaberHiltToHand()
{
	EquippedWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
}

void AMyProjectCharacter::SnapSaberHiltToHips()
{
	EquippedWeapon->Equip(GetMesh(), FName("HipsSocket"), this, this);
}

void AMyProjectCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMyProjectCharacter::InitializeMyProjectCharacterOverlay(bool ResetStudCount, bool SetHeartsTimer, bool InsideSetHUDHealth)
{
	APlayerController* PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		AMyProjectCharacterHUD* MyProjectCharacterHUD = Cast<AMyProjectCharacterHUD>(PlayerController->GetHUD());
		if (MyProjectCharacterHUD)
		{
			MyProjectCharacterOverlay = MyProjectCharacterHUD->GetMyProjectCharacterOverlay();

			if (MyProjectCharacterOverlay)
			{

				if (!InsideSetHUDHealth)
				{
					MyProjectCharacterOverlay->ShowHeartVisibilities(true, true, true, false, false, false, false, true);
				}

				if (ResetStudCount) 
				{
					MyProjectCharacterOverlay->SetStudCount(0);
					MyProjectCharacterOverlay->ShowStudIconVisibilities(false, false, false, false);
					MyProjectCharacterOverlay->ShowStudCountVisibility(false);
				}

				if (SetHeartsTimer)
				{
					FTimerManager& TimerManagerSclaeSingleHeartImg = GetWorldTimerManager();
					TimerManagerSclaeSingleHeartImg.SetTimer(ScaleHeartTimerHandle, [this]()
						{
							MyProjectCharacterOverlay->ScaleOverlayHearts();

						}, 1.f, true);
				}
			}
		}
	}
}

void AMyProjectCharacter::SetHUDHealth()
{
	if (!MyProjectCharacterOverlay)
	{
		InitializeMyProjectCharacterOverlay(false, false, true);
	}
	
	if (MyProjectCharacterOverlay && Attributes)
	{
		if (Attributes->GetHealth() == 75.f)
		{
			MyProjectCharacterOverlay->ShowHeartVisibilities(true, true, false, false, false, false, true, false);
		}
		else if (Attributes->GetHealth() == 50.f)
		{
			MyProjectCharacterOverlay->ShowHeartVisibilities(true, false, false, false, false, true, false, false);
		}
		else if (Attributes->GetHealth() == 25.f)
		{
			MyProjectCharacterOverlay->ShowHeartVisibilities(false, false, false, false, true, false, false, false);
		}
		else if (Attributes->GetHealth() <= .0f)
		{
			bEmptyHealthFromHit = true;
			MyProjectCharacterOverlay->ShowHeartVisibilities(false, false, false, false, false, false, false, false);
		}
	}
}

void AMyProjectCharacter::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* DefaultWeapon = World->SpawnActor<AWeapon>(WeaponClass);
		DefaultWeapon->Equip(GetMesh(), FName("HipsSocket"), this, this);
		EquippedWeapon = DefaultWeapon;
	}
}

void AMyProjectCharacter::EnablePlayer()
{
	Tags.Remove(FName("Dead"));
	ActionState = EActionState::EAS_Unoccupied;

	RebindAllInputs();

	if (bSaberOn)
	{
		SetSaberMeshMatOn();
	}

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetVisibility(true);
	
	if (MyProjectCharacterOverlay && Attributes)
	{
		Attributes->ResetHealth();
		Attributes->EmptyOrFillAllHearts(4);
	}
}

void AMyProjectCharacter::HandleFlashDamage(const FName& HitActorName, AActor* ActorOwner)
{
	StaticMesh = Cast<USkeletalMeshComponent>(ActorOwner->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	FString NameOfActor = HitActorName.ToString();

	if (NameOfActor.StartsWith("BP_MyProjectCharacter_C") && StaticMesh) {

		CreatePlayerMeshDynMatInstances(this);

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
			9, 5, 3, 7, 4, 8, 2, 0, 6, 1
		);

		FTimerManager& TimerManagerPlayer = GetWorldTimerManager();

		TimerManagerPlayer.SetTimer(PlayerFlashDamageTimerHandle, [this]()
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
				9, 5, 3, 7, 4, 8, 2, 0, 6, 1
			);
		}, 0.2f, false);
	}
}

