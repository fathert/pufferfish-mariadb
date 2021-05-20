/* pfcrypt user-defined functions for MariaDB
 *
 * Written in 2021 by Tim Fathers
 *
 * To the extent possible under law, the author(s) have dedicated all copyright
 * and related and neighboring rights to this software to the public domain
 * worldwide. This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along
 * with this software. If not, see
 * <http://creativecommons.org/publicdomain/zero/1.0/>. 
 * 
 * Requires sudo apt-get install libssl-dev and probably openssl.
 * */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mysql.h>
#include "pufferfish.h"

my_bool pfcrypt_hash_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count != 3)
  {
    strcpy(message, "PFCRYPT_HASH() requires a string and two integer parameters");
    return 1;
  }

  args->arg_type[0] = STRING_RESULT;
  args->arg_type[1] = INT_RESULT;
  args->arg_type[2] = INT_RESULT;

  return 0;
}

char *pfcrypt_hash(UDF_INIT *initid, UDF_ARGS *args, char *res, unsigned long *len, char *is_null, char *err)
{

  long long time_cost;
  long long memory_cost;
  int ret = 0;
  char *password;
  unsigned long password_length;

  /* password */
  if (!args->args[0])
  {
    *is_null = 1;
    strcpy(res, "PFCRYPT_HASH() password invalid");
    *len = strlen(res);
    return res;
  }

  password = args->args[0];
  password_length = args->lengths[0];
  time_cost = *((long long *)args->args[1]);
  memory_cost = *((long long *)args->args[2]);

  if ((ret = pf_newhash(password, password_length, time_cost, memory_cost, res)) != 0)
  {
    fprintf(stderr, "Error: %s\n", strerror(ret));
    *err = 1;
    *is_null = 1;
  }

  *len = strlen(res);
  return res;
}

my_bool pfcrypt_check_init(UDF_INIT *initid, UDF_ARGS *args, char *message)
{
  if (args->arg_count != 2)
  {
    strcpy(message, "PFCRYPT_CHECK() requires two string parameters");
    return 1;
  }

  args->arg_type[0] = STRING_RESULT;
  args->arg_type[1] = STRING_RESULT;

  return 0;
}

long long pfcrypt_check(UDF_INIT *initid, UDF_ARGS *args, char *is_null, char *err)
{
  char *password;
  unsigned long password_length;
  char *hash;
  int ret = 0;

  password = args->args[0];
  password_length = args->lengths[0];
  hash = args->args[1];

  if ((ret = pf_checkpass(hash, password, password_length)) != 0)
  {
    fprintf(stderr, "Invalid password!\n");
    return 0;
  }

  return 1;
}
