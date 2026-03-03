#include "../../include/common.h"

int main() {
    const char *p = "/tmp/sbox_test.txt";
    FILE *fp = fopen(p, "wb");
    if (!fp) die("fopen /tmp/sbox_test.txt");
    const char *msg = "cs485 sandbox tmp write test\n";
    if (fwrite(msg, 1, strlen(msg), fp) != strlen(msg)) die("fwrite");
    fclose(fp);

    char *r = read_small_file(p, 256);
    if (!r) fail("Wrote file but could not read it back");
    printf("INFO: read back: %s", r);
    free(r);
    pass("Write/read in /tmp works inside sandbox");
}
