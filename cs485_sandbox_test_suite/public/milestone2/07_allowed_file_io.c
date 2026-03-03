#include "../../include/common.h"

int main() {
    const char *p = "/tmp/allowed_io.txt";
    FILE *fp = fopen(p, "wb");
    if (!fp) die("fopen");
    const char *s = "allowed io\n";
    fwrite(s, 1, strlen(s), fp);
    fclose(fp);

    char *r = read_small_file(p, 128);
    if (!r) fail("could not read file back");
    printf("INFO: %s", r);
    free(r);
    pass("basic file I/O works");
}
