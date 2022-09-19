# CRC-64

Simple cross-platform command-line tool for computing **CRC-64** ([ECMA-182](https://www.ecma-international.org/wp-content/uploads/ECMA-182_1st_edition_december_1992.pdf)) checksums.

The `crc64` utility writes to the standard output three whitespace separated fields for each input file. These fields are the CRC (Cyclic Redundancy Check) checksum of the file, the total number of octets in the file and the file name. If *no* file name is specified, the standard input is used.

The generator polynomial use for CRC-64 computations is: **`0x42F0E1EBA9EA3693`**


## Synopsis

```
crc64.exe [OPTIONS] [<file_1> [<file_2> ... <file_n>]]
```

## Options

| Option                      | Description                                            |
|-----------------------------|--------------------------------------------------------|
| `-h`, `--help`, `--version` | Show help screen / show version information            |
| `-b`, `--binary`            | Output digest in binary format (default is hex-string) |
| `-d`, `--decimal`           | Output digest in decimal string format                 |
| `-u`, `--upper-case`        | Print digest as upper-case (default is lower-case)     |
| `-p`, `--no-padding`        | Print digest **without** any leading zeros             |
| `-s`, `--silent`            | Suppress error messages                                |
| `-e`, `--ignore-errors`     | Ignore I/O errors and proceed with the next file       |
| `-f`, `--no-flush`          | Do **not** flush output stream after each file         |
| `-z`, `--init-with-zero`    | Initialize CRC with 0x000…000 (default is 0xFFF…FFF)   |
| `-n`, `--negate-final`      | Negate the final CRC result                            |
| `-l`, `--append-length`     | Append to the input its length for CRC computation     |
| `-t`, `--self-test`         | Run integrated self-test and exit program              |


## License

This work has been released under the **CC0 1.0 Universal** license.

For details, please refer to:  
<https://creativecommons.org/publicdomain/zero/1.0/legalcode>
