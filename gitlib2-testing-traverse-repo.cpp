
#include "stdafx.h"
#include "git2.h"
#include <iostream>

int main(int argc, char * argv[])
{
	// init libgit
	git_libgit2_init();

	// -- open repo --
	const char * REPO_PATH = R"(D:\Documents\dev\github-repos\FLOAT-QEMU-3.1.0)";

	git_repository * repo = nullptr;
	git_repository_open(&repo, REPO_PATH);

	// -- create revision walker --
	git_revwalk * walker = nullptr;
	git_revwalk_new(&walker, repo);

	// sort log by chronological order 
	git_revwalk_sorting(walker, GIT_SORT_NONE);

	// start from HEAD
	git_revwalk_push_head(walker);

	// -- walk the walk --
	git_oid oid;

	while (!git_revwalk_next(&oid, walker))
	{
		// -- get the current commit --
		git_commit * commit = nullptr;
		git_commit_lookup(&commit, repo, &oid);

		// output
		std::cout << git_oid_tostr_s(&oid) << " " << git_commit_summary(commit) << " " << git_commit_author(commit)->name << std::endl;

		// free the commit
		git_commit_free(commit);
	}

	// -- clean up --
	git_revwalk_free(walker);
	git_repository_free(repo);

	git_libgit2_shutdown();

	return 0;
}