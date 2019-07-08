
#include <git2.h>
#include <iostream>
#include <chrono>

#define PRIuZ "Iu"

int fetch(git_repository *repo, const char* szTarget);
int checkout(git_repository *repo, const char* szTarget);

void format_elapsed_time(char *time_str, double elapsed) {
	int h, m, s, ms;
	h = m = s = ms = 0;
	ms = (int)(elapsed * 1000); // promote the fractional part to milliseconds
	h = ms / 3600000;
	ms -= (h * 3600000);
	m = ms / 60000;
	ms -= (m * 60000);
	s = ms / 1000;
	ms -= (s * 1000);
	sprintf_s(time_str, 100, "%02i:%02i:%02i.%03i", h, m, s, ms);
}


int main(int argc, char **argv)
{
	auto start = std::chrono::high_resolution_clock::now();

	git_libgit2_init();
	git_repository * repo = nullptr;

	// Open the repository
	int error = git_repository_open(&repo, R"(SanityGen-clone)");
	if (error < 0) {
		const git_error *e = giterr_last();
		printf("ERROR: %d/%d: %s\n", error, e->klass, e->message);
		exit(error);
	}

	// fetch origin
	fetch(repo, "origin");
	checkout(repo, "origin/master");

	git_repository_free(repo);
	git_libgit2_shutdown();

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	char szString[100];
	format_elapsed_time(szString, elapsed.count());
	std::cout << "Fetch and checkout completed in " << szString << std::endl;

	return 0;
}

int fetch(git_repository *repo, const char* szTarget)
{
	git_remote *remote = NULL;
	const git_transfer_progress *stats;
	git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;

	printf("Fetching %s for repo %p\n", szTarget, repo);
	if (git_remote_lookup(&remote, repo, szTarget) < 0)
		if (git_remote_create_anonymous(&remote, repo, szTarget) < 0)
			goto on_error;

	if (git_remote_fetch(remote, NULL, &fetch_opts, "fetch") < 0) {
		const git_error *e = giterr_last();
		printf("ERROR: %d/%d: %s\n", 0, e->klass, e->message);
		goto on_error;
	}

	stats = git_remote_stats(remote);
	if (stats->local_objects > 0) {
		printf("\rReceived %d/%d objects in %" PRIuZ " bytes (used %d local objects)\n",
			stats->indexed_objects, stats->total_objects, stats->received_bytes, stats->local_objects);
	}
	else {
		printf("\rReceived %d/%d objects in %" PRIuZ "bytes\n",
			stats->indexed_objects, stats->total_objects, stats->received_bytes);
	}

	git_remote_free(remote);

	return 0;

on_error:
	git_remote_free(remote);
	return -1;
}


int checkout(git_repository *repo, const char* szTarget)
{
	// Reference from the target name to checkout
	git_annotated_commit *checkout_target = NULL;
	git_reference *ref;
	int err = git_reference_dwim(&ref, repo, szTarget);
	if (err == GIT_OK)
	{
		git_annotated_commit_from_ref(&checkout_target, repo, ref);
		git_reference_free(ref);
	}
	else
	{
		git_object *obj;
		err = git_revparse_single(&obj, repo, szTarget);
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
	git_commit *target_commit = NULL;

	// Grab the commit we're interested to move to
	err = git_commit_lookup(&target_commit, repo, git_annotated_commit_id(checkout_target));
	if (err != 0) {
		fprintf(stderr, "failed to lookup commit: %s\n", git_error_last()->message);
		exit(err);
	}

	// Checkout the commit
	git_checkout_options oCheckoutOptions = GIT_CHECKOUT_OPTIONS_INIT;
	oCheckoutOptions.checkout_strategy = GIT_CHECKOUT_FORCE;
	err = git_checkout_tree(repo, (const git_object *)target_commit, &oCheckoutOptions);
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
	return 1;
}