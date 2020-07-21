# PHA

(Pytness' Hash Algorithm)

A custom hash algorithm made in c++

Supports 256 bit and 512 bit digests

#### This is a non secure hash algorithm and it should not be used for any production purposes

## phasum
#### Build

```bash
make
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

```
$ ./sha2sum --help
Usage: sha2sum [OPTION...]
Print SHA2 checksums

  -c, --capitalize           print checksum in capital letters
  -i, --input                take input from argument
  -l, --newline              append a \n to the output
  -r, --raw                  print raw checksum instead of hex
  -?, --help                 Give this help list
      --usage                Give a short usage message
```


```
$ ./hps
```
