#all: qcat qzip qunzip quicklz.so resunpacker respacker
all: quicklz.so resunpacker respacker

# Uncomment these for debugging
#LDFLAGS=-g
#CFLAGS=-g

#Uncomment these for Linux
LDFLAGS=-O3
#CFLAGS=-O3
CFLAGS=-O3 -DQLZ_STREAMING_BUFFER=0 -DQLZ_COMPRESSION_LEVEL=3
CC=gcc

qcat: qzip.o quicklz.o
	${CC} ${LDFLAGS} -o qcat qzip.o quicklz.o
	strip qcat


qzip: qzip.o quicklz.o
	${CC} ${LDFLAGS} -o qzip qzip.o quicklz.o
	strip qzip


qunzip: qzip.o quicklz.o
	${CC} ${LDFLAGS} -o qunzip qzip.o quicklz.o
	strip qunzip

resunpacker: resunpacker.o quicklz.o
	${CC} ${CFLAGS_HUAMI} ${LDFLAGS} -o resunpacker resunpacker.o quicklz.o
	strip resunpacker

respacker: respacker.o quicklz.o
	${CC} ${CFLAGS_HUAMI} ${LDFLAGS} -o respacker respacker.o quicklz.o
	strip respacker

quicklz.so: quicklz.o distutils
	python distutils build


clean:
	rm -f resunpacker respacker qcat qzip qunzip quicklz.so *.o
	rm -rf build/
