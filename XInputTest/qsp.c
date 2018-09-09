/*
    Rapid distance-based outlier detectio via sampling
    Copyright (C) 2013 Mahito Sugiyama

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    Contact: Mahito Sugiyama <mahito.sugiyama@tuebingen.mpg.de>

    Please refer the following article in your published research:
    Sugiyama, M., Borgwardt, K.M.: Rapid Distance-Based Outlier Detection via Sampling,
    Advances in Neural Information Processing Systems (NIPS 2013), 2013.

    https://github.com/BorgwardtLab/sampling-outlier-detection
*/
#include "stdlib2.h"
#include "qsp.h"

// input  -> X: data matrix (array), n: # of objects, d: # of dimensions, Xs:
// sample set, sid: sample indexes, ns: # of samples
// output -> result: an array of qsps
void qsp(double *X, long n, long d, long n_sample, long seed, double *score) {
  long i, j, point, *id_sample;
  double sum, res;

  // random sampling
  id_sample = (long *)malloc(sizeof(long) * n_sample);
  sampling(n_sample, 0, n - 1, seed, id_sample);

  // compute the outlierness score qsp for each data point
  for (point = 0; point < n; point++) {
    res = 0;
    for (i = 0; i < n_sample; i++) {
      if (point != id_sample[i]) {
        sum = 0;
        for (j = 0; j < d; j++)
          sum += fabs(X[point * d + j] - X[id_sample[i] * d + j]) *
                 fabs(X[point * d + j] - X[id_sample[i] * d + j]);
        if (res == 0)
          res = sum;
        else if (sum < res && sum > 0)
          res = sum;
      }
    }
    score[point] = sqrt(res);
  }
}

// random sampling
// if seed = 0, seed is randomly set
void sampling(long n_sample, long min, long max, long seed, long *id_sample) {
  long i, j = 0;
  time_t t;

  if (seed == 0)
    seed = (long) time(&t);

  for (i = 0; i < n_sample; i++) {
    do {
      srand(seed + j++);
      id_sample[i] = min + rand() % (max - min + 1);
    } while (checkArray(id_sample[i], i, id_sample) == 1);
  }
}

long checkArray(long id_current, long n_sample, long *id_sample) {
  long i;

  for (i = 0; i < n_sample; i++) {
    if (id_current == id_sample[i])
      return (1);
  }
  return (0);
}

// normalization of data (divide by SDs for each dimension)
void normalize(double *X, long n, long d) {
  long i, j, flag;
  double sum, *X_means;

  X_means = (double *)malloc(sizeof(double) * d);

  for (j = 0; j < d; j++) {
    sum = 0;
    for (i = 0; i < n; i++) {
      sum += X[i * d + j];
    }
    X_means[j] = sum / n;
  }

  for (j = 0; j < d; j++) {
    flag = 0;
    sum = X[j];
    for (i = 1; i < n; i++) {
      if (sum != X[i * d + j]) {
        flag = 1;
        break;
      }
    }
    if (flag == 1) {
      sum = 0;
      for (i = 0; i < n; i++)
        sum += (X[i * d + j] - X_means[j]) * (X[i * d + j] - X_means[j]);
      sum = sqrt(sum / (n - 1)); // unbiased SD
      for (i = 0; i < n; i++)
        X[i * d + j] = X[i * d + j] / sum;
    }
  }

  free(X_means);
  X_means = NULL;
}
