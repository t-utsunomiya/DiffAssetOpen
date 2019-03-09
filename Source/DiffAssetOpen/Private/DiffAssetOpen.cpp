#ifdef WITH_EDITOR
#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IDiffAssetOpen.h"
#include "DiffAssetOpenCommands.h"
#include "DiffAssetOpenDialog.h"
#include "LevelEditor.h"
#include "Interfaces/IMainFrameModule.h"
#include "Kismet2/DebuggerCommands.h"
#include "Framework/Application/SlateApplication.h"
#include "Editor.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"

#define LOCTEXT_NAMESPACE "DiffAssetOpen"

class FDiffAssetOpen : public IDiffAssetOpen
{
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	void OnWindowMenuExtension(FMenuBuilder& MenuBuilder);
	void OnDiffAssetOpenMenu();
	void OnMainFrameLoad(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjecWindow);
	void OnEditorLoaded(SWindow& SlateWindow, void* ViewportRHIPtr);

private:
	TWeakPtr<SWindow> RootWindow;
	TWeakPtr<SDiffAssetOpenDialog> DiffAssetOpenDialog;
	TSharedPtr<FExtender> Extender;
	bool IsCommandBind = false;
	FDelegateHandle LoadedDelegateHandle;
};

IMPLEMENT_MODULE( FDiffAssetOpen, DiffAssetOpen)

void FDiffAssetOpen::StartupModule()
{
	if (IsRunningCommandlet()) { return; }

	Extender = MakeShareable(new FExtender);
	Extender->AddMenuExtension(
		"Miscellaneous",
		EExtensionHook::After,
		NULL,
		FMenuExtensionDelegate::CreateRaw(this, &FDiffAssetOpen::OnWindowMenuExtension)
	);
	FLevelEditorModule& LevelEditorModule =
		FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
	LevelEditorModule.GetMenuExtensibilityManager()->AddExtender(Extender);
	IMainFrameModule& MainFrameModule =
		FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
	MainFrameModule.OnMainFrameCreationFinished().AddRaw(
		this, &FDiffAssetOpen::OnMainFrameLoad);
	
	FDiffAssetOpenCommands::Register();
	FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer();
	LoadedDelegateHandle = SlateRenderer->OnSlateWindowRendered().AddRaw(
		this, &FDiffAssetOpen::OnEditorLoaded);
}

void FDiffAssetOpen::ShutdownModule()
{
	if (Extender.IsValid() && FModuleManager::Get().IsModuleLoaded("LevelEditor"))
	{
		FLevelEditorModule& LevelEditorModule =
			FModuleManager::LoadModuleChecked<FLevelEditorModule>("LevelEditor");
		LevelEditorModule.GetMenuExtensibilityManager()->RemoveExtender(Extender);
	}

	if (FModuleManager::Get().IsModuleLoaded("MainFrame"))
	{
		IMainFrameModule& MainFrameModule =
			FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
		MainFrameModule.OnMainFrameCreationFinished().RemoveAll(this);
	}
}

void FDiffAssetOpen::OnMainFrameLoad(TSharedPtr<SWindow> InRootWindow, bool bIsNewProjectWindow)
{
	if ((!bIsNewProjectWindow) && (InRootWindow.IsValid()))
	{
		RootWindow = InRootWindow;
	}
}

void FDiffAssetOpen::OnEditorLoaded(SWindow& SlateWindow, void* ViewportRHIPtr)
{
	if (!GEditor)
	{
		return;
	}
	if (IsInGameThread())
	{
		FSlateRenderer* SlateRenderer = FSlateApplication::Get().GetRenderer();
		SlateRenderer->OnSlateWindowRendered().Remove(LoadedDelegateHandle);
	}
	if (!IsCommandBind)
	{
		const FDiffAssetOpenCommands& Commands = FDiffAssetOpenCommands::Get();
		if (FPlayWorldCommands::GlobalPlayWorldActions.IsValid())
		{
			FPlayWorldCommands::GlobalPlayWorldActions->MapAction(
				Commands.DiffAssetOpenCommand,
				FExecuteAction::CreateRaw(this, &FDiffAssetOpen::OnDiffAssetOpenMenu)
			);
		}
		IsCommandBind = true;
	}
}

void FDiffAssetOpen::OnWindowMenuExtension(FMenuBuilder& MenuBuilder)
{
	MenuBuilder.BeginSection("DiffAssetOpenHook", LOCTEXT("DiffAssetOpenPlugin", "DiffAssetOpenPlugin"));
	MenuBuilder.AddMenuEntry(
		LOCTEXT("Diff Asset Open", "Diff Asset Open"),
		LOCTEXT("Diff Asset Open", "Diff Asset Open"),
		FSlateIcon(),
		FUIAction(FExecuteAction::CreateRaw(this, &FDiffAssetOpen::OnDiffAssetOpenMenu)));
	MenuBuilder.EndSection();
}

void FDiffAssetOpen::OnDiffAssetOpenMenu()
{
	if (!DiffAssetOpenDialog.IsValid())
	{
		TSharedPtr<SDiffAssetOpenDialog> Dialog= SNew(SDiffAssetOpenDialog)
			.Title(LOCTEXT("DiffAssetOpenDialog", "DiffAssetOpenDialog"));
		DiffAssetOpenDialog = TWeakPtr<SDiffAssetOpenDialog>(Dialog);
		if (RootWindow.IsValid())
		{
			FSlateApplication::Get().AddWindowAsNativeChild(
				Dialog.ToSharedRef(), RootWindow.Pin().ToSharedRef());
		}
		Dialog->SetDialogContent();
	}
	DiffAssetOpenDialog.Pin()->BringToFront();
}

#undef LOCTEXT_NAMESPACE
#endif
