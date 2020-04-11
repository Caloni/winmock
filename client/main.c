#include "client.h"
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

int main() {
  struct CONNECTION* conn = NULL;
  if (winmock_connect("caloni.com.br", 80, &conn) == 0)
  {
    char buffer[100];
    unsigned int timeout = 10;
    if (winmock_send(conn, "ping?", sizeof("ping?")-1) > 0)
    {
      if (winmock_receive(conn, buffer, sizeof("pong!")-1, &timeout) > 0)
      {
        if (memcmp(buffer, "pong!", sizeof("pong!")-1) == 0)
        {
          printf("everything is just fine\n");
        }
      }
    }
  }
}

