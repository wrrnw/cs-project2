
/* Header file */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sha256.h"

#define MAX_PASSWORD_LENGTH 10000
#define START_ASCII_NUMBER 32 // from space character
#define END_ASCII_NUMBER 126  // to ~ character
#define FOUR_DIGITS_PASSWORD_LEN 4
#define SIX_DIGITS_PASSWORD_LEN 6


/* Function Prototype */
void runWithNoArg();
void runWithTwoArgs(char *argv[]);


int main(int argc, char *argv[]) {
  // If crack is running with no argument
  if(argc == 1) {
    runWithNoArg();
  }
  // If crack is running with one argument
  else if(argc == 2) {
    runWithOneArg(argv);
  }
  // If crack is running run with two arguments
  else if(argc == 3) {
    runWithTwoArgs(argv);
  }
  else {
    fprintf(stderr, "There must be 0,1 or 2 arguments\n");
    exit(EXIT_FAILURE);
  }
  return 0;
}




/* Case 1: If crack is running with no argument */
void runWithNoArg() {
  FILE *pwd4sha256_fp = fopen("pwd4sha256", "r");
  BYTE hash4[SHA256_BLOCK_SIZE];
  FILE *pwd6sha256_fp = fopen("pwd6sha256", "r");
  BYTE hash6[SHA256_BLOCK_SIZE];

  // Each specific digits in passwords
  int first_digit, second_digit, third_digit, fourth_digit, fifth_digit, sixth_digit;

  // Buffer to store current guess
  BYTE buffer[SHA256_BLOCK_SIZE];

  // 4-digit passwords start from 1 to 10
  int curr_hash_ID = 1;
  // Keep reading the pwd4sha256 file until we get all 10 hashes
  while (fread(hash4, sizeof(BYTE), SHA256_BLOCK_SIZE, pwd4sha256_fp)) {

    SHA256_CTX ctx4;

    // Brute-force generating 4-digits passwords
    char *guess = malloc(FOUR_DIGITS_PASSWORD_LEN * sizeof(char));
    for (first_digit = START_ASCII_NUMBER; first_digit < END_ASCII_NUMBER; first_digit++) {
      for (second_digit = START_ASCII_NUMBER; second_digit < END_ASCII_NUMBER; second_digit++) {
        for (third_digit = START_ASCII_NUMBER; third_digit < END_ASCII_NUMBER; third_digit++) {
          for (fourth_digit = START_ASCII_NUMBER; fourth_digit < END_ASCII_NUMBER; fourth_digit++) {
            guess[0] = first_digit;
            guess[1] = second_digit;
            guess[2] = third_digit;
            guess[3] = fourth_digit;

            BYTE *byte_guess = (BYTE *)guess;

            sha256_init(&ctx4);
            sha256_update(&ctx4, byte_guess, FOUR_DIGITS_PASSWORD_LEN);
            sha256_final(&ctx4, buffer);

            // Print when we find passwords matching hashes
            if (!memcmp(hash4, buffer, SHA256_BLOCK_SIZE)) {
              printf("%s %d\n", guess, curr_hash_ID);
            }
          }
        }
      }
    }
    curr_hash_ID++;
  }

  // 6-digit passwords start from 11 to 30
  curr_hash_ID = 11;
  // Keep reading the pwd6sha256 file until we get all hashes
  while (fread(hash6, sizeof(BYTE), SHA256_BLOCK_SIZE, pwd6sha256_fp))
  {
    SHA256_CTX ctx6;

    char *guess = malloc(SIX_DIGITS_PASSWORD_LEN * sizeof(char));
    // FOR loops to generate all possible 6-digit passwords
    for (first_digit = START_ASCII_NUMBER; first_digit < END_ASCII_NUMBER; first_digit++) {
      for (second_digit = START_ASCII_NUMBER; second_digit < END_ASCII_NUMBER; second_digit++) {
        for (third_digit = START_ASCII_NUMBER; third_digit < END_ASCII_NUMBER; third_digit++) {
          for (fourth_digit = START_ASCII_NUMBER; fourth_digit < END_ASCII_NUMBER; fourth_digit++) {
            for (fifth_digit = START_ASCII_NUMBER; fifth_digit < END_ASCII_NUMBER; fifth_digit++) {
              for (sixth_digit = START_ASCII_NUMBER; sixth_digit < END_ASCII_NUMBER; sixth_digit++) {
                guess[0] = first_digit;
                guess[1] = second_digit;
                guess[2] = third_digit;
                guess[3] = fourth_digit;
                guess[4] = fifth_digit;
                guess[5] = sixth_digit;

                // Change format from array of char to BYTE
                BYTE *byte_guess = (BYTE *)guess;

                sha256_init(&ctx6);
                sha256_update(&ctx6, byte_guess, SIX_DIGITS_PASSWORD_LEN);
                sha256_final(&ctx6, buffer);

                // Print when we find passwords matching hashes
                if (!memcmp(hash6, buffer, SHA256_BLOCK_SIZE))
                {
                  printf("%s %d\n", guess, curr_hash_ID);
                }
              }
            }
          }
        }
      }
    }
    curr_hash_ID++;
  }
  fclose(pwd4sha256_fp);
  fclose(pwd6sha256_fp);
}







void runWithOneArg(char *argv[]) {

}






/* Case 3: If crack is running run with two arguments */
void runWithTwoArgs(char *argv[]) {
  // Open sha256 file. Use "rb" because of opening non-text file
  FILE *sha_fp = fopen(argv[2], "rb");
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

  // Open passwords list file
  FILE *pwd_fp = fopen(argv[1], "r");
  if(pwd_fp == NULL) {
    perror("Error opening file");
    exit(EXIT_FAILURE);
  }
  char buffer[MAX_PASSWORD_LENGTH];
  while (fgets(buffer, MAX_PASSWORD_LENGTH, pwd_fp))
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

    int curr_hash_ID = 1;
    for (int i = 0; i < bytes_number; i = i + SHA256_BLOCK_SIZE)
    {

      // Print when we find passwords matching hashes
      if (!memcmp(hashes + i, hash, SHA256_BLOCK_SIZE))
      {
        printf("%s %d\n", buffer, curr_hash_ID);
      }
      curr_hash_ID++;
    }
  }
}
