#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "tests.h"

int main(void) {
  const struct CMUnitTest tests[] = {
    cmocka_unit_test(test_hmac_md5),
    cmocka_unit_test(test_smb_utf16),
    cmocka_unit_test(test_smb_buffer_init),
    cmocka_unit_test(test_smb_buffer_alloc),
    cmocka_unit_test(test_nb_encode),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
