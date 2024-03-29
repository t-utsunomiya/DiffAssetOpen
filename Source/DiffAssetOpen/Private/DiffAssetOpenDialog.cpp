﻿#ifdef WITH_EDITOR
#include "DiffAssetOpenDialog.h"
#include "Widgets/SCanvas.h"
#include "HAL/PlatformFileManager.h"
#include "AssetTypeActions_Base.h"
#include "IAssetTools.h"
#include "AssetToolsModule.h"
#include "Misc/Paths.h"
#include "Misc/MessageDialog.h"
#include "Application/SlateApplicationBase.h"
#include "Application/SlateWindowHelper.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Text/STextBlock.h"
#include "ObjectTools.h"
#include "UObject/UObjectIterator.h"

#define LOCTEXT_NAMESPACE "DiffAssetOpenDialog"

SDiffAssetOpenDialog::SDiffAssetOpenDialog()
 : WindowTitle(TEXT("DiffAssetOpenDialog"))
{
}

void SDiffAssetOpenDialog::SetDialogContent()
{
	ArgmentsTextBox = SNew(SEditableTextBox)
		.HintText(LOCTEXT("ArgmentsTextBoxHint", "Enter Argments: e.g. C:\\LeftPath.uasset,C:\\RightPath.uasset,LeftAssetName,RightAssetName"))
		.OnTextChanged_Raw(this, &SDiffAssetOpenDialog::OnChangeArgments);
	LeftPathTextBox = SNew(SEditableTextBox)
		.HintText(LOCTEXT("LeftPathTextBoxHint", "Enter Path: e.g. C:\\Users\\UserName\\AppData\\Local\\Temp\\NewBlueprint.uasset-rev3.svn000.tmp.uasset"))
		.OnTextChanged_Raw(this, &SDiffAssetOpenDialog::OnChangeLeftPath);
	RightPathTextBox = SNew(SEditableTextBox)
		.HintText(LOCTEXT("RightPathTextBoxHint", "Enter Path: e.g. C:\\Users\\UserName\\AppData\\Local\\Temp\\NewBlueprint.uasset-rev4.svn000.tmp.uasset"))
		.OnTextChanged_Raw(this, &SDiffAssetOpenDialog::OnChangeRightPath);
	LeftAssetNameTextBox = SNew(SEditableTextBox)
		.HintText(LOCTEXT("LeftAssetNameTextBoxHint", "Enter AssetName: e.g. NewBlueprint"))
		.OnTextChanged_Raw(this, &SDiffAssetOpenDialog::OnChangeLeftAssetName);
	RightAssetNameTextBox = SNew(SEditableTextBox)
		.HintText(LOCTEXT("RightAssetNameTextBoxHint", "Enter AssetName: e.g. NewBlueprint"))
		.OnTextChanged_Raw(this, &SDiffAssetOpenDialog::OnChangeRightAssetName);
	LeftChangelistTextBox = SNew(SEditableTextBox)
		.HintText(LOCTEXT("LeftChangelistTextBoxHint", "Enter Changelist(Revision): e.g. 1 (only number)"))
		.OnTextChanged_Raw(this, &SDiffAssetOpenDialog::OnChangeLeftChangelist);
	RightChangelistTextBox = SNew(SEditableTextBox)
		.HintText(LOCTEXT("RightChangelistTextBoxHint", "Enter Changelist(Revision): e.g. 2 (only number)"))
		.OnTextChanged_Raw(this, &SDiffAssetOpenDialog::OnChangeRightChangelist);

	#define ROW(Description, TextBox) \
		+ SVerticalBox::Slot().VAlign(VAlign_Top).Padding(2.f).AutoHeight() [ \
			SNew(SHorizontalBox) \
			+ SHorizontalBox::Slot().VAlign(VAlign_Center).HAlign(HAlign_Left).Padding(2.f).FillWidth(1.1f) \
			[ SNew(STextBlock).Text(LOCTEXT(Description, Description)) ] \
			+ SHorizontalBox::Slot().VAlign(VAlign_Center).HAlign(HAlign_Left).Padding(2.f).FillWidth(5.8f) \
			[ SNew(SCanvas) + SCanvas::Slot().VAlign(VAlign_Center).HAlign(HAlign_Left).Size(FVector2D(780.f, 18.f))[TextBox.ToSharedRef()] ] \
		]

	SetContent(
		SNew(SVerticalBox)
		ROW("Left Path: ", LeftPathTextBox)
		ROW("Right Path: ", RightPathTextBox)
		ROW("Left Asset Name: ", LeftAssetNameTextBox)
		ROW("Right Asset Name: ", RightAssetNameTextBox)
		ROW("Left Changelist(Revision): ", LeftChangelistTextBox)
		ROW("Right Changelist(Revision): ", RightChangelistTextBox)
		+ SVerticalBox::Slot().VAlign(VAlign_Top).Padding(2.f).AutoHeight()[
			SNew(SHorizontalBox)
				+ SHorizontalBox::Slot().VAlign(VAlign_Center).HAlign(HAlign_Center).Padding(2.f).FillWidth(1.1f)
				[SNew(STextBlock).Text(LOCTEXT("ApplyArgmentDescription", "^^^ Apply Argments ^^^"))]
		]
		ROW("Argments: ", ArgmentsTextBox)
		+ SVerticalBox::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom).Padding(2.f)
		[
			SNew(SButton).Text(LOCTEXT("Open", "Open"))
			.OnClicked_Raw(this, &SDiffAssetOpenDialog::OnOpenButtonClicked)
		]
		+ SVerticalBox::Slot().HAlign(HAlign_Right).VAlign(VAlign_Bottom).Padding(2.f)
		[
			SNew(SButton).Text(LOCTEXT("Delete Temp Dir", "Delete Temp Dir"))
			.OnClicked_Raw(this, &SDiffAssetOpenDialog::OnDeleteTempDirButtonClicked)
		]
	#undef ROW
	);
	Resize(FVector2D(940.f, 230.f));
}

