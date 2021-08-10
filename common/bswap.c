#include <common/bswap.h>
float bswap32f(float in_float)
{
   float ret_val;
   char *float_to_convert = ( char* ) &in_float;
   char *return_float = ( char* ) &ret_val;

   // swap the bytes into a temporary buffer
   return_float[0] = float_to_convert[3];
   return_float[1] = float_to_convert[2];
   return_float[2] = float_to_convert[1];
   return_float[3] = float_to_convert[0];

   return ret_val;
}
double bswap64f(double in_float)
{
   double ret_val;
   char *float_to_convert = ( char* ) & in_float;
   char *return_float = ( char* ) & ret_val;

   // swap the bytes into a temporary buffer
   return_float[0] = float_to_convert[7];
   return_float[1] = float_to_convert[6];
   return_float[2] = float_to_convert[5];
   return_float[3] = float_to_convert[4];
   return_float[4] = float_to_convert[3];
   return_float[5] = float_to_convert[2];
   return_float[6] = float_to_convert[1];
   return_float[7] = float_to_convert[0];
   return ret_val;
}
