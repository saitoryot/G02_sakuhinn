#define _CRT_SECURE_NO_WARNINGS

//�w�b�_�[�t�@�C���ǂݍ���
#include "DxLib.h"
#include "math.h"

//�}�N����`
#define GAME_WIDTH			800	//��ʂ̉��̑傫��
#define GAME_HEIGHT			600	//��ʂ̏c�̑傫��
#define GAME_COLOR			32	//��ʂ̃J���[�r�b�g

#define GAME_WINDOW_BAR		0				//�^�C�g���o�[�̓f�t�H���g�ɂ���
#define GAME_WINDOW_NAME	"�u���b�N����"	//�E�B���h�E�̃^�C�g��
#define GAME_FPS			60				//FPS�̐��l	

#define GAME_ICON_ID		333				//�Q�[���̃A�C�R��ID

//�p�X�̒���
#define PATH_MAX			255	//255�����܂�
#define NAME_MAX			255	//255�����܂�

//�L�[�{�[�h�̎��
#define KEY_MAX				256

//#define STR_GAME_OVER		TEXT("GAME OVER")

//�G���[���b�Z�[�W
#define IMAGE_LOAD_ERR_TITLE	TEXT("�摜�ǂݍ��݃G���[")

//�I���_�C�A���O�p
#define MOUSE_R_CLICK_TITLE		TEXT("�Q�[�����f")
#define MOUSE_R_CLICK_CAPTION	TEXT("�Q�[���𒆒f���A�^�C�g����ʂɖ߂�܂����H")

//�t�H���g
#define FONT_TANU_PATH			TEXT(".\\TanukiMagic.ttf")			//�t�H���g�̏ꏊ
#define FONT_TANU_NAME			TEXT("���ʂ������}�W�b�N")			//�t�H���g�̖��O

//�G���[���b�Z�[�W
#define FONT_INSTALL_ERR_TITLE	TEXT("�t�H���g�C���X�g�[���G���[")
#define FONT_CREATE_ERR_TITLE	TEXT("�t�H���g�쐬�G���[")

//�o�[�̉�����̍���
#define BAR_UNDER_HEIGHT	100

//�u���b�N�̐�
#define BLOCK_TATE_MAX	6
#define BLOCK_YOKO_MAX	10

//�u���b�N�̈�ԉ��̈ʒu
#define BLOCK_UNDER_HEIGHT	(GAME_HEIGHT - BAR_UNDER_HEIGHT - 200)

#define BLOCK_WIDTH		(GAME_WIDTH / BLOCK_YOKO_MAX)						//�u���b�N�̕�
#define BLOCK_HEIGHT	(BLOCK_UNDER_HEIGHT / BLOCK_TATE_MAX)				//�u���b�N�̍���

//�񋓌^
enum GAME_SCENE {
	GAME_SCENE_START,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
};	//�Q�[���̃V�[��

enum BLOCK_KIND {
	N,	//�Ȃɂ��Ȃ�
	R,	//��
	G,	//��
	B,	//��
	Y,	//��
	P,	//�s���N
	W	//��
};

//�\���̂̐錾
typedef struct STRUCT_IMAGE
{
	char path[PATH_MAX];		//�p�X
	int handle;					//�n���h��
	int x;						//X�ʒu
	int y;						//Y�ʒu
	int width;					//��
	int height;					//����
}IMAGE;	//�摜�\����

typedef struct STRUCT_BAR
{
	int x;
	int y;
	int width;
	int height;
	int speed;
}BAR;	//�o�[�\����

typedef struct STRCUT_BALL
{
	double centerX;	//���SX�ʒu
	double centerY;	//���SY�ʒu
	int hankei;		//���a
	int speed;		//���x
	int Angle;		//�p�x
}BALL;

// �O���[�o���ϐ�
//FPS�֘A
int StartTimeFps;				//����J�n����
int CountFps;					//�J�E���^
float CalcFps;					//�v�Z����

//�L�[�{�[�h�̓��͂��擾
char AllKeyState[KEY_MAX];		//���ׂẴL�[�̏�Ԃ�����
char OldAllKeyState[KEY_MAX];	//���ׂẴL�[�̏��(���O)������

//�Q�[���֘A
GAME_SCENE GameScene;			//�Q�[���̃V�[�����Ǘ�

//�{�[��
BALL ball;

//�o�[
BAR bar;

