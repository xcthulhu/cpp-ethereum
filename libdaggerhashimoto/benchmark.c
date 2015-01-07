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
    size = 15625000,
    trials = 100;
  unsigned int hash_bit = 0;
  num * dag = malloc(sizeof(num)*size);
  num nonce = num_zero;
  clock_t begin, end;
  double time_spent;
  char out[155];
  parameters params = get_default_params();

  params.n = size;
  produce_dag(dag, params, seed);
  for (int accesses = 1; accesses < 2000 ; ++accesses) {
    params.accesses = accesses;
    begin = clock();
    for (int i = 0 ; i < trials ; i++) {
      num_add(nonce, num_one);
      hash_bit ^= num_to_uint(hashimoto(dag, params, header, nonce)) & 1;
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("%i\t%f\t%d\n", accesses, time_spent, hash_bit);
  }
  free(dag);
  return EXIT_SUCCESS;
}
