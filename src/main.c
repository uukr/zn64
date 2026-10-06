#include <stdio.h>
#include <stdlib.h>

#include "include/lexer.h"

static zns read_file(const u8* filepath, u8** buf, usz* b_size) {
  FILE* file = fopen((const char*)filepath, "rb");
  if (!file) return ZNS_FILE_OPEN_ERR;

  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    return ZNS_FILE_INVALID_ERR;
  }

  i64 raw_size = ftell(file);
  if (raw_size < 0) {
    fclose(file);
    return ZNS_FILE_INVALID_ERR;
  }
  *b_size = (usz)raw_size;

  rewind(file);

  *buf = malloc(*b_size + 1);

  if (!*buf) {
    fclose(file);
    return ZNS_ALLOC_ERR;
  }

  if (fread(*buf, 1, *b_size, file) != *b_size) {
    free(*buf);
    *buf = 0;
    *b_size = 0;
    fclose(file);
    return ZNS_FILE_INVALID_ERR;
  }

  (*buf)[*b_size] = '\0';
  fclose(file);
  return ZNS_OK;
}

static zns alloc_tbuff(TokenBuf* tbuf) {
  u32 cap = ZND_TOKBUF_CAPACITY;
  Token* tmp = malloc(sizeof(*tmp) * cap);
  if (!tmp) return ZNS_ALLOC_ERR;
  *tbuf = (TokenBuf){
      .data = tmp,
      .len = 0,
      .cap = cap,
  };
  return ZNS_OK;
}
static zns alloc_nlbuff(U32Vec* vec) {
  u32 cap = ZND_NLBUF_CAPACITY;
  u32* tmp = malloc(sizeof(*tmp) * cap);
  if (!tmp) return ZNS_ALLOC_ERR;
  *vec = (U32Vec){
      .data = tmp,
      .len = 0,
      .cap = cap,
  };
  return ZNS_OK;
}

int main(int argc, char** argv) {
  if (argc < 2) return 1;

  usz len = 0;
  u8* buf = 0;
  zns status = ZNS_OK;

  TokenBuf tbuf = {0};
  Lexer lx = {0};

  const u8* file = (const u8*)argv[argc - 1];
  status = read_file(file, &buf, &len);
  if (status) goto cleanup;

  lx = (Lexer){
      .start = buf,
      .cursor = buf,
      .end = buf + len,
      .filepath = file,
      .nl_offs = {0},
  };

  status = alloc_tbuff(&tbuf);
  if (status) goto cleanup;

  status = alloc_nlbuff(&lx.nl_offs);
  if (status) goto cleanup;

  status = lex(&lx, &tbuf);
  if (status) goto cleanup;

  if (status == ZNS_OK) lex_dump(stdout, &lx, &tbuf);

cleanup:
  free(lx.nl_offs.data);
  free(tbuf.data);
  free(buf);

  return (int)status;
}