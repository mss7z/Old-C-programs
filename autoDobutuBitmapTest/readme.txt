autoDobutuのビットマップファイルの変換のテストプログラムです

android端末のフレームバッファ(例:/dev/graphics/fb0)のデータをビットマップファイルに変換します
当時は相当苦労した記憶があります
コードが汚すぎますが、高専1年の時に書いたやつなので許してください
マジックナンバーを平気でコード中に書いているのが恥ずかしいです

確認しているコンパイル方法　1
(コンパイラ:Borland C++ 5.5.1 for Win32 Copyright (c) 1993, 2000 Borland)
(リンカ:Turbo Incremental Link 5.00 Copyright (c) 1997, 2000 Borland)
のバージョンで
bcc32 bitmaptest.cpp

確認しているコンパイル方法　2
(gcc (Rev1, Built by MSYS2 project) 9.3.0)
のバージョンで
gcc bitmaptest.cpp