#include <stdio.h>
#include <stdint.h>


// symbols per bit
uint8_t spb = 8;

#define KERNEL_SIZE 5

// Gaussian kernel, normalized to 2^12
uint16_t kernel[KERNEL_SIZE] = {223, 1000, 1649, 1000, 223};

// Ring buffer and index
ring_buffer[KERNEL_SIZE] = {0};
rb_idx = 0;

int32_t *coeffp; // pointer to coefficients

static void generate_ref_samples(void) {
  uint8_t data[] = "UUUUUUUUUU OZ3RF OZ3RF TEST TEST TEST";

  printf("%s\n", data);

  uint32_t data_len;
  for (data_len = 0; data[data_len] != '\0'; data_len++) {}
  printf("Data length is %d\n", data_len);


  FILE *fp;
  FILE *fp_f;
  fp = fopen( "12bit_symbols.csv" , "w" );
  fp_f = fopen( "12bit_symbols_f.csv" , "w" );

  uint8_t b;
  uint32_t s;
  uint32_t d;
  uint16_t buf[KERNEL_SIZE] = {0};
  uint16_t out = 0;
  uint16_t acc = 0;
  for (s = 0; s < data_len; s++) {
    for (b = 8; b > 0; b--) {
      d = 0x01 & (data[s] >> (b-1));
      for (int j=0; j < spb; j++) {
        if ( d > 0) {
          fprintf(fp, "4096\n");
          out = 1;
        }
        else
        {
          fprintf(fp, "0\n");
          out = 0;
        }

        ring_buffer[rb_idx] = out;
        rb_idx = (rb_idx + 1) % KERNEL_SIZE;
        acc = 0;
        coeffp = kernel;
        for (int k=0; k < KERNEL_SIZE; k++) {
          acc += (uint16_t)(*coeffp++) * (uint16_t)(ring_buffer[(k+rb_idx)%KERNEL_SIZE]);
        }
        fprintf(fp_f, "%d\n", acc);
      }
    }
  }
  fclose(fp);
  fclose(fp_f);
  printf("Samples generated are %d\n", s*8*spb);



}

void  main(void) {
  generate_ref_samples();
}

