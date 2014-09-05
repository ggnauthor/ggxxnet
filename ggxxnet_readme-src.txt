【説明】
今更ですがソース公開します。
少数ながら7年近く経った今も利用してくれている人がいるにも関わらず、
自分が既知の様々な欠陥、とりわけセキュリティの問題に
対応できないことが主な理由です。

公開するソースは機能的にはver1.20-3のまま変更していません。

配布ライセンスはパブリックドメインとします。
改変や再配布など自由に使ってください。

【ビルド環境】
OS:     WindowsXP(x86), Windows7(x64)
IDE:    Microsoft Visual Studio 2005 Std
Module: zlib-1.2.5
        directx8 sdk (oct2004)

【ファイル説明】
ggxx.udd :
  ollydbg110のggxx.exe解析ファイル。uddディレクトリの下に置き
  パッチ済のggxx.exeをG:\dev\myproj\ggxxnetの下に置くと利用可能

zlib125 :
  zlib-1.2.5をvc++2005でビルドするための追加ファイル

ggxxnet : ggxxnetのソースコード
  ggnlobby.cgi : ロビーcgiスクリプト
  nettester    : 通信シミュレータ
  ggncfg       : ggxxnet_configのソースコード

【ビルド手順】
1 zlibのビルド
  1-1. zlib-1.2.5をダウンロード＆解凍しzlib125以下に置く
  1-2. ml.exeがない場合はダウンロードしパスを通しておく。
       http://www.microsoft.com/en-us/download/details.aspx?id=12654
  1-3. build_vc8.batを実行すると以下生成される
       G:\dev\myproj\ggxxnet\zlib125\contrib\vstudio\vc8\x86\ZlibStatDebug\zlibstat.lib
       G:\dev\myproj\ggxxnet\zlib125\contrib\vstudio\vc8\x86\ZlibStatRelease\zlibstat.lib

2 DirectX8の設定
  2-1. G:\dev\lib\DirectX8_SDK_OCT2004以下にDirectX8のSDKを配置する
       IncludeとLibだけでＯＫ

3 ggxxnetのビルド
  ggxxnet\ggxxnet80.vcprojを開きリビルド
  ggxxnet.dllが生成される

以上
