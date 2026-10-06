#include "./include/lexer.h"

#include <assert.h>
#include <stdlib.h>

static inline u8 to_lower(u8 c) { return (c >= 'A' && c <= 'Z') ? c + 32 : c; }

static inline bool is_nl(u8 c) { return (c == '\n' || c == '\r'); }
static inline bool is_ws(u8 c) { return (c == ' ' || c == '\t'); }
static inline bool is_digit(u8 c, u8 base) {
  switch (base) {
    case 2:
      return (c >= '0' && c <= '1');
    case 8:
      return (c >= '0' && c <= '7');
    case 10:
      return (c >= '0' && c <= '9');
    case 16:
      return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') ||
             (c >= 'A' && c <= 'F');
    default:
      return false;
  }
}
static inline bool is_alpha(u8 c) {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}
static inline bool is_alnum(u8 c) { return is_alpha(c) || is_digit(c, 10); }
static inline bool is_delimiter(u8 c) {
  return (c == '(' || c == ')' || c == '[' || c == ']' || c == '{' ||
          c == '}' || c == ',' || c == ':' || c == ';');
}
static inline bool is_operator_start(u8 c) {
  return (c == '~' || c == '@' || c == '+' || c == '*' || c == '/' ||
          c == '%' || c == '^' || c == '=' || c == '!' || c == '?' ||
          c == '-' || c == '&' || c == '|' || c == '<' || c == '>' || c == '.');
}
static bool is_integer(const u8* start, u16 len, u8 base) {
  if (len == 0) return false;
  if (start[0] == '_' || start[len - 1] == '_') return false;

  u16 i = 0;

  if (base != 10) {
    if (len < 3 || start[0] != '0') return false;

    u8 prefix = to_lower(start[1]);
    if ((base == 2 && prefix != 'b') || (base == 8 && prefix != 'o') ||
        (base == 16 && prefix != 'x')) {
      return false;
    }

    i = 2;
    if (start[i] == '_') return false;
  }

  bool prev_was_separator = false;

  for (; i < len; ++i) {
    if (start[i] == '_') {
      if (prev_was_separator) return false;
      prev_was_separator = true;
      continue;
    }

    prev_was_separator = false;

    if (!is_digit(start[i], base)) return false;
  }

  return true;
}

static inline bool is_at_end(const Lexer* lx) { return lx->cursor >= lx->end; }
static inline u8 peek(const Lexer* lx) {
  return is_at_end(lx) ? '\0' : *(lx->cursor);
}
static inline u8 peek_next(const Lexer* lx) {
  if (lx->cursor >= lx->end) return '\0';
  if (lx->cursor + 1 >= lx->end) return '\0';
  return lx->cursor[1];
}
static inline u8 advance(Lexer* lx) {
  return is_at_end(lx) ? '\0' : *(lx->cursor++);
}

static zns push_token(TokenBuf* tbuf, Token tok) {
  if (tbuf->len >= tbuf->cap) {
    u32 new_cap = tbuf->cap ? tbuf->cap * 2 : ZND_TOKBUF_CAPACITY;
    Token* tmp = realloc(tbuf->data, new_cap * sizeof(*tmp));

    if (!tmp) return ZNS_ALLOC_ERR;

    tbuf->data = tmp;
    tbuf->cap = new_cap;
  }

  tbuf->data[tbuf->len] = tok;
  tbuf->len += 1;
  return ZNS_OK;
}

static zns push_delimiter(u8 c, TokenBuf* tbuf, u32 offset) {
  TokenType tt;
  switch (c) {
    case '(': {
      tt = TOK_LPAREN;
      break;
    }
    case ')': {
      tt = TOK_RPAREN;
      break;
    }
    case '[': {
      tt = TOK_LBRACKET;
      break;
    }
    case ']': {
      tt = TOK_RBRACKET;
      break;
    }
    case '{': {
      tt = TOK_LBRACE;
      break;
    }
    case '}': {
      tt = TOK_RBRACE;
      break;
    }
    case ',': {
      tt = TOK_COMMA;
      break;
    }
    case ':': {
      tt = TOK_COLON;
      break;
    }
    case ';': {
      tt = TOK_SEMICOLON;
      break;
    }
    default: {
      return ZNS_OK;
    }
  }
  return push_token(tbuf, (Token){offset, 1, tt, 0});
}

