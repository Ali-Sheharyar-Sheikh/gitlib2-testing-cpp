#include <iostream>
#include <git2.h>

static void print_checkout_progress(const char *path, size_t completed_steps, size_t total_steps, void *payload)
{
	(void)payload;
	if (path == NULL) {
		printf("checkout started: % Iu steps\n", total_steps);
	}
	else {
		printf("checkout: %s %Iu/%Iu\n", path, completed_steps, total_steps);
	}
}

int main(int argc, char **argv)
{
	git_libgit2_init();
	git_repository * repo = nullptr;

	// Open the repository
	int error = git_repository_open(&repo, "SanityGen-clone");
	if (error < 0) {
		const git_error *e = giterr_last();
		printf("ERROR: %d/%d: %s\n", error, e->klass, e->message);
		exit(error);
	}

	// Check repository state before checkout
	int state = git_repository_state(repo);
	if (state != GIT_REPOSITORY_STATE_NONE) {
		fprintf(stderr, "repository is in unexpected state %d\n", state);
	}
	// else { wait for the current action to complete or terminate }

	// Reference from the target name to checkout
	git_annotated_commit *checkout_target = NULL;
	git_reference *ref;
	int err = git_reference_dwim(&ref, repo,"origin/master");
	if (err == GIT_OK) 
	{
		git_annotated_commit_from_ref(&checkout_target, repo, ref);
		git_reference_free(ref);
	}
	else
	{
		git_object *obj;
		err = git_revparse_single(&obj, repo, "origin/master");
		if (err == GIT_OK) {
			err = git_annotated_commit_lookup(&checkout_target, repo, git_object_id(obj));
			git_object_free(obj);
		}
		else
		{
			std::cout << "Error!" << std::endl;
			exit(err);
		}
	}

	git_checkout_options checkout_opts = GIT_CHECKOUT_OPTIONS_INIT;
	checkout_opts.checkout_strategy = GIT_CHECKOUT_FORCE;
	checkout_opts.progress_cb = print_checkout_progress;
	git_commit *target_commit = NULL;

	// Grab the commit we're interested to move to
	err = git_commit_lookup(&target_commit, repo, git_annotated_commit_id(checkout_target));
	if (err != 0) {
		fprintf(stderr, "failed to lookup commit: %s\n", git_error_last()->message);
		exit(err);
	}

	// Checkout the commit
	err = git_checkout_tree(repo, (const git_object *)target_commit, &checkout_opts);
	if (err != 0) {
		fprintf(stderr, "failed to checkout tree: %s\n", git_error_last()->message);
		exit(err);
	}

	// Checkout complete update the HEAD pointer
	if (git_annotated_commit_ref(checkout_target)) {
		err = git_repository_set_head(repo, git_annotated_commit_ref(checkout_target));
	}
	else {
		err = git_repository_set_head_detached_from_annotated(repo, checkout_target);
	}
	if (err != 0) {
		fprintf(stderr, "failed to update HEAD reference: %s\n", git_error_last()->message);
		exit(err);
	}

	git_commit_free(target_commit);
	git_repository_free(repo);
	git_libgit2_shutdown();

	return 0;
}