
/* MD5lib.h - md5 library
 */

/* Copyright (C) 1990-2, RSA Data Security, Inc. Created 1990. All
rights reserved.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
 */

/* The following makes MD default to MD5 if it has not already been
  defined with C compiler flags.
 */
 

#include <stdio.h>
#include <time.h>
#include <string.h>
extern "C"{
    #include "vs_shell.h"
};
#define MD 5

/* GLOBAL.H - RSAREF types and constants
 */

/* PROTOTYPES should be set to one if and only if the compiler supports
  function argument prototyping.
  The following makes PROTOTYPES default to 0 if it has not already
  been defined with C compiler flags.
 */
#ifndef PROTOTYPES
#define PROTOTYPES 0
#endif

#include "VSOpenDataType.h"

/* POINTER defines a generic pointer type */
typedef VS_UINT8 *POINTER;

/* UINT2 defines a two byte word */
typedef VS_UINT16 UINT2;

/* UINT4 defines a four byte word */
typedef VS_UINT32 UINT4;

/* PROTO_LIST is defined depending on how PROTOTYPES is defined above.
If using PROTOTYPES, then PROTO_LIST returns the list, otherwise it
  returns an empty list.
 */
#if PROTOTYPES
#define PROTO_LIST(list) list
#else
#define PROTO_LIST(list) ()
#endif


 /* Length of test block, number of test blocks.
 */
#define TEST_BLOCK_LEN 1000
#define TEST_BLOCK_COUNT 1000

  

/* Constants for MD5Transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

VS_INT8* MDString PROTO_LIST ((VS_INT8 *));
VS_INT8* MDFile PROTO_LIST ((VS_INT8 *));
VS_INT8* hmac_md5(VS_INT8* text, VS_INT8* key);

typedef struct {
  UINT4 state[4];                                   /* state (ABCD) */
  UINT4 count[2];        /* number of bits, modulo 2^64 (lsb first) */
  VS_UINT8 buffer[64];                         /* input buffer */
} MD5_CTX;