//�Q�[���X�R�A
int GameTokuten = 0;

//�f�t�H���g�̃t�H���g�T�C�Y
int DefaultFontSize = 0;

//�u���b�N
BLOCK_KIND blockKind[BLOCK_TATE_MAX][BLOCK_YOKO_MAX]
{
	{R,R,R,R,R,R,R,R,R,R},
	{G,G,G,G,G,G,G,G,G,G},
	{B,B,B,B,B,B,B,B,B,B},
	{Y,Y,Y,Y,Y,Y,Y,Y,Y,Y},
	{P,P,P,P,P,P,P,P,P,P},
	{W,W,W,W,W,W,W,W,W,W},
};

//�u���b�N�̓����蔻��
RECT blockColl[BLOCK_TATE_MAX][BLOCK_YOKO_MAX];

//�X�^�[�g�{�^������������
BOOL IsStart = FALSE;

BOOL IsOldColl = FALSE; //���O�ɓ������Ă�����
BOOL IsNewColl = FALSE; //���݂ɓ������Ă�����

//�O���̃v���g�^�C�v�錾
VOID MY_FPS_UPDATE(VOID);	//FPS�l���v���A�X�V����
VOID MY_FPS_DRAW(VOID);		//FPS�l��`�悷��
VOID MY_FPS_WAIT(VOID);		//FPS�l���v�����A�҂�

VOID MY_START(VOID);		//�X�^�[�g���
VOID MY_START_PROC(VOID);	//�X�^�[�g���
VOID MY_START_DRAW(VOID);	//�X�^�[�g���
VOID MY_PLAY_INIT(VOID);	//�v���C���(������)
VOID MY_PLAY(VOID);			//�v���C���
VOID MY_PLAY_PROC(VOID);	//�v���C���
VOID MY_PLAY_DRAW(VOID);	//�v���C���
VOID MY_END(VOID);			//�G���h���
VOID MY_END_PROC(VOID);		//�G���h���
VOID MY_END_DRAW(VOID);		//�G���h���

VOID MY_ALL_KEYDOWN_UPDATE(VOID);				//�L�[�̓��͏�Ԃ��X�V����
BOOL MY_KEY_DOWN(int KEY_INPUT_);				//�L�[�������Ă��邩�A�L�[�R�[�h�Ŕ��f����
BOOL MY_KEY_UP(int KEY_INPUT_);					//�L�[�������グ�����A�L�[�R�[�h�Ŕ��f����
BOOL MY_KEYDOWN_KEEP(int KEY_INPUT_, int msec);	//�L�[�����������Ă��邩�A�L�[�R�[�h�Ŕ��f����
BOOL MY_KEY_PUSH(int KEY_INPUT_);				//�L�[���v�b�V���������A�L�[�R�[�h�Ŕ��f����

void DrawBoxRect(RECT r, unsigned int color, bool b);			//RECT�𗘗p���Ďl�p��`��

BOOL MY_FONT_INSTALL_ONCE(VOID);	//�t�H���g�����̃\�t�g�p�ɁA�ꎞ�I�ɃC���X�g�[��
VOID MY_FONT_UNINSTALL_ONCE(VOID);	//�t�H���g�����̃\�t�g�p�ɁA�ꎞ�I�ɃA���C���X�g�[��

VOID MY_CHECK_BAR_BALL(VOID);			//�{�[���ƃo�[�̓����蔻��
BOOL MY_CHECK_BAR_BALL_COLL(VOID);		//�{�[���ƃo�[�̓����蔻��(����)

VOID MY_CHECK_BALL_BLOCK(VOID);			//�{�[���ƃu���b�N�̓����蔻��
BOOL MY_CHECK_BALL_BLOCK_COLL(RECT r);	//�{�[���ƃu���b�N�̓����蔻��(����)

