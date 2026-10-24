# Cipherbox

## Usage

### Compile
```
gcc ./src/main.c ./src/flip_bits.c ./src/strip_suffix.c ./src/xor_bits.c -o cipherbox
```

### Encrypt a file
```
./cipherbox file.txt -flag (-key)
```

You will get a file called `file.txt.cbx`.

### Decrypt a file
```
./cipherbox file.txt.cbx -flag (-key)
```

The program will automatically detect whether the file was encrypted by it.

## File format

| Bytes        | Content                                |
|--------------|----------------------------------------|
| 13           | Magic `cipherbox_enc`                  |
| 1            | Method: `0` = bit flip, `1` = XOR      |
| 1            | Length `n` of the original file name   |
| `n`          | Original file name (encrypted)         |
| rest         | File contents (encrypted)              |

## Flags

`-flip` using a flip bits method to encrypt file.

`-xor` using a xor method to encrypt file, password is needed.
