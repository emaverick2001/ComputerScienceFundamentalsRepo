#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"
#include "uint256.h"

typedef struct
{
  UInt256 zero;    // the value equal to 0
  UInt256 one;     // the value equal to 1
  UInt256 max;     // the value equal to (2^256)-1
  UInt256 msb_set; // the value equal to 2^255
  UInt256 rot;     // value used to test rotations
} TestObjs;

// Helper functions for implementing tests
void set_all(UInt256 *val, uint32_t wordval);

#define ASSERT_SAME(expected, actual)           \
  do                                            \
  {                                             \
    ASSERT(expected.data[0] == actual.data[0]); \
    ASSERT(expected.data[1] == actual.data[1]); \
    ASSERT(expected.data[2] == actual.data[2]); \
    ASSERT(expected.data[3] == actual.data[3]); \
    ASSERT(expected.data[4] == actual.data[4]); \
    ASSERT(expected.data[5] == actual.data[5]); \
    ASSERT(expected.data[6] == actual.data[6]); \
    ASSERT(expected.data[7] == actual.data[7]); \
  } while (0)

#define INIT_FROM_ARR(val, arr)      \
  do                                 \
  {                                  \
    for (unsigned i = 0; i < 8; ++i) \
      val.data[i] = arr[i];          \
  } while (0)

// Functions to create and cleanup the test fixture object
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// Declarations of test functions
void test_get_bits(TestObjs *objs);
void test_create_from_u32(TestObjs *objs);
void test_create(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_rotate_left(TestObjs *objs);
void test_rotate_right(TestObjs *objs);

int main(int argc, char **argv)
{
  if (argc > 1)
  {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(test_get_bits);
  TEST(test_create_from_u32);
  TEST(test_create);
  TEST(test_create_from_hex);
  TEST(test_format_as_hex);
  TEST(test_add);
  TEST(test_sub);
  TEST(test_negate);
  TEST(test_rotate_left);
  TEST(test_rotate_right);

  TEST_FINI();
}

// Set all of the "words" of a UInt256 to a specific initial value
void set_all(UInt256 *val, uint32_t wordval)
{
  for (unsigned i = 0; i < 8; ++i)
  {
    val->data[i] = wordval;
  }
}

TestObjs *setup(void)
{
  TestObjs *objs = (TestObjs *)malloc(sizeof(TestObjs));

  // initialize several UInt256 values "manually"
  set_all(&objs->zero, 0);
  set_all(&objs->one, 0);
  objs->one.data[0] = 1U;
  set_all(&objs->max, 0xFFFFFFFFU);

  // create a value with only the most-significant bit set
  uint32_t msb_set_data[8] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0x80000000U};
  INIT_FROM_ARR(objs->msb_set, msb_set_data);

  // value with nonzero values in least significant and most significant words
  // Note that the hex representation of this value is
  //   CD000000 00000000 00000000 00000000 00000000 00000000 00000000 000000AB
  uint32_t rot_data[8] = {0x000000ABU, 0U, 0U, 0U, 0U, 0U, 0U, 0xCD000000U};
  INIT_FROM_ARR(objs->rot, rot_data);

  return objs;
}

void cleanup(TestObjs *objs)
{
  free(objs);
}

void test_get_bits(TestObjs *objs)
{
  ASSERT(0U == uint256_get_bits(objs->zero, 0));
  ASSERT(0U == uint256_get_bits(objs->zero, 1));
  ASSERT(0U == uint256_get_bits(objs->zero, 2));
  ASSERT(0U == uint256_get_bits(objs->zero, 3));
  ASSERT(0U == uint256_get_bits(objs->zero, 4));
  ASSERT(0U == uint256_get_bits(objs->zero, 5));
  ASSERT(0U == uint256_get_bits(objs->zero, 6));
  ASSERT(0U == uint256_get_bits(objs->zero, 7));

  ASSERT(1U == uint256_get_bits(objs->one, 0));
  ASSERT(0U == uint256_get_bits(objs->one, 1));
  ASSERT(0U == uint256_get_bits(objs->one, 2));
  ASSERT(0U == uint256_get_bits(objs->one, 3));
  ASSERT(0U == uint256_get_bits(objs->one, 4));
  ASSERT(0U == uint256_get_bits(objs->one, 5));
  ASSERT(0U == uint256_get_bits(objs->one, 6));
  ASSERT(0U == uint256_get_bits(objs->one, 7));

  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 0));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 1));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 2));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 3));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 4));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 5));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 6));
  ASSERT(0xFFFFFFFFU == uint256_get_bits(objs->max, 7));
}