void SDiffAssetOpenDialog::SetFocusTopInputForm()
{
	SetFocus(ArgmentsTextBox.ToSharedRef());
}

bool SDiffAssetOpenDialog::Copy(const TCHAR* DestPath, const TCHAR* SrcPath)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.CreateDirectoryTree(GetData(FPaths::GetPath(DestPath))))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed create directory '%s'."), SrcPath);
		return false;
	}
	if (!PlatformFile.CopyFile(DestPath, SrcPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed copy file. From: '%s' To: '%s'."), SrcPath, DestPath);
		return false;
	}
	return true;
}

bool SDiffAssetOpenDialog::CopyToTempDiffFolder(const TCHAR* DestPath, const TCHAR* SrcPath)
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	if (!PlatformFile.FileExists(SrcPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Not found file '%s.'"), SrcPath);
		return false;
	}

	const int64 FileSize = PlatformFile.FileSize(SrcPath);
	const int64 WorningSize = 500 * 1024 * 1024;
	if (FileSize > WorningSize) // 大きなサイズのファイルをコピーするときは確認のダイアログを出す。
	{
		const FString MessageString = FString::Printf(
			TEXT("大きなサイズのファイルを コピーしようとしています。コピーを続けますか？\n")
			TEXT("コピー元: %s\nコピー先: %s\nサイズ: %d MB"), SrcPath, DestPath, (FileSize / (1024 * 1024)) + ((FileSize % (1024 * 1024))? 1:0));
		const FText TitleText = LOCTEXT("DiffAssetOpenDialog - FileSizeWarning", "DiffAssetOpenDialog - FileSizeWarning");
		if (FMessageDialog::Open(EAppMsgType::YesNo, FText::FromString(MessageString), &TitleText) == EAppReturnType::Yes)
		{
			return Copy(DestPath, SrcPath);
		}
		return false;
	}
	return Copy(DestPath, SrcPath);
}

bool SDiffAssetOpenDialog::GetPackageName(const FString& FilePath, FString& OutPackageName)
{
	FString FailureReason;
	if (!FPackageName::TryConvertFilenameToLongPackageName(FilePath, OutPackageName, &FailureReason))
	{
		UE_LOG(LogTemp, Error, TEXT("Convert package name failed. filename: '%s', reason: '%s'")
			, GetData(FilePath), GetData(FailureReason));
		return false;
	}
	return true;
}

