#include "UI/HorrorSimInventoryContainerWidget.h"

#include "Framework/Application/NavigationConfig.h"

void UHorrorSimInventoryContainerWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	if (FSlateApplication::IsInitialized())
	{
		// Get the application's navigation config
		TSharedRef<FNavigationConfig> NavConfig = FSlateApplication::Get().GetNavigationConfig();
		bWasTabNavigationEnabled = NavConfig->bTabNavigation;
		NavConfig->bTabNavigation = false;
	}
}

void UHorrorSimInventoryContainerWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
	if (FSlateApplication::IsInitialized())
	{
		TSharedRef<FNavigationConfig> NavConfig = FSlateApplication::Get().GetNavigationConfig();
		NavConfig->bTabNavigation = bWasTabNavigationEnabled;
	}
}
