#define setvgamode(modenum) asm mov ah, 0x00; \
	asm mov al, modenum; \
	asm int 0x10;

void Init_Sprites(void);
void Init_Palette(char *Filename);
void Init_Back(char *Filename);
int Load_Sprite(char *Filename);
void Update(void);
void Activate_Sprite(int N);
void Deactivate_Sprite(int N);
void Move_Sprite(int N, int AX, int AY);
void Move_Sprite_ABS(int N, int X, int Y);