FString SDiffAssetOpenDialog::GetDiffPath(const FString& SrcPath)
{
	return FPaths::CreateTempFilename(GetData(FPaths::DiffDir()),
		// ファイル名のベースネームに"."があるとロードできなかったので適当な文字に置換しています。
		// 詳しく調べてませんがアセット名に"."が使えないことが理由なのだと思います。
		// また、"."があっても読み込む方法があるのかもしれませんが調べられてません。
		GetData(FPaths::GetBaseFilename(SrcPath).Replace(TEXT("."), TEXT("_"))),
		GetData(TEXT(".") + FPaths::GetExtension(SrcPath)));
}

FReply SDiffAssetOpenDialog::OnOpenButtonClicked()
{
	if (LeftPath.IsEmptyOrWhitespace() || RightPath.IsEmptyOrWhitespace()
		|| RightAssetName.IsEmptyOrWhitespace() || RightAssetName.IsEmptyOrWhitespace())
	{
		UE_LOG(LogTemp, Error, TEXT("Input form is empty one or more. Please enter all input form."));
	}
	else
	{
		FString LeftSrcPath = LeftPath.ToString();
		FString RightSrcPath = RightPath.ToString();
		FPaths::NormalizeFilename(LeftSrcPath);
		FPaths::NormalizeFilename(RightSrcPath);
		FString const LeftDiffPath = GetDiffPath(LeftSrcPath);
		FString const RightDiffPath = GetDiffPath(RightSrcPath); ;

		if (CopyToTempDiffFolder(GetData(LeftDiffPath), GetData(LeftSrcPath)) &&
			CopyToTempDiffFolder(GetData(RightDiffPath), GetData(RightSrcPath)))
		{
			FString LeftPackageName;
			FString RightPackageName;
			if (GetPackageName(LeftDiffPath, LeftPackageName)
				&& GetPackageName(RightDiffPath, RightPackageName))
			{
				OpenDiffAssetsWindow(LeftPackageName, RightPackageName
					, LeftAssetName.ToString(), RightAssetName.ToString()
					, LeftChangelist, RightChangelist);
			}
		}
	}
	return FReply::Handled();
}

bool SDiffAssetOpenDialog::DeleteTempObject()
{
	TArray<UObject*> ObjectsToDelete;
	for (FThreadSafeObjectIterator It; It; ++It)
	{
		// GetPathName() に "/Temp/Diff/" が含まれているものを削除しようとするとBPの関数ノードのようなサブオブジェクトなども削除しようとしてしまい、
		// 不要に削除時間がかかってました。パッケージは GetName() で取得する文字列にも "/Temp/Diff" が含まれるので GetName() で判定しています。
		if (It->GetName().Find("/Temp/Diff/") == 0)
		{
			ObjectsToDelete.Add(*It);
		}
	}
	if (ObjectsToDelete.Num())
	{
		int32 ResultCount = ObjectTools::DeleteObjects(ObjectsToDelete);
		if (ResultCount != ObjectsToDelete.Num())
		{
			// Delete Dialog で削除をキャンセルすると実際に削除したオブジェクト数が削除しようとした数よりも少なくなる。
			// これはキャンセルしたときの正常な動作なので警告の出力は不要だと思うが、
			// キャンセルしたとき以外で正常に削除出来なかった時のために警告を出力している。
			UE_LOG(LogTemp, Warning, TEXT("Delete object num is min. Expect delete object num: %d Delete object num: %d"), ObjectsToDelete.Num(), ResultCount);
			return false;
		}
	}
	return true;
}

void SDiffAssetOpenDialog::DeleteTempDir()
{
	IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();
	const FString DirPath = FPaths::DiffDir();
	bool bDeleteSuccess = PlatformFile.DeleteDirectoryRecursively(*DirPath);
	if (!bDeleteSuccess)
	{
		UE_LOG(LogTemp, Error, TEXT("Dir delete faild. Dir: %s"), *DirPath);
	}
}

FReply SDiffAssetOpenDialog::OnDeleteTempDirButtonClicked()
{
	if (!DeleteTempObject())
	{
		return FReply::Handled();
	}
	DeleteTempDir();
	return FReply::Handled();
}

static void ArgToText(TArray<FString>& Args, int ArgIndex, FText& Text, TSharedPtr<SEditableTextBox>& TextBox)
{
	if (Args.Num() > ArgIndex)
	{
		Text = FText::FromString(Args[ArgIndex]);
	}
	else
	{
		Text = FText();
	}

	if (TextBox.IsValid())
	{
		TextBox->SetText(Text);
	}

}

