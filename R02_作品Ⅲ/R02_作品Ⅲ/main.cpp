#define _CRT_SECURE_NO_WARNINGS

//########## ヘッダーファイル読み込み ##########
#include "DxLib.h"
#include "math.h"

//########## マクロ定義 ##########
#define GAME_WIDTH			800	//画面の横の大きさ
#define GAME_HEIGHT			600	//画面の縦の大きさ
#define GAME_COLOR			32	//画面のカラービット

#define GAME_WINDOW_BAR		0				//タイトルバーはデフォルトにする
#define GAME_WINDOW_NAME	"GAME TITLE"	//ウィンドウのタイトル
#define GAME_FPS			60				//FPSの数値	

#define GAME_ICON_ID		333				//ゲームのアイコンID

//パスの長さ
#define PATH_MAX			255	//255文字まで
#define NAME_MAX			255	//255文字まで

//キーボードの種類
#define KEY_MAX				256

//エラーメッセージ
#define IMAGE_LOAD_ERR_TITLE	TEXT("画像読み込みエラー")

//終了ダイアログ用
#define MOUSE_R_CLICK_TITLE		TEXT("ゲーム中断")
#define MOUSE_R_CLICK_CAPTION	TEXT("ゲームを中断し、タイトル画面に戻りますか？")

//フォント
#define FONT_TANU_PATH			TEXT(".\\TanukiMagic.ttf")			//フォントの場所
#define FONT_TANU_NAME			TEXT("たぬき油性マジック")			//フォントの名前

//エラーメッセージ
#define FONT_INSTALL_ERR_TITLE	TEXT("フォントインストールエラー")
#define FONT_CREATE_ERR_TITLE	TEXT("フォント作成エラー")

//########## 列挙型 ##########
enum GAME_SCENE {
	GAME_SCENE_START,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
};	//ゲームのシーン

//########## 構造体の宣言 ##########
typedef struct STRUCT_IMAGE
{
	char path[PATH_MAX];		//パス
	int handle;					//ハンドル
	int x;						//X位置
	int y;						//Y位置
	int width;					//幅
	int height;					//高さ
}IMAGE;	//画像構造体

//########## グローバル変数 ##########
//FPS関連
int StartTimeFps;				//測定開始時刻
int CountFps;					//カウンタ
float CalcFps;					//計算結果

//キーボードの入力を取得
char AllKeyState[KEY_MAX];		//すべてのキーの状態が入る
char OldAllKeyState[KEY_MAX];	//すべてのキーの状態(直前)が入る

//ゲーム関連
GAME_SCENE GameScene;			//ゲームのシーンを管理

//########## 外部のプロトタイプ宣言 ##########
VOID MY_FPS_UPDATE(VOID);	//FPS値を計測、更新する
VOID MY_FPS_DRAW(VOID);		//FPS値を描画する
VOID MY_FPS_WAIT(VOID);		//FPS値を計測し、待つ

VOID MY_START(VOID);		//スタート画面
VOID MY_START_PROC(VOID);	//スタート画面
VOID MY_START_DRAW(VOID);	//スタート画面
VOID MY_PLAY_INIT(VOID);	//プレイ画面(初期化)
VOID MY_PLAY(VOID);			//プレイ画面
VOID MY_PLAY_PROC(VOID);	//プレイ画面
VOID MY_PLAY_DRAW(VOID);	//プレイ画面
VOID MY_END(VOID);			//エンド画面
VOID MY_END_PROC(VOID);		//エンド画面
VOID MY_END_DRAW(VOID);		//エンド画面

VOID MY_ALL_KEYDOWN_UPDATE(VOID);				//キーの入力状態を更新する
BOOL MY_KEY_DOWN(int KEY_INPUT_);				//キーを押しているか、キーコードで判断する
BOOL MY_KEY_UP(int KEY_INPUT_);					//キーを押し上げたか、キーコードで判断する
BOOL MY_KEYDOWN_KEEP(int KEY_INPUT_, int msec);	//キーを押し続けているか、キーコードで判断する
BOOL MY_KEY_PUSH(int KEY_INPUT_);				//キーをプッシュしたか、キーコードで判断する

BOOL MY_FONT_INSTALL_ONCE(VOID);	//フォントをこのソフト用に、一時的にインストール
VOID MY_FONT_UNINSTALL_ONCE(VOID);	//フォントをこのソフト用に、一時的にアンインストール

