#ifndef ZN_TOKEN_H
#define ZN_TOKEN_H

#include "./types.h"

typedef enum : u8 {
  TOK_META_BEGIN = 0,
  TOK_NONE = TOK_META_BEGIN,  // NONE
  TOK_EOF,                    // EOF
  TOK_IDENTIFIER,             // identifier
  TOK_META_END,               // meta tokens sentinel

  TOK_OPR_BEGIN,
  TOK_TILDE = TOK_OPR_BEGIN,  // '~'
  TOK_AT,                     // '@'
  TOK_ADD_ASSIGN,             // '+='
  TOK_PLUS,                   // '+'
  TOK_MUL_ASSIGN,             // '*='
  TOK_ASTERISK,               // '*'
  TOK_DIV_ASSIGN,             // '/='
  TOK_SLASH,                  // '/'
  TOK_MOD_ASSIGN,             // '%='
  TOK_PERCENT,                // '%'
  TOK_XOR_ASSIGN,             // '^='
  TOK_CARET,                  // '^'
  TOK_EQ,                     // '=='
  TOK_FARROW,                 // '=>'
  TOK_ASSIGN,                 // '='
  TOK_NEQ,                    // '!='
  TOK_BANG,                   // '!'
  TOK_SUB_ASSIGN,             // '-='
  TOK_ARROW,                  // '->'
  TOK_MINUS,                  // '-'
  TOK_NIL_COALESCE_ASSIGN,    // '??='
  TOK_NIL_COALESCE,           // '??'
  TOK_QUESTION_DOT,           // '?.'
  TOK_QUESTION,               // '?'
  TOK_LOGICAL_AND_ASSIGN,     // '&&='
  TOK_LOGICAL_AND,            // '&&'
  TOK_AND_ASSIGN,             // '&='
  TOK_AMPERSAND,              // '&'
  TOK_LOGICAL_OR_ASSIGN,      // '||='
  TOK_LOGICAL_OR,             // '||'
  TOK_OR_ASSIGN,              // '|='
  TOK_PIPE,                   // '|'
  TOK_RSHIFT_ASSIGN,          // '>>='
  TOK_RSHIFT,                 // '>>'
  TOK_GTE,                    // '>='
  TOK_GT,                     // '>'
  TOK_SPACESHIP,              // '<=>'
  TOK_LSHIFT_ASSIGN,          // '<<='
  TOK_LSHIFT,                 // '<<'
  TOK_LTE,                    // '<='
  TOK_LT,                     // '<'
  TOK_ELLIPSIS,               // '...'
  TOK_INCLUSIVE_RANGE,        // '..='
  TOK_RANGE,                  // '..'
  TOK_DEREF,                  // '.*'
  TOK_DOT,                    // '.'
  TOK_OPR_END,                // operator tokens sentinel

  TOK_KWD_BEGIN,
  TOK_FN = TOK_KWD_BEGIN,  // 'fn'
  TOK_IF,                  // 'if'
  TOK_IN,                  // 'in'
  TOK_IS,                  // 'is'
  TOK_ASM,                 // 'asm'
  TOK_VAR,                 // 'var'
  TOK_ELSE,                // 'else'
  TOK_ENUM,                // 'enum'
  TOK_GOTO,                // 'goto'
  TOK_LOOP,                // 'loop'
  TOK_PROP,                // 'prop'
  TOK_WITH,                // 'with'
  TOK_ALIAS,               // 'alias'
  TOK_BREAK,               // 'break'
  TOK_CATCH,               // 'catch'
  TOK_CONST,               // 'const'
  TOK_DEFER,               // 'defer'
  TOK_EXPORT,              // 'export'
  TOK_IMPORT,              // 'import'
  TOK_RETURN,              // 'return'
  TOK_SIZEOF,              // 'sizeof'
  TOK_STRUCT,              // 'struct'
  TOK_TYPEOF,              // 'typeof'
  TOK_ALIGNOF,             // 'alignof'
  TOK_DEFAULT,             // 'default'
  TOK_CONTINUE,            // 'continue'
  TOK_OFFSETOF,            // 'offsetof'
  TOK_KWD_END,             // keyword tokens sentinel

  TOK_PRIM_BEGIN,
  TOK_BOOL = TOK_PRIM_BEGIN,  // 'bool'
  TOK_I8,                     // 'i8'
  TOK_I16,                    // 'i16'
  TOK_I32,                    // 'i32'
  TOK_I64,                    // 'i64'
  TOK_U8,                     // 'u8'
  TOK_U16,                    // 'u16'
  TOK_U32,                    // 'u32'
  TOK_U64,                    // 'u64'
  TOK_F32,                    // 'f32'
  TOK_F64,                    // 'f64'
  TOK_PRIM_END,               // primitive tokens sentinel

  TOK_DELIM_BEGIN,
  TOK_LPAREN = TOK_DELIM_BEGIN,  // '('
  TOK_RPAREN,                    // ')'
  TOK_LBRACE,                    // '{'
  TOK_RBRACE,                    // '}'
  TOK_LBRACKET,                  // '['
  TOK_RBRACKET,                  // ']'
  TOK_COMMA,                     // ','
  TOK_COLON,                     // ':'
  TOK_SEMICOLON,                 // ';'
  TOK_DELIM_END,                 // delimiter tokens sentinel

  TOK_CONST_BEGIN,
  TOK_NIL = TOK_CONST_BEGIN,  // 'nil'
  TOK_NAN,                    // 'nan'
  TOK_INF,                    // 'inf'
  TOK_TRUE,                   // 'true'
  TOK_FALSE,                  // 'false'
  TOK_UNDEFINED,              // 'undefined'
  TOK_CONST_END,              // constant tokens sentinel

  TOK_LIT_BEGIN,
  TOK_CHARL = TOK_LIT_BEGIN,  // ''\0''
  TOK_INTL,                   // '0'
  TOK_FLOATL,                 // '0.0'
  TOK_STRINGL,                // '"string"'
  TOK_LIT_END,                // literal tokens sentinel
  TOK_END,                    // tokentypes sentinel (literal_sentinel + 1)
} TokenType;

