// Copyright (c) 2017 - 2025, Samsonov Andrei. All Rights Reserved.

#include "Public/ModWidget.h"

#include "Public/MainGameInstance.h"
#include "StaticLogger.h"
#include "Qr/LuaState.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Border.h"
#include "Components/HorizontalBox.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Layout/Margin.h"
#include "GameFramework/PlayerController.h"
#include "Misc/AssertionMacros.h"

static APlayerController *GetModWidgetOwningPC() {
  check(UMainGameInstance::Singleton);
  UWorld *World = UMainGameInstance::Singleton->GetWorld();
  check(World);
  APlayerController *PC = World->GetFirstPlayerController();
  check(PC);
  return PC;
}

void UModButton::NativePreConstruct() {
  Super::NativePreConstruct();
  EnsureWidgetTree();
}

void UModButton::BeginDestroy() {
  if (IsValid(ButtonWidget)) {
    ButtonWidget->OnClicked.RemoveAll(this);
  }
  OnClick.reset();
  Super::BeginDestroy();
}

void UModButton::EnsureWidgetTree() {
  if (!WidgetTree) {
    return;
  }
  if (WidgetTree->RootWidget) {
    return;
  }

  UVerticalBox *Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ModButtonRoot"));
  WidgetTree->RootWidget = Root;

  ButtonWidget = WidgetTree->ConstructWidget<UButton>(UButton::StaticClass(), TEXT("ModButtonButton"));
  LabelTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ModButtonLabel"));
  LabelTextBlock->SetText(FText::GetEmpty());
  ButtonWidget->AddChild(LabelTextBlock);
  Root->AddChild(ButtonWidget);
  ButtonWidget->OnClicked.AddDynamic(this, &UModButton::HandleClicked);
}

void UModButton::SetLabelText(const FText &NewLabel) {
  EnsureWidgetTree();
  check(LabelTextBlock);
  LabelTextBlock->SetText(NewLabel);
}

void UModButton::HandleClicked() {
  check(OnClick.has_value());
  checkf(OnClick->isFunction(), TEXT("ModButton: OnClick must be a Lua function"));
  evo::LuaState::CallAndHandleError(*OnClick);
}

UModButton *UModButton::LuaNew(std::string_view LabelUtf8, const luabridge::LuaRef &OnClickRef) {
  checkf(OnClickRef.isFunction(), TEXT("ModButton.new: second argument must be a Lua function"));
  APlayerController *PC = GetModWidgetOwningPC();
  UModButton *W = CreateWidget<UModButton>(PC, UModButton::StaticClass());
  check(W);
  W->OnClick = OnClickRef;
  W->SetLabelText(FText::FromString(UTF8_TO_TCHAR(std::string(LabelUtf8).c_str())));
  return W;
}

void UModHorizontalBox::EnsureContainer() {
  checkf(WidgetTree != nullptr, TEXT("UModHorizontalBox::EnsureContainer: WidgetTree is null on %s"), *GetNameSafe(this));

  if (Container != nullptr) {
    checkf(WidgetTree->RootWidget == Container,
           TEXT("UModHorizontalBox: RootWidget (%s) is not the horizontal container on %s — do not use a widget BP root on ModHorizontalBox."),
           *GetNameSafe(WidgetTree->RootWidget),
           *GetNameSafe(this));
    return;
  }

  checkf(WidgetTree->RootWidget == nullptr,
         TEXT("UModHorizontalBox: RootWidget already set (%s) but horizontal container was not created on %s — "
              "NativePreConstruct must not pre-fill the tree for this C++ widget."),
         *GetNameSafe(WidgetTree->RootWidget),
         *GetNameSafe(this));

  Container = WidgetTree->ConstructWidget<UHorizontalBox>(UHorizontalBox::StaticClass(), TEXT("ModHorizontalBox"));
  WidgetTree->RootWidget = Container;
}

void UModHorizontalBox::NativePreConstruct() {
  Super::NativePreConstruct();
  EnsureContainer();
}

void UModHorizontalBox::AddModChild(UModWidget *Child) {
  EnsureContainer();
  check(Child);
  checkf(Child->Slot == nullptr,
         TEXT("UModHorizontalBox::AddModChild: %s already has a parent slot; remove it first or use a new widget instance."),
         *GetNameSafe(Child));
  Container->AddChild(Child);
}

