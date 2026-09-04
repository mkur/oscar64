#include "cio.h"

#define ACIO_IOCB_BASE	0x0340

#define ACIO_ICHID	0
#define ACIO_ICCMD	2
#define ACIO_ICSTA	3
#define ACIO_ICBAL	4
#define ACIO_ICBLL	8
#define ACIO_ICAX1	10
#define ACIO_ICAX2	11

#define ACIO_CMD_OPEN	3
#define ACIO_CMD_GET	7
#define ACIO_CMD_PUT	11
#define ACIO_CMD_CLOSE	12

static volatile char * acio_iocb(char channel)
{
	return (volatile char *)(ACIO_IOCB_BASE + ((unsigned)channel << 4));
}

static char acio_call(char channel)
{
	return __asm
	{
		lda channel
		asl
		asl
		asl
		asl
		tax
		jsr $e456
		sty accu
	};
}

#pragma native(acio_call)

bool acio_is_open(char channel)
{
	return channel < ACIO_CHANNELS && acio_iocb(channel)[ACIO_ICHID] != 0xff;
}

char acio_status(char channel)
{
	if (channel < ACIO_CHANNELS)
		return acio_iocb(channel)[ACIO_ICSTA];
	return 0xff;
}

bool acio_open(char channel, const char * name, char mode)
{
	if (!channel || channel >= ACIO_CHANNELS || acio_is_open(channel))
		return false;

	volatile char * iocb = acio_iocb(channel);
	iocb[ACIO_ICCMD] = ACIO_CMD_OPEN;
	iocb[ACIO_ICBAL] = (unsigned)name;
	iocb[ACIO_ICBAL + 1] = (unsigned)name >> 8;
	iocb[ACIO_ICAX1] = mode;
	iocb[ACIO_ICAX2] = 0;

	return !(acio_call(channel) & 0x80);
}

bool acio_close(char channel)
{
	if (!channel || channel >= ACIO_CHANNELS || !acio_is_open(channel))
		return false;

	volatile char * iocb = acio_iocb(channel);
	iocb[ACIO_ICCMD] = ACIO_CMD_CLOSE;

	return !(acio_call(channel) & 0x80);
}

static int acio_transfer(char channel, char command, void * data, int size)
{
	if (!channel || channel >= ACIO_CHANNELS || !acio_is_open(channel))
		return -1;
	if (size <= 0)
		return 0;

	volatile char * iocb = acio_iocb(channel);
	iocb[ACIO_ICCMD] = command;
	iocb[ACIO_ICBAL] = (unsigned)data;
	iocb[ACIO_ICBAL + 1] = (unsigned)data >> 8;
	iocb[ACIO_ICBLL] = size;
	iocb[ACIO_ICBLL + 1] = size >> 8;

	char status = acio_call(channel);
	int actual = iocb[ACIO_ICBLL] | ((unsigned)iocb[ACIO_ICBLL + 1] << 8);

	if ((status & 0x80) && status != ACIO_ERROR_EOF && !actual)
		return -1;
	return actual;
}

int acio_getc(char channel)
{
	char c;
	if (acio_read(channel, &c, 1) == 1)
		return (unsigned char)c;
	return -1;
}

int acio_putc(char channel, char c)
{
	if (acio_write(channel, &c, 1) == 1)
		return (unsigned char)c;
	return -1;
}

int acio_read(char channel, void * data, int size)
{
	if (acio_status(channel) == ACIO_ERROR_EOF)
		return 0;
	return acio_transfer(channel, ACIO_CMD_GET, data, size);
}

int acio_write(char channel, const void * data, int size)
{
	return acio_transfer(channel, ACIO_CMD_PUT, (void *)data, size);
}