typedef enum : u8 {
  TOK_FLAG_NONE = 0,
  TOK_FLAG_ESCAPED = 1 << 0,
  TOK_FLAG_INVALID = 1 << 1,
} TokenFlag;

// Token { offset: u32, len: u16, type: u8, flag: u8 }
typedef struct {
  u32 offset;
  u16 len;
  TokenType type;
  TokenFlag flag;
} Token;

// TokenBuf { data: Token, len: u32, cap: u32 }
typedef struct {
  Token* data;
  u32 len;
  u32 cap;
} TokenBuf;

enum : u32 {
  ZN_INFO_TOKEN_COUNT_META = TOK_META_END - TOK_META_BEGIN,
  ZN_INFO_TOKEN_COUNT_OPERATOR = TOK_OPR_END - TOK_OPR_BEGIN,
  ZN_INFO_TOKEN_COUNT_KEYWORD = TOK_KWD_END - TOK_KWD_BEGIN,
  ZN_INFO_TOKEN_COUNT_PRIMITIVE = TOK_PRIM_END - TOK_PRIM_BEGIN,
  ZN_INFO_TOKEN_COUNT_DELIMITER = TOK_DELIM_END - TOK_DELIM_BEGIN,
  ZN_INFO_TOKEN_COUNT_CONSTANT = TOK_CONST_END - TOK_CONST_BEGIN,
  ZN_INFO_TOKEN_COUNT_LITERAL = TOK_LIT_END - TOK_LIT_BEGIN,
  ZN_INFO_TOKEN_COUNT =
      (ZN_INFO_TOKEN_COUNT_META + ZN_INFO_TOKEN_COUNT_OPERATOR +
       ZN_INFO_TOKEN_COUNT_KEYWORD + ZN_INFO_TOKEN_COUNT_PRIMITIVE +
       ZN_INFO_TOKEN_COUNT_DELIMITER + ZN_INFO_TOKEN_COUNT_CONSTANT +
       ZN_INFO_TOKEN_COUNT_LITERAL),
};

const u8* token_type_name(TokenType tt);
const u8* token_type_lexeme(TokenType tt);
const u8* token_type_category(TokenType tt);

#endif