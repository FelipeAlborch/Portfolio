#include <CUnit/CUnit.h>
#include <ipc.h>

#define PORT "5000"
#define LOCALHOST "127.0.0.1"
#define BACKLOG 10

void test_client_server_connection(void) {
    // Check that server socket connects
    int server_socket = conn_create(SERVER, LOCALHOST, PORT);
    CU_ASSERT_NOT_EQUAL(server_socket, -1);
    
    // Check that the socket is listening
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    int status = getsockname(server_socket, (struct sockaddr *)&addr, &addr_len);
    CU_ASSERT_EQUAL(status, 0);
    CU_ASSERT_EQUAL(addr.sin_family, AF_INET);
    CU_ASSERT_EQUAL(ntohs(addr.sin_port), PORT);
    CU_ASSERT_EQUAL(addr.sin_addr.s_addr, INADDR_ANY);

    // Check that client socket connects
    int client_socket = conn_create(CLIENT, LOCALHOST, PORT);
    CU_ASSERT_NOT_EQUAL(client_socket, -1);    

    // Check that the socket is in client mode
    status = getsockname(client_socket, (struct sockaddr *)&addr, &addr_len);
    CU_ASSERT_EQUAL(status, 0);
    CU_ASSERT_EQUAL(addr.sin_family, AF_INET);
    CU_ASSERT_EQUAL(addr.sin_port, htons(PORT));
    // CU_ASSERT_EQUAL(ntohs(addr.sin_port), PORT);
    // char *ip_str = inet_ntoa(addr.sin_addr);
    // CU_ASSERT_EQUAL(ip_str, LOCALHOST);
    CU_ASSERT_EQUAL(addr.sin_addr.s_addr, inet_addr(LOCALHOST));

    // Check that sockets close successfully
    CU_ASSERT_EQUAL(close(server_socket), 0);
    CU_ASSERT_EQUAL(close(client_socket), 0);
}

// void test_client_connect()
// {
//     int sockfd = conn_create(SERVER, NULL, PORT);
//     assert(sockfd >= 0);

//     int pid = fork();
//     if (pid == 0)
//     {
//         // Child process
//         int sockfd = conn_create(CLIENT, "localhost", PORT);
//         assert(sockfd >= 0);
//         assert(conn_close(sockfd) == 0);
//         exit(0);
//     }
//     else if (pid > 0)
//     {
//         // Parent process
//         int newsockfd = conn_accept(sockfd);
//         assert(newsockfd >= 0);
//         assert(conn_close(newsockfd) == 0);
//         assert(conn_close(sockfd) == 0);
//     }
// }

void conn_client_server_test()
{
    CU_ASSERT(2 * 3 == 6);
}

void ipc_test()
{
    CU_ASSERT(2 * 3 == 6);
}

int init() { return 0; }

int clean() { return 0; }

void add_ipc_suite() {
    CU_pSuite suite = CU_add_suite(__FILE__, init, clean);
    CU_add_test(suite, "test_client_server_connection", test_client_server_connection);
    CU_add_test(suite, "conn_client_server_test", conn_client_server_test);
    CU_add_test(suite, "ipc_test", ipc_test);
}