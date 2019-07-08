
#include "git2.h"
#include <iostream>

int main()
{
	git_libgit2_init();
	git_repository *repo = NULL;
	const char *url = R"(C:\Users\Alis\Documents\Visual Studio 2017\Projects\gitlib2-testing-cpp\testing-repos\clone-test/.git)";
	const char *path = "./clone-test3/";
	int error = git_clone(&repo, url, path, NULL);
	if (error < 0) {
		const git_error *e = giterr_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		exit(error);
	}
	git_repository_free(repo);
	git_libgit2_shutdown();
	return 0;
}