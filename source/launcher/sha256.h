#include <stdint.h>

struct sha256_state {
  uint64_t length;
  unsigned long state[8], curlen;
  unsigned char buf[64];
};
typedef struct sha256_state my_sha256_ctx;

void my_sha256_init(struct sha256_state *md);
int my_sha256_update(struct sha256_state *md, const unsigned char *in, unsigned long inlen);
int my_sha256_final(unsigned char *out, struct sha256_state *md);
