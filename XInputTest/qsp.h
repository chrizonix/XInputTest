#pragma once

#ifdef __cplusplus
extern "C" {
#endif
    // copyright: see the header of "qsp.c"
    // compute qsp for every data point
    void qsp(double *X, long n, long d, long n_sample, long seed, double *score);

    // random sampling
    void sampling(long n, long min, long max, long seed, long *id_sample);
    long checkArray(long id_current, long n_sample, long *id_sample);

    // normalization of data (divide by SDs for each dimension)
    void normalize(double *X, long n, long d);
#ifdef __cplusplus
}
#endif