UModHorizontalBox *UModHorizontalBox::LuaNew() {
  APlayerController *PC = GetModWidgetOwningPC();
  UModHorizontalBox *W = CreateWidget<UModHorizontalBox>(PC, UModHorizontalBox::StaticClass());
  check(W);
  return W;
}

void UModBorder::EnsureBorder() {
  checkf(WidgetTree != nullptr, TEXT("UModBorder::EnsureBorder: WidgetTree is null on %s"), *GetNameSafe(this));

  if (BorderWidget != nullptr) {
    checkf(WidgetTree->RootWidget == BorderWidget,
           TEXT("UModBorder: RootWidget (%s) is not the border on %s — do not use a widget BP root on ModBorder."),
           *GetNameSafe(WidgetTree->RootWidget),
           *GetNameSafe(this));
    return;
  }

  checkf(WidgetTree->RootWidget == nullptr,
         TEXT("UModBorder: RootWidget already set (%s) but border was not created on %s — NativePreConstruct must not pre-fill the tree for this C++ widget."),
         *GetNameSafe(WidgetTree->RootWidget),
         *GetNameSafe(this));

  BorderWidget = WidgetTree->ConstructWidget<UBorder>(UBorder::StaticClass(), TEXT("ModBorder"));
  WidgetTree->RootWidget = BorderWidget;
}

void UModBorder::NativePreConstruct() {
  Super::NativePreConstruct();
  EnsureBorder();
}

void UModBorder::SetModChild(UModWidget *Child) {
  EnsureBorder();
  check(Child);
  checkf(Child->Slot == nullptr,
         TEXT("UModBorder::SetModChild: %s already has a parent slot; remove it first or use a new widget instance."),
         *GetNameSafe(Child));
  BorderWidget->SetContent(Child);
}

void UModBorder::SetUniformPadding(float Uniform) {
  EnsureBorder();
  BorderWidget->SetPadding(FMargin(Uniform));
}

UModBorder *UModBorder::LuaNew() {
  APlayerController *PC = GetModWidgetOwningPC();
  UModBorder *W = CreateWidget<UModBorder>(PC, UModBorder::StaticClass());
  check(W);
  return W;
}

void UModTextBlock::EnsureWidgetTree() {
  if (!WidgetTree) {
    return;
  }
  if (WidgetTree->RootWidget) {
    return;
  }

  UVerticalBox *Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ModTextBlockRoot"));
  WidgetTree->RootWidget = Root;

  TextWidget = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ModTextBlockLabel"));
  TextWidget->SetText(FText::GetEmpty());
  Root->AddChild(TextWidget);
}

void UModTextBlock::NativePreConstruct() {
  Super::NativePreConstruct();
  EnsureWidgetTree();
}

void UModTextBlock::SetTextUtf8(std::string_view TextUtf8) {
  EnsureWidgetTree();
  check(TextWidget);
  TextWidget->SetText(FText::FromString(UTF8_TO_TCHAR(std::string(TextUtf8).c_str())));
}

UModTextBlock *UModTextBlock::LuaNew(std::string_view InitialUtf8) {
  APlayerController *PC = GetModWidgetOwningPC();
  UModTextBlock *W = CreateWidget<UModTextBlock>(PC, UModTextBlock::StaticClass());
  check(W);
  W->SetTextUtf8(InitialUtf8);
  return W;
}

void UModVerticalBox::EnsureContainer() {
  checkf(WidgetTree != nullptr, TEXT("UModVerticalBox::EnsureContainer: WidgetTree is null on %s"), *GetNameSafe(this));

  if (Container != nullptr) {
    checkf(WidgetTree->RootWidget == Container,
           TEXT("UModVerticalBox: RootWidget (%s) is not the vertical container on %s — do not use a widget BP root on ModVerticalBox."),
           *GetNameSafe(WidgetTree->RootWidget),
           *GetNameSafe(this));
    return;
  }

  checkf(WidgetTree->RootWidget == nullptr,
         TEXT("UModVerticalBox: RootWidget already set (%s) but vertical container was not created on %s — "
              "NativePreConstruct must not pre-fill the tree for this C++ widget."),
         *GetNameSafe(WidgetTree->RootWidget),
         *GetNameSafe(this));

  Container = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ModVerticalBox"));
  WidgetTree->RootWidget = Container;
}

void UModVerticalBox::NativePreConstruct() {
  Super::NativePreConstruct();
  EnsureContainer();
}

