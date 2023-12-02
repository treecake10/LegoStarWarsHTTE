#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "MyProjectCharacterOverlay.generated.h"

UCLASS()
class MYPROJECT_API UMyProjectCharacterOverlay : public UUserWidget
{
	GENERATED_BODY()
public:

	void SetStudCount(int32 Count);
	void ScaleOverlayHearts();
	void RenderHeartBeats();

	UFUNCTION(BlueprintCallable)
	void ShowHeartVisibilities(bool SH1, bool SH2, bool SH3, bool SH4, bool BH1, bool BH2, bool BH3, bool BH4);

	UFUNCTION(BlueprintCallable)
	void ShowStudIconVisibilities(bool SS, bool GS, bool BS, bool PS);

	UFUNCTION(BlueprintCallable)
	void ShowStudCountVisibility(bool ShowCount);

	bool AllHeartsValid();
	bool AllStudIconsValid();

private:

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* StudCountText;

	UPROPERTY(meta = (BindWidget))
	UImage* SilverStudImage;

	UPROPERTY(meta = (BindWidget))
	UImage* GoldStudImage;

	UPROPERTY(meta = (BindWidget))
	UImage* BlueStudImage;

	UPROPERTY(meta = (BindWidget))
	UImage* PurpleStudImage;

	UPROPERTY(meta = (BindWidget))
	UImage* StaticHeart1;

	UPROPERTY(meta = (BindWidget))
	UImage* StaticHeart2;

	UPROPERTY(meta = (BindWidget))
	UImage* StaticHeart3;

	UPROPERTY(meta = (BindWidget))
	UImage* StaticHeart4;

	UPROPERTY(meta = (BindWidget))
	UImage* BeatingHeart1;

	UPROPERTY(meta = (BindWidget))
	UImage* BeatingHeart2;

	UPROPERTY(meta = (BindWidget))
	UImage* BeatingHeart3;

	UPROPERTY(meta = (BindWidget))
	UImage* BeatingHeart4;

	UPROPERTY(EditAnywhere, Category = "MyProjectCharacter Overlay Settings")
	float MinScaleHearts = 1.0f;

	UPROPERTY(EditAnywhere, Category = "MyProjectCharacter Overlay Settings")
	float MaxScaleHearts = 1.2f;

	float CurrentScaleHearts = 1.0f;

	bool ScaleUpHearts = true;
	
};
