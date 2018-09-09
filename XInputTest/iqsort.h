#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    // copyright: see the header of "qsp.c"
    // index qsort
    void iqsort(long *index, double *score, long n);

    // comparison function for index qsort
    int compare(const void *a, const void *b);
#ifdef __cplusplus
}
#endif
