# DiffAssetOpen
UE4 のアセット差分ツールを開くプラグイン
TortoiseSVN のコミットログウィンドウなどから uassset を開く事を目的として作成しています。

## 設定
1. このリポジトリを UE4 プロジェクトフォルダの /Plugin/Developer フォルダにコピーするかチェックアウトしてください。
2. UE4 プロジェクトのプラグイン設定でこのプラグインを有効にしてください。

## 使い方
1. メニューから "Window" - "Developer Tools" - "Diff Asset Open" を選択して DiffAssetOpenDialog を開いてください。
　ショートカットキー Shift + Ctrl + Alt + D を押して開くこともできます。

2. ダイアログの入力フォームを入力してください。
```
  Left Path(L): 比較対象の uasset のファイルパス1
    （例: C:\Users\UserName\AppData\Local\Temp\NewBlueprint.uasset-rev3.svn000.tmp.uasset）

  Right Path(R): 比較対象の uasset のファイルパス2
    （例: C:\Users\UserName\AppData\Local\Temp\NewBlueprint.uasset-rev4.svn000.tmp.uasset）

  Left Asset Name(E): 比較対象のアセット名1
    （例: NewBlueprint）

  Right Asset Name(I): 比較対象のアセット名2
    （例: NewBlueprint）
```

3. Open ボタンを押すと差分ツールを開きます。

DiffAssetOpenRelationTool を使った使用例<br>
[![](https://img.youtube.com/vi/PoC-79Rl0C0/0.jpg)](https://www.youtube.com/watch?v=PoC-79Rl0C0)

## 注意
- 指定したパスのファイルを Temp/Diff フォルダにコピーします。サイズの大きいファイルはコピーに時間がかかる可能性があります。
サイズの大きなファイル（500MB以上）はコピーする前に確認ダイアログを表示します。
- UE4 の差分ツールで対応していないアセットは開いても差分は見れません。ブループリントアセットの差分は確認できる事を確認しています。
- DiffAssetOpenDialog を開くショートカットキーは TortoiseSVN と連携する際に使用します。ショートカットキーを変更するとうまく連携できなくなります。