//�v���O�����ōŏ��Ɏ��s�����֐�
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);				//Log.txt���o�͂��Ȃ�
	ChangeWindowMode(TRUE);								//�E�B���h�E���[�h�ɐݒ�
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);	//�w��̐��l�ŃE�B���h�E��\������
	SetWindowStyleMode(GAME_WINDOW_BAR);				//�^�C�g���o�[�̓f�t�H���g�ɂ���
	SetMainWindowText(TEXT(GAME_WINDOW_NAME));			//�E�B���h�E�̃^�C�g���̕���
	SetAlwaysRunFlag(TRUE);								//��A�N�e�B�u�ł����s����
	SetWindowIconID(GAME_ICON_ID);						//�A�C�R���t�@�C����Ǎ�

	if (DxLib_Init() == -1) { return -1; }	//�c�w���C�u��������������

	//Draw�n�֐��͗���ʂɕ`��
	SetDrawScreen(DX_SCREEN_BACK);

	//�t�H���g���ꎞ�I�ɃC���X�g�[��
	if (MY_FONT_INSTALL_ONCE() == FALSE) { return -1; }

	//�Q�[���V�[���̓X�^�[�g��ʂ���
	GameScene = GAME_SCENE_START;

	//�����ɃA���`�G�C���A�X��������
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_8X8);

	//�f�t�H���g�̃t�H���g�T�C�Y���擾
	DefaultFontSize = GetFontSize();

	//�������[�v
	while (TRUE)
	{
		if (ProcessMessage() != 0) { break; }	//���b�Z�[�W�����̌��ʂ��G���[�̂Ƃ��A�����I��
		if (ClearDrawScreen() != 0) { break; }	//��ʂ������ł��Ȃ������Ƃ��A�����I��

		MY_ALL_KEYDOWN_UPDATE();				//�����Ă���L�[��Ԃ��擾

		MY_FPS_UPDATE();	//FPS�̏���[�X�V]

		//�V�[�����Ƃɏ������s��
		switch (GameScene)
		{
		case GAME_SCENE_START:
			MY_START();	//�X�^�[�g���
			break;
		case GAME_SCENE_PLAY:
			MY_PLAY();	//�v���C���
			break;
		case GAME_SCENE_END:
			MY_END();	//�G���h���
			break;
		}

		MY_FPS_DRAW();		//FPS�̏���[�`��]

		ScreenFlip();		//���j�^�̃��t���b�V�����[�g�̑����ŗ���ʂ��ĕ`��

		MY_FPS_WAIT();		//FPS�̏���[�҂�]
	}

	//�ꎞ�I�ɃC���X�g�[�������t�H���g���A���C���X�g�[��
	MY_FONT_UNINSTALL_ONCE();

	DxLib_End();	//�c�w���C�u�����g�p�̏I������

	return 0;
}

//�X�^�[�g���
VOID MY_START(VOID)
{
	MY_START_PROC();	//�X�^�[�g��ʂ̏���
	MY_START_DRAW();	//�X�^�[�g��ʂ̕`��

	return;
}

//�X�^�[�g��ʂ̏���
VOID MY_START_PROC(VOID)
{
	//�G���^�[�L�[����������A�v���C�V�[���ֈړ�����
	if (MY_KEY_PUSH(KEY_INPUT_RETURN) == TRUE)
	{
		//�v���C��ʂ�������
		MY_PLAY_INIT();

		//�v���C��ʂ�
		GameScene = GAME_SCENE_PLAY;
	}

	return;
}

//�X�^�[�g��ʂ̕`��
VOID MY_START_DRAW(VOID)
{

	DrawString(0, 0, "�X�^�[�g���(�G���^�[�L�[�������ĉ�����)", GetColor(255, 255, 255));
	return;
}

//�v���C���(������)
VOID MY_PLAY_INIT(VOID)
{
	//�u���b�N������
	for (int tate = 0; tate < BLOCK_TATE_MAX; tate++)
	{
		for (int yoko = 0; yoko < BLOCK_YOKO_MAX; yoko++)
		{
			//�u���b�N�̓����蔻��̈��ݒ�
			blockColl[tate][yoko].left = (yoko + 0) * BLOCK_WIDTH + 1;
			blockColl[tate][yoko].top = (tate + 0) * BLOCK_HEIGHT + 1;
			blockColl[tate][yoko].right = (yoko + 1) * BLOCK_WIDTH - 1;
			blockColl[tate][yoko].bottom = (tate + 1) * BLOCK_HEIGHT - 1;
		}
	}

	//�o�[������
	bar.width = 100;	//�o�[�̕�
	bar.height = 25;	//�o�[�̍���
	bar.speed = 4;		//�o�[�̑���
	bar.x = GAME_WIDTH / 2 - bar.width / 2;	//�o�[�̏����ʒu�͉�ʂ̒��S
	bar.y = GAME_HEIGHT - BAR_UNDER_HEIGHT;	//�o�[�̏����ʒu�͉�ʂ̉����

	//�{�[��������
	ball.hankei = 20;
	ball.centerX = GAME_WIDTH / 2;
	ball.centerY = BLOCK_UNDER_HEIGHT + ball.hankei;
	ball.speed = 2;
	ball.Angle = 150;	//�f�t�H���g�͉�����

	//�Q�[���X�R�A������
	GameTokuten = 0;

	IsOldColl = FALSE; //���O�ɓ������Ă�����
	IsNewColl = FALSE; //���݂ɓ������Ă�����

	//�X�^�[�g�{�^���������Ă��Ȃ�
	IsStart = FALSE;

	return;
}

