/**
 * This version is stamped on May 10, 2016
 *
 * Contact:
 *   Louis-Noel Pouchet <pouchet.ohio-state.edu>
 *   Tomofumi Yuki <tomofumi.yuki.fr>
 *
 * Web address: http://polybench.sourceforge.net
 */
/* correlation.c: this file is part of PolyBench/C */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/* Include polybench common header. */
#include <polybench.h>

/* Include benchmark-specific header. */
#include "correlation.h"

/* Array initialization. */
static void init_array(const int m,
                       const int n,
                       DATA_TYPE *float_n,
                       DATA_TYPE POLYBENCH_2D(data, N, M, n, m))
{
  int i, j;

  *float_n = (DATA_TYPE)N;

  for (i = 0; i < N; i++)
    for (j = 0; j < M; j++)
      data[i][j] = (DATA_TYPE)(i * j) / M + i;
}

/* DCE code. Must scan the entire live-out data.
   Can be used also to check the correctness of the output. */
static void print_array(const int m,
                        char *filename,
                        DATA_TYPE POLYBENCH_2D(corr, M, M, m, m))

{
  FILE *output = fopen(filename, "w");
  if (!output)
  {
    fprintf(stderr, "Unable to open output file %s.", filename);
    exit(1);
  }

  int i, j;

  POLYBENCH_DUMP_START;
  POLYBENCH_DUMP_BEGIN("corr");
  for (i = 0; i < m; i++)
    for (j = 0; j < m; j++)
    {
      if ((i * m + j) % 20 == 0)
        fprintf(output, "\n");
      fprintf(output, DATA_PRINTF_MODIFIER, corr[i][j]);
    }
  POLYBENCH_DUMP_END("corr");
  POLYBENCH_DUMP_FINISH;

  fclose(output);
}

/* Main computational kernel. The whole function will be timed,
   including the call and return. */
static void kernel_correlation(const int m, const int n,
                               const DATA_TYPE float_n,
                               DATA_TYPE POLYBENCH_2D(data, N, M, n, m),
                               DATA_TYPE POLYBENCH_2D(corr, M, M, m, m),
                               DATA_TYPE POLYBENCH_1D(mean, M, m),
                               DATA_TYPE POLYBENCH_1D(stddev, M, m))
{
  int i, j, k;

  const DATA_TYPE eps = SCALAR_VAL(0.1);

#pragma scop
  POLYBENCH_2D_ARRAY_DECL(data_transposed, DATA_TYPE, M, N, m, n);

  for (i = 0; i < _PB_N; i++)
    for (j = 0; j < _PB_M; j++)
    {
      double ij = data[i][j];
      (*data_transposed)[j][i] = ij;
    }

  for (j = 0; j < _PB_M; j++)
  {
    DATA_TYPE sum = SCALAR_VAL(0.0);
    for (i = 0; i < _PB_N; i++)
      sum += (*data_transposed)[j][i];
    mean[j] = sum / float_n;
  }

  for (j = 0; j < _PB_M; j++)
  {
    DATA_TYPE std = SCALAR_VAL(0.0);
    for (i = 0; i < _PB_N; i++)
      std += (data[i][j] - mean[j]) * (data[i][j] - mean[j]);
    std /= float_n;
    std = SQRT_FUN(std);
    /* The following in an inelegant but usual way to handle
       near-zero std. dev. values, which below would cause a zero-
       divide. */
    stddev[j] = std <= eps ? SCALAR_VAL(1.0) : std;
  }

  /* Center and reduce the column vectors. */
  for (i = 0; i < _PB_N; i++)
    for (j = 0; j < _PB_M; j++)
    {
      data[i][j] -= mean[j];
      data[i][j] /= SQRT_FUN(float_n) * stddev[j];
    }

  /* Calculate the m * m correlation matrix. */
  for (i = 0; i < _PB_M - 1; i++)
  {
    corr[i][i] = SCALAR_VAL(1.0);
    for (j = i + 1; j < _PB_M; j++)
    {
      double sum = SCALAR_VAL(0.0);
      for (k = 0; k < _PB_N; k++)
        sum += (data[k][i] * data[k][j]);
      corr[j][i] = corr[i][j] = sum;
    }
  }

  corr[_PB_M - 1][_PB_M - 1] = SCALAR_VAL(1.0);

  for (i = 0; i < _PB_M; i++)
    for (j = 0; j < _PB_N; j++)
    {
      double ij = (*data_transposed)[i][j];
      data[j][i] = ij;
    }

  POLYBENCH_FREE_ARRAY(data_transposed);
#pragma endscop
}

int main(int argc, char **argv)
{
  /* Retrieve problem size. */
  const int n = N;
  const int m = M;

  /* Variable declaration/allocation. */
  DATA_TYPE float_n;
  POLYBENCH_2D_ARRAY_DECL(data, DATA_TYPE, N, M, n, m);
  POLYBENCH_2D_ARRAY_DECL(corr, DATA_TYPE, M, M, m, m);
  POLYBENCH_1D_ARRAY_DECL(mean, DATA_TYPE, M, m);
  POLYBENCH_1D_ARRAY_DECL(stddev, DATA_TYPE, M, m);

  /* Initialize array(s). */
  init_array(m, n, &float_n, POLYBENCH_ARRAY(data));

  /* Start timer. */
  polybench_start_instruments;

  /* Run kernel. */
  kernel_correlation(m, n, float_n,
                     POLYBENCH_ARRAY(data),
                     POLYBENCH_ARRAY(corr),
                     POLYBENCH_ARRAY(mean),
                     POLYBENCH_ARRAY(stddev));

  /* Stop and print timer. */
  polybench_stop_instruments;
  polybench_print_instruments;

  /* Prevent dead-code elimination. All live-out data must be printed
     by the function call in argument. */
  polybench_prevent_dce(print_array(m, "correlation.txt", POLYBENCH_ARRAY(corr)));

  /* Be clean. */
  POLYBENCH_FREE_ARRAY(data);
  POLYBENCH_FREE_ARRAY(corr);
  POLYBENCH_FREE_ARRAY(mean);
  POLYBENCH_FREE_ARRAY(stddev);

  return 0;
}