/*void MD5Init PROTO_LIST ((MD5_CTX *));
void MD5Update PROTO_LIST
  ((MD5_CTX *, VS_UINT8 *, VS_UINT32));
void MD5Final PROTO_LIST ((VS_UINT8 [16], MD5_CT X *));

static void MD5Transform PROTO_LIST ((UINT4 [4], VS_UINT8 [64]));
static void Encode PROTO_LIST
  ((VS_UINT8 *, UINT4 *, VS_UINT32));
static void Decode PROTO_LIST
  ((UINT4 *, VS_UINT8 *, VS_UINT32));
static void MD5_memcpy PROTO_LIST ((POINTER, POINTER, VS_UINT32));
static void MD5_memset PROTO_LIST ((POINTER, VS_INT32, VS_UINT32));
*/
static VS_UINT8 PADDING[64] = {
  0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.
 */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.
 */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
Rotation is separate from addition to prevent recomputation.
 */
#define FF(a, b, c, d, x, s, ac) { \
 (a) += F ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) {     \
 (a) += G ((b), (c), (d)) + (x) + (UINT4)(ac); \
 (a) = ROTATE_LEFT ((a), (s));  \
 (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) {  \
 (a) += H ((b), (c), (d)) + (x) + (UINT4)(ac);  \
 (a) = ROTATE_LEFT ((a), (s)); \
 (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) {  \
 (a) += I ((b), (c), (d)) + (x) + (UINT4)(ac);  \
 (a) = ROTATE_LEFT ((a), (s));      \
 (a) += (b);         \
  }
void MD5Init (MD5_CTX *context);
void MD5Update(MD5_CTX *context, VS_UINT8 *input,VS_UINT32 inputLen);
  
void MD5Final (VS_UINT8 digest[16], MD5_CTX *context);
static void MD5Transform  (UINT4 [4], VS_UINT8 [64]) ;
static void Encode(VS_UINT8 *, UINT4 *, VS_UINT32);
static void Decode (UINT4 *, VS_UINT8 *, VS_UINT32);
static void MD5_memcpy(POINTER, POINTER, VS_UINT32);
static void MD5_memset(POINTER, VS_INT32, VS_UINT32);

/* MD5 initialization. Begins an MD5 operation, writing a new context.
 */
void MD5Init (MD5_CTX *context)
                                        /* context */
{
  context->count[0] = context->count[1] = 0;
  /* Load magic initialization constants.
*/
  context->state[0] = 0x67452301;
  context->state[1] = 0xefcdab89;
  context->state[2] = 0x98badcfe;
  context->state[3] = 0x10325476;
}

/* MD5 block update operation. Continues an MD5 message-digest
  operation, processing another message block, and updating the
  context.
 */
void MD5Update (MD5_CTX *context, VS_UINT8 *input,VS_UINT32 inputLen  )
                                         /* context */
                              /* input block */
                     /* length of input block */
{
  VS_UINT32 i, index, partLen;

  /* Compute number of bytes mod 64 */
  index = (VS_UINT32)((context->count[0] >> 3) & 0x3F);

  /* Update number of bits */
  if ((context->count[0] += ((UINT4)inputLen << 3))
  < ((UINT4)inputLen << 3))
 context->count[1]++;
  context->count[1] += ((UINT4)inputLen >> 29);

  partLen = 64 - index;

  /* Transform as many times as possible.
*/
  if (inputLen >= partLen) {
 MD5_memcpy
   ((POINTER)&context->buffer[index], (POINTER)input, partLen);
 MD5Transform (context->state, context->buffer);

 for (i = partLen; i + 63 < inputLen; i += 64)
   MD5Transform (context->state, &input[i]);

 index = 0;
  }
  else
 i = 0;

  /* Buffer remaining input */
  MD5_memcpy
 ((POINTER)&context->buffer[index], (POINTER)&input[i],
  inputLen-i);
}

/* MD5 finalization. Ends an MD5 message-digest operation, writing the
  the message digest and zeroizing the context.
 */
void MD5Final (VS_UINT8 digest[16], MD5_CTX *context)
                       /* message digest */
                                        /* context */
{
  VS_UINT8 bits[8];
  VS_UINT32 index, padLen;

  /* Save number of bits */
  Encode (bits, context->count, 8);

  /* Pad out to 56 mod 64.
*/
  index = (VS_UINT32)((context->count[0] >> 3) & 0x3f);
  padLen = (index < 56) ? (56 - index) : (120 - index);
  MD5Update (context,(VS_UINT8*) PADDING, padLen);

  /* Append length (before padding) */
  MD5Update (context, bits, 8);
  /* Store state in digest */
  Encode (digest, context->state, 16);

  /* Zeroize sensitive information.
*/
  MD5_memset ((POINTER)context, 0, sizeof (*context));
}

/* MD5 basic transformation. Transforms state based on block.
 */
static void MD5Transform (UINT4 state[4],
VS_UINT8 block[64])
 
{
 UINT4 a = state[0], b = state[1], c = state[2], d = state[3], x[16];

  Decode (x, block, 64);

  /* Round 1 */
  FF (a, b, c, d, x[ 0], S11, 0xd76aa478); /* 1 */
  FF (d, a, b, c, x[ 1], S12, 0xe8c7b756); /* 2 */
  FF (c, d, a, b, x[ 2], S13, 0x242070db); /* 3 */
  FF (b, c, d, a, x[ 3], S14, 0xc1bdceee); /* 4 */
  FF (a, b, c, d, x[ 4], S11, 0xf57c0faf); /* 5 */
  FF (d, a, b, c, x[ 5], S12, 0x4787c62a); /* 6 */
  FF (c, d, a, b, x[ 6], S13, 0xa8304613); /* 7 */
  FF (b, c, d, a, x[ 7], S14, 0xfd469501); /* 8 */
  FF (a, b, c, d, x[ 8], S11, 0x698098d8); /* 9 */
  FF (d, a, b, c, x[ 9], S12, 0x8b44f7af); /* 10 */
  FF (c, d, a, b, x[10], S13, 0xffff5bb1); /* 11 */
  FF (b, c, d, a, x[11], S14, 0x895cd7be); /* 12 */
  FF (a, b, c, d, x[12], S11, 0x6b901122); /* 13 */
  FF (d, a, b, c, x[13], S12, 0xfd987193); /* 14 */
  FF (c, d, a, b, x[14], S13, 0xa679438e); /* 15 */
  FF (b, c, d, a, x[15], S14, 0x49b40821); /* 16 */

 /* Round 2 */
  GG (a, b, c, d, x[ 1], S21, 0xf61e2562); /* 17 */
  GG (d, a, b, c, x[ 6], S22, 0xc040b340); /* 18 */
  GG (c, d, a, b, x[11], S23, 0x265e5a51); /* 19 */
  GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa); /* 20 */
  GG (a, b, c, d, x[ 5], S21, 0xd62f105d); /* 21 */
  GG (d, a, b, c, x[10], S22,  0x2441453); /* 22 */
  GG (c, d, a, b, x[15], S23, 0xd8a1e681); /* 23 */
  GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8); /* 24 */
  GG (a, b, c, d, x[ 9], S21, 0x21e1cde6); /* 25 */
  GG (d, a, b, c, x[14], S22, 0xc33707d6); /* 26 */
  GG (c, d, a, b, x[ 3], S23, 0xf4d50d87); /* 27 */
  GG (b, c, d, a, x[ 8], S24, 0x455a14ed); /* 28 */
  GG (a, b, c, d, x[13], S21, 0xa9e3e905); /* 29 */
  GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8); /* 30 */
  GG (c, d, a, b, x[ 7], S23, 0x676f02d9); /* 31 */
  GG (b, c, d, a, x[12], S24, 0x8d2a4c8a); /* 32 */

  /* Round 3 */
  HH (a, b, c, d, x[ 5], S31, 0xfffa3942); /* 33 */
  HH (d, a, b, c, x[ 8], S32, 0x8771f681); /* 34 */
  HH (c, d, a, b, x[11], S33, 0x6d9d6122); /* 35 */
  HH (b, c, d, a, x[14], S34, 0xfde5380c); /* 36 */
  HH (a, b, c, d, x[ 1], S31, 0xa4beea44); /* 37 */
  HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9); /* 38 */
  HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60); /* 39 */
  HH (b, c, d, a, x[10], S34, 0xbebfbc70); /* 40 */
  HH (a, b, c, d, x[13], S31, 0x289b7ec6); /* 41 */
  HH (d, a, b, c, x[ 0], S32, 0xeaa127fa); /* 42 */
  HH (c, d, a, b, x[ 3], S33, 0xd4ef3085); /* 43 */
  HH (b, c, d, a, x[ 6], S34,  0x4881d05); /* 44 */
  HH (a, b, c, d, x[ 9], S31, 0xd9d4d039); /* 45 */
  HH (d, a, b, c, x[12], S32, 0xe6db99e5); /* 46 */
  HH (c, d, a, b, x[15], S33, 0x1fa27cf8); /* 47 */
  HH (b, c, d, a, x[ 2], S34, 0xc4ac5665); /* 48 */

  /* Round 4 */
  II (a, b, c, d, x[ 0], S41, 0xf4292244); /* 49 */
  II (d, a, b, c, x[ 7], S42, 0x432aff97); /* 50 */
  II (c, d, a, b, x[14], S43, 0xab9423a7); /* 51 */
  II (b, c, d, a, x[ 5], S44, 0xfc93a039); /* 52 */
  II (a, b, c, d, x[12], S41, 0x655b59c3); /* 53 */
  II (d, a, b, c, x[ 3], S42, 0x8f0ccc92); /* 54 */
  II (c, d, a, b, x[10], S43, 0xffeff47d); /* 55 */
  II (b, c, d, a, x[ 1], S44, 0x85845dd1); /* 56 */
  II (a, b, c, d, x[ 8], S41, 0x6fa87e4f); /* 57 */
  II (d, a, b, c, x[15], S42, 0xfe2ce6e0); /* 58 */
  II (c, d, a, b, x[ 6], S43, 0xa3014314); /* 59 */
  II (b, c, d, a, x[13], S44, 0x4e0811a1); /* 60 */
  II (a, b, c, d, x[ 4], S41, 0xf7537e82); /* 61 */
  II (d, a, b, c, x[11], S42, 0xbd3af235); /* 62 */
  II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb); /* 63 */
  II (b, c, d, a, x[ 9], S44, 0xeb86d391); /* 64 */

  state[0] += a;
  state[1] += b;
  state[2] += c;
  state[3] += d;

  /* Zeroize sensitive information.
  */
  MD5_memset ((POINTER)x, 0, sizeof (x));
}