//�v���C���
VOID MY_PLAY(VOID)
{
	MY_PLAY_PROC();	//�v���C��ʂ̏���
	MY_PLAY_DRAW();	//�v���C��ʂ̕`��

	return;
}

//�v���C��ʂ̏���
VOID MY_PLAY_PROC(VOID)
{
	//�X�y�[�X�L�[����������A�X�^�[�g�V�[���ֈړ�����
	if (MY_KEY_PUSH(KEY_INPUT_SPACE) == TRUE)
	{
		GameScene = GAME_SCENE_END;

		return;
	}

	//�G���^�[�L�[����������Q�[���X�^�[�g�I
	if (IsStart == FALSE)
	{
		if (MY_KEY_PUSH(KEY_INPUT_RETURN) == TRUE)
		{
			IsStart = TRUE;
		}

		return;	//�Q�[���̏��������Ȃ�
	}

	//�E�L�[����������A�o�[���E�ֈړ�����
	if (MY_KEY_DOWN(KEY_INPUT_RIGHT) == TRUE)
	{
		//�o�[����ʊO�ɍs���Ȃ��悤�ɂ���
		if (bar.x + bar.width + bar.speed <= GAME_WIDTH)
		{
			bar.x += bar.speed;	//�o�[�̈ʒu��ς���
		}
	}

	//���L�[����������A�o�[���E�ֈړ�����
	if (MY_KEY_DOWN(KEY_INPUT_LEFT) == TRUE)
	{
		//�o�[����ʊO�ɍs���Ȃ��悤�ɂ���
		if (bar.x - bar.speed >= 0)
		{
			bar.x -= bar.speed;	//�o�[�̈ʒu��ς���
		}
	}

	//�{�[���̈ʒu��ύX
	//�ʓx(���W�A��)���x�~�~����(��)/�P�W�O
	ball.centerX += cos(ball.Angle * DX_PI / 180.0) * ball.speed;
	ball.centerY += sin(ball.Angle * DX_PI / 180.0) * ball.speed;


	//����
	if (ball.centerX - ball.hankei < 0)
	{
		ball.centerX = 0 + ball.hankei;			//�ʒu���C��

		//�����𔽓]
		ball.Angle = 180 - ball.Angle;

		//if (ball.speed > 1) { ball.speed--; }	//�X�s�[�h�_�E��
	}
	else if (ball.centerX + ball.hankei > GAME_WIDTH)	//�E��
	{
		ball.centerX = GAME_WIDTH - ball.hankei;	//�ʒu���C��

		//�����𔽓]
		ball.Angle = 180 - ball.Angle;

		//if (ball.speed > 1) { ball.speed--; }		//�X�s�[�h�_�E��
	}
	else if (ball.centerY - ball.hankei < 0)	//�㑤
	{
		ball.centerY = 0 + ball.hankei;			//�ʒu���C��

		//�����𔽓]
		ball.Angle = 360 - ball.Angle;

		//if (ball.speed > 1) { ball.speed--; }	//�X�s�[�h�_�E��
	}
	else if (ball.centerY + ball.hankei > GAME_HEIGHT)	//����
	{
		ball.centerY = GAME_HEIGHT - ball.hankei;	//�ʒu���C��

		//�����𔽓]
		ball.Angle = 360 - ball.Angle;

		//if (ball.speed > 1) { ball.speed--; }		//�X�s�[�h�_�E��
	}

	//�{�[���ƃo�[�̓����蔻��
	MY_CHECK_BAR_BALL();

	//�{�[���ƃu���b�N�̓����蔻��
	MY_CHECK_BALL_BLOCK();

	return;
}

