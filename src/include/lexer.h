#ifndef ZN_LEXER_H
#define ZN_LEXER_H

#include <stdio.h>

#include "./token.h"
#include "./types.h"

// Lexer { start: const *u8, cursor: const *u8, end: const *u8, nl_offs: U32Vec
// }
typedef struct {
  const u8* start;
  const u8* cursor;
  const u8* end;
  const u8* filepath;

  U32Vec nl_offs;  // newline-offsets vector
} Lexer;

zns lex(Lexer* lx, TokenBuf* tbuf);
void lex_dump(FILE* out, const Lexer* lx, const TokenBuf* tbuf);

#endif