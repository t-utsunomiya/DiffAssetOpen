#pragma once

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
	void OnChangeArgments(const FText& NewPath);
	void OnChangeLeftPath(const FText& NewPath);
	void OnChangeRightPath(const FText& NewPath);
	void OnChangeLeftAssetName(const FText& NewAssetName);
	void OnChangeRightAssetName(const FText& NewAssetName);
	void OnChangeLeftChangelist(const FText& NewChangelist);
	void OnChangeRightChangelist(const FText& NewChangelist);
	void SetFocus(TSharedRef<SWidget> TargetWidget);
	bool Copy(const TCHAR* DestPath, const TCHAR* SrcPath);
	bool CopyToTempDiffFolder(const TCHAR* DestPath, const TCHAR* SrcPath);
	bool GetPackageName(const FString& FilePath, FString& OutPackageName);
	void OpenDiffAssetsWindow(const FString& InLeftPath, const FString& InRightPath
		, const FString& InLeftAssetName, const FString& InRightAssetName
		, int32 InLeftChangelist, int32 InRightChangelist) const;
	FString GetDiffPath(const FString& SrcPath);
	bool DeleteTempObject();
	void DeleteTempDir();

private:
	FText Argments;
	FText LeftPath;
	FText RightPath;
	FText LeftAssetName;
	FText RightAssetName;
	int32 LeftChangelist = 1;
	int32 RightChangelist = 2; //一般的にリビジョンの異なるファイルの差分を見るので、 Leftとは異なる初期値を設定しておく。
	TSharedPtr<SEditableTextBox> ArgmentsTextBox;
	TSharedPtr<SEditableTextBox> LeftPathTextBox;
	TSharedPtr<SEditableTextBox> RightPathTextBox;
	TSharedPtr<SEditableTextBox> LeftAssetNameTextBox;
	TSharedPtr<SEditableTextBox> RightAssetNameTextBox;
	TSharedPtr<SEditableTextBox> LeftChangelistTextBox;
	TSharedPtr<SEditableTextBox> RightChangelistTextBox;
	const TCHAR* WindowTitle;
};

#endif
