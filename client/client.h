#include <WinSock2.h>

struct CONNECTION;

int winmock_connect(const char* host, short port, struct CONNECTION** connection);
int winmock_send(struct CONNECTION* connection, const void* buffer, unsigned int len);
int winmock_receive(struct CONNECTION* connection, void* buffer, int len, const unsigned* timeout_in_seconds);