static zns push_operator(u8 c, Lexer* lx, TokenBuf* tbuf, u32 offset) {
  TokenType stt;
  u16 len = 1;

  switch (c) {
    case '~': {
      stt = TOK_TILDE;
      break;
    }
    case '@': {
      stt = TOK_AT;
      break;
    }
    case '+': {
      if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_ADD_ASSIGN;
        len = 2;
      } else {
        stt = TOK_PLUS;
      }
      break;
    }
    case '*': {
      if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_MUL_ASSIGN;
        len = 2;
      } else {
        stt = TOK_ASTERISK;
      }
      break;
    }
    case '/': {
      if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_DIV_ASSIGN;
        len = 2;
      } else {
        stt = TOK_SLASH;
      }
      break;
    }
    case '%': {
      if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_MOD_ASSIGN;
        len = 2;
      } else {
        stt = TOK_PERCENT;
      }
      break;
    }
    case '^': {
      if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_XOR_ASSIGN;
        len = 2;
      } else {
        stt = TOK_CARET;
      }
      break;
    }
    case '=': {
      if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_EQ;
        len = 2;
      } else if (peek(lx) == '>') {
        advance(lx);
        stt = TOK_FARROW;
        len = 2;
      } else {
        stt = TOK_ASSIGN;
      }
      break;
    }
    case '!': {
      if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_NEQ;
        len = 2;
      } else {
        stt = TOK_BANG;
      }
      break;
    }
    case '-': {
      if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_SUB_ASSIGN;
        len = 2;
      } else if (peek(lx) == '>') {
        advance(lx);
        stt = TOK_ARROW;
        len = 2;
      } else {
        stt = TOK_MINUS;
      }
      break;
    }
    case '?': {
      if (peek(lx) == '?') {
        advance(lx);
        if (peek(lx) == '=') {
          stt = TOK_NIL_COALESCE_ASSIGN;
          len = 3;
        } else {
          stt = TOK_NIL_COALESCE;
          len = 2;
        }
      } else if (peek(lx) == '.') {
        advance(lx);
        stt = TOK_QUESTION_DOT;
        len = 2;
      } else {
        stt = TOK_QUESTION;
      }
      break;
    }
    case '&': {
      if (peek(lx) == '&') {
        advance(lx);
        if (peek(lx) == '=') {
          advance(lx);
          stt = TOK_LOGICAL_AND_ASSIGN;
          len = 3;
        } else {
          stt = TOK_LOGICAL_AND;
          len = 2;
        }
      } else if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_AND_ASSIGN;
        len = 2;
      } else {
        stt = TOK_AMPERSAND;
      }
      break;
    }
    case '|': {
      if (peek(lx) == '|') {
        advance(lx);
        if (peek(lx) == '=') {
          advance(lx);
          stt = TOK_LOGICAL_OR_ASSIGN;
          len = 3;
        } else {
          stt = TOK_LOGICAL_OR;
          len = 2;
        }
      } else if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_OR_ASSIGN;
        len = 2;
      } else {
        stt = TOK_PIPE;
      }
      break;
    }
    case '<': {
      if (peek(lx) == '<') {
        advance(lx);
        if (peek(lx) == '=') {
          advance(lx);
          stt = TOK_LSHIFT_ASSIGN;
          len = 3;
        } else {
          stt = TOK_LSHIFT;
          len = 2;
        }
      } else if (peek(lx) == '=') {
        advance(lx);
        if (peek(lx) == '>') {
          advance(lx);
          stt = TOK_SPACESHIP;
          len = 3;
        } else {
          stt = TOK_LTE;
          len = 2;
        }
      } else {
        stt = TOK_LT;
      }
      break;
    }
    case '>': {
      if (peek(lx) == '>') {
        advance(lx);
        if (peek(lx) == '=') {
          advance(lx);
          stt = TOK_RSHIFT_ASSIGN;
          len = 3;
        } else {
          stt = TOK_RSHIFT;
          len = 2;
        }
      } else if (peek(lx) == '=') {
        advance(lx);
        stt = TOK_GTE;
        len = 2;
      } else {
        stt = TOK_GT;
      }
      break;
    }
    case '.': {
      if (peek(lx) == '.') {
        advance(lx);
        if (peek(lx) == '.') {
          advance(lx);
          stt = TOK_ELLIPSIS;
          len = 3;
        } else if (peek(lx) == '=') {
          advance(lx);
          stt = TOK_INCLUSIVE_RANGE;
          len = 3;
        } else {
          stt = TOK_RANGE;
          len = 2;
        }
      } else if (peek(lx) == '*') {
        advance(lx);
        stt = TOK_DEREF;
        len = 2;
      } else {
        stt = TOK_DOT;
      }
      break;
    }
    default: {
      return ZNS_OK;
    }
  }
  return push_token(tbuf, (Token){offset, len, stt, 0});
}

