﻿#pragma once

#ifdef WITH_EDITOR

#include "CoreMinimal.h"
#include "Widgets/SWindow.h"

class SDiffAssetOpenDialog : public SWindow
{
public:
	SDiffAssetOpenDialog();
	/**
	 * ダイアログの中身を設定する.
	 */
	void SetDialogContent();
	/**
	 * フォーカスを一番上の入力フォームに合わせます。
	 */
	void SetFocusTopInputForm();

private:
	FReply OnOpenButtonClicked();
	FReply OnDeleteTempDirButtonClicked();
	void OnChangeLeftPath(const FText& NewPath);
	void OnChangeRightPath(const FText& NewPath);
	void OnChangeLeftAssetName(const FText& NewAssetName);
	void OnChangeRightAssetName(const FText& NewAssetName);
	void SetFocus(TSharedRef<SWidget> TargetWidget);
	bool Copy(const TCHAR* DestPath, const TCHAR* SrcPath);
	bool CopyToTempDiffFolder(const TCHAR* DestPath, const TCHAR* SrcPath);
	bool GetPackageName(const FString& FilePath, FString& OutPackageName);
	void OpenDiffAssetsWindow(const FString& InLeftPath, const FString& InRightPath
		, const FString& InLeftAssetName, const FString& InRightAssetName) const;
	FString GetDiffPath(const FString& SrcPath);
	bool DeleteTempObject();
	void DeleteTempDir();

private:
	FText LeftPath;
	FText RightPath;
	FText LeftAssetName;
	FText RightAssetName;
	TSharedPtr<SWidget> LeftPathTextBox;
	TSharedPtr<SWidget> RightPathTextBox;
	TSharedPtr<SWidget> LeftAssetNameTextBox;
	TSharedPtr<SWidget> RightAssetNameTextBox;
	const TCHAR* WindowTitle;
};

#endif
