// CategoryTabWidget.h
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/HorizontalBox.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "CategoryTabWidget.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCategorySelected, int32, CategoryIndex);

UCLASS()
class UCategoryTabWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(BlueprintAssignable)
    FOnCategorySelected OnCategorySelected;

    UFUNCTION(BlueprintCallable)
    void SetCategories(const TArray<FString>& CategoryNames);

    UFUNCTION(BlueprintCallable)
    void SetSelectedIndex(int32 Index);

protected:
    virtual void NativeConstruct() override;
    virtual TSharedRef<SWidget> RebuildWidget() override;

private:
    TSharedPtr<SHorizontalBox> TabContainer;
    TArray<FString> Categories;
    int32 SelectedIndex = 0;

    void CreateTabs();
    FReply OnTabClicked(int32 Index);
};
