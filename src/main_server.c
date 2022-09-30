#include <time.h>
#include <signal.h>
#include <unistd.h>

#include "server.h"
#include "server_parameters.h"
#include "random_sequence_util.h"

static Server *server = 0;

void signal_handler(int sig)
{
    if (sig == SIGKILL)
    {
        if (server && !Server_isDaemon(server))
        {
            Server_terminate(server);
            Server_free(server);
        }
    }
}

int main(int argc, char **argv)
{
    init_random_generator(time(NULL));
    ServerParameters serverParams = ServerParameters_parse(argc, argv);
    server = Server_new(serverParams);
    signal(SIGKILL, signal_handler);
    Server_start(server);
    return 0;
}