//�{�[���ƃo�[�̓����蔻��
VOID MY_CHECK_BAR_BALL(VOID)
{
	//���O�̓����蔻������擾
	IsOldColl = IsNewColl;

	//�{�[���ƃo�[�̓����蔻��
	if (MY_CHECK_BAR_BALL_COLL() == TRUE)
	{
		IsNewColl = TRUE;	//���݂͓������Ă���

		//�{�[�����o�[�ɂ߂荞�ނƁA�����Ɣ��]���������Ă��܂��̂ŁA
		//���߂Ă̓����蔻��̂Ƃ��̂݁A���]��������
		if (IsOldColl == FALSE && IsNewColl == TRUE)
		{
			ball.Angle = -ball.Angle;	//�����𔽓]
			ball.speed += 2;				//�X�s�[�h�A�b�v
		}
	}
	else
	{
		IsNewColl = FALSE;	//���݂͓������Ă��Ȃ�
	}
	return;
}

//�{�[���ƃo�[�̓����蔻��(����)
BOOL MY_CHECK_BAR_BALL_COLL(VOID)
{

	//�o�[�̎l�p�̈�~�Q
	RECT sikaku_a;	//�̈�A�i�c���j�̎l�p
	RECT sikaku_b;	//�̈�B�i�����j�̎l�p

	//�̈�A�i�c���j�̎l�p
	sikaku_a.left = bar.x;
	sikaku_a.top = bar.y - ball.hankei;
	sikaku_a.right = bar.x + bar.width;
	sikaku_a.bottom = bar.y + bar.height + ball.hankei;

	//�̈�A�i�c���j�̓����蔻��
	if (sikaku_a.left < ball.centerX &&
		sikaku_a.top <  ball.centerY &&
		sikaku_a.right >  ball.centerX &&
		sikaku_a.bottom >  ball.centerY)
	{
		return TRUE;
	}

	//�̈�B�i�����j�̎l�p
	sikaku_b.left = bar.x - ball.hankei;
	sikaku_b.top = bar.y;
	sikaku_b.right = bar.x + bar.width + ball.hankei;
	sikaku_b.bottom = bar.y + bar.height;

	//�̈�B�i�����j�̓����蔻��
	if (sikaku_b.left < ball.centerX &&
		sikaku_b.top <  ball.centerY &&
		sikaku_b.right >  ball.centerX &&
		sikaku_b.bottom >  ball.centerY)
	{
		return TRUE;
	}

	//�O�����̒藝
	int a;
	int b;
	int c;

	//�~�̈�P(����)
	int en1_x = bar.x;
	int en1_y = bar.y;

	//�~�̈�P�Ƃ̓����蔻��
	a = en1_x - ball.centerX;
	b = en1_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//�~�̈�P�̒��S����{�[�����S�̋������A�~�̈�P�̔��a�{�{�[���̔��a�����Z���Ƃ�
	if (c <= ball.hankei) { return TRUE; }

	//�~�̈�Q�i�����j
	int en2_x = bar.x;
	int en2_y = bar.y + bar.height;

	//�~�̈�Q�Ƃ̓����蔻��
	a = en2_x - ball.centerX;
	b = en2_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//�~�̈�Q�̒��S����{�[�����S�̋������A�~�̈�Q�̔��a�{�{�[���̔��a�����Z���Ƃ�
	if (c <= ball.hankei) { return TRUE; }

	//�~�̈�R�i�E��j
	int en3_x = bar.x + bar.width;
	int en3_y = bar.y;

	//�~�̈�R�Ƃ̓����蔻��
	a = en3_x - ball.centerX;
	b = en3_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//�~�̈�R�̒��S����{�[�����S�̋������A�~�̈�R�̔��a�{�{�[���̔��a�����Z���Ƃ�
	if (c <= ball.hankei) { return TRUE; }

	//�~�̈�S�i�E���j
	int en4_x = bar.x + bar.width;
	int en4_y = bar.y;

	//�~�̈�S�Ƃ̓����蔻��
	a = en4_x - ball.centerX;
	b = en4_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//�~�̈�S�̒��S����{�[�����S�̋������A�~�̈�S�̔��a�{�{�[���̔��a�����Z���Ƃ�
	if (c <= ball.hankei) { return TRUE; }

	return FALSE;
}

