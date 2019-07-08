#include "git2.h"
#include <iostream>

int main()
{
	/* Convert a SHA to an OID */
	const char *sha = "4a202b346bb0fb0db7eff3cffeb3c70babbd2045";
	git_oid oid = { 0 };
	int error = git_oid_fromstr(&oid, sha);

	/* Make a shortened printable string from an OID */
	char shortsha[20] = { 0 };
	git_oid_tostr(shortsha, 20, &oid);
	return 0;
}