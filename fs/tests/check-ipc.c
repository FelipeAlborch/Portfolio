#include <check.h>
#include <ipc.h>

#define SERV_PORT "5000"
#define LOCALHOST "127.0.0.1"
#define SERVER_IP "127.0.0.2"
#define CLIENT_IP "127.0.0.3"

START_TEST (test_client_server_connection)
{
    // Check that server socket creates
    int server_socket = conn_create(SERVER, SERVER_IP, SERV_PORT);
    ck_assert_int_ne(server_socket, -1);
    
    // Check that server socket binds to correct port
    int error;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    error = getsockname(server_socket, (struct sockaddr *)&addr, &addr_len);
    ck_assert_int_eq(error, 0);
    ck_assert_int_eq(addr.sin_family, AF_INET);
    ck_assert_str_eq(inet_ntoa(addr.sin_addr), SERVER_IP);
    ck_assert_int_eq(ntohs(addr.sin_port), atoi(SERV_PORT));

    // Check that client socket creates
    int client_socket = conn_create(CLIENT, SERVER_IP, SERV_PORT);
    ck_assert_int_ne(client_socket, -1);    

    // Check that client socket connects to server socket
    error = getsockname(client_socket, (struct sockaddr *)&addr, &addr_len);
    ck_assert_int_eq(error, 0);
    ck_assert_int_eq(addr.sin_family, AF_INET);
    error = getpeername(client_socket, (struct sockaddr *)&addr, &addr_len);
    ck_assert_int_eq(error, 0);
    ck_assert_str_eq(inet_ntoa(addr.sin_addr), SERVER_IP);
    ck_assert_int_eq(ntohs(addr.sin_port), atoi(SERV_PORT));

    // Check that sockets close successfully
    ck_assert_int_eq(close(server_socket), 0);
    ck_assert_int_eq(close(client_socket), 0);
}
END_TEST

Suite *ipc_test_suite(void)
{
    Suite *s = suite_create(__FILE__);
    TCase *tc = tcase_create(__FILE__);
    tcase_add_test(tc, test_client_server_connection);
    suite_add_tcase(s, tc);

    return s;
}