//�{�[���ƃu���b�N�̓����蔻��
VOID MY_CHECK_BALL_BLOCK(VOID)
{
	for (int tate = 0; tate < BLOCK_TATE_MAX; tate++)
	{
		for (int yoko = 0; yoko < BLOCK_YOKO_MAX; yoko++)
		{
			//�u���b�N������Ƃ�
			if (blockKind[tate][yoko] != N)
			{
				//�{�[���ƃu���b�N�̓����蔻��
				if (MY_CHECK_BALL_BLOCK_COLL(blockColl[tate][yoko]) == TRUE)
				{
					ball.Angle = -ball.Angle;	//�����𔽓]

					ball.speed++;		//�X�s�[�h�A�b�v

					//�Y������u���b�N������
					blockKind[tate][yoko] = N;

					return;
				}
			}
		}
	}
	return;
}

//�{�[���ƃo�[�̓����蔻��(����)
BOOL MY_CHECK_BALL_BLOCK_COLL(RECT r)
{

	//�o�[�̎l�p�̈�~�Q
	RECT sikaku_a;	//�̈�A�i�c���j�̎l�p
	RECT sikaku_b;	//�̈�B�i�����j�̎l�p

	//�̈�A�i�c���j�̎l�p
	sikaku_a.left = r.left;
	sikaku_a.top = r.top - ball.hankei;
	sikaku_a.right = r.right;
	sikaku_a.bottom = r.bottom + ball.hankei;

	//�̈�A�i�c���j�̓����蔻��
	if (sikaku_a.left < ball.centerX &&
		sikaku_a.top <  ball.centerY &&
		sikaku_a.right >  ball.centerX &&
		sikaku_a.bottom >  ball.centerY)
	{
		return TRUE;
	}

	//�̈�B�i�����j�̎l�p
	sikaku_b.left = r.left - ball.hankei;
	sikaku_b.top = r.top;
	sikaku_b.right = r.right + ball.hankei;
	sikaku_b.bottom = r.bottom;

	//�̈�B�i�����j�̓����蔻��
	if (sikaku_b.left < ball.centerX &&
		sikaku_b.top <  ball.centerY &&
		sikaku_b.right >  ball.centerX &&
		sikaku_b.bottom >  ball.centerY)
	{
		return TRUE;
	}

	//�O�����̒藝
	int a;
	int b;
	int c;

	//�~�̈�P(����)
	int en1_x = r.left;
	int en1_y = r.top;

	//�~�̈�P�Ƃ̓����蔻��
	a = en1_x - ball.centerX;
	b = en1_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//�~�̈�P�̒��S����{�[�����S�̋������A�~�̈�P�̔��a�{�{�[���̔��a�����Z���Ƃ�
	if (c <= ball.hankei) { return TRUE; }

	//�~�̈�Q�i�����j
	int en2_x = r.left;
	int en2_y = r.bottom;

	//�~�̈�Q�Ƃ̓����蔻��
	a = en2_x - ball.centerX;
	b = en2_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//�~�̈�Q�̒��S����{�[�����S�̋������A�~�̈�Q�̔��a�{�{�[���̔��a�����Z���Ƃ�
	if (c <= ball.hankei) { return TRUE; }

	//�~�̈�R�i�E��j
	int en3_x = r.right;
	int en3_y = r.top;

	//�~�̈�R�Ƃ̓����蔻��
	a = en3_x - ball.centerX;
	b = en3_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//�~�̈�R�̒��S����{�[�����S�̋������A�~�̈�R�̔��a�{�{�[���̔��a�����Z���Ƃ�
	if (c <= ball.hankei) { return TRUE; }

	//�~�̈�S�i�E���j
	int en4_x = r.right;
	int en4_y = r.bottom;

	//�~�̈�S�Ƃ̓����蔻��
	a = en4_x - ball.centerX;
	b = en4_y - ball.centerY;
	c = pow(a, 2.0) + pow(b, 2.0);
	c = sqrt(c);

	//�~�̈�S�̒��S����{�[�����S�̋������A�~�̈�S�̔��a�{�{�[���̔��a�����Z���Ƃ�
	if (c <= ball.hankei) { return TRUE; }

	return FALSE;
}