void test_create_from_u32(TestObjs *objs)
{
  UInt256 zero = uint256_create_from_u32(0U);
  UInt256 one = uint256_create_from_u32(1U);
  // Cant test max since only 1 place value is being initialized
  //  UInt256 max = uint256_create_from_u32(0xFFFFFFFFU);

  ASSERT_SAME(objs->zero, zero);
  ASSERT_SAME(objs->one, one);
  // ASSERT_SAME(objs->max, max);
}

void test_create(TestObjs *objs)
{
  // telling compiler objs is not being used
  (void)objs;

  uint32_t data1[8] = {1U, 2U, 3U, 4U, 5U, 6U, 7U, 8U};
  UInt256 val1 = uint256_create(data1);
  ASSERT(1U == val1.data[0]);
  ASSERT(2U == val1.data[1]);
  ASSERT(3U == val1.data[2]);
  ASSERT(4U == val1.data[3]);
  ASSERT(5U == val1.data[4]);
  ASSERT(6U == val1.data[5]);
  ASSERT(7U == val1.data[6]);
  ASSERT(8U == val1.data[7]);

  uint32_t data2[8] = {0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU};

  UInt256 val2 = uint256_create(data2);

  ASSERT_SAME(objs->max, val2);
}

void test_create_from_hex(TestObjs *objs)
{

  UInt256 hex_63_char;
  set_all(&hex_63_char, 0);
  hex_63_char.data[7] = 16777216;
  UInt256 hex_63_char_test = uint256_create_from_hex("100000000000000000000000000000000000000000000000000000000000000");
  ASSERT_SAME(hex_63_char, hex_63_char_test);

  UInt256 hex_4_char;
  set_all(&hex_4_char, 0);
  hex_4_char.data[0] = 4112;
  UInt256 hex_4_char_test = uint256_create_from_hex("1010");
  ASSERT_SAME(hex_4_char, hex_4_char_test);

  UInt256 zero = uint256_create_from_hex("0");
  ASSERT_SAME(objs->zero, zero);

  UInt256 one = uint256_create_from_hex("1");
  ASSERT_SAME(objs->one, one);

  UInt256 two_actual;
  set_all(&two_actual, 0U);
  two_actual.data[0] = 2U;
  UInt256 two = uint256_create_from_hex("2");
  ASSERT_SAME(two_actual, two);

  UInt256 max = uint256_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, max);

  UInt256 hex_65_char_test = uint256_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
  ASSERT_SAME(objs->max, hex_65_char_test);

  UInt256 num_and_letter;
  uint32_t num_and_letter_data[8] = {0xabcd1234U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
  INIT_FROM_ARR(num_and_letter, num_and_letter_data);

  UInt256 num_and_letter_test = uint256_create_from_hex("abcd1234");
  ASSERT_SAME(num_and_letter, num_and_letter_test);
}

void test_format_as_hex(TestObjs *objs)
{
  char *s;

  s = uint256_format_as_hex(objs->zero);
  ASSERT(0 == strcmp("0", s));
  free(s);

  s = uint256_format_as_hex(objs->one);
  ASSERT(0 == strcmp("1", s));
  free(s);

  UInt256 two;
  set_all(&two, 0U);
  two.data[0] = 2U;
  s = uint256_format_as_hex(two);
  ASSERT(0 == strcmp("2", s));
  free(s);

  s = uint256_format_as_hex(objs->msb_set);
  ASSERT(0 == strcmp("8000000000000000000000000000000000000000000000000000000000000000", s));
  free(s);

  s = uint256_format_as_hex(objs->max);
  ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff", s));
  free(s);
}

