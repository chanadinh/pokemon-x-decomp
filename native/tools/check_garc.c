#include "../assets/garc.h"

#include <inttypes.h>
#include <stdio.h>

/* Read allocation indexes only. No member payload is loaded. */
int main(int argc, char **argv)
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s ARCHIVE [ARCHIVE ...]\n", argv[0]);
        return 2;
    }
    unsigned failed = 0;
    uint64_t members = 0;
    for (int i = 1; i < argc; ++i) {
        struct px_garc *archive = NULL;
        char error[192];
        if (!px_garc_open(argv[i], &archive, error, sizeof(error))) {
            fprintf(stderr, "%s: %s\n", argv[i], error);
            ++failed;
            continue;
        }
        members += px_garc_count(archive);
        px_garc_close(archive);
    }
    printf("GARC C index validation: %d archives; %" PRIu64 " members; %u failures\n",
           argc - 1, members, failed);
    return failed == 0 ? 0 : 1;
}
