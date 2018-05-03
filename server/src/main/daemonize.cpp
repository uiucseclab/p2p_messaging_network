/* daemonize.cpp
 *	Reference:
 *	https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux
 */

#include "daemonize.h"

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h> // umask
#include <sys/types.h>

// system logging
#include <string.h>
#include <syslog.h>
#include <errno.h>

// local forward declarations
static void handle_signals();
static pid_t fork_parent_process();

static void handle_signals()
{
	/* Catch, ignore, and handle signals */
	//TODO: Properly handle signals
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
}

static pid_t fork_parent_process()
{
	pid_t ret_pid;
	ret_pid = fork();

	/* An error occurred */
	if(ret_pid < 0)
	{
		syslog(LOG_ERR, "[daemonize] Failed to fork parent process: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Success: Let the parent terminate */
	if(ret_pid > 0)
	{
		syslog(LOG_NOTICE, "[daemonize] Successful fork, parent terminating.");
		exit(EXIT_SUCCESS);
	}

	return ret_pid;
}

void close_all_fd()
{
	/* Close all open file descriptors */
	int fd_idx;
	for(fd_idx = sysconf(_SC_OPEN_MAX); fd_idx >= 0; fd_idx--)
	{
		close(fd_idx);
	}
}

void setup_daemon()
{
	pid_t pid;

	/* Fork off parent process */
	pid = fork_parent_process();

	/* On success: The child process becomes session leader */
	if(setsid() < 0)
	{
		syslog(LOG_ERR, "[daemonize] Child process failed to become session leader: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* Catch, ignore, and handle signals */
	handle_signals();

	/* Fork off for the second time */
	pid = fork_parent_process();

	/* Set new file permissions */
	umask(0);

	/* Change the working directory to the root directory
	 *	or another appripriate directory
	 *
	 * TODO: Decide on an appropriate working directory
	 *	Perhaps have it as an argument. Possible security issue.
	 */
	chdir("/");

	close_all_fd();

	// Questioning if the log file needs to be opened
	openlog(NULL, LOG_PID, LOG_DAEMON);
}

void teardown_daemon()
{
	closelog();
}