static zns push_nl_offset(Lexer* lx, u32 offset) {
  U32Vec* vec = &lx->nl_offs;

  if (vec->len >= vec->cap) {
    u32 new_cap = vec->cap ? vec->cap * 2 : ZND_NLBUF_CAPACITY;
    u32* tmp = realloc(vec->data, new_cap * sizeof(*tmp));

    if (!tmp) return ZNS_ALLOC_ERR;

    vec->data = tmp;
    vec->cap = new_cap;
  }

  vec->data[vec->len] = offset;
  vec->len += 1;

  return ZNS_OK;
}

static TokenType match_keyword_or_identifier(const u8* text, u16 len) {
  // Hash or string table lookups for keywords
  struct {
    const char* kwd;
    TokenType type;
  } keywords[] = {
      {"fn", TOK_FN},
      {"if", TOK_IF},
      {"in", TOK_IN},
      {"is", TOK_IS},
      {"asm", TOK_ASM},
      {"pub", TOK_EXPORT},
      {"var", TOK_VAR},
      {"else", TOK_ELSE},
      {"enum", TOK_ENUM},
      {"goto", TOK_GOTO},
      {"loop", TOK_LOOP},
      {"prop", TOK_PROP},
      {"with", TOK_WITH},
      {"alias", TOK_ALIAS},
      {"break", TOK_BREAK},
      {"catch", TOK_CATCH},
      {"const", TOK_CONST},
      {"defer", TOK_DEFER},
      {"import", TOK_IMPORT},
      {"return", TOK_RETURN},
      {"sizeof", TOK_SIZEOF},
      {"struct", TOK_STRUCT},
      {"alignof", TOK_ALIGNOF},
      {"default", TOK_DEFAULT},
      {"continue", TOK_CONTINUE},
      {"offsetof", TOK_OFFSETOF},

      // Primitives
      {"bool", TOK_BOOL},
      {"i8", TOK_I8},
      {"i16", TOK_I16},
      {"i32", TOK_I32},
      {"i64", TOK_I64},
      {"u8", TOK_U8},
      {"u16", TOK_U16},
      {"u32", TOK_U32},
      {"u64", TOK_U64},
      {"f32", TOK_F32},
      {"f64", TOK_F64},

      // Constants
      {"nil", TOK_NIL},
      {"nan", TOK_NAN},
      {"inf", TOK_INF},
      {"true", TOK_TRUE},
      {"false", TOK_FALSE},
      {"undefined", TOK_UNDEFINED},
  };

  u32 kwd_count = sizeof(keywords) / sizeof(keywords[0]);
  for (u32 i = 0; i < kwd_count; ++i) {
    u16 klen = 0;
    while (keywords[i].kwd[klen]) klen++;
    if (klen == len) {
      bool match = true;
      for (u16 j = 0; j < len; ++j) {
        if (text[j] != (u8)keywords[i].kwd[j]) {
          match = false;
          break;
        }
      }
      if (match) return keywords[i].type;
    }
  }

  return TOK_IDENTIFIER;
}

