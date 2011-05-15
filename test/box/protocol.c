#include <connector/c/client.h>
#include <include/errcode.h>
#include <stdio.h>

/** Server connection. Reused between tests. */
struct tnt_connection *conn;

/** Test the ping command. */
void test_ping()
{
	const char message[]= {
		0xd, 0x0, 0x0, 0x0,    0x11, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0,    0x1, 0x0, 0x0, 0x0,
		0x4, 0x1, 0x0, 0x0, 0x0 };
	int res = tnt_execute_raw(conn, message, sizeof message, 0);
	printf("return_code: %d\n", res); /* =0 */
}


/** A test case for Bug#702397
 * https://bugs.launchpad.net/tarantool/+bug/702397 "If SELECT
 * request specifies tuple count 0, no error"
 */

void test_bug702397()
{
	const char message[]= {
		0x11, 0x0, 0x0, 0x0,    0x14, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0,     0x0, 0x0, 0x0, 0x0,     0x0, 0x0, 0x0, 0x0,
		0xff, 0xff, 0xff, 0xff, 0x0, 0x0, 0x0, 0x0 };
	struct tnt_result tnt_res;
	int res = tnt_execute_raw(conn, message, sizeof message, &tnt_res);
	printf("return_code: %s, %s\n",
	       tnt_errcode_str(tnt_res.errcode >> 8),
	       tnt_errcode_desc(tnt_res.errcode >> 8));
}


/** A test case for Bug#702399
 * https://bugs.launchpad.net/tarantool/+bug/702399
 * ERR_CODE_ILLEGAL_PARAMS is returned when there is no such key
 */

void test_bug702399()
{
	const char message[]= {
		0x11, 0x0, 0x0, 0x0,    0x1d, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
		0x1, 0x0, 0x0, 0x0,     0x0, 0x0, 0x0, 0x0,
		0xff, 0xff, 0xff, 0xff,
		0x1, 0x0, 0x0, 0x0,     0x1, 0x0, 0x0, 0x0,
		0x4,    0x1, 0x0, 0x0, 0x0 };
	struct tnt_result tnt_res;
	int res = tnt_execute_raw(conn, message, sizeof message, &tnt_res);
	printf("return_code: %s, %s\n",
	       tnt_errcode_str(tnt_res.errcode >> 8),
	       tnt_errcode_desc(tnt_res.errcode >> 8));
}

/** A test case for Bug#776962
 * https://bugs.launchpad.net/tarantool/+bug/776962
 * Server crashes on junk operation id
 */

void test_bug776962()
{
	const char message[]= {
		0xff, 0x0, 0xff, 0x0,    0x4, 0x0, 0x0, 0x0,
		0x0, 0x0, 0x0, 0x0,    0x0, 0x0, 0x0, 0x0};
	struct tnt_result tnt_res;
	int res = tnt_execute_raw(conn, message, sizeof message, &tnt_res);
	printf("return_code: %s, %s\n",
	       tnt_errcode_str(tnt_res.errcode >> 8),
	       tnt_errcode_desc(tnt_res.errcode >> 8));
}

int main()
{
	conn = tnt_connect("localhost", 33013);
	if (conn == NULL)
		return 1;

	test_ping();
	test_bug702397();
	test_bug702399();
	test_bug776962();

	tnt_disconnect(conn);
	return 0;
}
