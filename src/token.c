#include "./include/token.h"

static struct {
  const char* name;
  const char* lexeme;
} token_names[TOK_END] = {
    [TOK_NONE] = {"none", "NONE"},
    [TOK_EOF] = {"eof", "EOF"},
    [TOK_IDENTIFIER] = {"identifier", "{identifier}"},
    [TOK_TILDE] = {"tilde", "~"},
    [TOK_AT] = {"at", "@"},
    [TOK_ADD_ASSIGN] = {"add_assign", "+="},
    [TOK_PLUS] = {"plus", "+"},
    [TOK_MUL_ASSIGN] = {"mul_assign", "*="},
    [TOK_ASTERISK] = {"asterisk", "*"},
    [TOK_DIV_ASSIGN] = {"div_assign", "/="},
    [TOK_SLASH] = {"slash", "/"},
    [TOK_MOD_ASSIGN] = {"mod_assign", "%="},
    [TOK_PERCENT] = {"percent", "%"},
    [TOK_XOR_ASSIGN] = {"xor_assign", "^="},
    [TOK_CARET] = {"caret", "^"},
    [TOK_EQ] = {"eq", "=="},
    [TOK_FARROW] = {"farrow", "=>"},
    [TOK_ASSIGN] = {"assign", "="},
    [TOK_NEQ] = {"neq", "!="},
    [TOK_BANG] = {"bang", "!"},
    [TOK_SUB_ASSIGN] = {"sub_assign", "-="},
    [TOK_ARROW] = {"arrow", "->"},
    [TOK_MINUS] = {"minus", "-"},
    [TOK_NIL_COALESCE_ASSIGN] = {"nil_coalesce_assign", "?\?="},
    [TOK_NIL_COALESCE] = {"nil_coalesce", "?\?"},
    [TOK_QUESTION_DOT] = {"question_dot", "?."},
    [TOK_QUESTION] = {"question", "?"},
    [TOK_LOGICAL_AND_ASSIGN] = {"logical_and_assign", "&&="},
    [TOK_LOGICAL_AND] = {"logical_and", "&&"},
    [TOK_AND_ASSIGN] = {"and_assign", "&="},
    [TOK_AMPERSAND] = {"ampersand", "&"},
    [TOK_LOGICAL_OR_ASSIGN] = {"logical_or_assign", "||="},
    [TOK_LOGICAL_OR] = {"logical_or", "||"},
    [TOK_OR_ASSIGN] = {"or_assign", "|="},
    [TOK_PIPE] = {"pipe", "|"},
    [TOK_RSHIFT_ASSIGN] = {"rshift_assign", ">>="},
    [TOK_RSHIFT] = {"rshift", ">>"},
    [TOK_GTE] = {"gte", ">="},
    [TOK_GT] = {"gt", ">"},
    [TOK_SPACESHIP] = {"spaceship", "<=>"},
    [TOK_LSHIFT_ASSIGN] = {"lshift_assign", "<<="},
    [TOK_LSHIFT] = {"lshift", "<<"},
    [TOK_LTE] = {"lte", "<="},
    [TOK_LT] = {"lt", "<"},
    [TOK_ELLIPSIS] = {"ellipsis", "..."},
    [TOK_INCLUSIVE_RANGE] = {"inclusive_range", "..="},
    [TOK_RANGE] = {"range", ".."},
    [TOK_DEREF] = {"deref", ".*"},
    [TOK_DOT] = {"dot", "."},
    [TOK_FN] = {"fn", "fn"},
    [TOK_IF] = {"if", "if"},
    [TOK_IN] = {"in", "in"},
    [TOK_IS] = {"is", "is"},
    [TOK_ASM] = {"asm", "asm"},
    [TOK_VAR] = {"var", "var"},
    [TOK_ELSE] = {"else", "else"},
    [TOK_ENUM] = {"enum", "enum"},
    [TOK_GOTO] = {"goto", "goto"},
    [TOK_LOOP] = {"loop", "loop"},
    [TOK_PROP] = {"prop", "prop"},
    [TOK_WITH] = {"with", "with"},
    [TOK_ALIAS] = {"alias", "alias"},
    [TOK_BREAK] = {"break", "break"},
    [TOK_CATCH] = {"catch", "catch"},
    [TOK_CONST] = {"const", "const"},
    [TOK_DEFER] = {"defer", "defer"},
    [TOK_EXPORT] = {"export", "export"},
    [TOK_IMPORT] = {"import", "import"},
    [TOK_RETURN] = {"return", "return"},
    [TOK_SIZEOF] = {"sizeof", "sizeof"},
    [TOK_STRUCT] = {"struct", "struct"},
    [TOK_TYPEOF] = {"typeof", "typeof"},
    [TOK_ALIGNOF] = {"alignof", "alignof"},
    [TOK_DEFAULT] = {"default", "default"},
    [TOK_CONTINUE] = {"continue", "continue"},
    [TOK_OFFSETOF] = {"offsetof", "offsetof"},
    [TOK_BOOL] = {"bool", "bool"},
    [TOK_I8] = {"i8", "i8"},
    [TOK_I16] = {"i16", "i16"},
    [TOK_I32] = {"i32", "i32"},
    [TOK_I64] = {"i64", "i64"},
    [TOK_U8] = {"u8", "u8"},
    [TOK_U16] = {"u16", "u16"},
    [TOK_U32] = {"u32", "u32"},
    [TOK_U64] = {"u64", "u64"},
    [TOK_F32] = {"f32", "f32"},
    [TOK_F64] = {"f64", "f64"},
    [TOK_LPAREN] = {"lparen", "("},
    [TOK_RPAREN] = {"rparen", ")"},
    [TOK_LBRACE] = {"lbrace", "{"},
    [TOK_RBRACE] = {"rbrace", "}"},
    [TOK_LBRACKET] = {"lbracket", "["},
    [TOK_RBRACKET] = {"rbracket", "]"},
    [TOK_COMMA] = {"comma", ","},
    [TOK_COLON] = {"colon", ":"},
    [TOK_SEMICOLON] = {"semicolon", ";"},
    [TOK_NIL] = {"nil", "nil"},
    [TOK_NAN] = {"nan", "nan"},
    [TOK_INF] = {"inf", "inf"},
    [TOK_TRUE] = {"true", "true"},
    [TOK_FALSE] = {"false", "false"},
    [TOK_UNDEFINED] = {"undefined", "undefined"},
    [TOK_CHARL] = {"charl", "{charl}"},
    [TOK_INTL] = {"intl", "{intl}"},
    [TOK_FLOATL] = {"floatl", "{floatl}"},
    [TOK_STRINGL] = {"stringl", "{stringl}"},
};