static void ArgToTextBox(TArray<FString>& Args, int ArgIndex, TSharedPtr<SEditableTextBox>& TextBox)
{
	FText Text;
	if (Args.Num() > ArgIndex)
	{
		Text = FText::FromString(Args[ArgIndex]);
	}
	else
	{
		Text = FText();
	}

	if (TextBox.IsValid())
	{
		TextBox->SetText(Text);
	}

}

void SDiffAssetOpenDialog::OnChangeArgments(const FText& NewText)
{
	Argments = NewText;

	TArray<FString> Args;
	NewText.ToString().ParseIntoArray(Args, TEXT(","));

	ArgToText(Args, 0, LeftPath, LeftPathTextBox);
	ArgToText(Args, 1, RightPath, RightPathTextBox);
	ArgToText(Args, 2, LeftAssetName, LeftAssetNameTextBox);
	ArgToText(Args, 3, RightAssetName, RightAssetNameTextBox);
	ArgToTextBox(Args, 4, LeftChangelistTextBox);
	ArgToTextBox(Args, 5, RightChangelistTextBox);
}

void SDiffAssetOpenDialog::OnChangeLeftPath(const FText& NewPath)
{
	LeftPath = NewPath;
}

void SDiffAssetOpenDialog::OnChangeRightPath(const FText& NewPath)
{
	RightPath = NewPath;
}

void SDiffAssetOpenDialog::OnChangeLeftAssetName(const FText& NewAssetName)
{
	LeftAssetName = NewAssetName;
}

void SDiffAssetOpenDialog::OnChangeRightAssetName(const FText& NewAssetName)
{
	RightAssetName = NewAssetName;
}

void SDiffAssetOpenDialog::OnChangeLeftChangelist(const FText& NewChangelist)
{
	if (NewChangelist.IsNumeric())
	{
		LeftChangelist = FCString::Atoi(*NewChangelist.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Left Changelist is not number. Please input number. Changelist: %s"), *NewChangelist.ToString());
	}
}

void SDiffAssetOpenDialog::OnChangeRightChangelist(const FText& NewChangelist)
{
	if (NewChangelist.IsNumeric())
	{
		RightChangelist = FCString::Atoi(*NewChangelist.ToString());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Right Changelist is not number. Please input number. Changelist: %s"), *NewChangelist.ToString());
	}
}

void SDiffAssetOpenDialog::SetFocus(TSharedRef<SWidget> TargetWidget)
{
	TArray< TSharedRef<SWindow> > DiffAssetSetting;
	DiffAssetSetting.Add(SharedThis(this));
	FWidgetPath WidgetToFocusPath;
	if (FSlateWindowHelper::FindPathToWidget(DiffAssetSetting, TargetWidget, WidgetToFocusPath))
	{
		FSlateApplicationBase::Get().SetAllUserFocus(WidgetToFocusPath, EFocusCause::SetDirectly);
	}
}

void SDiffAssetOpenDialog::OpenDiffAssetsWindow(const FString& InLeftPath, const FString& InRightPath
	, const FString& InLeftAssetName, const FString& InRightAssetName
	, int32 InLeftChangelist, int32 InRightChangelist) const
{
	FRevisionInfo LeftVersionInfo;
	LeftVersionInfo.Revision = FString::FromInt(InLeftChangelist);
	LeftVersionInfo.Changelist = InLeftChangelist;
	LeftVersionInfo.Date = FDateTime();

	FRevisionInfo RightVersionInfo;
	RightVersionInfo.Revision = FString::FromInt(InRightChangelist);
	RightVersionInfo.Changelist = InRightChangelist;
	RightVersionInfo.Date = FDateTime();

	UPackage* LeftAssetPackage = LoadPackage(NULL, *InLeftPath, LOAD_DisableCompileOnLoad);
	UObject* LeftAsset = FindObject<UObject>(LeftAssetPackage, *InLeftAssetName);

	UPackage* RightAssetPackage = LoadPackage(NULL, *InRightPath, LOAD_DisableCompileOnLoad);
	UObject* RightAsset = FindObject<UObject>(RightAssetPackage, *InRightAssetName);

	FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
	AssetToolsModule.Get().DiffAssets(LeftAsset, RightAsset, LeftVersionInfo, RightVersionInfo);
}

#undef LOCTEXT_NAMESPACE
#endif
