#define _CRT_SECURE_NO_WARNINGS
#include "gtest/gtest.h"
extern "C" {
#include "client_mock.h"
#include "../client/client.h"
}

#include <string>

using namespace std;

static string last_send;

extern "C" {

	int select_default(int nfds, fd_set FAR* readfds, fd_set FAR* writefds, fd_set FAR* exceptfds, const struct timeval FAR* timeout)
	{
		return 1;
	}

	int getaddrinfo_default(PCSTR pNodeName, PCSTR pServiceName, const ADDRINFOA* pHints, PADDRINFOA* ppResult)
	{
		static struct addrinfo addr_result = {};
		static struct sockaddr sock_addr = { };
		addr_result.ai_addr = &sock_addr;
		*ppResult = &addr_result;
		return 0;
	}

	int send_default(SOCKET s, const char FAR* buf, int len, int flags)
	{
		string msg(buf, len);
		last_send = msg;
		return len;
	}

	int recv_default(SOCKET s, char FAR* buf, int len, int flags)
	{
		char pong[] = "pong!";

		if (last_send == "ping?")
		{
			if (len == sizeof("pong!") - 1 )
			{
				strcpy(buf, pong);
				return len;
			}
		}

		return 0;
	}
}


class clientTest : public ::testing::Test {
protected:
	clientTest() {
		getaddrinfo_mock = getaddrinfo_default;
		select_mock = select_default;
		send_mock = send_default;
		recv_mock = recv_default;
	};

	~clientTest() override {
	}

	void SetUp() override {
	}

	void TearDown() override {
	}

	// your stuff
};


TEST_F(clientTest, ConnectSendReceive)
{
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