//�v���C��ʂ̕`��
VOID MY_PLAY_DRAW(VOID)
{
	//�u���b�N�`��
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


	//�o�[�̕`��
	DrawBox(
		bar.x,
		bar.y,
		bar.x + bar.width,
		bar.y + bar.height,
		GetColor(0, 255, 255),
		TRUE);

	//�{�[���̕`��
	DrawCircle(ball.centerX, ball.centerY, ball.hankei, GetColor(255, 165, 0), TRUE);

	//�Q�[���X�R�A�`��
	DrawFormatString(0, 20, GetColor(255, 255, 255), "SCORE:%05d", GameTokuten);

	DrawFormatString(352, 300, GetColor(255, 0, 0), "GameOver", GameTokuten);
    


	//�X�^�[�g���Ă��Ȃ��Ƃ�
	if (IsStart == FALSE)
	{
		//�t�H���g�T�C�Y�ύX
		SetFontSize(100);
		DrawString(
			0,
			GAME_HEIGHT / 2,
			"PUSH ENTER", GetColor(255, 255, 255));
		SetFontSize(DefaultFontSize);
	}

	DrawString(0, 0, "�v���C���(�X�y�[�X�L�[�������ĉ�����)", GetColor(255, 255, 255));

	DrawFormatString(0, 40, GetColor(255, 255, 255), "Angle:%3d", ball.Angle);
	return;
}

//�G���h���
VOID MY_END(VOID)
{
	MY_END_PROC();	//�G���h��ʂ̏���
	MY_END_DRAW();	//�G���h��ʂ̕`��
	return;
}

//�G���h��ʂ̏���
VOID MY_END_PROC(VOID)
{
	//�X�y�[�X�L�[����������A�X�^�[�g�V�[���ֈړ�����
	if (MY_KEY_PUSH(KEY_INPUT_SPACE) == TRUE)
	{
		GameScene = GAME_SCENE_START;
		return;
	}

	return;
}

//�G���h��ʂ̕`��
VOID MY_END_DRAW(VOID)
{

	DrawString(0, 0, "�G���h���(�X�y�[�X�L�[�������ĉ�����)", GetColor(255, 255, 255));
	return;
}

//�摜���܂Ƃ߂č폜����֐�
VOID MY_DELETE_IMAGE(VOID)
{
	return;
}

//RECT�𗘗p���Ďl�p��`��
void DrawBoxRect(RECT r, unsigned int color, bool b)
{
	//��������ɕ`��
	DrawBox(r.left, r.top, r.right, r.bottom, color, b);
	return;
}

// FPS�l���v���A�X�V����֐� 
VOID MY_FPS_UPDATE(VOID)
{
	if (CountFps == 0) //1�t���[���ڂȂ玞�����L��
	{
		StartTimeFps = GetNowCount();
	}

	if (CountFps == GAME_FPS) //60�t���[���ڂȂ畽�ς��v�Z
	{
		int now = GetNowCount();
		//���݂̎��Ԃ���A0�t���[���ڂ̎��Ԃ������AFPS�̐��l�Ŋ��遁1FPS�ӂ�̕��ώ��Ԃ��v�Z�����
		CalcFps = 1000.f / ((now - StartTimeFps) / (float)GAME_FPS);
		CountFps = 0;
		StartTimeFps = now;
	}
	CountFps++;
	return;
}

// FPS�l��`�悷��֐� 
VOID MY_FPS_DRAW(VOID)
{
	//�������`��
	DrawFormatString(0, GAME_HEIGHT - 20, GetColor(255, 255, 255), "FPS:%.1f", CalcFps);
	return;
}

// FPS�l���v�����A�҂֐�
VOID MY_FPS_WAIT(VOID)
{
	int resultTime = GetNowCount() - StartTimeFps;					//������������
	int waitTime = CountFps * 1000 / GAME_FPS - resultTime;	//�҂ׂ�����

	if (waitTime > 0)		//�w���FPS�l���������ꍇ
	{
		WaitTimer(waitTime);	//�҂�
	}
	return;
}

// �L�[�̓��͏�Ԃ��X�V����֐� 
VOID MY_ALL_KEYDOWN_UPDATE(VOID)
{

	char TempKey[KEY_MAX];	//�ꎞ�I�ɁA���݂̃L�[�̓��͏�Ԃ��i�[����

	//���O�̃L�[���͂��Ƃ��Ă���
	for (int i = 0; i < KEY_MAX; i++)
	{
		OldAllKeyState[i] = AllKeyState[i];
	}

	GetHitKeyStateAll(TempKey); // �S�ẴL�[�̓��͏�Ԃ𓾂�

	for (int i = 0; i < KEY_MAX; i++)
	{
		if (TempKey[i] != 0)	//������Ă���L�[�̃L�[�R�[�h�������Ă���Ƃ�
		{
			AllKeyState[i]++;	//������Ă���
		}
		else
		{
			AllKeyState[i] = 0;	//������Ă��Ȃ�
		}
	}
	return;
}