//########## プログラムで最初に実行される関数 ##########
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);				//Log.txtを出力しない
	ChangeWindowMode(TRUE);								//ウィンドウモードに設定
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//指定の数値でウィンドウを表示する
	SetWindowStyleMode(GAME_WINDOW_BAR);				//タイトルバーはデフォルトにする
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));			//ウィンドウのタイトルの文字
	SetAlwaysRunFlag(TRUE);								//非アクティブでも実行する
	SetWindowIconID(GAME_ICON_ID);						//アイコンファイルを読込

	if (DxLib_Init() == -1) { return -1; }	//ＤＸライブラリ初期化処理

	//Draw系関数は裏画面に描画
	SetDrawScreen(DX_SCREEN_BACK);

	//フォントを一時的にインストール
	if (MY_FONT_INSTALL_ONCE() == FALSE) { return -1; }

	//ゲームシーンはスタート画面から
	GameScene = GAME_SCENE_START;

	//文字にアンチエイリアスをかける
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_8X8);

	//無限ループ
	while (TRUE)
	{
		if (ProcessMessage() != 0) { break; }	//メッセージ処理の結果がエラーのとき、強制終了
		if (ClearDrawScreen() != 0) { break; }	//画面を消去できなかったとき、強制終了

		MY_ALL_KEYDOWN_UPDATE();				//押しているキー状態を取得

		MY_FPS_UPDATE();	//FPSの処理[更新]

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_START:
			MY_START();	//スタート画面
			break;
		case GAME_SCENE_PLAY:
			MY_PLAY();	//プレイ画面
			break;
		case GAME_SCENE_END:
			MY_END();	//エンド画面
			break;
		}

		MY_FPS_DRAW();		//FPSの処理[描画]

		ScreenFlip();		//モニタのリフレッシュレートの速さで裏画面を再描画

		MY_FPS_WAIT();		//FPSの処理[待つ]
	}

	//一時的にインストールしたフォントをアンインストール
	MY_FONT_UNINSTALL_ONCE();

	DxLib_End();	//ＤＸライブラリ使用の終了処理

	return 0;
}

//スタート画面
VOID MY_START(VOID)
{
	MY_START_PROC();	//スタート画面の処理
	MY_START_DRAW();	//スタート画面の描画

	return;
}

//スタート画面の処理
VOID MY_START_PROC(VOID)
{
	//スペースキーを押したら、プレイシーンへ移動する
	if (MY_KEY_PUSH(KEY_INPUT_SPACE) == TRUE)
	{
		//プレイ画面を初期化
		MY_PLAY_INIT();

		//プレイ画面へ
		GameScene = GAME_SCENE_PLAY;
	}

	return;
}

//スタート画面の描画
VOID MY_START_DRAW(VOID)
{

	DrawString(0, 0, "スタート画面(エンターキーを押して下さい)", GetColor(255, 255, 255));
	return;
}

//プレイ画面(初期化)
VOID MY_PLAY_INIT(VOID)
{
	return;
}

//プレイ画面
VOID MY_PLAY(VOID)
{
	MY_PLAY_PROC();	//プレイ画面の処理
	MY_PLAY_DRAW();	//プレイ画面の描画

	return;
}

//プレイ画面の処理
VOID MY_PLAY_PROC(VOID)
{
	//スペースキーを押したら、スタートシーンへ移動する
	if (MY_KEY_PUSH(KEY_INPUT_SPACE) == TRUE)
	{
		GameScene = GAME_SCENE_END;

		return;
	}

	return;
}

//プレイ画面の描画
VOID MY_PLAY_DRAW(VOID)
{

	DrawString(0, 0, "プレイ画面(スペースキーを押して下さい)", GetColor(255, 255, 255));
	return;
}

//エンド画面
VOID MY_END(VOID)
{
	MY_END_PROC();	//エンド画面の処理
	MY_END_DRAW();	//エンド画面の描画
	return;
}

//エンド画面の処理
VOID MY_END_PROC(VOID)
{
	//スペースキーを押したら、スタートシーンへ移動する
	if (MY_KEY_PUSH(KEY_INPUT_SPACE) == TRUE)
	{
		GameScene = GAME_SCENE_START;
		return;
	}

	return;
}

//エンド画面の描画
VOID MY_END_DRAW(VOID)
{

	DrawString(0, 0, "エンド画面(スペースキーを押して下さい)", GetColor(255, 255, 255));
	return;
}

//########## FPS値を計測、更新する関数 ##########
VOID MY_FPS_UPDATE(VOID)
{
	if (CountFps == 0) //1フレーム目なら時刻を記憶
	{
		StartTimeFps = GetNowCount();
	}

	if (CountFps == GAME_FPS) //60フレーム目なら平均を計算
	{
		int now = GetNowCount();
		//現在の時間から、0フレーム目の時間を引き、FPSの数値で割る＝1FPS辺りの平均時間が計算される
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)GAME_FPS);
		CountFps = 0;
		StartTimeFps = now;
	}
	CountFps++;
	return;
}

