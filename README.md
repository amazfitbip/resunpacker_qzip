# Resunpacker qzip

This is a res unpacker (and packer) for amazfit bip/cor (and perhaps newer smartwatch/band)

The unpacker has been written by analyzing the firmware of amazfit cor. After some research i've discovered that the routine decompression is based of QuickLZ (the FAST compression library) http://www.quicklz.com/.

So I've rewritten the code to use this library.

The packer/unpacker is a great hack... I've spend very low time and have some limitation. (Eg. static malloc size...)
if you need to pack/unpack bigger file you may need to increase the malloc instruction parameters... Feel free to submit pull request.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. See deployment for notes on how to deploy the project on a live system.

### Prerequisites

To build the code you should just need gcc and make.

```
make
```

### Running

just run
```
./respacker <filename>
```

or

```
./resunpacker <filename>
```

to get the unpacked resource.

Please note that if you unpack then repack a file you can get a slight different but valid file!

### Warranty

This project come without any warranty. I don't assume any responsability for device bricks.

Trust me.. it worked for me but has passed many time after the last time i've used it.

I release it becouse I don't want my effort get lost.

Eddi


#Greetings

Thanks to GizRom team for providing support, Ferruccio and Yener for help and testing.