//�L�[�������Ă��邩�A�L�[�R�[�h�Ŕ��f����
//���@���Fint�F�L�[�R�[�h�FKEY_INPUT_???
BOOL MY_KEY_DOWN(int KEY_INPUT_)
{
	//�L�[�R�[�h�̃L�[�������Ă��鎞
	if (AllKeyState[KEY_INPUT_] != 0)
	{
		return TRUE;	//�L�[�������Ă���
	}
	else
	{
		return FALSE;	//�L�[�������Ă��Ȃ�
	}
}

//�L�[�������グ�����A�L�[�R�[�h�Ŕ��f����
//���@���Fint�F�L�[�R�[�h�FKEY_INPUT_???
BOOL MY_KEY_UP(int KEY_INPUT_)
{
	if (OldAllKeyState[KEY_INPUT_] >= 1	//���O�͉����Ă���
		&& AllKeyState[KEY_INPUT_] == 0)	//���͉����Ă��Ȃ��Ƃ�
	{
		return TRUE;	//�L�[�������グ�Ă���
	}
	else
	{
		return FALSE;	//�L�[�������グ�Ă��Ȃ�
	}
}

//�L�[���v�b�V���������A�L�[�R�[�h�Ŕ��f����
//���@���Fint�F�L�[�R�[�h�FKEY_INPUT_???
BOOL MY_KEY_PUSH(int KEY_INPUT_)
{
	if (OldAllKeyState[KEY_INPUT_] == 0		//���O�͉����Ă��Ȃ���
		&& AllKeyState[KEY_INPUT_] >= 1)	//���͉����Ă���Ƃ�
	{
		return TRUE;	//�L�[���v�b�V�������i���������Ă��A�P��̂ݔ����j
	}
	else
	{
		return FALSE;	//�L�[���v�b�V�����Ă��Ȃ����A���������Ă���
	}
}

//�L�[�����������Ă��邩�A�L�[�R�[�h�Ŕ��f����
//���@���Fint�F�L�[�R�[�h�FKEY_INPUT_???
//���@���Fint�F�L�[�����������鎞��(�~���b)
BOOL MY_KEYDOWN_KEEP(int KEY_INPUT_, int milliTime)
{
	int MilliSec = 1000;	//�P�b��1000�~���b

	//���������鎞��=�b���~FPS�l
	//��j60FPS�̃Q�[���ŁA1�b�ԉ���������Ȃ�A1�b�~60FPS
	int UpdateTime = (milliTime / MilliSec) * GAME_FPS;

	if (AllKeyState[KEY_INPUT_] > UpdateTime)
	{
		return TRUE;	//���������Ă���
	}
	else
	{
		return FALSE;	//���������Ă��Ȃ�
	}
}

//�̈�̓����蔻�������֐�
BOOL MY_CHECK_RECT_COLL(RECT a, RECT b)
{
	if (a.left < b.right &&
		a.top < b.bottom &&
		a.right > b.left &&
		a.bottom > b.top
		)
	{
		return TRUE;	//�������Ă���
	}

	return FALSE;		//�������Ă��Ȃ�
}

//�t�H���g�����̃\�t�g�p�ɁA�ꎞ�I�ɃC���X�g�[��
BOOL MY_FONT_INSTALL_ONCE(VOID)
{
	//�t�H���g���ꎞ�I�ɓǂݍ���(WinAPI)
	if (AddFontResourceEx(FONT_TANU_PATH, FR_PRIVATE, NULL) == 0)
	{
		//�G���[���b�Z�[�W�\��
		MessageBox(GetMainWindowHandle(), FONT_TANU_NAME, FONT_INSTALL_ERR_TITLE, MB_OK);
		return FALSE;
	}

	return TRUE;
}

//�t�H���g�����̃\�t�g�p�ɁA�ꎞ�I�ɃA���C���X�g�[��
VOID MY_FONT_UNINSTALL_ONCE(VOID)
{
	//�ꎞ�I�ɓǂݍ��񂾃t�H���g���폜(WinAPI)
	RemoveFontResourceEx(FONT_TANU_PATH, FR_PRIVATE, NULL);

	return;
}