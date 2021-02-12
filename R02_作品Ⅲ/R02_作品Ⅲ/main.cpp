#define _CRT_SECURE_NO_WARNINGS

//ヘッダーファイル読み込み
#include "DxLib.h"
#include "math.h"

//マクロ定義
#define GAME_WIDTH			800	//画面の横の大きさ
#define GAME_HEIGHT			600	//画面の縦の大きさ
#define GAME_COLOR			32	//画面のカラービット

#define GAME_WINDOW_BAR		0				//タイトルバーはデフォルトにする
#define GAME_WINDOW_NAME	"ブロック崩し"	//ウィンドウのタイトル
#define GAME_FPS			60				//FPSの数値	

#define GAME_ICON_ID		333				//ゲームのアイコンID

//パスの長さ
#define PATH_MAX			255	//255文字まで
#define NAME_MAX			255	//255文字まで

//キーボードの種類
#define KEY_MAX				256

//#define STR_GAME_OVER		TEXT("GAME OVER")

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

//バーの下からの高さ
#define BAR_UNDER_HEIGHT	100

//ブロックの数
#define BLOCK_TATE_MAX	6
#define BLOCK_YOKO_MAX	10

//ブロックの一番下の位置
#define BLOCK_UNDER_HEIGHT	(GAME_HEIGHT - BAR_UNDER_HEIGHT - 200)

#define BLOCK_WIDTH		(GAME_WIDTH / BLOCK_YOKO_MAX)						//ブロックの幅
#define BLOCK_HEIGHT	(BLOCK_UNDER_HEIGHT / BLOCK_TATE_MAX)				//ブロックの高さ

//列挙型
enum GAME_SCENE {
	GAME_SCENE_START,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
};	//ゲームのシーン

enum BLOCK_KIND {
	N,	//なにもない
	R,	//赤
	G,	//緑
	B,	//青
	Y,	//黄
	P,	//ピンク
	W	//白
};

//構造体の宣言
typedef struct STRUCT_IMAGE
{
	char path[PATH_MAX];		//パス
	int handle;					//ハンドル
	int x;						//X位置
	int y;						//Y位置
	int width;					//幅
	int height;					//高さ
}IMAGE;	//画像構造体

typedef struct STRUCT_BAR
{
	int x;
	int y;
	int width;
	int height;
	int speed;
}BAR;	//バー構造体

typedef struct STRCUT_BALL
{
	double centerX;	//中心X位置
	double centerY;	//中心Y位置
	int hankei;		//半径
	int speed;		//速度
	int Angle;		//角度
}BALL;

// グローバル変数
//FPS関連
int StartTimeFps;				//測定開始時刻
int CountFps;					//カウンタ
float CalcFps;					//計算結果

//キーボードの入力を取得
char AllKeyState[KEY_MAX];		//すべてのキーの状態が入る
char OldAllKeyState[KEY_MAX];	//すべてのキーの状態(直前)が入る

//ゲーム関連
GAME_SCENE GameScene;			//ゲームのシーンを管理

//ボール
BALL ball;

//バー
BAR bar;

//ゲームスコア
int GameTokuten = 0;

//デフォルトのフォントサイズ
int DefaultFontSize = 0;

//ブロック
BLOCK_KIND blockKind[BLOCK_TATE_MAX][BLOCK_YOKO_MAX]
{
	{R,R,R,R,R,R,R,R,R,R},
	{G,G,G,G,G,G,G,G,G,G},
	{B,B,B,B,B,B,B,B,B,B},
	{Y,Y,Y,Y,Y,Y,Y,Y,Y,Y},
	{P,P,P,P,P,P,P,P,P,P},
	{W,W,W,W,W,W,W,W,W,W},
};

//ブロックの当たり判定
RECT blockColl[BLOCK_TATE_MAX][BLOCK_YOKO_MAX];

//スタートボタンを押したか
BOOL IsStart = FALSE;

BOOL IsOldColl = FALSE; //直前に当たっていたか
BOOL IsNewColl = FALSE; //現在に当たっていたか

//外部のプロトタイプ宣言
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

