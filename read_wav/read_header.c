#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>

#define SUBCHUNK1_SIZE 16

typedef struct
{
        // RIFF chunk descriptor
        char chunkID[4];        // 4, big, "RIFF" ASCII
        int32_t chunkSize;      // 4, lit, 4+(8+subchunk1Size)+(8+subchunk2Size)
        char format[4];         // 4, big, "WAVE" ASCII

        // fmt sub-chunk
        char subchunk1ID[4];    // 4, big, "fmt " ASCII
        int32_t subchunk1Size;  // 4, lit, 16 for PCM
        int16_t audioFormat;    // 2, lit, PCM=1 (other ~ compression)
        int16_t numChannels;    // 2, lit, Mono=1, Stereo=2, etc.
        int32_t sampleRate;     // 4, lit, 8000, 44100, etc. (no. samples per sec)
        int32_t byteRate;       // 4, lit, sampleRate*numChannels*bitsPerSample/8
        int16_t blockAlign;     // 2, lit, numChannels*bitsperSample/8
        int16_t bitsPerSample;  // 2, lit, 8 bits=8 (unsigned), 16 bits=16, etc.

        // data sub-chunk
        char subchunk2ID[4];    // 4, big, "data" ASCII
        int32_t subchunk2Size;  // 4, lit, numSample*numChannels*bitsPerSample/8
} header_t;

int read_chunkID(FILE *f, header_t *h)
{
        if (fread(&(h->chunkID), 4, 1, f) != 1) {
                fprintf(stderr, "Reading error\n");
                return 1;
        }
        if (h->chunkID[0] != 'R' || h->chunkID[1] != 'I' || h->chunkID[2] != 'F' \
			|| h->chunkID[3] != 'F') {
                fprintf(stderr, "File is not RIFF\n");
                return 1;
        }
	return 0;
}

int read_chunkSize(FILE *f, header_t *h)
{
	if (fread(&(h->chunkSize), 4, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	return 0;
}

int read_format(FILE *f, header_t *h)
{
	if (fread(&(h->format), 4, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	if (h->format[0] != 'W' || h->format[1] != 'A' || h->format[2] != 'V' || \
			h->format[3] != 'E') {
		fprintf(stderr, "File is not WAVE\n");
		return 1;
	}
	return 0;
}

int read_subchunk1ID(FILE *f, header_t *h)
{
	if (fread(&(h->subchunk1ID), 4, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	if (h->subchunk1ID[0] != 'f' || h->subchunk1ID[1] != 'm' || \
			h->subchunk1ID[2] != 't' || h->subchunk1ID[3] != ' ') {
		fprintf(stderr, "Invalid subchunk1ID\n");
		return 1;
	}
	return 0;
}

int read_subchunk1Size(FILE *f, header_t *h)
{
	if (fread(&(h->subchunk1Size), 4, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	if (h->subchunk1Size != 16) {
		fprintf(stderr, "File is not PCM\n");
		return 1;
	}
	return 0;
}

int read_audioFormat(FILE *f, header_t *h)
{
	if (fread(&(h->audioFormat), 2, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	return 0;
}

int read_numChannels(FILE *f, header_t *h)
{
	if (fread(&(h->numChannels), 2, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	return 0;
}

int read_sampleRate(FILE *f, header_t *h)
{
	if (fread(&(h->sampleRate), 4, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	return 0;
}

int read_byteRate(FILE *f, header_t *h)
{
	if (fread(&(h->byteRate), 4, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	return 0;
}

int read_blockAlign(FILE *f, header_t *h)
{
	if (fread(&(h->blockAlign), 2, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	return 0;
}

int read_bitsPerSample(FILE *f, header_t *h)
{
	if (fread(&(h->bitsPerSample), 2, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	return 0;
}

int read_subchunk2ID(FILE *f, header_t *h)
{
	if (fread(&(h->subchunk2ID), 4, 1, f) != 1) {
		fprintf(stderr, "Reading erro\n");
		return 1;
	}
	if (h->subchunk2ID[0] != 'd' || h->subchunk2ID[1] != 'a' || \
			h->subchunk2ID[2] != 't' || h->subchunk2ID[3] != 'a') {
		fprintf(stderr, "Invalid subchunk2ID\n");
		return 1;
	}
	return 0;
}

int read_subchunk2Size(FILE *f, header_t *h)
{
	if (fread(&(h->subchunk2Size), 4, 1, f) != 1) {
		fprintf(stderr, "Reading error\n");
		return 1;
	}
	return 0;
}

void print_header(header_t h)
{
        fprintf(stdout, "Duration: %ds\n", h.subchunk2Size/h.byteRate);
        fprintf(stdout, "ChunkSize: %d bytes\n", h.chunkSize);
        fprintf(stdout, "AudioFormat: %d\n", h.audioFormat);
        fprintf(stdout, "NumChannels: %d\n", h.numChannels);
        fprintf(stdout, "SampleRate: %d\n", h.sampleRate);
        fprintf(stdout, "BitsPerSample: %d\n", h.bitsPerSample);
}

int read_header(char *file_name)
{
	FILE *f = fopen(file_name, "r");
	if (f == NULL) {
		fprintf(stderr, "Cannot open %s\n", file_name);
		return 1;
	}
	header_t h;
	if (read_chunkID(f, &h))
		return 1;
	if (read_chunkSize(f, &h))
		return 1;
	if (read_format(f, &h))
		return 1;
	if (read_subchunk1ID(f, &h))
		return 1;
	if (read_subchunk1Size(f, &h))
		return 1;
	if (read_audioFormat(f, &h))
		return 1;
	if (read_numChannels(f, &h))
		return 1;
	if (read_sampleRate(f, &h))
		return 1;
	if (read_byteRate(f, &h))
		return 1;
	if (read_blockAlign(f, &h))
		return 1;
	if (read_bitsPerSample(f, &h))
		return 1;
	if (read_subchunk2ID(f, &h))
		return 1;
	if (read_subchunk2Size(f, &h))
		return 1;
	print_header(h);
	return 0;
}

int main(int argc, char *argv[])
{
	if (argc == 1) {
		fprintf(stderr, "Usage: program_name file_name...\n");
		return 1;
	}
	for (int i = 1; i < argc; ++i) {
		fprintf(stdout, "Reading %s...\n", argv[i]);
		sleep(1);
		read_header(argv[i]);
		if (i != argc-1)
			fprintf(stdout, "\n");
	}
	return 0;
}
