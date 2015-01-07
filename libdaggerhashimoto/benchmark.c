#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "daggerhashimoto.h"
#include "num.h"

int main(void){
  const num
    header = read_num("123"),
    seed = read_num("7");
  const int
    size = 100000,
    trials = 100;
  num dag[size], nonce = num_zero;
  clock_t begin, end;
  double time_spent;
  FILE *log = fopen("out.log", "w");
  char out[155];
  parameters params = get_default_params();

  params.n = size;
  produce_dag(dag, params, seed);
  for (int accesses = 1; accesses < 1000 ; ++accesses) {
    params.accesses = accesses;
    begin = clock();
    for (int i = 0 ; i < trials ; i++) {
      num_add(nonce, num_one);
      write_num(out, hashimoto(dag, params, header, nonce));
      fprintf(log,"%s\n",out);
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%i\t%f\n", accesses, time_spent);
  }
  return EXIT_SUCCESS;
}