void DrawBoxRect(RECT r, unsigned int color, bool b);			//RECTを利用して四角を描画

BOOL MY_FONT_INSTALL_ONCE(VOID);	//フォントをこのソフト用に、一時的にインストール
VOID MY_FONT_UNINSTALL_ONCE(VOID);	//フォントをこのソフト用に、一時的にアンインストール

VOID MY_CHECK_BAR_BALL(VOID);			//ボールとバーの当たり判定
BOOL MY_CHECK_BAR_BALL_COLL(VOID);		//ボールとバーの当たり判定(処理)

VOID MY_CHECK_BALL_BLOCK(VOID);			//ボールとブロックの当たり判定
BOOL MY_CHECK_BALL_BLOCK_COLL(RECT r);	//ボールとブロックの当たり判定(処理)

//プログラムで最初に実行される関数
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

	//デフォルトのフォントサイズを取得
	DefaultFontSize = GetFontSize();

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
	//エンターキーを押したら、プレイシーンへ移動する
	if (MY_KEY_PUSH(KEY_INPUT_RETURN) == TRUE)
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
	//ブロック初期化
	for (int tate = 0; tate < BLOCK_TATE_MAX; tate++)
	{
		for (int yoko = 0; yoko < BLOCK_YOKO_MAX; yoko++)
		{
			//ブロックの当たり判定領域を設定
			blockColl[tate][yoko].left = (yoko + 0) * BLOCK_WIDTH + 1;
			blockColl[tate][yoko].top = (tate + 0) * BLOCK_HEIGHT + 1;
			blockColl[tate][yoko].right = (yoko + 1) * BLOCK_WIDTH - 1;
			blockColl[tate][yoko].bottom = (tate + 1) * BLOCK_HEIGHT - 1;
		}
	}

	//バー初期化
	bar.width = 100;	//バーの幅
	bar.height = 25;	//バーの高さ
	bar.speed = 4;		//バーの速さ
	bar.x = GAME_WIDTH / 2 - bar.width / 2;	//バーの初期位置は画面の中心
	bar.y = GAME_HEIGHT - BAR_UNDER_HEIGHT;	//バーの初期位置は画面の下寄り

	//ボール初期化
	ball.hankei = 20;
	ball.centerX = GAME_WIDTH / 2;
	ball.centerY = BLOCK_UNDER_HEIGHT + ball.hankei;
	ball.speed = 2;
	ball.Angle = 150;	//デフォルトは下向き

	//ゲームスコア初期化
	GameTokuten = 0;

	IsOldColl = FALSE; //直前に当たっていたか
	IsNewColl = FALSE; //現在に当たっていたか

	//スタートボタンを押していない
	IsStart = FALSE;

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

	//エンターキーを押したらゲームスタート！
	if (IsStart == FALSE)
	{
		if (MY_KEY_PUSH(KEY_INPUT_RETURN) == TRUE)
		{
			IsStart = TRUE;
		}

		return;	//ゲームの処理をしない
	}

	//右キーを押したら、バーが右へ移動する
	if (MY_KEY_DOWN(KEY_INPUT_RIGHT) == TRUE)
	{
		//バーが画面外に行かないようにする
		if (bar.x + bar.width + bar.speed <= GAME_WIDTH)
		{
			bar.x += bar.speed;	//バーの位置を変える
		}
	}

	//左キーを押したら、バーが右へ移動する
	if (MY_KEY_DOWN(KEY_INPUT_LEFT) == TRUE)
	{
		//バーが画面外に行かないようにする
		if (bar.x - bar.speed >= 0)
		{
			bar.x -= bar.speed;	//バーの位置を変える
		}
	}

	//ボールの位置を変更
	//弧度(ラジアン)＝度×円周率(π)/１８０
	ball.centerX += cos(ball.Angle * DX_PI / 180.0) * ball.speed;
	ball.centerY += sin(ball.Angle * DX_PI / 180.0) * ball.speed;


	//左側
	if (ball.centerX - ball.hankei < 0)
	{
		ball.centerX = 0 + ball.hankei;			//位置を修正

		//向きを反転
		ball.Angle = 180 - ball.Angle;

		//if (ball.speed > 1) { ball.speed--; }	//スピードダウン
	}
	else if (ball.centerX + ball.hankei > GAME_WIDTH)	//右側
	{
		ball.centerX = GAME_WIDTH - ball.hankei;	//位置を修正

		//向きを反転
		ball.Angle = 180 - ball.Angle;

		//if (ball.speed > 1) { ball.speed--; }		//スピードダウン
	}
	else if (ball.centerY - ball.hankei < 0)	//上側
	{
		ball.centerY = 0 + ball.hankei;			//位置を修正

		//向きを反転
		ball.Angle = 360 - ball.Angle;

		//if (ball.speed > 1) { ball.speed--; }	//スピードダウン
	}
	else if (ball.centerY + ball.hankei > GAME_HEIGHT)	//下側
	{
		ball.centerY = GAME_HEIGHT - ball.hankei;	//位置を修正

		//向きを反転
		ball.Angle = 360 - ball.Angle;

		//if (ball.speed > 1) { ball.speed--; }		//スピードダウン
	}

	//ボールとバーの当たり判定
	MY_CHECK_BAR_BALL();

	//ボールとブロックの当たり判定
	MY_CHECK_BALL_BLOCK();

	return;
}

