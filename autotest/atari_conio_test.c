#include <conio.h>

#define ATARI_CH	(*(volatile char *)0x02fc)

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

	return 0;
}
