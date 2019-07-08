#include <iostream>
#include <git2.h>


int main()
{
	git_libgit2_init();
	git_repository * repo = nullptr;
	
	// Open the repository
	int error = git_repository_open(&repo, R"(C:\Users\Alis\Documents\Visual Studio 2017\Projects\gitlib2-testing-cpp\clone-repo)");
	if (error < 0) {
		const git_error *e = giterr_last();
		printf("ERROR: %d/%d: %s\n", error, e->klass, e->message);
		exit(error);
	}

	// Declare and initialize the revision walker
	git_revwalk * walker = nullptr;
	git_revwalk_new(&walker, repo);
	git_revwalk_push_head(walker);
	git_oid oid;
	while (!git_revwalk_next(&oid, walker))
	{
		// -- get the current commit --
		git_commit * commit = nullptr;
		git_commit_lookup(&commit, repo, &oid);
		std::cout << git_oid_tostr_s(&oid) << " " << git_commit_summary(commit) << " " << git_commit_author(commit)->name << std::endl;
		git_commit_free(commit);
	}

	// Clean Up
	git_revwalk_free(walker);
	git_repository_free(repo);
	git_libgit2_shutdown();

	return 0;
}