void test_add(TestObjs *objs)
{
  UInt256 result;

  result = uint256_add(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_add(objs->zero, objs->one);
  ASSERT_SAME(objs->one, result);

  result = uint256_add(objs->max, objs->zero);
  ASSERT_SAME(objs->max, result);

  result = uint256_add(objs->zero, objs->max);
  ASSERT_SAME(objs->max, result);

  result = uint256_add(objs->max, objs->one);
  ASSERT_SAME(objs->zero, result);

  uint32_t two_data[8] = {2U};
  UInt256 two;
  INIT_FROM_ARR(two, two_data);
  result = uint256_add(objs->one, objs->one);
  ASSERT_SAME(two, result);

  UInt256 case1;
  uint32_t case1_data[8] = {0xFFFFFFFFU};
  UInt256 case1s;
  uint32_t case1s_data[8] = {0U, 1U, 0U, 0U, 0U, 0U, 0U, 0U};

  INIT_FROM_ARR(case1, case1_data);
  INIT_FROM_ARR(case1s, case1s_data);
  result = uint256_add(case1, objs->one);
  ASSERT_SAME(case1s, result);

  UInt256 case2;
  uint32_t case2_data[8] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0xFFFFFFFFU};
  UInt256 one_at_end;
  uint32_t one_at_end_data[8] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U};
  UInt256 case2s;
  uint32_t case2s_data[8] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};

  INIT_FROM_ARR(case2, case2_data);
  INIT_FROM_ARR(case2s, case2s_data);
  INIT_FROM_ARR(one_at_end, one_at_end_data);
  result = uint256_add(case2, one_at_end);
  ASSERT_SAME(case2s, result);

  result = uint256_add(objs->max, objs->one);
  ASSERT_SAME(objs->zero, result);
}

