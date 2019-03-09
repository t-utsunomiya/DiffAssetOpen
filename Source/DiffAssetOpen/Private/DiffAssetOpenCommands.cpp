#if WITH_EDITOR

#include "DiffAssetOpenCommands.h"

#define LOCTEXT_NAMESPACE "DiffAssetOpenCommand"

void FDiffAssetOpenCommands::RegisterCommands()
{
	UI_COMMAND(DiffAssetOpenCommand
		, "Call DiffAssetOpen Command", "Description: call DiffAssetOpen Command"
		// なるべく他のキー設定と被らないように EModifierkey を複数設定しています。
		// 手動で起動するのではなく、ツールを介して DiffAssetPpenDialog を開くことを想定しているので
		// EModifierkey の複数設定は使い勝手の低下につながらないと思います。
		// EKeys::F12 を設定すると、 EModifierKey と一緒に押したときは正しく動作するものの
		// F12 単体で押したときに例外が発生したので使用を控えています。
		, EUserInterfaceActionType::None, FInputChord(EKeys::D
			, EModifierKey::Control|EModifierKey::Shift|EModifierKey::Alt));
}

#undef LOCTEXT_NAMESPACE

#endif