void UModVerticalBox::AddModChild(UModWidget *Child) {
  EnsureContainer();
  check(Child);
  checkf(Child->Slot == nullptr,
         TEXT("UModVerticalBox::AddModChild: %s already has a parent slot; remove it first or use a new widget instance."),
         *GetNameSafe(Child));
  Container->AddChild(Child);
}

UModVerticalBox *UModVerticalBox::LuaNew() {
  APlayerController *PC = GetModWidgetOwningPC();
  UModVerticalBox *W = CreateWidget<UModVerticalBox>(PC, UModVerticalBox::StaticClass());
  check(W);
  return W;
}

void UModWindow::NativePreConstruct() {
  Super::NativePreConstruct();
  EnsureWidgetTree();
}

void UModWindow::EnsureWidgetTree() {
  if (!WidgetTree) {
    return;
  }
  if (WidgetTree->RootWidget) {
    return;
  }

  UVerticalBox *Root = WidgetTree->ConstructWidget<UVerticalBox>(UVerticalBox::StaticClass(), TEXT("ModWindowRoot"));
  WidgetTree->RootWidget = Root;

  TitleTextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("ModWindowTitle"));
  TitleTextBlock->SetText(FText::GetEmpty());
  Root->AddChild(TitleTextBlock);

  ContentOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass(), TEXT("ModWindowContentOverlay"));
  Root->AddChild(ContentOverlay);
}

void UModWindow::SetWindowTitle(const FText &Title) {
  EnsureWidgetTree();
  check(TitleTextBlock);
  TitleTextBlock->SetText(Title);
}

void UModWindow::SetContent(UModWidget *Content) {
  EnsureWidgetTree();
  check(ContentOverlay);
  check(Content);
  ContentOverlay->ClearChildren();
  ContentOverlay->AddChild(Content);
}

void UModWindow::Show(int32 ZOrder) {
  AddToViewport(ZOrder);
}

void UModWindow::Close() {
  RemoveFromParent();
}

UModWindow *UModWindow::LuaNew(std::string_view TitleUtf8) {
  APlayerController *PC = GetModWidgetOwningPC();
  UModWindow *W = CreateWidget<UModWindow>(PC, UModWindow::StaticClass());
  check(W);
  W->SetWindowTitle(FText::FromString(UTF8_TO_TCHAR(std::string(TitleUtf8).c_str())));
  LOG(INFO_LL) << "ModWindow created title=\"" << UTF8_TO_TCHAR(std::string(TitleUtf8).c_str()) << "\" widget="
               << *W->GetName();
  return W;
}

void UModWidgetLibrary::lua_reg(lua_State *L) {
  using namespace luabridge;

  // clang-format off
  getGlobalNamespace(L)
    .deriveClass<UModWidget, UObject>("ModWidget")
    .addFunction("__tostring", &UModWidget::ToString)
    .endClass()
    .deriveClass<UModButton, UModWidget>("ModButton")
    .addStaticFunction("new", &UModButton::LuaNew)
    .endClass()
    .deriveClass<UModHorizontalBox, UModWidget>("ModHorizontalBox")
    .addStaticFunction("new", &UModHorizontalBox::LuaNew)
    .addFunction("add", &UModHorizontalBox::AddModChild)
    .endClass()
    .deriveClass<UModBorder, UModWidget>("ModBorder")
    .addStaticFunction("new", &UModBorder::LuaNew)
    .addFunction("set_child", &UModBorder::SetModChild)
    .addFunction("set_padding", &UModBorder::SetUniformPadding)
    .endClass()
    .deriveClass<UModTextBlock, UModWidget>("ModTextBlock")
    .addStaticFunction("new", &UModTextBlock::LuaNew)
    .addFunction("set_text", &UModTextBlock::SetTextUtf8)
    .endClass()
    .deriveClass<UModVerticalBox, UModWidget>("ModVerticalBox")
    .addStaticFunction("new", &UModVerticalBox::LuaNew)
    .addFunction("add", &UModVerticalBox::AddModChild)
    .endClass()
    .deriveClass<UModWindow, UModWidget>("ModWindow")
    .addStaticFunction("new", &UModWindow::LuaNew)
    .addFunction("set_content", &UModWindow::SetContent)
    .addFunction("show", +[](UModWindow *self) { self->Show(0); })
    .addFunction("close", &UModWindow::Close)
    .endClass();
  // clang-format on
}
