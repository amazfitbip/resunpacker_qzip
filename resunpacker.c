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

int stream_decompress() {

	char *file_data, *decompressed_chunk;
	unsigned int compress_size; // r4@2
	unsigned int decompress_size; // r7@2
	char * decompressed; // r11@2
	int compressed_len; // r10@5
	int decompressed_len; // ST0C_4@5
	
	file_data = res.tmp_buffer;
	memcpy(file_data,res.compressedres,res.cmpfsize);
	
	decompressed_chunk = malloc(0x1400);
	qlz_state_decompress *state_decompress = (qlz_state_decompress *)malloc(sizeof(qlz_state_decompress));
	memset(state_decompress, 0, sizeof(qlz_state_decompress));
	
	compress_size = 0;
	decompress_size = 0;
	decompressed = res.uncompressedres;

	while ( 1 )
	{
		if ( (res.cmpfsize) <= compress_size )
		{
			printf("[%d]I: compress_size is %d,decompress size is %d\n", 0, compress_size, decompress_size);
			free(decompressed_chunk);
			free(state_decompress);
			printf("[%d]I: decompress over\n", 0);
			res.uncfsize=decompress_size;
			return 1;
		}

		printf("[%d]I: decompress circle\n", 0);
		if ( res.cmpfsize < compress_size + 9 )
			break;

		memcpy(file_data,res.compressedres,9);
		compressed_len = *(int16_t *)(file_data + 1);
		decompressed_len = *(int16_t *)(file_data + 5);
		printf(
			"[%d]I: flag is %d ,compress len is %d,decompress len is %d \r\n",
			0,
			*file_data,
			compressed_len,
			decompressed_len);
		if ( decompressed_len == 0x1000 )
		{
			if (debug) printf("file_data ADDR: %08x\n", file_data);
			if (debug) printf("res.compressedres_addr %08x\n", res.compressedres);

			memcpy(file_data,res.compressedres,compressed_len);
			qlz_decompress(file_data, decompressed_chunk, state_decompress);
			memcpy(decompressed,decompressed_chunk,0x1000);
			decompressed += 0x1000;
			compress_size += compressed_len;
			decompress_size += 0x1000;
			if (debug) printf("aggiungo compressed_len %04x\n", compressed_len);
			res.compressedres += compressed_len;
			//printf("[%d]I: decompress_size is %d,compress_size is %d \r\n", 0, decompress_size, compressed_len);
			printf("[%d]I: decompress_size is %04x,compress_size is %04x \r\n", 0, decompress_size, compressed_len);

      }
      else
      {
		memcpy(decompressed,res.compressedres,(res.cmpfsize) - compress_size);
		decompress_size += (res.cmpfsize) - compress_size;
		compress_size = (res.cmpfsize);
		printf("[%d]I: last data \r\n", 0);

      }
    }

  return 1;
}

int main(int argc, char *argv[]) {
	int count = 0;
	
	printf ("Amazfit RES unpacker\n");
	/*if (argc > 1) {
		for (count = 1; count < argc; count++) {
			printf("argv[%d] = %s\n", count, argv[count]);
		}
	} else {
		printf("The command had no other arguments.\n");
	}*/

	char * filename_unp;
	if (argc > 1) {
		filename_unp = malloc(strlen(argv[1]) + 5);
		strcpy(filename_unp,argv[1]);
		strcat(filename_unp,".unp");
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
	res.cmpfsize = ftell(f);

	fseek(f, 0, SEEK_SET);  //same as rewind(f);

	res.tmp_buffer = malloc(res.cmpfsize + 1);

	res.compressedres = malloc(res.cmpfsize + 1);
	fread(res.compressedres, res.cmpfsize, 1, f);
	fclose(f);

	//FIXME - fa schifo
	res.uncompressedres = malloc(0x1024*0x1024*2);
    
	stream_decompress();

	//write uncompressed data
	printf("FIXME - point to new buffer\n");
	FILE *f2 = fopen(filename_unp,"wb");
	fwrite(res.uncompressedres, 1, res.uncfsize, f2);
	printf("File %s written\n",filename_unp);
	fclose(f2);
}
