# patchbin - Binary File Patcher

## Overview

`patchbin` is a command-line utility for patching binary files. It provides a simple and intuitive way to modify binary files by inserting, replacing, or performing arithmetic operations on specific offsets within the file.

## Features

- Insert bytes at a given offset (increases file size)
- Replace bytes at a given offset (keeps file size)
- Add/Subtract values from existing data with a given data type
- Apply patches to multiple files

## Usage

### Basic Syntax

```
patchbin [options]
```
### Options

|Option|Arguments|Description|
|-|-|-|
| `-f`,`--file` | `[File Path]` | Specifies the file to be patched |
| `-d`,`--directory` | `[Directory Path]` | Specifies a directory in which all files will be patched. |
| `-h`,`--help` |  | Print help information |
| `-i`,`--insert` | `[Offset] [Bytes]` | Insert bytes at a given offset |
| `-r`,`--replace` | `[Offset] [Bytes]` | Replaces bytes at a given offset |
| `-a`,`--add` | `[Offset] [Data Type] [Value]` | Add/Subtract a value to the bytes at a given offset |
| `-s`,`--set` | `[Offset] [Data Type] [Value]` | Set data at a given offset to a specific value |
| `-e`,`--endian` | `[Endian]` | Select the endian for the given patches.<br>Choices: little, big (default: little) |

The `--add` and `--set` operations support the following data types:

| Type | Description | Size |
|-|-|-|
| `s8` | Signed 8-bit integer | 1 Byte |
| `u8` | Unsigned 8-bit integer | 1 Byte |
| `s16` | Signed 16-bit integer | 2 Bytes |
| `u16` | Unsigned 16-bit integer | 2 Bytes |
| `s32` | Signed 32-bit integer | 4 Bytes |
| `u32` | Unsigned 32-bit integer | 4 Bytes |
| `s64` | Signed 64-bit integer | 8 Bytes |
| `u64` | Unsigned 64-bit integer | 8 Bytes |
| `f32` | 32-bit floating point (float) | 4 Bytes |
| `f64` | 64-bit floating point (double) | 8 Bytes |

Multiple files can be specified by adding them each prefixed with a `-f`/`--file` specifier.
Example:

`patchbin -f file1.bin -f file2.bin -f file3.bin -r 0x00 FF`

# Building

Requirements: cmake >=3.10, GNU Make and a C compiler

```bash
git clone https://github.com/jagger1407/patchbin
cd patchbin
cmake .
make
```
