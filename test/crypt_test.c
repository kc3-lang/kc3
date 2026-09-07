#include <string.h>
#include "../libkc3/crypt.h"
#include "../libkc3/str.h"
#include "test.h"

#define CRYPT_TEST_CHECK_PASSWORD(password, stored_hash, expected)      \
  do {                                                               \
    s_str pass;                                                      \
    bool result = ! (expected);                                      \
    str_init_1(&pass, NULL, password);                                \
    TEST_EQ(crypt_check_password(&pass, stored_hash, &result),          \
            &result);                                                \
    TEST_EQ(result, expected);                                       \
  } while (0)

static const char g_crypt_test_password[] =
  "Xy01@#\x01\x02\x80\x7f\xff\r\n\x81\t !";
static const char g_crypt_test_hash[] =
  "$6$rounds=1234$abc0123456789$"
  "BCpt8zLrc/RcyuXmCDOE1ALqMXB2MH6n1g891HhFj8.w7LxGv.FTkqq6Vxc/"
  "km3Y0jE0j24jY5PIv/oOu6reg1";

TEST_CASE_PROTOTYPE(crypt_check_password_match);
TEST_CASE_PROTOTYPE(crypt_check_password_mismatch);
TEST_CASE_PROTOTYPE(crypt_check_password_modified_hash);
TEST_CASE_PROTOTYPE(crypt_check_password_hash_length);
TEST_CASE_PROTOTYPE(crypt_hash_password);

void crypt_test (void)
{
  TEST_CASE_RUN(crypt_check_password_match);
  TEST_CASE_RUN(crypt_check_password_mismatch);
  TEST_CASE_RUN(crypt_check_password_modified_hash);
  TEST_CASE_RUN(crypt_check_password_hash_length);
  TEST_CASE_RUN(crypt_hash_password);
}

TEST_CASE(crypt_check_password_match)
{
  s_str hash;
  str_init_1(&hash, NULL, g_crypt_test_hash);
  CRYPT_TEST_CHECK_PASSWORD(g_crypt_test_password, &hash, true);
}
TEST_CASE_END(crypt_check_password_match)

TEST_CASE(crypt_check_password_mismatch)
{
  s_str hash;
  str_init_1(&hash, NULL, g_crypt_test_hash);
  CRYPT_TEST_CHECK_PASSWORD("wrong password", &hash, false);
  CRYPT_TEST_CHECK_PASSWORD("", &hash, false);
}
TEST_CASE_END(crypt_check_password_mismatch)

TEST_CASE(crypt_check_password_modified_hash)
{
  s_str hash;
  uw i;
  char saved;
  char value[sizeof(g_crypt_test_hash)];
  memcpy(value, g_crypt_test_hash, sizeof(value));
  str_init_1(&hash, NULL, value);
  i = sizeof("$6$rounds=1234$abc0123456789$") - 1;
  while (i < hash.size) {
    saved = value[i];
    value[i] = saved == '.' ? '/' : '.';
    CRYPT_TEST_CHECK_PASSWORD(g_crypt_test_password, &hash, false);
    value[i] = saved;
    i++;
  }
}
TEST_CASE_END(crypt_check_password_modified_hash)

TEST_CASE(crypt_check_password_hash_length)
{
  s_str hash;
  s_str suffix = {{NULL}, 1, {"x"}};
  s_str extended;
  str_init_1(&hash, NULL, "$6$rounds=1234$abc0123456789$");
  CRYPT_TEST_CHECK_PASSWORD(g_crypt_test_password, &hash, false);
  str_init_1(&hash, NULL, g_crypt_test_hash);
  hash.size--;
  CRYPT_TEST_CHECK_PASSWORD(g_crypt_test_password, &hash, false);
  hash.size++;
  TEST_ASSERT(str_init_concatenate(&extended, &hash, &suffix));
  CRYPT_TEST_CHECK_PASSWORD(g_crypt_test_password, &extended, false);
  str_clean(&extended);
}
TEST_CASE_END(crypt_check_password_hash_length)

TEST_CASE(crypt_hash_password)
{
  s_str hash;
  s_str pass;
  str_init_1(&pass, NULL, "KC3 test password");
  TEST_ASSERT(crypt_hash_password(&pass, &hash));
  CRYPT_TEST_CHECK_PASSWORD("KC3 test password", &hash, true);
  CRYPT_TEST_CHECK_PASSWORD("wrong password", &hash, false);
  str_clean(&hash);
}
TEST_CASE_END(crypt_hash_password)
