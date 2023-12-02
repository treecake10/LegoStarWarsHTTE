#include "HUD/MyProjectCharacterHUD.h"
#include "HUD/MyProjectCharacterOverlay.h"

void AMyProjectCharacterHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	if (World)
	{
		APlayerController* Controller = World->GetFirstPlayerController();
		if (Controller && MyProjectCharacterOverlayClass)
		{
			MyProjectCharacterOverlay = CreateWidget<UMyProjectCharacterOverlay>(Controller, MyProjectCharacterOverlayClass);
			MyProjectCharacterOverlay->AddToViewport();
		}
	}
}
