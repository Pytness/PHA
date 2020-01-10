# PHA

(Pytness' Hash Algorithm)

A custom hash algorithm made in c++

Supports 256 bit and 512 bit digests

## phasum
#### Compile

```bash
g++ lib/pha.cpp src/phasum.cpp -o phasum

# or

cd src; ./build.sh
```

#### Usage

```
$ ./phasum --help
Usage: phasum [OPTION...]
Print PHA checksums

  -a, --algorithm            256 (default), 512
  -c, --capitalize           print checksum in capital letters
  -i, --input                take input from argument
  -l, --newline              append a \n to the output
  -r, --raw                  print raw checksum instead of hex
  -?, --help                 Give this help list
      --usage                Give a short usage message
```
