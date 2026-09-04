#include <conio.h>

#define ATARI_CH	(*(volatile char *)0x02fc)
#define ATARI_SAVMSC	(*(volatile char **)0x0058)
#define ATARI_COLOR1	(*(volatile char *)0x02c5)
#define ATARI_COLOR2	(*(volatile char *)0x02c6)
#define ATARI_COLOR4	(*(volatile char *)0x02c8)

int main(void)
{
	iocharmap(IOCHM_ASCII);

	ATARI_CH = 0xff;
	if (kbhit())
		return 1;
	if (getchx() != 0)
		return 2;

	ATARI_CH = 0x1f;
	if (!kbhit())
		return 3;
	if (getch() != '1')
		return 4;
	if (ATARI_CH != 0xff)
		return 5;

	ATARI_CH = 0x0c;
	if (getch() != '\n')
		return 6;

	ATARI_CH = 0x1f;
	if (getchx() != '1')
		return 7;
	if (kbhit())
		return 8;

	ATARI_CH = 0x1f;
	if (getche() != '1')
		return 9;

	textcolor(COLOR_LT_RED);
	bgcolor(COLOR_BLUE);
	bordercolor(COLOR_YELLOW);
	if (ATARI_COLOR1 != 0x0c)
		return 10;
	if (ATARI_COLOR2 != 0x74)
		return 11;
	if (ATARI_COLOR4 != 0xee)
		return 12;

	bgcolor(0xa8);
	if (ATARI_COLOR2 != 0xa8)
		return 13;

	clrscr();
	gotoxy(0, 0);
	revers(1);
	putch('A');
	revers(0);
	putch('B');
	if (ATARI_SAVMSC[0] != 0xa1)
		return 14;
	if (ATARI_SAVMSC[1] != 0x22)
		return 15;

	return 0;
}