//ボールとバーの当たり判定
VOID MY_CHECK_BAR_BALL(VOID)
{
	//直前の当たり判定情報を取得
	IsOldColl = IsNewColl;

	//ボールとバーの当たり判定
	if (MY_CHECK_BAR_BALL_COLL() == TRUE)
	{
		IsNewColl = TRUE;	//現在は当たっている

		//ボールがバーにめり込むと、ずっと反転処理をしてしまうので、
		//初めての当たり判定のときのみ、反転させたい
		if (IsOldColl == FALSE && IsNewColl == TRUE)
		{
			ball.Angle = -ball.Angle;	//向きを反転
			ball.speed += 2;				//スピードアップ
		}
	}
	else
	{
		IsNewColl = FALSE;	//現在は当たっていない
	}
	return;
}

//ボールとバーの当たり判定(処理)
BOOL MY_CHECK_BAR_BALL_COLL(VOID)
{

	//バーの四角領域×２
	RECT sikaku_a;	//領域A（縦長）の四角
	RECT sikaku_b;	//領域B（横長）の四角

	//領域A（縦長）の四角
	sikaku_a.left = bar.x;
	sikaku_a.top = bar.y - ball.hankei;
	sikaku_a.right = bar.x + bar.width;
	sikaku_a.bottom = bar.y + bar.height + ball.hankei;

	//領域A（縦長）の当たり判定
	if (sikaku_a.left < ball.centerX &&
		sikaku_a.top <  ball.centerY &&
		sikaku_a.right >  ball.centerX &&
		sikaku_a.bottom >  ball.centerY)
	{
		return TRUE;
	}

	//領域B（横長）の四角
	sikaku_b.left = bar.x - ball.hankei;
	sikaku_b.top = bar.y;
	sikaku_b.right = bar.x + bar.width + ball.hankei;
	sikaku_b.bottom = bar.y + bar.height;

	//領域B（横長）の当たり判定
	if (sikaku_b.left < ball.centerX &&
		sikaku_b.top <  ball.centerY &&
		sikaku_b.right >  ball.centerX &&
		sikaku_b.bottom >  ball.centerY)
	{
		return TRUE;
	}

	//三平方の定理
	int a;
	int b;
	int c;

	//円領域１(左上)
	int en1_x = bar.x;
	int en1_y = bar.y;

	//円領域１との当たり判定
	a = en1_x - ball.centerX;
	b = en1_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//円領域１の中心からボール中心の距離が、円領域１の半径＋ボールの半径よりも短いとき
	if (c <= ball.hankei) { return TRUE; }

	//円領域２（左下）
	int en2_x = bar.x;
	int en2_y = bar.y + bar.height;

	//円領域２との当たり判定
	a = en2_x - ball.centerX;
	b = en2_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//円領域２の中心からボール中心の距離が、円領域２の半径＋ボールの半径よりも短いとき
	if (c <= ball.hankei) { return TRUE; }

	//円領域３（右上）
	int en3_x = bar.x + bar.width;
	int en3_y = bar.y;

	//円領域３との当たり判定
	a = en3_x - ball.centerX;
	b = en3_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//円領域３の中心からボール中心の距離が、円領域３の半径＋ボールの半径よりも短いとき
	if (c <= ball.hankei) { return TRUE; }

	//円領域４（右下）
	int en4_x = bar.x + bar.width;
	int en4_y = bar.y;

	//円領域４との当たり判定
	a = en4_x - ball.centerX;
	b = en4_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//円領域４の中心からボール中心の距離が、円領域４の半径＋ボールの半径よりも短いとき
	if (c <= ball.hankei) { return TRUE; }

	return FALSE;
}

