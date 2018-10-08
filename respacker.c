#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>

#include "quicklz.h"

int debug = 0;

struct res_t {

    long cmpfsize;
    char *compressedres;
    char *tmp_buffer;

    long uncfsize;
    char *uncompressedres;

};

struct res_t res;

/*
C Integer data types on this 32-bit machine
--------------------------------------------------------------
short                    2 bytes - int16
int                      4 bytes - int32
long                     4 bytes
long long                8 bytes


  _BYTE           an unknown type; the only known info is its size: 1 byte
  _WORD           an unknown type; the only known info is its size: 2 bytes
  _DWORD          an unknown type; the only known info is its size: 4 bytes
  _QWORD          an unknown type; the only known info is its size: 8 bytes
  _OWORD          an unknown type; the only known info is its size: 16 bytes
  _TBYTE          10-byte floating point value

*/
//dword = long
//byte =char - int 8

int stream_compress() {

	char *file_data, *compress_chunk;
	unsigned int compress_size; // r4@2
	int decompress_size; // r7@2
	char * compressed; // r11@2
	int compressed_len; // r10@5
	int decompressed_len; // ST0C_4@5

	size_t len2;

	char * orig_uncompressedres = res.uncompressedres;


	file_data = res.tmp_buffer;
	memcpy(file_data,res.uncompressedres,res.uncfsize);

	compress_chunk = malloc(0x1400);
	qlz_state_compress *state_compress = (qlz_state_compress *)malloc(sizeof(qlz_state_compress));
	memset(state_compress, 0, sizeof(qlz_state_compress));
	
	compress_size = 0;
	decompress_size = 0;
	compressed = res.compressedres;


	while ( 1 )
	{
      
		if ( (res.uncfsize) <= decompress_size )
		{
			printf("[%d]I: compress_size is %d,decompress size is %d\n", 0, compress_size, decompress_size);
			free(compress_chunk);
			free(state_compress);
			printf("[%d]I: compress over\n", 0);
			res.cmpfsize=compress_size;
			return 1;
		}
		printf("[%d]I: compress circle\n", 0);
		if ( res.uncfsize < compress_size + 9 ) //FIXME
			break;

		if ((res.uncfsize - decompress_size) >= 0x1000 ) //test primo giro  - se 
		{
			if (debug) printf("file_data ADDR: %08x\n", file_data);
			if (debug) printf("res.uncompressedres_addr %08x\n", res.uncompressedres);

			memcpy(file_data,res.uncompressedres,0x1000);
			//size_t qlz_compress(const void *source, char *destination, size_t size, qlz_state_compress *state)
			len2 = qlz_compress(file_data, compress_chunk, 0x1000, state_compress);
			memcpy(compressed,compress_chunk,len2);

			compressed += len2;
			compress_size += len2;
			decompress_size += 0x1000;
			res.uncompressedres += 0x1000;
			//printf("[%d]I: decompress_size is %d,compress_size is %d \r\n", 0, decompress_size, compressed_len);
			printf("[%d]I: decompress_size is %04x,compress_size is %04x \r\n", 0, 0x1000, len2);
		}
		else
		{
			memcpy(compressed,res.uncompressedres,res.uncfsize - decompress_size);
			compress_size += (res.uncfsize - decompress_size);
			decompress_size = (res.uncfsize) ;
			printf("[%d]I: last data \r\n", 0);
		}
		printf(
		"[%d]I: flag is %d ,compress len is %d,decompress len is %d \r\n",
		0,
		*compress_chunk,
		len2,
		0x1000);
	}
	return 1;
}

int main(int argc, char *argv[]) {
	int count = 0;
	
	printf ("Amazfit RES packer\n");
	/*if (argc > 1) {
		for (count = 1; count < argc; count++) {
			printf("argv[%d] = %s\n", count, argv[count]);
		}
	} else {
		printf("The command had no other arguments.\n");
	}*/

	char * filename_packed;
	if (argc > 1) {
		filename_packed = malloc(strlen(argv[1]) + 5);
		strcpy(filename_packed,argv[1]);
		strcat(filename_packed,".cmp");
	} else {
		printf("Wrong syntax, please use:\n\n\t%s <filename>\n",argv[0]);
		exit(1);
	}

	FILE *f = fopen(argv[1], "rb");
	if (!f) {
		printf("Error opening %s\n", argv[1]);
		exit(1);
	}
	fseek(f, 0, SEEK_END);
	res.uncfsize = ftell(f);

	fseek(f, 0, SEEK_SET);  //same as rewind(f);

	res.tmp_buffer = malloc(res.uncfsize + 1);

	res.uncompressedres = malloc(res.uncfsize + 1);
	fread(res.uncompressedres, res.uncfsize, 1, f);
	fclose(f);

	//FIXME - fa schifo
	res.compressedres = malloc(0x1024*0x1024*2);
    
	stream_compress();

	//write compressed data
	printf("FIXME - point to new buffer\n");
	FILE *f2 = fopen(filename_packed,"wb");
	fwrite(res.compressedres, 1, res.cmpfsize, f2);
	printf("File %s written\n",filename_packed);
	fclose(f2);
}