const u8* token_type_name(TokenType tt) {
  if (tt >= TOK_END || token_names[tt].name == NULL)
    return (const u8*)"invalid";
  return (const u8*)token_names[tt].name;
}

const u8* token_type_lexeme(TokenType tt) {
  if (tt >= TOK_END || token_names[tt].name == NULL)
    return (const u8*)"invalid";
  return (const u8*)token_names[tt].lexeme;
}

const u8* token_type_category(TokenType tt) {
  if (tt >= TOK_META_BEGIN && tt < TOK_META_END) return (const u8*)"meta";
  if (tt >= TOK_OPR_BEGIN && tt < TOK_OPR_END) return (const u8*)"operator";
  if (tt >= TOK_KWD_BEGIN && tt < TOK_KWD_END) return (const u8*)"keyword";
  if (tt >= TOK_PRIM_BEGIN && tt < TOK_PRIM_END) return (const u8*)"primitive";
  if (tt >= TOK_DELIM_BEGIN && tt < TOK_DELIM_END)
    return (const u8*)"delimiter";
  if (tt >= TOK_CONST_BEGIN && tt < TOK_CONST_END) return (const u8*)"constant";
  if (tt >= TOK_LIT_BEGIN && tt < TOK_LIT_END) return (const u8*)"literal";

  return (const u8*)"invalid";
}