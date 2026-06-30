# BF-JIT

A JIT compiler for the [Brainfuck](https://en.wikipedia.org/wiki/Brainfuck) language.

BF-JIT reads a Brainfuck source file, lowers it to a small intermediate
representation (IR), applies a few peephole optimisations, and then uses
[GNU Lightning](https://www.gnu.org/software/lightning/) to JIT-compile the IR
to native machine code which is executed directly.

## How it works

1. **Parse** — the source file is read and any non-instruction characters are
   stripped.
2. **Lower to IR** — instructions are translated into an IR of typed opcodes
   (`intermediate_representation.c`). Runs of `+`/`-` and `>`/`<` are coalesced
   into a single instruction with a count argument.
3. **Optimise** — clear-cell loops (`[-]` / `[+]`) are folded into a single
   `OP_CLEAR_CELL`, and matching `[` / `]` jumps are resolved to their targets.
4. **JIT compile** — the IR is emitted to native code via GNU Lightning and run
   against a 30,000-cell tape (`jit.c`).

## Dependencies

- A C compiler with **C23** support (recent Clang or GCC).
- **CMake** 4.2 or newer.
- **GNU Lightning** (provides `lightning.h` and `liblightning`).

### Installing GNU Lightning

**macOS (Homebrew)**

```sh
brew install gnu-lightning
```

The `CMakeLists.txt` already adds `/opt/homebrew` to the search path on Apple
platforms, so CMake will find the Homebrew-installed library automatically.

**Debian / Ubuntu**

```sh
sudo apt-get install libgnu-lightning-dev
```

**From source** (any platform)

```sh
git clone https://git.savannah.gnu.org/git/lightning.git
cd lightning
./bootstrap && ./configure && make && sudo make install
```

## Building

This project uses CMake. From the repository root:

```sh
cmake -S . -B build
cmake --build build
```

This produces the `BF_JIT` executable at `build/BF_JIT`.

### Build type / optimisation flags

CMake's standard build-type flags apply. For an optimised release build:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

| Build type         | Compiler flags        | Use for                        |
| ------------------ | --------------------- | ------------------------------ |
| `Debug`            | `-g`                  | Development, debugging         |
| `Release`          | `-O3 -DNDEBUG`        | Fastest runtime                |
| `RelWithDebInfo`   | `-O2 -g -DNDEBUG`     | Optimised, with symbols        |
| `MinSizeRel`       | `-Os -DNDEBUG`        | Smallest binary                |

If GNU Lightning is installed in a non-standard location, point CMake at it:

```sh
cmake -S . -B build \
  -DLIGHTNING_INCLUDE_DIR=/path/to/include \
  -DLIGHTNING_LIBRARY=/path/to/liblightning.so
```

## Usage

```sh
./build/BF_JIT <filename> [-d]
```

| Argument     | Description                                                          |
| ------------ | ------------------------------------------------------------------- |
| `<filename>` | Path to the Brainfuck source file to compile and run.               |
| `-d`         | Dump the optimised IR to `<filename-without-extension>_dump.txt`.   |

### Example

```sh
./build/BF_JIT hello.bf
```

Dumping the IR alongside execution:

```sh
./build/BF_JIT hello.bf -d
# writes hello_dump.txt containing one IR instruction per line, e.g.
# OP_ADD 8
# OP_JUMP_IF_ZERO 12
# ...
```
