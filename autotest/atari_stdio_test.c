#include <stdio.h>

int main(void)
{
	char data[8];
	FILE * fp = fopen("H1:OSCAR64.BIN", "wb");
	if (!fp)
		return 1;

	fputc(0x41, fp);
	fputs("BC", fp);
	fwrite("DEF", 1, 3, fp);
	fprintf(fp, "%c%u", 'G', 8);
	if (fclose(fp))
		return 2;

	fp = fopen("H1:OSCAR64.BIN", "rb");
	if (!fp)
		return 3;

	int c = fgetc(fp);
	size_t n = fread(data, 1, sizeof(data), fp);
	bool end = feof(fp);
	if (fclose(fp))
		return 4;

	fp = fopen("H1:OSCAR64.TXT", "w");
	if (!fp)
		return 5;
	fputs("LINE\n", fp);
	fprintf(fp, "0123456789012345678901234567890123456789:%u\n", 42);
	if (fclose(fp))
		return 6;

	fp = fopen("H1:OSCAR64.TXT", "r");
	if (!fp)
		return 7;
	char * line = fgets(data, sizeof(data), fp);
	fscanf(fp, "%c", data + 7);
	fclose(fp);

	return c != 'A' || n != 7 || !end || !line || data[4] != '\n';
}
