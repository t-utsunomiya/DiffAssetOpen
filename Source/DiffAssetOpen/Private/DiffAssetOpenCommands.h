#pragma once

#if WITH_EDITOR

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "EditorStyleSet.h"

class FDiffAssetOpenCommands : public TCommands<FDiffAssetOpenCommands>
{
public:
	FDiffAssetOpenCommands()
		: TCommands<FDiffAssetOpenCommands>(TEXT("DiffAssetOpen")
		, NSLOCTEXT("Contexts", "DiffAssetOpen", "DiffAssetOpen Command")
		, NAME_None, FEditorStyle::GetStyleSetName())
	{
	}

	virtual void RegisterCommands() override;

	TSharedPtr<class FUICommandInfo> DiffAssetOpenCommand;
};

#endif
