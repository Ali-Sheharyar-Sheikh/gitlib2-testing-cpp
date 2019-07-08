#include <iostream>
#include <git2.h>

int main(int argc, char **argv)
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

	/** Make sure we're not about to checkout while something else is going on */
	int state = git_repository_state(repo);
	if (state != GIT_REPOSITORY_STATE_NONE) {
		fprintf(stderr, "repository is in unexpected state %d\n", state);
	}

	git_annotated_commit *checkout_target = NULL;
	git_reference *ref;
	int err = git_reference_dwim(&ref, repo, "origin/commit6-branch");
	if (err == GIT_OK) {
		git_annotated_commit_from_ref(&checkout_target, repo, ref);
		git_reference_free(ref);
	}

	// Checkout Repository
	git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
	git_commit *target_commit = NULL;
	err = git_commit_lookup(&target_commit, repo, git_annotated_commit_id(checkout_target));
	if (err != 0) {
		fprintf(stderr, "failed to lookup commit: %s\n", git_error_last()->message);
	}
	err = git_checkout_tree(repo, (const git_object *)target_commit, &checkout_opts);
	if (err != 0) {
		fprintf(stderr, "failed to checkout tree: %s\n", git_error_last()->message);
	}
	if (git_annotated_commit_ref(checkout_target)) {
		err = git_repository_set_head(repo, git_annotated_commit_ref(checkout_target));
	}
	else {
		err = git_repository_set_head_detached_from_annotated(repo, checkout_target);
	}
	if (err != 0) {
		fprintf(stderr, "failed to update HEAD reference: %s\n", git_error_last()->message);
		goto cleanup;
	}

cleanup:
	git_commit_free(target_commit);

	git_repository_free(repo);
	git_libgit2_shutdown();

	return 0;
}