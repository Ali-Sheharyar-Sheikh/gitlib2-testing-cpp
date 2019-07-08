
#include "git2.h"
#include <iostream>
#include <chrono>

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


int main()
{
	auto start = std::chrono::high_resolution_clock::now();

	git_libgit2_init();
	git_repository *repo = NULL;
	const char *url = R"(D:\Documents\DevDesigns\SanityGen\SanityGen/.git)";
	const char *path = "SanityGen-clone";
	int error = git_clone(&repo, url, path, NULL);
	if (error < 0) {
		const git_error *e = giterr_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		exit(error);
	}
	git_repository_free(repo);
	git_libgit2_shutdown();

	auto end = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	char szString[100];
	format_elapsed_time(szString, elapsed.count());
	
	std::cout << "Repository clone completed in " << szString << std::endl;

	return 0;
}