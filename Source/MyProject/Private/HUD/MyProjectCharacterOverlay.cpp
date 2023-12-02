#include "HUD/MyProjectCharacterOverlay.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UMyProjectCharacterOverlay::SetStudCount(int32 Count)
{
	if (StudCountText)
	{
		const FString String = FString::Printf(TEXT("%d"), Count);
		const FText Text = FText::FromString(String);
		StudCountText->SetText(Text);
	}
}

void UMyProjectCharacterOverlay::ScaleOverlayHearts()
{
	if (AllHeartsValid())
	{
        if (ScaleUpHearts)
        {
            CurrentScaleHearts += 0.2f;
            if (CurrentScaleHearts >= MaxScaleHearts)
            {
                ScaleUpHearts = false;
            }
        }
        else
        {
            CurrentScaleHearts -= 0.2f;
            if (CurrentScaleHearts <= MinScaleHearts)
            {
                ScaleUpHearts = true;
            }
        }

        RenderHeartBeats();
       
	}
}

void UMyProjectCharacterOverlay::RenderHeartBeats()
{
    FVector2D NewScale = FVector2D(CurrentScaleHearts, CurrentScaleHearts);

    UImage* BeatingHearts[] = { BeatingHeart1, BeatingHeart2, BeatingHeart3, BeatingHeart4 };
    for (UImage* Heart : BeatingHearts)
    {
        Heart->SetRenderScale(NewScale);
    }
}

void UMyProjectCharacterOverlay::ShowHeartVisibilities(bool SH1, bool SH2, bool SH3, bool SH4, bool BH1, bool BH2, bool BH3, bool BH4)
{
    if (AllHeartsValid()) 
    {
        TArray<UImage*> HeartImages =
        {
            StaticHeart1,
            StaticHeart2,
            StaticHeart3,
            StaticHeart4,
            BeatingHeart1,
            BeatingHeart2,
            BeatingHeart3,
            BeatingHeart4
        };
        TArray<bool> HeartBoolStates = { SH1, SH2, SH3, SH4, BH1, BH2, BH3, BH4 };

        for (int32 i = 0; i < HeartImages.Num(); ++i)
        {
            ESlateVisibility VisibleOrHidden = HeartBoolStates.IsValidIndex(i) && HeartBoolStates[i] ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
            HeartImages[i]->SetVisibility(VisibleOrHidden);
        }
    }
    
}

void UMyProjectCharacterOverlay::ShowStudIconVisibilities(bool SS, bool GS, bool BS, bool PS)
{
    if (AllStudIconsValid())
    {
        TArray<UImage*> StudIconImages =
        {
            SilverStudImage,
            GoldStudImage,
            BlueStudImage,
            PurpleStudImage
        };

        TArray<bool> StudIconBoolStates = { SS, GS, BS, PS };

        for (int32 i = 0; i < StudIconImages.Num(); ++i)
        {
            ESlateVisibility VisibleOrHidden = StudIconBoolStates.IsValidIndex(i) && StudIconBoolStates[i] ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
            StudIconImages[i]->SetVisibility(VisibleOrHidden);
        }
    }
}

void UMyProjectCharacterOverlay::ShowStudCountVisibility(bool ShowCount)
{
    if (ShowCount)
    {
        StudCountText->SetVisibility(ESlateVisibility::Visible);
    }
    else 
    {
        StudCountText->SetVisibility(ESlateVisibility::Hidden);
    }
}

bool UMyProjectCharacterOverlay::AllHeartsValid()
{
    TArray<UImage*> HeartImages =
    {
        StaticHeart1,
        StaticHeart2,
        StaticHeart3,
        StaticHeart4,
        BeatingHeart1,
        BeatingHeart2,
        BeatingHeart3,
        BeatingHeart4
    };

    bool bAllValidHearts = true;

    for (UImage* Heart : HeartImages)
    {
        if (!Heart)
        {
            bAllValidHearts = false;
            break;
        }
    }

    return bAllValidHearts;
}

bool UMyProjectCharacterOverlay::AllStudIconsValid()
{
    TArray<UImage*> StudIconImages =
    {
        SilverStudImage,
        GoldStudImage,
        BlueStudImage,
        PurpleStudImage
    };

    bool bAllValidStudIcons = true;

    for (UImage* StudIcon : StudIconImages)
    {
        if (!StudIcon)
        {
            bAllValidStudIcons = false;
            break;
        }
    }

    return bAllValidStudIcons;
}


