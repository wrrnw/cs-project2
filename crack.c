/* Header file */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "sha256.h"

/* Constants */
#define MAX_PASSWORD_LENGTH 10000
#define START_ASCII_NUMBER 32 // from space character
#define END_ASCII_NUMBER 126  // to ~ character
#define UPPER_CHAR_START_ASCII_NUMBER 65
#define UPPER_CHAR_END_ASCII_NUMBER 90
#define LOWER_CHAR_START_ASCII_NUMBER 97
#define LOWER_CHAR_END_ASCII_NUMBER 122
#define NUMBER_CHAR_START_ASCII_NUMBER 48
#define NUMBER_CHAR_END_ASCII_NUMBER 57
#define FOUR_DIGITS_PASSWORD_LEN 4
#define SIX_DIGITS_PASSWORD_LEN 6
#define FOUR_DIGITS_PASSWORD_START_ID 1
#define SIX_DIGITS_PASSWORD_START_ID 11


/* Function Prototype */
void runWithNoArg();
void runWithOneArg(char *argv[]);
void runWithTwoArgs(char *argv[]);
int checkIfPasswordMatched(SHA256_CTX *ctx, BYTE *guess_in_BYTE, size_t len,
   BYTE *buffer, BYTE *hash);


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
  FILE *pwd6sha256_fp = fopen("pwd6sha256", "r");
  BYTE hash[SHA256_BLOCK_SIZE];

  // Each specific digits in passwords
  int first_digit, second_digit, third_digit, fourth_digit, fifth_digit, sixth_digit;

  // Buffer to store current guess
  BYTE buffer[SHA256_BLOCK_SIZE];

  // 4-digit passwords start from 1 to 10
  int curr_hash_ID = FOUR_DIGITS_PASSWORD_START_ID;
  // For each password in hash file, generate all possibilities(95^4)
  while (fread(hash, sizeof(BYTE), SHA256_BLOCK_SIZE, pwd4sha256_fp)) {

    SHA256_CTX ctx;

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

            BYTE *guess_in_BYTE = (BYTE *)guess;

            int password_is_matched = checkIfPasswordMatched(&ctx, guess_in_BYTE,
              FOUR_DIGITS_PASSWORD_LEN, buffer, hash);
            // If current guess matches hash password
            if(password_is_matched) {
              printf("%s %d\n", guess, curr_hash_ID);
            }
          }
        }
      }
    }
    curr_hash_ID++;
  }


  // 6-digit passwords start from 11 to 30
  curr_hash_ID = SIX_DIGITS_PASSWORD_START_ID;
  // For each password in hash file, generate all possibilities(95^6)
  while (fread(hash, sizeof(BYTE), SHA256_BLOCK_SIZE, pwd6sha256_fp))
  {
    SHA256_CTX ctx;

    char *guess = malloc(SIX_DIGITS_PASSWORD_LEN * sizeof(char));
    // Brute-force generating 6-digits passwords
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
                BYTE *guess_in_BYTE = (BYTE *)guess;

                int password_is_matched = checkIfPasswordMatched(&ctx, guess_in_BYTE,
                  SIX_DIGITS_PASSWORD_LEN, buffer, hash);
                // If current guess matches hash password
                if(password_is_matched) {
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






/* Case 2: If crack is running with one argument
 * For generating good guesses. I follow a common human strategy to create
 * passwords, which is the frequency of appearence of characters is:
 * Lower case characters > Upper case characters > Numbers > Symbols
 * And we find in ASCII table that:
 * Lowercase characters start with 97(a) and end with 122(z)
 * Uppercase characters start with 65(A) and end with 90(Z)
 * Numbers start with 48(0) and end with 57(9)
 * Other ASCII chars from 32 to 126 except above are symbols
 */
void runWithOneArg(char *argv[]) {
  int number_of_guesses = atoi(argv[1]);
  int has_guessed = 0;
  // Each specific digits in passwords
  int first_digit, second_digit, third_digit, fourth_digit, fifth_digit, sixth_digit;


  char *guess = malloc(SIX_DIGITS_PASSWORD_LEN * sizeof(char));

  // First: Generating 6-digits passwords including only lowercase characters
  for (first_digit = LOWER_CHAR_START_ASCII_NUMBER; first_digit < LOWER_CHAR_END_ASCII_NUMBER; first_digit++) {
    for (second_digit = LOWER_CHAR_START_ASCII_NUMBER; second_digit < LOWER_CHAR_END_ASCII_NUMBER; second_digit++) {
      for (third_digit = LOWER_CHAR_START_ASCII_NUMBER; third_digit < LOWER_CHAR_END_ASCII_NUMBER; third_digit++) {
        for (fourth_digit = LOWER_CHAR_START_ASCII_NUMBER; fourth_digit < LOWER_CHAR_END_ASCII_NUMBER; fourth_digit++) {
          for (fifth_digit = LOWER_CHAR_START_ASCII_NUMBER; fifth_digit < LOWER_CHAR_END_ASCII_NUMBER; fifth_digit++) {
            for (sixth_digit = LOWER_CHAR_START_ASCII_NUMBER; sixth_digit < LOWER_CHAR_END_ASCII_NUMBER; sixth_digit++) {
              guess[0] = first_digit;
              guess[1] = second_digit;
              guess[2] = third_digit;
              guess[3] = fourth_digit;
              guess[4] = fifth_digit;
              guess[5] = sixth_digit;

              printf("%s\n", guess);
              has_guessed++;
              if (has_guessed == number_of_guesses)
              {
                return;
              }
            }
          }
        }
      }
    }
  }

  // Second: Generating 6-digits passwords including only uppercase characters
  for (first_digit = UPPER_CHAR_START_ASCII_NUMBER; first_digit < UPPER_CHAR_END_ASCII_NUMBER; first_digit++) {
    for (second_digit = UPPER_CHAR_START_ASCII_NUMBER; second_digit < UPPER_CHAR_END_ASCII_NUMBER; second_digit++) {
      for (third_digit = UPPER_CHAR_START_ASCII_NUMBER; third_digit < UPPER_CHAR_END_ASCII_NUMBER; third_digit++) {
        for (fourth_digit = UPPER_CHAR_START_ASCII_NUMBER; fourth_digit < UPPER_CHAR_END_ASCII_NUMBER; fourth_digit++) {
          for (fifth_digit = UPPER_CHAR_START_ASCII_NUMBER; fifth_digit < UPPER_CHAR_END_ASCII_NUMBER; fifth_digit++) {
            for (sixth_digit = UPPER_CHAR_START_ASCII_NUMBER; sixth_digit < UPPER_CHAR_END_ASCII_NUMBER; sixth_digit++) {
              guess[0] = first_digit;
              guess[1] = second_digit;
              guess[2] = third_digit;
              guess[3] = fourth_digit;
              guess[4] = fifth_digit;
              guess[5] = sixth_digit;

              printf("%s\n", guess);
              has_guessed++;
              if (has_guessed == number_of_guesses)
              {
                return;
              }
            }
          }
        }
      }
    }
  }

  // Third: Generating 6-digits passwords including only numbers
  for (first_digit = NUMBER_CHAR_START_ASCII_NUMBER; first_digit < NUMBER_CHAR_END_ASCII_NUMBER; first_digit++) {
    for (second_digit = NUMBER_CHAR_START_ASCII_NUMBER; second_digit < NUMBER_CHAR_END_ASCII_NUMBER; second_digit++) {
      for (third_digit = NUMBER_CHAR_START_ASCII_NUMBER; third_digit < NUMBER_CHAR_END_ASCII_NUMBER; third_digit++) {
        for (fourth_digit = NUMBER_CHAR_START_ASCII_NUMBER; fourth_digit < NUMBER_CHAR_END_ASCII_NUMBER; fourth_digit++) {
          for (fifth_digit = NUMBER_CHAR_START_ASCII_NUMBER; fifth_digit < NUMBER_CHAR_END_ASCII_NUMBER; fifth_digit++) {
            for (sixth_digit = NUMBER_CHAR_START_ASCII_NUMBER; sixth_digit < NUMBER_CHAR_END_ASCII_NUMBER; sixth_digit++) {
              guess[0] = first_digit;
              guess[1] = second_digit;
              guess[2] = third_digit;
              guess[3] = fourth_digit;
              guess[4] = fifth_digit;
              guess[5] = sixth_digit;

              printf("%s\n", guess);
              has_guessed++;
              if (has_guessed == number_of_guesses)
              {
                return;
              }
            }
          }
        }
      }
    }
  }

}






/* Case 3: If crack is running with two arguments */
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

    int curr_hash_ID = FOUR_DIGITS_PASSWORD_START_ID;
    for (int i = 0; i < bytes_number; i = i + SHA256_BLOCK_SIZE)
    {

      // If current guess matches hash password
      if (!memcmp(hashes + i, hash, SHA256_BLOCK_SIZE))
      {
        printf("%s %d\n", buffer, curr_hash_ID);
      }
      curr_hash_ID++;
    }
  }
}




/* Check if current guess matches hash password
 * Return 1 if matches, 0 if not matches
 */
int checkIfPasswordMatched(SHA256_CTX *ctx, BYTE *guess_in_BYTE, size_t len,
   BYTE *buffer, BYTE *hash) {
     sha256_init(ctx);
     sha256_update(ctx, guess_in_BYTE, len);
     sha256_final(ctx, buffer);
     if (!memcmp(hash, buffer, SHA256_BLOCK_SIZE)) {
       return 1;
     }
     return 0;
   }
