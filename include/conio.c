#include "conio.h"

static IOCharMap		giocharmap = IOCHM_ASCII;

#if defined(__C128__)
#pragma code(lowcode)
__asm bsout
{	
		ldx #0
		stx 0xff00
		jsr 0xffd2
		sta 0xff01
}
__asm bsin
{
		lda #0
		sta 0xff00
		jsr 0xffe4
		sta 0xff01	
}
__asm bsget
{
		lda #0
		sta 0xff00
		jsr 0xffcf
		sta 0xff01		
}
__asm bsplot
{	
		lda #0
		sta 0xff00
		jsr 0xfff0
		sta 0xff01
}
__asm bsinit
{	
		lda #0
		sta 0xff00
		jsr 0xff81
		sta 0xff01
}
__asm dswap
{	
		sta 0xff00
		jsr 0xff5f
		sta 0xff01
}
#pragma code(code)
#elif defined(__C128B__) || defined(__C128E__)
#define dswap 	0xff5f
#define bsout	0xffd2 : a->a
#define bsin	0xffe4
#define bsget	0xffcf : a->a
#define bsplot	0xfff0
#define bsinit	0xff81
#elif defined(__PLUS4__)
#pragma code(lowcode)
__asm bsout
{	
		sta 0xff3e
		jsr 0xffd2
		sta 0xff3f
}
__asm bsin
{
		sta 0xff3e
		jsr 0xffe4
		sta 0xff3f
}
__asm bsget
{
		sta 0xff3e
		jsr 0xffcf
		sta 0xff3f
}
__asm bsplot
{	
		sta 0xff3e
		jsr 0xfff0
		sta 0xff3f
}
__asm bsinit
{	
		sta 0xff3e
		jsr 0xff81
		sta 0xff3f
}
#pragma code(code)
#elif defined(__ATARI__)
#define ATASCII_CLEAR	0x7d
#define ATASCII_EOL	0x9b

#define ATARI_ROWCRS	0x0054
#define ATARI_COLCRS	0x0055
#define ATARI_CRSINH	0x02f0
#define ATARI_CH		0x02fc
#define ATARI_COLOR1	0x02c5
#define ATARI_COLOR2	0x02c6
#define ATARI_COLOR4	0x02c8

static char		gatrevers;

static const char gatcolors[16] = {
	0x00, 0x0e, 0x32, 0x96, 0x68, 0xc4, 0x74, 0xee,
	0x4a, 0xe4, 0x3c, 0x04, 0x06, 0xcc, 0x7c, 0x0a
};

static char atcolor(char c)
{
	if (c < 16)
		return gatcolors[c];
	else
		return c;
}

__asm bsout
{
		tax
		lda	0xe407
		pha
		lda 0xe406
		pha
		txa
}

__asm bsin
{
		lda	0xe405
		pha
		lda 0xe404
		pha
}

__asm bsget
{
		lda	0xe405
		pha
		lda 0xe404
		pha
}

__asm bskey
{
		lda	0xe425
		pha
		lda 0xe424
		pha
}

__asm bsplot
{

}
__asm bsinit
{

}
#elif defined(__VIC20__)
#define bsout	0xffd2 : a->a
#define bsin	0xffe4
#define bsplot	0xfff0
#define bsget	0xffcf : a->a
__asm bsinit
{
	lda #147
	jmp $ffd2	
}
#elif defined(__CBMPET__)
#define bsout	0xffd2 : a->a
#define bsin	0xffe4
__asm bsplot
{
    /* no equivalent on PET */
}
__asm bsinit
{
    /* no equivalent on PET */
}
#define bsget	0xffcf
#else
#define bsout	0xffd2 : a->a
#define bsin	0xffe4
#define bsplot	0xfff0
#define bsinit	0xff81
#define bsget	0xffcf : a->a
#endif

#if defined(__C128__) || defined(__C128B__) || defined(__C128E__)
void dispmode40col(void)
{
	if (*(volatile char *)0xd7 >= 128)
	{
		__asm
		{		
			jsr dswap
		}
	}
}

void dispmode80col(void)
{
	if (*(volatile char *)0xd7 < 128)
	{
		__asm
		{		
			jsr dswap
		}
	}
}
#endif


void iocharmap(IOCharMap chmap)
{
	giocharmap = chmap;	
#if !defined(__ATARI__)
	if (chmap == IOCHM_PETSCII_1)
		putrch(128 + 14);
	else if (chmap == IOCHM_PETSCII_2)
		putrch(14);
#endif
}

void putrch(char c)
{
	__asm {
		lda 	c
		jsr		bsout
	}
}

void putpch(char c)
{
#if defined(__ATARI__)
	if (giocharmap >= IOCHM_ASCII && c == '\n')
		c = ATASCII_EOL;
	c |= gatrevers;
#else
	if (giocharmap >= IOCHM_ASCII)
	{
		if (c == '\n')
			c = 13;
		else if (c == '\t')
		{
			char n = wherex() & 3;
			do {
				putrch(' ');
			} while (++n < 4);
			return;
		}
		else if (giocharmap >= IOCHM_PETSCII_1)
		{
			if (c >= 65 && c < 123)
			{
				if (c >= 97 || c < 91)
				{
#if defined(__CBMPET__)
					if (c >= 97)
						c ^= 0xa0;
					c ^= 0x20;
#else
					c ^= 0x20;
#endif				

					if (giocharmap == IOCHM_PETSCII_1)
						c &= 0xdf;
				}
			}
		}
	}

#endif

	putrch(c);
}

