#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "Rsa.h"

int8_t buffer[1024];
const int MAX_DIGITS = 50;
int i, j = 0;

// This should totally be in the math library.
int64_t  gcd(int64_t  a, int64_t  b)
{
  int64_t  c;
  while (a != 0)
  {
    c = a;
    a = b % a;
    b = c;
  }
  return b;
}

int64_t  ExtEuclid(int64_t  a, int64_t  b)
{
  int64_t  x = 0, y = 1, u = 1, v = 0, gcd = b, m, n, q, r;
  while (a != 0)
  {
    q = gcd / a;
    r = gcd % a;
    m = x - u * q;
    n = y - v * q;
    gcd = a;
    a = r;
    x = u;
    y = v;
    u = m;
    v = n;
  }
  return y;
}
static inline int64_t  modmult(int64_t  a, int64_t  b, int64_t  mod)
{
  // this is necessary since we will be dividing by a
  if (a == 0)
  {
    return 0;
  }
  register int64_t  product = a * b;
  //if multiplication does not overflow, we can use it
  if (product / a == b)
  {
    return product % mod;
  }
  // if a % 2 == 1 i. e. a >> 1 is not a / 2
  if (a & 1)
  {
    product = modmult((a >> 1), b, mod);
    if ((product << 1) > product)
    {
      return (((product << 1) % mod) + b) % mod;
    }
  }
  //implicit else
  product = modmult((a >> 1), b, mod);
  if ((product << 1) > product)
  {
    return (product << 1) % mod;
  }
  //implicit else: this is about 10x slower than the code above, but it will not overflow
  int64_t  sum;
  sum = 0;
  while (b > 0)
  {
    if (b & 1)
      sum = (sum + a) % mod;
    a = (2 * a) % mod;
    b >>= 1;
  }
  return sum;
}
int64_t  rsa_modExp(int64_t  b, int64_t  e, int64_t  m)
{
  int64_t  product;
  product = 1;
  if (b < 0 || e < 0 || m <= 0)
  {
    return -1;
  }
  b = b % m;
  while (e > 0)
  {
    if (e & 1)
    {
      product = modmult(product, b, m);
    }
    b = modmult(b, b, m);
    e >>= 1;
  }
  return product;
}

int64_t  *rsa_encrypt(const int8_t *message, const unsigned long message_size,
                       const public_key_class_t *pub)
{
  int64_t  *encrypted = malloc(sizeof(int64_t ) * message_size);
  if (encrypted == NULL)
  {
    fprintf(stderr, "Error: Heap allocation failed.\n");
    return NULL;
  }
  int64_t  i = 0;
  for (i = 0; i < message_size; i++)
  {
    if ((encrypted[i] = rsa_modExp(message[i], pub->exponent, pub->modulus)) == -1)
      return NULL;
  }
  return encrypted;
}

int8_t *rsa_decrypt(const int64_t  *message,
                  const unsigned long message_size,
                  const private_key_class_t *priv)
{
  if (message_size % sizeof(int64_t ) != 0)
  {
    fprintf(stderr,
            "Error: message_size is not divisible by %d, so cannot be output of rsa_encrypt\n", (int)sizeof(int64_t ));
    return NULL;
  }
  // We allocate space to do the decryption (temp) and space for the output as a int8_t array
  // (decrypted)
  int8_t *decrypted = malloc(message_size / sizeof(int64_t ));
  int8_t *temp = malloc(message_size);
  if ((decrypted == NULL) || (temp == NULL))
  {
    fprintf(stderr,
            "Error: Heap allocation failed.\n");
    return NULL;
  }
  // Now we go through each 8-byte chunk and decrypt it.
  int64_t  i = 0;
  for (i = 0; i < message_size / 8; i++)
  {
    if ((temp[i] = rsa_modExp(message[i], priv->exponent, priv->modulus)) == -1)
    {
      free(temp);
      return NULL;
    }
  }
  // The result should be a number in the int8_t range, which gives back the original byte.
  // We put that into decrypted, then return.
  for (i = 0; i < message_size / 8; i++)
  {
    decrypted[i] = temp[i];
  }
  free(temp);
  return decrypted;
}