//ボールとブロックの当たり判定
VOID MY_CHECK_BALL_BLOCK(VOID)
{
	for (int tate = 0; tate < BLOCK_TATE_MAX; tate++)
	{
		for (int yoko = 0; yoko < BLOCK_YOKO_MAX; yoko++)
		{
			//ブロックがあるとき
			if (blockKind[tate][yoko] != N)
			{
				//ボールとブロックの当たり判定
				if (MY_CHECK_BALL_BLOCK_COLL(blockColl[tate][yoko]) == TRUE)
				{
					ball.Angle = -ball.Angle;	//向きを反転

					ball.speed++;		//スピードアップ

					//該当するブロックを消す
					blockKind[tate][yoko] = N;

					return;
				}
			}
		}
	}
	return;
}

//ボールとバーの当たり判定(処理)
BOOL MY_CHECK_BALL_BLOCK_COLL(RECT r)
{

	//バーの四角領域×２
	RECT sikaku_a;	//領域A（縦長）の四角
	RECT sikaku_b;	//領域B（横長）の四角

	//領域A（縦長）の四角
	sikaku_a.left = r.left;
	sikaku_a.top = r.top - ball.hankei;
	sikaku_a.right = r.right;
	sikaku_a.bottom = r.bottom + ball.hankei;

	//領域A（縦長）の当たり判定
	if (sikaku_a.left < ball.centerX &&
		sikaku_a.top <  ball.centerY &&
		sikaku_a.right >  ball.centerX &&
		sikaku_a.bottom >  ball.centerY)
	{
		return TRUE;
	}

	//領域B（横長）の四角
	sikaku_b.left = r.left - ball.hankei;
	sikaku_b.top = r.top;
	sikaku_b.right = r.right + ball.hankei;
	sikaku_b.bottom = r.bottom;

	//領域B（横長）の当たり判定
	if (sikaku_b.left < ball.centerX &&
		sikaku_b.top <  ball.centerY &&
		sikaku_b.right >  ball.centerX &&
		sikaku_b.bottom >  ball.centerY)
	{
		return TRUE;
	}

	//三平方の定理
	int a;
	int b;
	int c;

	//円領域１(左上)
	int en1_x = r.left;
	int en1_y = r.top;

	//円領域１との当たり判定
	a = en1_x - ball.centerX;
	b = en1_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//円領域１の中心からボール中心の距離が、円領域１の半径＋ボールの半径よりも短いとき
	if (c <= ball.hankei) { return TRUE; }

	//円領域２（左下）
	int en2_x = r.left;
	int en2_y = r.bottom;

	//円領域２との当たり判定
	a = en2_x - ball.centerX;
	b = en2_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//円領域２の中心からボール中心の距離が、円領域２の半径＋ボールの半径よりも短いとき
	if (c <= ball.hankei) { return TRUE; }

	//円領域３（右上）
	int en3_x = r.right;
	int en3_y = r.top;

	//円領域３との当たり判定
	a = en3_x - ball.centerX;
	b = en3_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//円領域３の中心からボール中心の距離が、円領域３の半径＋ボールの半径よりも短いとき
	if (c <= ball.hankei) { return TRUE; }

	//円領域４（右下）
	int en4_x = r.right;
	int en4_y = r.bottom;

	//円領域４との当たり判定
	a = en4_x - ball.centerX;
	b = en4_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//円領域４の中心からボール中心の距離が、円領域４の半径＋ボールの半径よりも短いとき
	if (c <= ball.hankei) { return TRUE; }

	return FALSE;
}

