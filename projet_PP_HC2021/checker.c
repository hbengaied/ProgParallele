#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#include "check.h"
#include "util.h"
#include "problem.h"
#include "solution.h"

#define HYPERTHREADING 1

static problem_t  p;
static solution_t s;

int main(int argc, char* argv[])
{
  int score_reference, score_kernel;
  double time_reference, time_kernel, speedup, efficiency;

  if (argc != 3) {
    fprintf(stderr, "usage: %s problem solution\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  CHECK(problem_read(argv[1], &p) == 0);
  CHECK(solution_read(argv[2], &s, &p) == 0);

  time_reference = omp_get_wtime();
  CHECK(solution_check(&s, &p) == 0);
  score_reference = solution_score(&s, &p);
  time_reference = omp_get_wtime() - time_reference;
  printf("Reference time : %3.5lf s\n", time_reference);
  fprintf(stderr, "Score %d\n", score_reference);
  printf("\n");

  printf("Number of proc : %d\n", omp_get_num_procs());
  time_kernel = omp_get_wtime();
  CHECK(solution_check_kernel(&s, &p) == 0);
  score_kernel = solution_score_kernel(&s, &p);
  time_kernel = omp_get_wtime() - time_kernel;
  printf("Kernel time : %3.5lf s\n", time_kernel);
  fprintf(stderr, "Score %d\n", score_kernel); 
  printf("\n"); 

  speedup = time_reference / time_kernel;
  efficiency = speedup / (omp_get_num_procs() / (1 + HYPERTHREADING));
  printf("Speedup ------ : %3.5lf\n", speedup);
  printf("Efficiency --- : %3.5lf\n", efficiency);

  if(score_reference != score_kernel)
    printf("Bad result :(\n");
  else
    printf("OK result :)\n");

  // Write the score file
  util_write_score(argv[2], score_reference);

  return(0);
}