static char convch(char ch)
{
#if defined(__ATARI__)
	if (giocharmap >= IOCHM_ASCII && ch == ATASCII_EOL)
		ch = '\n';
#else

	if (giocharmap >= IOCHM_ASCII)
	{
		if (ch == 13)
			ch = 10;
		else if (giocharmap >= IOCHM_PETSCII_1)
		{
			if (ch >= 65 && ch < 219)
			{
				if (ch >= 193)
					ch ^= 0xa0;
				if (ch < 123 && (ch >= 97 || ch < 91))
					ch ^= 0x20;
			}
		}
	}

#endif
	return ch;	
}

char getrch(void)
{
	return __asm {
		jsr bsget
		sta accu
	};
}

char getpch(void)
{
	return convch(getrch());
}


char kbhit(void)
{
#if defined(__ATARI__)
	return *(volatile char *)ATARI_CH != 0xff;
#elif defined(__CBMPET__)
	return __asm
	{
		lda $9e
		sta	accu
	};
#else
	return __asm
	{
		lda $c6
		sta	accu
	};
#endif
}

char getche(void)
{
#if defined(__ATARI__)
	char ch = __asm {
		jsr	bskey
		sta accu
	};
#else
	char ch;
	do {
		ch = __asm {
			jsr	bsin
			sta accu
		};		
	} while (!ch);
#endif

#if defined(__ATARI__)
	putrch(ch | gatrevers);
#else
	__asm {
		lda ch
		jsr	bsout
	}
#endif

	return convch(ch);
}

char getch(void)
{
#if defined(__ATARI__)
	char ch = __asm {
		jsr	bskey
		sta accu
	};
#else
	char ch;
	do {
		ch = __asm {
			jsr	bsin
			sta accu
		};
	} while (!ch);
#endif

	return convch(ch);
}

char getchx(void)
{
#if defined(__ATARI__)
	if (!kbhit())
		return 0;

	char ch = __asm {
		jsr	bskey
		sta accu
	};
#else
	char ch = __asm {
		jsr	bsin
		sta accu
	};
#endif

	return convch(ch);
}

void putch(char c)
{
	putpch(c);
}

void clrscr(void)
{
#if defined(__ATARI__)
	putrch(ATASCII_CLEAR);
#else
	putrch(147);
#endif
}

void textcursor(bool show)
{
#if defined(__ATARI__)
	*(volatile char *)ATARI_CRSINH = show ? 0 : 1;
#else
	*(volatile char *)0xcc = show ? 0 : 1;
#endif
}

void gotoxy(char cx, char cy)
{
#if defined(__ATARI__)
	*(volatile char *)ATARI_ROWCRS = cy;
	*(volatile char *)ATARI_COLCRS = cx;
	*(volatile char *)(ATARI_COLCRS + 1) = 0;
#elif defined(__CBMPET__)
#define CURS_X 0xc6
#define CURS_Y 0xd8
#define SCREEN_PTR 0xc4
#define SCR_LINELEN 0xd5

	__assume(cy < 25);

	*(volatile char *)CURS_X = cx;
	*(volatile char *)CURS_Y = cy;

	if (*(volatile char *)SCR_LINELEN > 40)
		cy <<= 1;

	const unsigned	off = cy * 40;

	* (volatile unsigned *)SCREEN_PTR = off + 0x8000;
#else
	__asm
	{
		ldx	cy
		ldy	cx
		clc
		jsr bsplot
	}
#endif
}

void textcolor(char c)
{
#if defined(__ATARI__)
	// In ANTIC mode 2, COLOR2 supplies the hue and COLOR1 the text luminance.
	*(volatile char *)ATARI_COLOR1 = atcolor(c) & 0x0e;
#else
	*(volatile char *)0x0286 = c;
#endif
}

void bgcolor(char c)
{
#if defined(__ATARI__)
	*(volatile char *)ATARI_COLOR2 = atcolor(c);
#else
	*(volatile char *)0xd021 = c;
#endif
}

void bordercolor(char c)
{
#if defined(__ATARI__)
	*(volatile char *)ATARI_COLOR4 = atcolor(c);
#else
	*(volatile char *)0xd020 = c;
#endif
}

void revers(char r)
{
#if defined(__ATARI__)
	gatrevers = r ? 0x80 : 0;
#else
	if (r) 
		putrch(18);
	else
		putrch(18 + 128);
#endif
}

char wherex(void)
{
#if defined(__ATARI__)
	return *(volatile char *)ATARI_COLCRS;
#elif defined(__C128__) || defined(__C128B__) || defined(__C128E__)
	return *(volatile char *)0xec;
#elif defined(__PLUS4__)
	return *(volatile char *)0xca;	
#else
	return *(volatile char *)0xd3;
#endif
}

char wherey(void)
{
#if defined(__ATARI__)
	return *(volatile char *)ATARI_ROWCRS;
#elif defined(__C128__) || defined(__C128B__) || defined(__C128E__)
	return *(volatile char *)0xeb;
#elif defined(__PLUS4__)
	return *(volatile char *)0xcd;	
#else
	return *(volatile char *)0xd6;
#endif
}