//プレイ画面の描画
VOID MY_PLAY_DRAW(VOID)
{
	//ブロック描画
	for (int tate = 0; tate < BLOCK_TATE_MAX; tate++)
	{
		for (int yoko = 0; yoko < BLOCK_YOKO_MAX; yoko++)
		{
			switch (blockKind[tate][yoko])
			{
			case R:
				DrawBox(
					blockColl[tate][yoko].left,
					blockColl[tate][yoko].top,
					blockColl[tate][yoko].right,
					blockColl[tate][yoko].bottom,
					GetColor(255, 0, 0),
					TRUE);
				break;

			case G:
				DrawBox(
					blockColl[tate][yoko].left,
					blockColl[tate][yoko].top,
					blockColl[tate][yoko].right,
					blockColl[tate][yoko].bottom,
					GetColor(0, 255, 0),
					TRUE);
				break;

			case B:
				DrawBox(
					blockColl[tate][yoko].left,
					blockColl[tate][yoko].top,
					blockColl[tate][yoko].right,
					blockColl[tate][yoko].bottom,
					GetColor(0, 0, 255),
					TRUE);
				break;

			case Y:
				DrawBox(
					blockColl[tate][yoko].left,
					blockColl[tate][yoko].top,
					blockColl[tate][yoko].right,
					blockColl[tate][yoko].bottom,
					GetColor(255, 255, 0),
					TRUE);
				break;

			case P:
				DrawBox(
					blockColl[tate][yoko].left,
					blockColl[tate][yoko].top,
					blockColl[tate][yoko].right,
					blockColl[tate][yoko].bottom,
					GetColor(255, 0, 255),
					TRUE);
				break;

			case W:
				DrawBox(
					blockColl[tate][yoko].left,
					blockColl[tate][yoko].top,
					blockColl[tate][yoko].right,
					blockColl[tate][yoko].bottom,
					GetColor(255, 255, 255),
					TRUE);
				break;

			default:
				break;
			}
		}
	}


	//バーの描画
	DrawBox(
		bar.x,
		bar.y,
		bar.x + bar.width,
		bar.y + bar.height,
		GetColor(0, 255, 255),
		TRUE);

	//ボールの描画
	DrawCircle(ball.centerX, ball.centerY, ball.hankei, GetColor(255, 165, 0), TRUE);

	//ゲームスコア描画
	DrawFormatString(0, 20, GetColor(255, 255, 255), "SCORE:%05d", GameTokuten);

	DrawFormatString(352, 300, GetColor(255, 0, 0), "GameOver", GameTokuten);
    


	//スタートしていないとき
	if (IsStart == FALSE)
	{
		//フォントサイズ変更
		SetFontSize(100);
		DrawString(
			0,
			GAME_HEIGHT / 2,
			"PUSH ENTER", GetColor(255, 255, 255));
		SetFontSize(DefaultFontSize);
	}

	DrawString(0, 0, "プレイ画面(スペースキーを押して下さい)", GetColor(255, 255, 255));

	DrawFormatString(0, 40, GetColor(255, 255, 255), "Angle:%3d", ball.Angle);
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

//画像をまとめて削除する関数
VOID MY_DELETE_IMAGE(VOID)
{
	return;
}

//RECTを利用して四角を描画
void DrawBoxRect(RECT r, unsigned int color, bool b)
{
	//引数を基に描画
	DrawBox(r.left, r.top, r.right, r.bottom, color, b);
	return;
}

// FPS値を計測、更新する関数 
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

// FPS値を描画する関数 
VOID MY_FPS_DRAW(VOID)
{
	//文字列を描画
	DrawFormatString(0, GAME_HEIGHT - 20, GetColor(255, 255, 255), "FPS:%.1f", CalcFps);
	return;
}

// FPS値を計測し、待つ関数
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

// キーの入力状態を更新する関数 
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{

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