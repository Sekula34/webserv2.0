#ifndef _GNU_SOURCE
# define _GNU_SOURCE
#endif
#include <unistd.h>
#include <dlfcn.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
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
#define PRINT_INTERCEPTION 1

// Define a function pointer type for the real execve
typedef int (*execve_t)(const char *pathname, char *const argv[], char *const envp[]);
static execve_t real_execve = NULL;

static int call_count = 0;
#define FAIL_ON_CALL 2 // Customize to fail on the nth call

/// POSSIBLE ERRNO VALUES FOR EXECVE FAILURE
int custom_errno = ENOENT;    // No such file or directory
// Uncomment the one you want to use for testing
// int custom_errno = E2BIG // The total number of bytes in the environment (envp) and argument  list  (argv)  is  too large.

// int custom_errno = EACCES // Search  permission  is denied on a component of the path prefix of pathname or the name of a script interpreter.
// (See also path_resolution(7).) The file or a script interpreter is not a regular file.
// Execute permission is denied for the file or a script or ELF interpreter. The filesystem is mounted noexec.

// int custom_errno = EAGAIN // (since Linux 3.1) Having changed its real UID using one of the set*uid() calls, the caller was—and is now still—above  its  RLIMIT_NPROC  resource limit (see setrlimit(2)).  For a more detailed explanation of this error, see NOTES.

// int custom_errno = EFAULT // pathname or one of the pointers in the vectors argv or envp points outside your  accessible address space.

// int custom_errno = EINVAL // An  ELF  executable  had more than one PT_INTERP segment (i.e., tried to name more than one interpreter).

// int custom_errno = EIO // An I/O error occurred.

// int custom_errno = EISDIR // An ELF interpreter was a directory.

// int custom_errno = ELIBBAD // An ELF interpreter was not in a recognized format.

// int custom_errno = ELOOP // Too many symbolic links were encountered in resolving pathname or the name of a  script or ELF interpreter.
// The  maximum  recursion  limit  was reached during recursive script interpretation (see "Interpreter scripts", above).
// Before Linux 3.8, the error produced for this case  was ENOEXEC.
       
// int custom_errno = EMFILE // The per-process limit on the number of open file descriptors has been reached.

// int custom_errno = ENAMETOOLONG // pathname is too long.

// int custom_errno = ENFILE // The system-wide limit on the total number of open files has been reached.

// int custom_errno = ENOENT // The file pathname or a script or ELF interpreter does not exist.

// int custom_errno = ENOEXEC // An executable is not in a recognized format, is for the wrong architecture, or has some other format error that means it cannot be executed.

// int custom_errno = ENOMEM // Insufficient kernel memory was available.

// int custom_errno = ENOTDIR // A component of the path prefix of pathname or a script or ELF interpreter is not a  directory.

// int custom_errno = EPERM // The  filesystem  is mounted nosuid, the user is not the superuser, and the file has the set-user-ID or set-group-ID bit set.
// The process is being traced, the user is not the superuser and the file  has  the  setuser-ID or set-group-ID bit set.
// A  "capability-dumb"  applications would not obtain the full set of permitted capabilities granted by the executable file.  See capabilities(7).

// int custom_errno = ETXTBSY // The specified executable was open for writing by one or more processes.


int execve(const char *pathname, char *const argv[], char *const envp[])
{
	if (!real_execve)
	{
		// Load the actual execve function
		real_execve = (execve_t)dlsym(RTLD_NEXT, "execve");
		if (!real_execve)
		{
			const char *error_msg = RED "Error locating the real execve\n" NC;
			syscall(SYS_write, FD_TO_WRITE, error_msg, strlen(error_msg));
			exit(EXIT_FAILURE);
		}
	}

	call_count++;
	const char *separator = CYAN "==============================================================================\n" NC;

	if (PRINT_INTERCEPTION)
	{
		const char *intercept_msg = PURPLE "Intercepted execve2\n" NC;
		syscall(SYS_write, FD_TO_WRITE, separator, strlen(separator));
		syscall(SYS_write, FD_TO_WRITE, intercept_msg, strlen(intercept_msg));
		syscall(SYS_write, FD_TO_WRITE, separator, strlen(separator));
	}

	// Fail on the specified call number
	if (call_count == FAIL_ON_CALL)
	{
		const char *fail_msg = PURPLE "Simulated execve failure\n" NC;
		syscall(SYS_write, FD_TO_WRITE, separator, strlen(separator));
		syscall(SYS_write, FD_TO_WRITE, fail_msg, strlen(fail_msg));
		syscall(SYS_write, FD_TO_WRITE, separator, strlen(separator));
		errno = custom_errno; // Set errno to the chosen error code
		return (-1);            // Simulate failure by returning -1
	}

	// Call the real execve function
	return (real_execve(pathname, argv, envp));
}
