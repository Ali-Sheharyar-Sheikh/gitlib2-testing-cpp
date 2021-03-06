// gitlib2-testing-cpp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "git2.h"
#include <iostream>

static void create_initial_commit(git_repository *repo)
{
	git_signature *sig;
	git_index *index;
	git_oid tree_id, commit_id;
	git_tree *tree;

	if (git_signature_default(&sig, repo) < 0)
		std::cout << "Unable to create a commit signature. Perhaps 'user.name' and 'user.email' are not set" << std::endl;
	if (git_repository_index(&index, repo) < 0)
		std::cout << "Could not open repository index" << std::endl;

	if (git_index_write_tree(&tree_id, index) < 0)
		std::cout << "Unable to write initial tree from index" << std::endl;


	if (git_tree_lookup(&tree, repo, &tree_id) < 0)
		std::cout << "Could not look up initial tree" << std::endl;

	if (git_commit_create_v(
		&commit_id, repo, "HEAD", sig, sig,
		NULL, "Initial commit", tree, 0) < 0)
		std::cout << "Could not create the initial commit" << std::endl;

	git_index_free(index);
	git_tree_free(tree);
	git_signature_free(sig);

}

int main()
{
	git_libgit2_init();
	git_repository *repo = NULL;

	// Open repository
	int error = git_repository_init(&repo, "./git-repo-testing/", false);
	std::cout << error << std::endl;
	if (error == 0)
	{
		create_initial_commit(repo);
	}

	git_repository_free(repo);
	git_libgit2_shutdown();
	std::cin.get();
	return 0;
}

