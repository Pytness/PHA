# PCHA

(Pytness' Custom Hash Algorithm)

A custom hash algorithm made in c++

Supports 256 bit and 512 bit digests

## pchasum
#### Compile

```bash
g++ lib/pcha.cpp src/pchasum.cpp -o pchasum
```

#### Usage

```
$ ./pchasum --help
Usage: pchasum [OPTION...]
Print PCHA checksums

  -a, --algorithm            256 (default), 512
  -c, --capitalize           print checksum in capital letters
  -i, --input                take input from argument
  -p, --prettify             prettify output
  -r, --raw                  print raw checksum instead of hex
  -?, --help                 Give this help list
      --usage                Give a short usage message
````