/* Encodes input (UINT4) into output (VS_UINT8). Assumes len is
  a multiple of 4.
 */
static void Encode (VS_UINT8 *output,
UINT4 *input,
VS_UINT32 len)
 
{
  VS_UINT32 i, j;

  for (i = 0, j = 0; j < len; i++, j += 4) {
 output[j] = (VS_UINT8)(input[i] & 0xff);
 output[j+1] = (VS_UINT8)((input[i] >> 8) & 0xff);
 output[j+2] = (VS_UINT8)((input[i] >> 16) & 0xff);
 output[j+3] = (VS_UINT8)((input[i] >> 24) & 0xff);
  }
}

/* Decodes input (VS_UINT8) into output (UINT4). Assumes len is
  a multiple of 4.
 */
static void Decode (UINT4 *output,
VS_UINT8 *input,
VS_UINT32 len)
 
{
  VS_UINT32 i, j;

  for (i = 0, j = 0; j < len; i++, j += 4)
 output[i] = ((UINT4)input[j]) | (((UINT4)input[j+1]) << 8) |
   (((UINT4)input[j+2]) << 16) | (((UINT4)input[j+3]) << 24);
}

/* Note: Replace "for loop" with standard memcpy if possible.
 */

static void MD5_memcpy (POINTER output,
POINTER input,
VS_UINT32 len)
 
