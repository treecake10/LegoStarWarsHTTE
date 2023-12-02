#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MyProjectCharacterHUD.generated.h"

class UMyProjectCharacterOverlay;

UCLASS()
class MYPROJECT_API AMyProjectCharacterHUD : public AHUD
{
	GENERATED_BODY() 

protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, Category = MyProjectCharacter)
	TSubclassOf<UMyProjectCharacterOverlay> MyProjectCharacterOverlayClass;

	UPROPERTY()
	UMyProjectCharacterOverlay* MyProjectCharacterOverlay;

public:
	FORCEINLINE UMyProjectCharacterOverlay* GetMyProjectCharacterOverlay() const { return MyProjectCharacterOverlay; }
	
};
