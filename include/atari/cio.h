#ifndef ATARI_CIO_H
#define ATARI_CIO_H

// Atari Central Input/Output (CIO) channels.  IOCB 0 is normally owned by
// the screen editor, so applications should allocate one of channels 1..7.

#define ACIO_CHANNELS		8

enum acio_mode
{
	ACIO_MODE_READ		= 4,
	ACIO_MODE_WRITE		= 8,
	ACIO_MODE_APPEND	= 9,
	ACIO_MODE_UPDATE	= 12
};

enum acio_error
{
	ACIO_SUCCESS		= 1,
	ACIO_ERROR_EOF		= 0x88
};

// Return true if the IOCB is currently assigned to a device or file.

bool acio_is_open(char channel);

// Return the status byte from the last operation on this IOCB.  Atari CIO
// status values with bit 7 set are errors.

char acio_status(char channel);

// Open and close an exact Atari device specification, such as "D1:FILE.DAT"
// or "E:".  The name may be terminated by either zero or ATASCII EOL.

bool acio_open(char channel, const char * name, char mode);

bool acio_close(char channel);

// Byte and binary-buffer access.  Read and write return the number of bytes
// transferred, or -1 when an error occurs before any byte is transferred.

int acio_getc(char channel);

int acio_putc(char channel, char c);

int acio_read(char channel, void * data, int size);

int acio_write(char channel, const void * data, int size);

#pragma compile("cio.c")

#endif
