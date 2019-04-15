#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#define SUBCHUNK1SIZE 16
#define AUDIO_FORMAT 1
#define NUM_CHANNELS 1
#define SAMPLE_RATE 8000
#define BITS_PER_SAMPLE 8
#define BYTE_RATE (SAMPLE_RATE * NUM_CHANNELS * BITS_PER_SAMPLE / 8);
#define BLOCK_ALIGN (NUM_CHANNELS * BITS_PER_SAMPLE / 8);
#define NUM_SAMPLE SAMPLE_RATE;

struct header
{
	// RIFF chunk descriptor
	char chunkID[4];	// 4, big, "RIFF" ASCII
	int32_t chunkSize;	// 4, lit, 4+(8+subchunk1Size)+(8+subchunk2Size)
	char format[4];		// 4, big, "WAVE" ASCII

	// fmt sub-chunk
	char subchunk1ID[4];	// 4, big, "fmt " ASCII
	int32_t subchunk1Size;	// 4, lit, 16 for PCM
	int16_t audioFormat;	// 2, lit, PCM=1 (other ~ compression)
	int16_t numChannels;	// 2, lit, Mono=1, Stereo=2, etc.
	int32_t sampleRate;	// 4, lit, 8000, 44100, etc. (no. samples per sec)
	int32_t byteRate;	// 4, lit, sampleRate*numChannels*bitsPerSample/8
	int16_t blockAlign;	// 2, lit, numChannels*bitsperSample/8
	int16_t bitsPerSample;	// 2, lit, 8 bits=8 (unsigned), 16 bits=16, etc.

	// data sub-chunk
	char subchunk2ID[4];	// 4, big, "data" ASCII
	int32_t subchunk2Size;	// 4, lit, numSample*numChannels*bitsPerSample/8
};

void create_header(struct header *h, int32_t numSample)
{
	h->chunkID[0] = 'R';
	h->chunkID[1] = 'I';
	h->chunkID[2] = 'F';
	h->chunkID[3] = 'F';
	h->format[0] = 'W';
	h->format[1] = 'A';
	h->format[2] = 'V';
	h->format[3] = 'E';

	h->subchunk1ID[0] = 'f';
	h->subchunk1ID[1] = 'm';
	h->subchunk1ID[2] = 't';
	h->subchunk1ID[3] = ' ';
	h->subchunk1Size = SUBCHUNK1SIZE;
	h->audioFormat = AUDIO_FORMAT;
	h->numChannels = NUM_CHANNELS;
	h->sampleRate = SAMPLE_RATE;
	h->byteRate = BYTE_RATE;
	h->blockAlign = BLOCK_ALIGN;
	h->bitsPerSample = BITS_PER_SAMPLE;

	h->subchunk2ID[0] = 'd';
	h->subchunk2ID[1] = 'a';
	h->subchunk2ID[2] = 't';
	h->subchunk2ID[3] = 'a';

	h->subchunk2Size = numSample * h->numChannels * h->bitsPerSample / 8;
	h->chunkSize = 20 + h->subchunk1Size + h->subchunk2Size;
}

int write_header(FILE *f, int32_t numSample)
{
	struct header h;
	create_header(&h, numSample);
	return fwrite(&h, sizeof(h), 1, f);
}

int write_data(FILE *f, int32_t numSample)
{
	srand(time(NULL));
	unsigned char c;
	for (int i = 0; i < numSample; ++i) {
		c = rand() % 256;
		int re = fwrite(&c, sizeof(c), 1, f);
		if (re != 1)
			return 0;
		re = fwrite(&c, sizeof(c), 1, f);
		if (re != 1)
			return 0;
	}
	return 1;
}

int main(int argc, char *argv[])
{
	FILE *f;
	int32_t numSample = NUM_SAMPLE;
	if (argc == 1)
		f = fopen("noise.wav", "w");
	else if (argc == 3) {
		numSample = atoi(argv[2]);
		f = fopen(argv[1], "w");
	} else {
		printf("Usage: program_name [file_name numSample]\n");
                return 1;
	}
	if (f == NULL) {
		printf("Cannot open file\n");
		return 1;
	}
	if (write_header(f, numSample) != 1) {
		printf("Cannot write header\n");
		return 1;
	}
	if (write_data(f, numSample) != 1) {
		printf("Cannot write data\n");
		return 1;
	}
	return 0;
}