//########## FPS値を描画する関数 ##########
VOID MY_FPS_DRAW(VOID)
{
	//文字列を描画
	DrawFormatString(0, GAME_HEIGHT - 20, GetColor(255, 255, 255), "FPS:%.1f", CalcFps);
	return;
}

//########## FPS値を計測し、待つ関数 ##########
VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;					//かかった時間
	int waitTime = CountFps * 1000 / GAME_FPS - resultTime;	//待つべき時間

	if (waitTime > 0)		//指定のFPS値よりも早い場合
	{
		WaitTimer(waitTime);	//待つ
	}
	return;
}

//########## キーの入力状態を更新する関数 ##########
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{
	//参考：https://dxlib.xsrv.jp/function/dxfunc_input.html

	char TempKey[KEY_MAX];	//一時的に、現在のキーの入力状態を格納する

	//直前のキー入力をとっておく
	for (int i = 0; i < KEY_MAX; i++)
	{
		OldAllKeyState[i] = AllKeyState[i];
	}

	GetHitKeyStateAll(TempKey); // 全てのキーの入力状態を得る

	for (int i = 0; i < KEY_MAX; i++)
	{
		if (TempKey[i] != 0)	//押されているキーのキーコードを押しているとき
		{
			AllKeyState[i]++;	//押されている
		}
		else
		{
			AllKeyState[i] = 0;	//押されていない
		}
	}
	return;
}

//キーを押しているか、キーコードで判断する
//引　数：int：キーコード：KEY_INPUT_???
BOOL MY_KEY_DOWN(int KEY_INPUT_)
{
	//キーコードのキーを押している時
	if (AllKeyState[KEY_INPUT_] != 0)
	{
		return TRUE;	//キーを押している
	}
	else
	{
		return FALSE;	//キーを押していない
	}
}

//キーを押し上げたか、キーコードで判断する
//引　数：int：キーコード：KEY_INPUT_???
BOOL MY_KEY_UP(int KEY_INPUT_)
{
	if (OldAllKeyState[KEY_INPUT_] >= 1	//直前は押していて
		&& AllKeyState[KEY_INPUT_] == 0)	//今は押していないとき
	{
		return TRUE;	//キーを押し上げている
	}
	else
	{
		return FALSE;	//キーを押し上げていない
	}
}

//キーをプッシュしたか、キーコードで判断する
//引　数：int：キーコード：KEY_INPUT_???
BOOL MY_KEY_PUSH(int KEY_INPUT_)
{
	if (OldAllKeyState[KEY_INPUT_] == 0		//直前は押していなくて
		&& AllKeyState[KEY_INPUT_] >= 1)	//今は押しているとき
	{
		return TRUE;	//キーをプッシュした（押し続けても、１回のみ発生）
	}
	else
	{
		return FALSE;	//キーをプッシュしていないか、押し続けている
	}
}

//キーを押し続けているか、キーコードで判断する
//引　数：int：キーコード：KEY_INPUT_???
//引　数：int：キーを押し続ける時間(ミリ秒)
BOOL MY_KEYDOWN_KEEP(int KEY_INPUT_, int milliTime)
{
	int MilliSec = 1000;	//１秒は1000ミリ秒

	//押し続ける時間=秒数×FPS値
	//例）60FPSのゲームで、1秒間押し続けるなら、1秒×60FPS
	int UpdateTime = (milliTime / MilliSec) * GAME_FPS;

	if (AllKeyState[KEY_INPUT_] > UpdateTime)
	{
		return TRUE;	//押し続けている
	}
	else
	{
		return FALSE;	//押し続けていない
	}
}

//領域の当たり判定をする関数
BOOL MY_CHECK_RECT_COLL(RECT a, RECT b)
{
	if (a.left < b.right &&
		a.top < b.bottom &&
		a.right > b.left &&
		a.bottom > b.top
		)
	{
		return TRUE;	//当たっている
	}

	return FALSE;		//当たっていない
}

//フォントをこのソフト用に、一時的にインストール
BOOL MY_FONT_INSTALL_ONCE(VOID)
{
	//フォントを一時的に読み込み(WinAPI)
	if (AddFontResourceEx(FONT_TANU_PATH, FR_PRIVATE, NULL) == 0)
	{
		//エラーメッセージ表示
		MessageBox(GetMainWindowHandle(), FONT_TANU_NAME, FONT_INSTALL_ERR_TITLE, MB_OK);
		return FALSE;
	}

	return TRUE;
}

//フォントをこのソフト用に、一時的にアンインストール
VOID MY_FONT_UNINSTALL_ONCE(VOID)
{
	//一時的に読み込んだフォントを削除(WinAPI)
	RemoveFontResourceEx(FONT_TANU_PATH, FR_PRIVATE, NULL);

	return;
}