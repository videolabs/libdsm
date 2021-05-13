/*
 * Copyright (C) 2011-2014 Free Software Foundation, Inc.
 *
 * This file is part of LIBTASN1.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Written by Simon Josefsson
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "libtasn1.h"

struct tv
{
  int der_len;
  const unsigned char *der_str;
  int len;
  const unsigned char *string;
  int expected_error;
  int ber;
};

static const struct tv tv[] = {
  /* primitive octet strings */
  {.der_len = 10,
   .der_str =
   (void*)"\x04\x08\x01\x23\x45\x67\x89\xab\xcd\xef",
   .len = 8,
   .string =
   (void*)"\x01\x23\x45\x67\x89\xab\xcd\xef",
   .ber = 0},
  {.der_len = 22,
   .der_str =
   (void*)"\x04\x14\x13\x00\xd9\xa8\x47\xf7\xf2\x1c\xf4\xb0\xec\x5f\xc1\x73\xe5\x1b\x25\xc2\x62\x27",
   .len = 20,
   .string =
   (void*)"\x13\x00\xD9\xA8\x47\xF7\xF2\x1C\xF4\xB0\xEC\x5F\xC1\x73\xE5\x1B\x25\xC2\x62\x27"},

  /* long type of length */
  {.der_len = 23,
   .der_str =
   (void*)"\x04\x81\x14\x13\x00\xd9\xa8\x47\xf7\xf2\x1c\xf4\xb0\xec\x5f\xc1\x73\xe5\x1b\x25\xc2\x62\x27",
   .len = 20,
   .string =
   (void*)"\x13\x00\xD9\xA8\x47\xF7\xF2\x1C\xF4\xB0\xEC\x5F\xC1\x73\xE5\x1B\x25\xC2\x62\x27",
   .ber = 1},
  {.der_len = 11,
   .der_str =
   (void*)"\x04\x81\x08\x01\x23\x45\x67\x89\xab\xcd\xef",
   .len = 8,
   .string =
   (void*)"\x01\x23\x45\x67\x89\xab\xcd\xef",
   .ber = 1},

  /* constructed - indefinite */
  {.der_len = 11,
   .der_str = (void*)"\x24\x80\x04\x05\x01\x02\x03\x04\x05\x00\x00",
   .len = 5,
   .string = (void*)"\x01\x02\x03\x04\x05",
   .ber = 1,
   }
};

int
main (int argc, char *argv[])
{
  unsigned char str[100];
  unsigned char der[100];
  int der_len = sizeof (der);
  int str_size = sizeof (str);
  unsigned char *tmp = NULL;
  int ret, ret_len;
  size_t i;

  for (i = 0; i < sizeof (tv) / sizeof (tv[0]); i++)
    {
      /* Decode */

      if (tv[i].ber == 0)
	{
	  str_size = sizeof (str);
	  ret =
	    asn1_get_octet_der (tv[i].der_str + 1,
				tv[i].der_len - 1, &ret_len, str,
				sizeof (str), &str_size);
	  if (ret != tv[i].expected_error)
	    {
	      fprintf (stderr,
		       "%d: asn1_get_octet_der iter %lu: got %d expected %d\n",
		       __LINE__, (unsigned long) i, ret,
		       tv[i].expected_error);
	      return 1;
	    }

	  if (ret_len != tv[i].der_len - 1)
	    {
	      fprintf (stderr,
		       "%d: error in DER, length returned is %d, had %d\n",
		       __LINE__, ret_len, tv[i].der_len - 1);
	      return 1;
	    }

	  if (str_size != tv[i].len
	      || memcmp (tv[i].string, str, tv[i].len) != 0)
	    {
	      fprintf (stderr,
		       "%d: memcmp iter %lu: got invalid decoding\n",
		       __LINE__, (unsigned long) i);
	      return 1;
	    }

	  /* Encode */
	  der_len = sizeof (der);
	  asn1_octet_der (str, str_size, der, &der_len);

	  if (der_len != tv[i].der_len - 1
	      || memcmp (tv[i].der_str + 1, der, tv[i].der_len - 1) != 0)
	    {
	      fprintf (stderr,
		       "encoding iter %lu: got invalid encoding\n",
		       (unsigned long) i);
	      return 1;
	    }
	}
      ret =
	asn1_decode_simple_ber (ASN1_ETYPE_OCTET_STRING,
				tv[i].der_str, tv[i].der_len,
				&tmp, (unsigned int*)&str_size, (unsigned int*)&der_len);
      if (ret != tv[i].expected_error)
	{
	  fprintf (stderr,
		   "%d: asn1_decode_simple_ber iter %lu: got %s expected %s\n",
		   __LINE__, (unsigned long) i, asn1_strerror(ret), asn1_strerror(tv[i].expected_error));
	  return 1;
	}

      if (der_len != tv[i].der_len)
	{
	  fprintf (stderr,
		   "%d: error in iter %lu: DER, length returned is %d, had %d\n",
		   __LINE__, (unsigned long)i, der_len, tv[i].der_len);
	  return 1;
	}

      if (str_size != tv[i].len || memcmp (tv[i].string, tmp, tv[i].len) != 0)
	{
	  fprintf (stderr,
		   "%d: memcmp iter %lu: got invalid decoding\n",
		   __LINE__, (unsigned long) i);
	  return 1;
	}
      free (tmp);
      tmp = NULL;

    }

  return 0;
}
