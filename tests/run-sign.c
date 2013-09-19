/* run-sign.c  - Helper to perform a sign operation
   Copyright (C) 2009 g10 Code GmbH

   This file is part of GPGME.

   GPGME is free software; you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   GPGME is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this program; if not, see <http://www.gnu.org/licenses/>.
*/

/* We need to include config.h so that we know whether we are building
   with large file system (LFS) support. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <gpgme.h>

#define PGM "run-sign"

#include "run-support.h"


static int verbose;


static void
print_result (gpgme_sign_result_t result, gpgme_sig_mode_t type)
{
  gpgme_invalid_key_t invkey;
  gpgme_new_signature_t sig;

  for (invkey = result->invalid_signers; invkey; invkey = invkey->next)
    printf ("Signing key `%s' not used: %s <%s>\n",
            nonnull (invkey->fpr),
            gpg_strerror (invkey->reason), gpg_strsource (invkey->reason));

  for (sig = result->signatures; sig; sig = sig->next)
    {
      printf ("Key fingerprint: %s\n", nonnull (sig->fpr));
      printf ("Signature type : %d\n", sig->type);
      printf ("Public key algo: %d\n", sig->pubkey_algo);
      printf ("Hash algo .....: %d\n", sig->hash_algo);
      printf ("Creation time .: %ld\n", sig->timestamp);
      printf ("Sig class .....: 0x%u\n", sig->sig_class);
    }
}



static int
show_usage (int ex)
{
  fputs ("usage: " PGM " [options] FILE\n\n"
         "Options:\n"
         "  --verbose        run in verbose mode\n"
         "  --openpgp        use the OpenPGP protocol (default)\n"
         "  --cms            use the CMS protocol\n"
         "  --uiserver       use the UI server\n"
         "  --key NAME       use key NAME for signing\n"
         , stderr);
  exit (ex);
}


int
main (int argc, char **argv)
{
  int last_argc = -1;
  gpgme_error_t err;
  gpgme_ctx_t ctx;
  const char *key_string = NULL;
  gpgme_protocol_t protocol = GPGME_PROTOCOL_OpenPGP;
  gpgme_sig_mode_t sigmode = GPGME_SIG_MODE_NORMAL;
  gpgme_data_t in, out;
  gpgme_sign_result_t result;

  if (argc)
    { argc--; argv++; }

  while (argc && last_argc != argc )
    {
      last_argc = argc;
      if (!strcmp (*argv, "--"))
        {
          argc--; argv++;
          break;
        }
      else if (!strcmp (*argv, "--help"))
        show_usage (0);
      else if (!strcmp (*argv, "--verbose"))
        {
          verbose = 1;
          argc--; argv++;
        }
      else if (!strcmp (*argv, "--openpgp"))
        {
          protocol = GPGME_PROTOCOL_OpenPGP;
          argc--; argv++;
        }
      else if (!strcmp (*argv, "--cms"))
        {
          protocol = GPGME_PROTOCOL_CMS;
          argc--; argv++;
        }
      else if (!strcmp (*argv, "--uiserver"))
        {
          protocol = GPGME_PROTOCOL_UISERVER;
          argc--; argv++;
        }
      else if (!strcmp (*argv, "--key"))
        {
          argc--; argv++;
          if (!argc)
            show_usage (1);
          key_string = *argv;
          argc--; argv++;
        }
      else if (!strncmp (*argv, "--", 2))
        show_usage (1);

    }

  if (argc != 1)
    show_usage (1);

  if (key_string && protocol == GPGME_PROTOCOL_UISERVER)
    {
      fprintf (stderr, PGM ": ignoring --key in UI-server mode\n");
      key_string = NULL;
    }

  init_gpgme (protocol);

  err = gpgme_new (&ctx);
  fail_if_err (err);
  gpgme_set_protocol (ctx, protocol);
  gpgme_set_armor (ctx, 1);

  if (key_string)
    {
      gpgme_key_t akey;

      err = gpgme_get_key (ctx, key_string, &akey, 1);
      if (err)
        {
          exit (1);
        }
      err = gpgme_signers_add (ctx, akey);
      fail_if_err (err);
      gpgme_key_unref (akey);
    }

  err = gpgme_data_new_from_file (&in, *argv, 1);
  if (err)
    {
      fprintf (stderr, PGM ": error reading `%s': %s\n",
               *argv, gpg_strerror (err));
      exit (1);
    }

  err = gpgme_data_new (&out);
  fail_if_err (err);

  err = gpgme_op_sign (ctx, in, out, sigmode);
  result = gpgme_op_sign_result (ctx);
  if (result)
    print_result (result, sigmode);
  if (err)
    {
      fprintf (stderr, PGM ": signing failed: %s\n", gpg_strerror (err));
      exit (1);
    }

  fputs ("Begin Output:\n", stdout);
  print_data (out);
  fputs ("End Output.\n", stdout);
  gpgme_data_release (out);

  gpgme_data_release (in);

  gpgme_release (ctx);
  return 0;
}
