#include "video.h"
#include <mem.h>
#include <alloc.h>
#include <stdlib.h>
#include <dos.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define MAXSPRITES 128

typedef struct {
	char *Buffer;
	int x, y;
	int w, h;
	int Shown;
} SPRITE;

SPRITE Sprite[MAXSPRITES];

char *Page, *Back;

void Init_Palette(char *Filename)
{
	int i;
	FILE *In;

	if ((In = fopen(Filename, "rb")) == NULL) {
		setvgamode(0x02);
		perror("Cannot load palette");
		exit(1);
	}

	outportb(0x3C8, 0);
	for (i = 0; i < 768; i++) {
		outportb(0x3C9, fgetc(In));
	}

	fclose(In);
}

void Init_Back(char *Filename)
{
	FILE *In;

	if ((In = fopen(Filename, "rb")) == NULL) {
		setvgamode(0x02);
		perror("Cannot open background");
		exit(1);
	}

	if ((Back = farmalloc(64000u)) == NULL) {
		setvgamode(0x02);
		errno = ENOMEM;
		perror("Cannot virtualize background");
		exit(1);
	}

	fread(Back, 64000u, 1, In);

	fclose(In);
}


void Init_Sprites(void)
{
	int i;

	for (i = 0; i < MAXSPRITES; i++) {
		Sprite[i].Buffer = NULL;
		Sprite[i].x = 0; Sprite[i].y = 0;
		Sprite[i].w = 0; Sprite[i].h = 0;
		Sprite[i].Shown = 0;
	}

	if ((Page = farmalloc(64000u)) == NULL) {
		setvgamode(0x02);
		errno = ENOMEM;
		perror("Cannot virtualize screen");
		exit(1);
	}
}

int Next_Sprite(void)
{
	int i;

	for (i = 0; i < MAXSPRITES; i++) {
		if (Sprite[i].Buffer == NULL) return i;
	}

	setvgamode(0x02);
	errno = EDOM;
	perror("Out of sprites");
	exit(1);

	return 0;
}

// Not the correct way to do it, but it will work here.
void ScanForString(FILE *fp, char *String)
{
	int i, c;

	for (i = 0; i < strlen(String); i++) {
		while (1) {
			c = fgetc(fp);
			if (c == String[i] || c == EOF) break;
		};
	}
}

void Activate_Sprite(int N)
{
	if (N >= 0 && N < MAXSPRITES) {
		Sprite[N].Shown = 1;
	} else {
		setvgamode(0x02);
		errno = EDOM;
		perror("Bad sprite number");
		exit(1);
	}
}

void Deactivate_Sprite(int N)
{
	if (N >= 0 && N < MAXSPRITES) {
		Sprite[N].Shown = 0;
	} else {
		setvgamode(0x02);
		errno = EDOM;
		perror("Bad sprite number");
		exit(1);
	}
}

void Move_Sprite(int N, int AX, int AY)
{
	if (N >= 0 && N < MAXSPRITES) {
		Sprite[N].x += AX;
		Sprite[N].y += AY;
	} else {
		setvgamode(0x02);
		errno = EDOM;
		perror("Bad sprite number");
		exit(1);
	}
}

void Move_Sprite_ABS(int N, int X, int Y)
{
	if (N >= 0 && N < MAXSPRITES) {
		Sprite[N].x = X;
		Sprite[N].y = Y;
	} else {
		setvgamode(0x02);
		errno = EDOM;
		perror("Bad sprite number");
		exit(1);
	}
}

int Load_Sprite(char *Filename)
{
	FILE *Fin;
	int sx, sy, ex, ey, h, w;
	int N;

	if ((Fin = fopen(Filename, "rt")) == NULL) {
		setvgamode(0x02);
		perror("Cannot load sprite");
		exit(1);
	}

	fseek(Fin, -50, SEEK_END);
	ScanForString(Fin, "DIMS:");
	fscanf(Fin, "%d %d %d %d", &sx, &sy, &ex, &ey);
	fclose(Fin);

	h = ey - sy + 1;
	w = ex - sx + 1;

	N = Next_Sprite();

	Sprite[N].x = sx;
	Sprite[N].y = sy;
	Sprite[N].h = h;
	Sprite[N].w = w;
	Sprite[N].Shown = 0;

	if ((Sprite[N].Buffer = farmalloc(h * w)) == NULL) {
		setvgamode(0x02);
		errno = ENOMEM;
		perror("Cannot load sprite");
		exit(1);
	}

	Fin = fopen(Filename, "rb");
	if (fread(Sprite[N].Buffer, w, h, Fin) != h) {
		setvgamode(0x02);
		errno = EFAULT;
		perror("Cannot load sprite");
		exit(1);
	}
	fclose(Fin);

	return N;
}

void Show_Sprite(int N)
{
	int h, w;
	int x, y;
	int left, lrem = 0, rrem = 0;

	int BSeg, BOff;
	int PSeg, POff;


	BSeg = FP_SEG(Sprite[N].Buffer);
	BOff = FP_OFF(Sprite[N].Buffer);
	PSeg = FP_SEG(Page);
	POff = FP_OFF(Page);

	h = Sprite[N].h; w = Sprite[N].w;
	x = Sprite[N].x; y = Sprite[N].y;

	if (x < 0) {
		lrem = 0 - x;
		w -= lrem;
		x = 0;

		if (w <= 0) return;
	}

	if (x + w > 319) {
		w = 320 - x;
		rrem = Sprite[N].w - w;

		if (w <= 0) return;
	}

	asm push ds
	asm push es
	asm push si
	asm push di

	_DS = BSeg;
	_SI = BOff + lrem;

	_ES = PSeg;
	left = POff + (y * 320) + x;

	asm mov di, left
	asm mov ax, di
	asm add ax, w

	asm jmp Begin:

Up:
	asm inc si
	asm inc di
	asm jmp Start

Begin:
	asm mov bx, h

Start:
	asm cmp di, ax
	asm je Out
	asm cmp byte ptr ds:si, 0
	asm je Up

	// Not zero.  Copy it to the screen.
	asm movsb

	asm jmp Start

Out:
	// We've copied enough bytes for this row.
	asm add left, 320
	asm mov di, left
	asm mov ax, di
	asm add ax, w
	asm add si, lrem
	asm add si, rrem

	// Have we done enough rows yet?
	asm dec bx
	asm jnz Start

	// Yes, time to quit.
	asm pop di
	asm pop si
	asm pop es
	asm pop ds

	return;
}

void Update(void)
{
	int i;

	memcpy(Page, Back, 64000u);

	for (i = 0; i < MAXSPRITES; i++)
		if (Sprite[i].Shown == 1) {
			Show_Sprite(i);
		}

	memcpy(MK_FP(0xA000, 0), Page, 64000u);
}