static zns lex_whitespace(Lexer* lx) {
  while (!is_at_end(lx)) {
    u8 c = peek(lx);

    // newline: '\n' OR '\r' OR '\r''\n'
    if (is_nl(c)) {
      u32 offset = (u32)(lx->cursor - lx->start);

      if (c == '\r' && peek_next(lx) == '\n') {
        advance(lx);
        advance(lx);
      } else {
        advance(lx);
      }

      zns status = push_nl_offset(lx, offset);
      if (status != ZNS_OK) return status;

      continue;
    }

    // whitespace: '\t' OR ' '
    if (is_ws(c)) {
      advance(lx);
      continue;
    }

    // Line comment
    if (c == '/' && peek_next(lx) == '/') {
      while (!is_at_end(lx) && !is_nl(peek(lx))) advance(lx);

      continue;
    }

    // Block comment
    if (c == '/' && peek_next(lx) == '*') {
      advance(lx);
      advance(lx);

      while (!is_at_end(lx) && !(peek(lx) == '*' && peek_next(lx) == '/')) {
        if (is_nl(peek(lx))) {
          u32 offset = (u32)(lx->cursor - lx->start);

          if (peek(lx) == '\r' && peek_next(lx) == '\n') {
            advance(lx);
            advance(lx);
          } else {
            advance(lx);
          }

          zns status = push_nl_offset(lx, offset);
          if (status != ZNS_OK) return status;
        } else {
          advance(lx);
        }
      }

      if (!is_at_end(lx)) {
        advance(lx);
        advance(lx);
      }

      continue;
    }

    break;
  }

  return ZNS_OK;
}

static zns lex_identifier(Lexer* lx, TokenBuf* tbuf, u32 offset) {
  while (!is_at_end(lx) && (is_alnum(peek(lx)) || peek(lx) == '_')) {
    advance(lx);
  }

  u16 len = (u16)((lx->cursor - lx->start) - offset);
  TokenType type = match_keyword_or_identifier(lx->start + offset, len);
  return push_token(tbuf, (Token){offset, len, type, TOK_FLAG_NONE});
}

static zns lex_number(Lexer* lx, TokenBuf* tbuf, u32 offset) {
  u8 base = 10;

  if (lx->start[offset] == '0') {
    u8 prefix = to_lower(peek(lx));
    if (prefix == 'b') {
      base = 2;
      advance(lx);
    } else if (prefix == 'o') {
      base = 8;
      advance(lx);
    } else if (prefix == 'x') {
      base = 16;
      advance(lx);
    }
  }

  while (!is_at_end(lx) && (is_digit(peek(lx), base) || peek(lx) == '_')) {
    advance(lx);
  }

  TokenType type = TOK_INTL;

  // Float check (only applies if base 10 and followed by '.' not '..')
  if (base == 10 && peek(lx) == '.' && peek_next(lx) != '.' &&
      peek_next(lx) != '=') {
    type = TOK_FLOATL;
    advance(lx);  // consume '.'

    while (!is_at_end(lx) && (is_digit(peek(lx), 10) || peek(lx) == '_')) {
      advance(lx);
    }
  }

  u16 len = (u16)((lx->cursor - lx->start) - offset);
  bool valid =
      is_integer(lx->start + offset, len, base) || (type == TOK_FLOATL);

  TokenFlag flag = valid ? TOK_FLAG_NONE : TOK_FLAG_INVALID;
  return push_token(tbuf, (Token){offset, len, type, flag});
}

