#include "video.h"
#include <conio.h>
#include <dos.h>
#include <stdlib.h>

void XMas_Demo(void)
{
	int s;
	int i;
	int XH, XF;
	int C = 0;
	int sl;
	int TF;
	int D[10][3];
	int tr, s1, s2, s3, t3;

	tr = Load_Sprite("TREE.CLB");
	sl = Load_Sprite("REINS.CLB");

	for (i = 0; i < 8; i++) {
		D[i][0] = Load_Sprite("DEER1.CLB");
		D[i][1] = Load_Sprite("DEER2.CLB");
	}

	TF = Load_Sprite("TOFROM.CLB");

	XH = Load_Sprite("XMASH.CLB");
	XF = Load_Sprite("XMASF.CLB");
	s1 = Load_Sprite("SANTA1.CLB");
	s2 = Load_Sprite("SANTA2.CLB");
	s3 = Load_Sprite("SANTATU.CLB");
	t3 = Load_Sprite("TREEG.CLB");

	Update();
	delay(2000);

	Move_Sprite_ABS(sl, -200, 81);
	Activate_Sprite(sl);

	for (i = 0; i < 8; i++) {
		Move_Sprite_ABS(D[i][0], -10 - i * 22, 83 - abs(3 - i) * 1);
		Move_Sprite_ABS(D[i][1], -10 - i * 22, 82 - abs(3 - i) * 1);
	}

	for (s = 0; s < 280; s++) {
		Update();

		Move_Sprite(sl, 2, 0);

		for (i = 0; i < 8; i++) {
			Move_Sprite(D[i][0], 2, 0);
			Move_Sprite(D[i][1], 2, 0);

			if (C == 0) {
				Deactivate_Sprite(D[i][0]);
				Activate_Sprite(D[i][1]);
			} else {
				Deactivate_Sprite(D[i][1]);
				Activate_Sprite(D[i][0]);
			}
		}

		C = 1 - C;
		delay(50);
	}

	Update();
	delay(400);

	Activate_Sprite(XH);
	Update();

	delay(250);

	Deactivate_Sprite(XH);
	Activate_Sprite(XF);
	Update();

	delay(3000);

	Activate_Sprite(TF);
	Update();

	delay(6000);

	Move_Sprite_ABS(sl, -200, 86);
	Activate_Sprite(sl);

	for (i = 0; i < 8; i++) {
		Move_Sprite_ABS(D[i][0], -10 - i * 22, 88 - abs(3 - i) * 1);
		Move_Sprite_ABS(D[i][1], -10 - i * 22, 87 - abs(3 - i) * 1);
	}

	for (s = 0; s < 290; s++) {
		Update();

		Move_Sprite(sl, 2, 0);

		if (s == 200) {
			Activate_Sprite(tr);
			Move_Sprite_ABS(tr, 210, 83);
		}

		if (s > 201) {
			Move_Sprite(tr, 0, 1);
		}

		if ((s > 260) && ((s % 3) == 1)) {
			Move_Sprite(tr, 0, 1);
		}

		for (i = 0; i < 8; i++) {
			Move_Sprite(D[i][0], 2, 0);
			Move_Sprite(D[i][1], 2, 0);

			if (C == 0) {
				Deactivate_Sprite(D[i][0]);
				Activate_Sprite(D[i][1]);
			} else {
				Deactivate_Sprite(D[i][1]);
				Activate_Sprite(D[i][0]);
			}
		}

		C = 1 - C;
		delay(50);
	}

	Move_Sprite_ABS(s1, -6, 180);
	Move_Sprite_ABS(s2, -6, 180);
	Move_Sprite_ABS(s3, -6, 180);

	for (i = 0; i < 110; i++) {
		Update();

		if (i > 60 && i < 68) {
			delay(20);
		}

		if (i > 65 && i < 68) {
			delay(20);
		}


		if (i == 68) {
			Deactivate_Sprite(s2);
			Deactivate_Sprite(s1);
			Activate_Sprite(s3);
			Update();
			delay(1500);
			Deactivate_Sprite(tr);
			Activate_Sprite(t3);
			Update();
			delay(1500);
			Deactivate_Sprite(s3);
		}

		Move_Sprite(s1, 3, 0);
		Move_Sprite(s2, 3, 0);
		Move_Sprite(s3, 3, 0);

		if (C == 0) {
			Deactivate_Sprite(s2);
			Activate_Sprite(s1);
		} else {
			Activate_Sprite(s2);
			Deactivate_Sprite(s1);
		}

		C = 1 - C;
		delay(100);
	}

	delay(3000);

	// kill first random number
	s = random(100);

	do {
	s = random(100) - 50;
	Move_Sprite_ABS(sl, -200, 86 + s);
	Activate_Sprite(sl);

	for (i = 0; i < 8; i++) {
		Move_Sprite_ABS(D[i][0], -10 - i * 22, 88 + s - abs(3 - i) * 1);
		Move_Sprite_ABS(D[i][1], -10 - i * 22, 87 + s - abs(3 - i) * 1);
	}

	for (s = 0; s < 290; s++) {
		Update();

		Move_Sprite(sl, 2, 0);

		for (i = 0; i < 8; i++) {
			Move_Sprite(D[i][0], 2, 0);
			Move_Sprite(D[i][1], 2, 0);

			if (C == 0) {
				Deactivate_Sprite(D[i][0]);
				Activate_Sprite(D[i][1]);
			} else {
				Deactivate_Sprite(D[i][1]);
				Activate_Sprite(D[i][0]);
			}
		}

		C = 1 - C;
		delay(50);
	}

	Update();
	delay(5000);
	} while (!kbhit());

	getch();
}

void main(void)
{
	setvgamode(0x13);

	Init_Palette("XMAS.PAL");
	Init_Back("BACK.CLB");
	Init_Sprites();

	XMas_Demo();

	setvgamode(0x03);
}