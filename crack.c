
/* Header file */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sha256.h"

#define PASSWORD_MAX_LENGTH 10000

/* Function Prototype */
void run_with_two_arguments(char *argv[]);


int main(int argc, char *argv[]) {

  /* If crack run with no argument */
  if(argc == 1) {

  }
  /* If crack run with one argument */
  if(argc == 2) {

  }
  /* If crack run with two arguments */
  if(argc == 3) {
    run_with_two_arguments(argv);
  }
  return 0;
}



void run_with_two_arguments(char *argv[]) {
  FILE *sha_fp = fopen(argv[2], "rb"); //Use "rb" because of opening non-text file
  if(sha_fp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
  BYTE hash[SHA256_BLOCK_SIZE];
  // Initialise cipher context
  SHA256_CTX ctx;

  // Finding number of bytes of the hash file
  fseek(sha_fp, 0, SEEK_END);
  int bytes_number = ftell(sha_fp);
  // Point back to the beginning of file
  fseek(sha_fp, 0, SEEK_SET);

  // Allocating memory to store hash file
  BYTE *hashes = malloc(bytes_number * sizeof(BYTE));
  fread(hashes, sizeof(BYTE), bytes_number, sha_fp);
  fclose(sha_fp);

  FILE *pwd_fp = fopen(argv[1], "r");
  if(pwd_fp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
  char buffer[PASSWORD_MAX_LENGTH];
  while (fgets(buffer, PASSWORD_MAX_LENGTH, pwd_fp))
  {
    char *pos;
    if ((pos = strchr(buffer, '\n')) != NULL)
    {
      *pos = '\0';
    }

    BYTE *byte_pwd = (BYTE *)buffer;

    sha256_init(&ctx);
    sha256_update(&ctx, byte_pwd, strlen(buffer));
    sha256_final(&ctx, hash);

    int hash_number = 1;
    for (int i = 0; i < bytes_number; i = i + SHA256_BLOCK_SIZE)
    {

      // Print when we find passwords matching hashes
      if (!memcmp(hashes + i, hash, SHA256_BLOCK_SIZE))
      {
        printf("%s %d\n", buffer, hash_number);
      }
      hash_number++;
    }
  }
}