{
  VS_UINT32 i;

  for (i = 0; i < len; i++)
  output[i] = input[i];
}

/* Note: Replace "for loop" with standard memset if possible.
 */
static void MD5_memset (POINTER output,
VS_INT32 value,
VS_UINT32 len)
 
{
  VS_UINT32 i;

  for (i = 0; i < len; i++)
 ((VS_INT8 *)output)[i] = (VS_INT8)value;
}

/* Digests a string and prints the result.
 */
VS_INT8* MDString (VS_INT8 *string)
 
{
  MD5_CTX context;
  VS_UINT8 digest[16];
  VS_INT8 output1[34];
 static  VS_INT8 output[33]={""};
  VS_UINT32 len = (VS_UINT32)strlen (string);
  VS_INT32 i;
  MD5Init (&context);
  MD5Update (&context, (VS_UINT8*)string, len);
  MD5Final (digest, &context);

  for (i = 0; i < 16; i++)
 {sprintf(&(output1[2*i]),"%02x",(VS_UINT8)digest[i]);
  sprintf(&(output1[2*i+1]),"%02x",(VS_UINT8)(digest[i]<<4));
  }
  for(i=0;i<32;i++)
  output[i]=output1[i];
  output[32] = 0;
  return output;
}

VS_INT8* MDDataBuf (VS_INT8 *Buf,VS_INT32 BufSize)
 
{
  MD5_CTX context;
  VS_UINT8 digest[16];
  VS_INT8 output1[34];
 static  VS_INT8 output[33]={""};
  VS_UINT32 len = BufSize;
  VS_INT32 i;
  MD5Init (&context);
  MD5Update (&context, (VS_UINT8*)Buf, len);
  MD5Final (digest, &context);

  for (i = 0; i < 16; i++)
 {sprintf(&(output1[2*i]),"%02x",(VS_UINT8)digest[i]);
  sprintf(&(output1[2*i+1]),"%02x",(VS_UINT8)(digest[i]<<4));
  }
  for(i=0;i<32;i++)
  output[i]=output1[i];
  output[32] = 0;
  return output;
}

VS_INT8* MDDataBuf1 (VS_INT8 *Buf,VS_INT32 BufSize)
 
{
  MD5_CTX context;
  VS_UINT8 digest[16];
  VS_INT8 output1[34];
 static  VS_INT8 output[33]={""};
  VS_UINT32 len = BufSize;
  VS_INT32 i;
  MD5Init (&context);
  MD5Update (&context, (VS_UINT8*)Buf, len);
  MD5Final (digest, &context);

  for (i = 0; i < 16; i++)
 {sprintf(&(output1[2*i]),"%02x",(VS_UINT8)digest[i]);
  sprintf(&(output1[2*i+1]),"%02x",(VS_UINT8)(digest[i]<<4));
  }
  for(i=0;i<32;i++)
  output[i]=output1[i];
  output[32] = 0;
  return output;
}

/* Digests a file and prints the result.
 */