void test_sub(TestObjs *objs)
{
  UInt256 result;

  result = uint256_sub(objs->zero, objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->one, objs->one);
  ASSERT_SAME(objs->zero, result);

  result = uint256_sub(objs->zero, objs->one);
  ASSERT_SAME(objs->max, result);

  UInt256 five;
  uint32_t five_data[8] = {5U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
  INIT_FROM_ARR(five, five_data);

  UInt256 four;
  uint32_t four_data[8] = {4U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
  INIT_FROM_ARR(four, four_data);

  result = uint256_sub(five, objs->one);
  ASSERT_SAME(four, result);

  result = uint256_sub(objs->zero, objs->max);
  ASSERT_SAME(objs->one, result);
}

void test_negate(TestObjs *objs)
{
  UInt256 result;

  result = uint256_negate(objs->zero);
  ASSERT_SAME(objs->zero, result);

  result = uint256_negate(objs->one);
  ASSERT_SAME(objs->max, result);

  result = uint256_negate(objs->max);
  ASSERT_SAME(objs->one, result);

  UInt256 five;
  uint32_t five_data[8] = {5U, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
  INIT_FROM_ARR(five, five_data);

  UInt256 five_s;
  uint32_t five_s_data[8] = {4294967291U, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU};
  INIT_FROM_ARR(five_s, five_s_data);

  result = uint256_negate(five);
  ASSERT_SAME(five_s, result);

  UInt256 max_first;
  uint32_t max_first_data[8] = {0xFFFFFFFFU, 0U, 0U, 0U, 0U, 0U, 0U, 0U};
  UInt256 max_first_s;
  uint32_t max_first_s_data[8] = {1U, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU};

  INIT_FROM_ARR(max_first, max_first_data);
  INIT_FROM_ARR(max_first_s, max_first_s_data);
  result = uint256_negate(max_first);
  ASSERT_SAME(max_first_s, result);

  UInt256 middle_is_one;
  uint32_t middle_is_one_data[8] = {0U, 0U, 0U, 1U, 0U, 0U, 0U, 0U};
  UInt256 middle_is_one_s;
  uint32_t middle_is_one_s_data[8] = {0U, 0U, 0U, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU};

  INIT_FROM_ARR(middle_is_one, middle_is_one_data);
  INIT_FROM_ARR(middle_is_one_s, middle_is_one_s_data);
  result = uint256_negate(middle_is_one);
  ASSERT_SAME(middle_is_one_s, result);

  UInt256 last_is_one;
  uint32_t last_is_one_data[8] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 1U};
  INIT_FROM_ARR(last_is_one, last_is_one_data);

  UInt256 last_is_one_s;
  uint32_t last_is_one_s_data[8] = {0U, 0U, 0U, 0U, 0U, 0U, 0U, 0xFFFFFFFFU};
  INIT_FROM_ARR(last_is_one_s, last_is_one_s_data);

  result = uint256_negate(last_is_one);
  ASSERT_SAME(last_is_one_s, result);
}

void test_rotate_left(TestObjs *objs)
{
  UInt256 result;

  // rotating the value with just the most significant bit set
  // one position to the left should result in the value equal to 1
  // (i.e., the value with only the least significant bit set)
  result = uint256_rotate_left(objs->msb_set, 1);
  ASSERT_SAME(objs->one, result);

  result = uint256_rotate_left(objs->msb_set, 0);
  ASSERT_SAME(result, result);

  // rot val: (first = most sig)
  //    CD000000, 0U, 0U, 0U, 0U, 0U, 0U, 000000AB
  // after rotating the "rot" value left by 4 bits, the resulting value should be
  //   D0000000 00000000 00000000 00000000 00000000 00000000 00000000 00000ABC
  result = uint256_rotate_left(objs->rot, 4);
  ASSERT(0x00000ABCU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xD0000000U == result.data[7]);

  UInt256 alt_num;
  uint32_t alt_num_data[8] = {0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU};
  INIT_FROM_ARR(alt_num, alt_num_data);

  UInt256 alt_num_32shift;
  uint32_t alt_num_32shift_data[8] = {0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U};
  INIT_FROM_ARR(alt_num_32shift, alt_num_32shift_data);
  result = uint256_rotate_left(alt_num, 32);
  ASSERT_SAME(alt_num_32shift, result);

  UInt256 alt_num_4shift;
  uint32_t alt_num_4shift_data[8] = {0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU};
  INIT_FROM_ARR(alt_num_4shift, alt_num_4shift_data);

  result = uint256_rotate_left(alt_num_4shift, 4);
  ASSERT(0x0000000FU == result.data[0]);
  ASSERT(0xFFFFFFF0U == result.data[1]);
  ASSERT(0x0000000FU == result.data[2]);
  ASSERT(0xFFFFFFF0U == result.data[3]);
  ASSERT(0x0000000FU == result.data[4]);
  ASSERT(0xFFFFFFF0U == result.data[5]);
  ASSERT(0x0000000FU == result.data[6]);
  ASSERT(0xFFFFFFF0U == result.data[7]);
}

void test_rotate_right(TestObjs *objs)
{
  UInt256 result;

  // rotating 1 right by 1 position should result in a value with just
  // the most-significant bit set
  result = uint256_rotate_right(objs->one, 1);
  ASSERT_SAME(objs->msb_set, result);

  // after rotating the "rot" value right by 4 bits, the resulting value should be
  //   BCD00000 00000000 00000000 00000000 00000000 00000000 00000000 0000000A
  result = uint256_rotate_right(objs->rot, 4);
  ASSERT(0x0000000AU == result.data[0]);
  ASSERT(0U == result.data[1]);
  ASSERT(0U == result.data[2]);
  ASSERT(0U == result.data[3]);
  ASSERT(0U == result.data[4]);
  ASSERT(0U == result.data[5]);
  ASSERT(0U == result.data[6]);
  ASSERT(0xBCD00000U == result.data[7]);

  UInt256 alt_num;
  uint32_t alt_num_data[8] = {0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU};
  INIT_FROM_ARR(alt_num, alt_num_data);

  UInt256 alt_num_32shift;
  uint32_t alt_num_32shift_data[8] = {0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U, 0xFFFFFFFFU, 0x00000000U};
  INIT_FROM_ARR(alt_num_32shift, alt_num_32shift_data);
  result = uint256_rotate_right(alt_num, 32);
  ASSERT_SAME(alt_num_32shift, result);

  result = uint256_rotate_right(objs->zero, 273);
  ASSERT_SAME(objs->zero, result);
}