static zns lex_string(Lexer* lx, TokenBuf* tbuf, u32 offset) {
  TokenFlag flag = TOK_FLAG_NONE;

  while (!is_at_end(lx) && peek(lx) != '"') {
    if (peek(lx) == '\\') {
      flag = TOK_FLAG_ESCAPED;
      advance(lx);  // Skip backslash
    }
    if (is_nl(peek(lx))) break;  // Unterminated string on newline
    advance(lx);
  }

  if (peek(lx) == '"') {
    advance(lx);  // Consume closing '"'
  } else {
    flag = TOK_FLAG_INVALID;
  }

  u16 len = (u16)((lx->cursor - lx->start) - offset);
  return push_token(tbuf, (Token){offset, len, TOK_STRINGL, flag});
}

static zns lex_char(Lexer* lx, TokenBuf* tbuf, u32 offset) {
  TokenFlag flag = TOK_FLAG_NONE;

  if (peek(lx) == '\\') {
    flag = TOK_FLAG_ESCAPED;
    advance(lx);
  }
  advance(lx);  // Consume character byte

  if (peek(lx) == '\'') {
    advance(lx);  // Consume closing '\''
  } else {
    flag = TOK_FLAG_INVALID;
  }

  u16 len = (u16)((lx->cursor - lx->start) - offset);
  return push_token(tbuf, (Token){offset, len, TOK_CHARL, flag});
}

static u32 line_for_offset(const Lexer* lx, u32 offset) {
  u32 lo = 0;
  u32 hi = lx->nl_offs.len;

  while (lo < hi) {
    u32 mid = lo + (hi - lo) / 2;

    if (lx->nl_offs.data[mid] < offset) {
      lo = mid + 1;
    } else {
      hi = mid;
    }
  }

  return lo + 1;
}
static u32 column_for_offset(const Lexer* lx, u32 offset) {
  u32 lo = 0;
  u32 hi = lx->nl_offs.len;

  while (lo < hi) {
    u32 mid = lo + (hi - lo) / 2;

    if (lx->nl_offs.data[mid] < offset) {
      lo = mid + 1;
    } else {
      hi = mid;
    }
  }

  if (lo == 0) {
    return offset + 1;
  }

  return offset - lx->nl_offs.data[lo - 1];
}

void lex_dump(FILE* out, const Lexer* lx, const TokenBuf* tbuf) {
  for (u32 i = 0; i < tbuf->len; ++i) {
    Token tok = tbuf->data[i];
    const u8* lexeme = lx->start + tok.offset;

    u32 line = line_for_offset(lx, tok.offset);
    u32 column = column_for_offset(lx, tok.offset);

    fprintf(out, "%s '%.*s' %s:%u:%u\n", token_type_name(tok.type), tok.len,
            lexeme, lx->filepath, line, column);
  }
}

zns lex(Lexer* lx, TokenBuf* tbuf) {
  zns status = ZNS_OK;

  while (!is_at_end(lx)) {
    status = lex_whitespace(lx);
    if (status != ZNS_OK) return status;

    if (is_at_end(lx)) break;

    const u8* tok_start = lx->cursor;
    u32 offset = (u32)(tok_start - lx->start);
    u8 c = advance(lx);

    if (is_delimiter(c)) {
      status = push_delimiter(c, tbuf, offset);
      if (status != ZNS_OK) return status;
      continue;
    }
    if (is_operator_start(c)) {
      status = push_operator(c, lx, tbuf, offset);
      if (status != ZNS_OK) return status;
      continue;
    }

    switch (c) {
      case '"':
        status = lex_string(lx, tbuf, offset);
        break;
      case '\'':
        status = lex_char(lx, tbuf, offset);
        break;
      default:
        if (is_alpha(c) || c == '_') {
          status = lex_identifier(lx, tbuf, offset);
        } else if (is_digit(c, 10)) {
          status = lex_number(lx, tbuf, offset);
        } else {
          status =
              push_token(tbuf, (Token){offset, 1, TOK_NONE, TOK_FLAG_INVALID});
        }
        break;
    }

    if (status != ZNS_OK) return status;
  }

  assert(lx->cursor == lx->end);

  Token teof = (Token){
      .offset = (u32)(lx->end - lx->start),
      .len = 0,
      .type = TOK_EOF,
      .flag = TOK_FLAG_NONE,
  };

  status = push_token(tbuf, teof);

  return status;
}