VS_INT8* MDFile (VS_INT8 *filename)
 
{ static VS_INT8 output[33]={""};
  FILE *file;
  MD5_CTX context;
  VS_INT32 len;
  VS_UINT8 buffer[1024], digest[16];
  VS_INT32 i;
  VS_INT8 output1[32];
  if ((file = vs_file_fopen (filename, "rb")) == NULL)
   { printf ("%s can't be openedn", filename);
    return 0;
   }
  else {
       MD5Init (&context);
     while ( (len = (VS_INT32)fread (buffer, 1, 1024, file)) != 0)
       MD5Update (&context, buffer, len);
    MD5Final (digest, &context);
    fclose (file);
    for (i = 0; i < 16; i++)
     {sprintf(&(output1[2*i]),"%02x",(VS_UINT8)digest[i]);
        sprintf(&(output1[2*i+1]),"%02x",(VS_UINT8)(digest[i]<<4));
          }
        for(i=0;i<32;i++)
       output[i]=output1[i];
        return output;
       }
}

VS_INT8* hmac_md5(VS_INT8* text,VS_INT8*  key)
{
        VS_INT8   digest[16];
        VS_INT8   output1[32];
         static VS_INT8 output[33]={""};
        MD5_CTX context;
        VS_UINT8 k_ipad[65];    /* inner padding -
                                      * key XORd with ipad
                                      */
        VS_UINT8 k_opad[65];    /* outer padding -
                                      * key XORd with opad
                                      */
        VS_UINT8 tk[16];
        VS_INT32 i;
        VS_INT32 text_len = (VS_INT32)strlen (text);
        VS_INT32 key_len=(VS_INT32)strlen(key);
        /* if key is longer than 64 bytes reset it to key=MD5(key) */
        if (key_len > 64) {

                MD5_CTX      tctx;

                MD5Init(&tctx);
                MD5Update(&tctx,(VS_UINT8*) key, key_len);
                MD5Final(tk, &tctx);

                key = (VS_INT8*)tk;
                key_len = 16;
        }

        /*
         * the HMAC_MD5 transform looks like:
         *
         * MD5(K XOR opad, MD5(K XOR ipad, text))
         *
         * where K is an n byte key
         * ipad is the byte 0x36 repeated 64 times
         * opad is the byte 0x5c repeated 64 times
         * and text is the data being protected
         */

        /* start out by storing key in pads */
        
        /*bzero( k_ipad, sizeof k_ipad);
          bzero( k_opad, sizeof k_opad);
        */

        for(i=0;i<65;i++)
        k_ipad[i]=(VS_UINT8)0;
        for(i=0;i<65;i++)
        k_opad[i]=(VS_UINT8)0;

        /*bcopy( key, k_ipad, key_len);
          bcopy( key, k_opad, key_len);
         */
         for(i=0;i<key_len;i++)
        {k_ipad[i]=(VS_UINT8)key[i];
         k_opad[i]=(VS_UINT8)key[i];
         }

        /* XOR key with ipad and opad values */
        for (i=0; i<64; i++) {
                k_ipad[i] ^= 0x36;
                k_opad[i] ^= 0x5c;
        }
        /*
         * perform inner MD5
         */
        MD5Init(&context);                   /* init context for 1st
                                              * pass */
        MD5Update(&context, k_ipad, 64);      /* start with inner pad */
        MD5Update(&context, (VS_UINT8*)text, text_len); /* then text of datagram 

*/
        MD5Final((VS_UINT8*)digest, &context);          /* finish up 1st pass */
        /*
         * perform outer MD5
         */
        MD5Init(&context);                   /* init context for 2nd
                                              * pass */
        MD5Update(&context, k_opad, 64);     /* start with outer pad */
        MD5Update(&context,(VS_UINT8*) digest, 16);     /* then results of 1st
                                              * hash */
        MD5Final((VS_UINT8*)digest, &context);          /* finish up 2nd pass */
        for (i = 0; i < 16; i++)
        {sprintf(&(output1[2*i]),"%02x",(VS_UINT8)digest[i]);
         sprintf(&(output1[2*i+1]),"%02x",(VS_UINT8)(digest[i]<<4));
          }
        for(i=0;i<32;i++)
        output[i]=output1[i]; 
        return output;     
}


