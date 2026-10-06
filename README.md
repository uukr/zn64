# zn64

A zero-dependency C23 lexer built from scratch for a custom programming language.

I started this project to get hands-on experience with C, compiler design, low-level memory management, and performance engineering.

###### I'm starting to believe premature optimization is the root of all evil.

## Features

- **Zero Dependencies** - Built entirely with standard C23.
- **Single-Pass Lexing** - Scans source buffers directly using byte cursors.
- **Compact Token Layout** - Uses compact 8-byte token structures for better cache locality.
- **Position Tracking** - Stores newline offsets for fast line/column resolution via binary search.
- **Rich Token Set** - Supports multi-character operators, integer, float, string, and character literals, plus hex/octal/binary numbers and language keywords.

## Architecture

- `src/include/types.h` - Fixed-width integer types and status codes.
- `src/include/token.h` - Token types, flags, and token buffer layouts.
- `src/include/lexer.h` - Lexer state, cursor logic, and token matching routines.
- `docs/TOKENS` - Token definitions for the lexer.
- `docs/sample.zn` - Sample source file exercising the lexer token set.

## Building and Running

The `zn.sh` helper script provides build, run, and cleanup commands.

```bash
# Build
./zn.sh build

# Run on a source file
./zn.sh run path/to/file.zn

# Clean build artifacts
./zn.sh clean
```
