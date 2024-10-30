#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>

// Color codes for output
#define BLACK "\033[0;30m"
#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define BLUE "\033[0;34m"
#define PURPLE "\033[0;35m"
#define CYAN "\033[0;36m"
#define WHITE "\033[0;37m"
#define NC "\033[0m" // No color

// Macro to choose the file descriptor for printing messages
#define FD_TO_WRITE 1
// Macro to control interception message printing
#define PRINT_INTERCEPTION 0

static int call_count = 0;
#define FAIL_ON_CALL 1 // Customize to fail on the nth call

// POSSIBLE ERRNO VALUES FOR DUP2 FAILURE
// Uncomment the one you want to use for testing
int custom_errno = EBADF;  // Bad file descriptor
// int custom_errno = EBUSY;  // Device or resource busy
// int custom_errno = EINTR;  // Interrupted function call
// int custom_errno = EINVAL; // Invalid argument
// int custom_errno = EMFILE;  // Too many open files

// Define a function pointer type for the real dup2
typedef int (*dup2_t)(int, int);
static dup2_t real_dup2 = NULL;

int dup2(int oldfd, int newfd)
{
	if (!real_dup2)
	{
		// Load the actual dup2 function
		real_dup2 = (dup2_t)dlsym(RTLD_NEXT, "dup2");
		if (!real_dup2)
		{
			const char *error_msg = RED "Error locating the real dup2\n" NC;
			syscall(SYS_write, 2, error_msg, strlen(error_msg));
			exit(EXIT_FAILURE);
		}
	}

	call_count++;
	const char *separator = CYAN "==============================================================================\n" NC;

	if (PRINT_INTERCEPTION)
	{
		const char *intercept_msg = PURPLE "Intercepted dup2\n" NC;
		syscall(SYS_write, FD_TO_WRITE, separator, strlen(separator));
		syscall(SYS_write, FD_TO_WRITE, intercept_msg, strlen(intercept_msg));
		syscall(SYS_write, FD_TO_WRITE, separator, strlen(separator));
	}

	// Fail on the specified call number
	if (call_count == FAIL_ON_CALL)
	{
		const char *fail_msg = PURPLE "Simulated dup2 failure\n" NC;
		syscall(SYS_write, FD_TO_WRITE, separator, strlen(separator));
		syscall(SYS_write, FD_TO_WRITE, fail_msg, strlen(fail_msg)); // Instead of strlen, could use sizeof("Msg" -1);
		syscall(SYS_write, FD_TO_WRITE, separator, strlen(separator));
		errno = custom_errno; // Set errno to the chosen error code
		return (-1);            // Simulate failure by returning -1
	}

	// Call the real dup2 function
	return (real_dup2(oldfd, newfd));
}

/* 
#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

static int call_count = 0;
#define FAIL_ON_CALL 1 // Customize to fail on the nth call

// Define a function pointer type for the real dup2
typedef int (*dup2_t)(int, int);
static dup2_t real_dup2 = NULL;

int dup2(int oldfd, int newfd)
{
	if (!real_dup2)
	{
		// Load the actual dup2 function
		real_dup2 = (dup2_t)dlsym(RTLD_NEXT, "dup2");
		if (!real_dup2)
		{
			const char *error_msg = "Error locating the real dup2\n";
			write(2, error_msg, strlen(error_msg));
			exit(EXIT_FAILURE);
		}
	}

	call_count++;
	const char *msg = "Intercepted dup2\n";
	write(1, msg, strlen(msg));  // Optional debug print

	// Fail on the specified call number
	if (call_count == FAIL_ON_CALL)
	{
		const char *fail_msg = "Simulated dup2 failure\n";
		write(1, fail_msg, strlen(fail_msg));
		errno = EBADF; // Set errno to a typical dup2 error code (Bad file descriptor)
		return -1;      // Simulate failure by returning -1
	}

	// Call the real dup2 function
	return real_dup2(oldfd, newfd);
}
 */