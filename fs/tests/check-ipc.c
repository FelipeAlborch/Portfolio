#include <check.h>
#include <ipc.h>

#define LOCALHOST "127.0.0.1"
#define PORT_5000 "5000"
#define PORT_5001 "5001"
#define SERVER_IP "127.0.0.2"
#define CLIENT_IP "127.0.0.3"
#define ECHO_TEXT "Hello, server!"

t_queue *g_sockets;

void *clients_handler(void *arg)
{
    int server_socket = *(int *)arg;
    free(arg);

    int new_socket;
    while((new_socket = conn_accept(server_socket)) != -1)
    {
        if (new_socket == -1) perror("conn_accept");

        queue_push(g_sockets, (void *)allocate_int(new_socket));

        int status;
        char buf[SOMAXCONN] = {0};

        status = recv(new_socket, buf, sizeof(buf), 0);
        if (status == -1) perror("recv");

        status = send(new_socket, buf, sizeof(buf), 0);
        if (status == -1) perror("send");

        status = conn_close(new_socket);

        if (!conn_is_open(server_socket)) break;
    }

    pthread_exit((void *) 0);
}

void ipc_unchecked_setup(void)
{ 
    int *server_socket = malloc(sizeof(int));
    
    g_sockets = queue_create();

    *server_socket = conn_create(SERVER, LOCALHOST, PORT_5000);

    queue_push(g_sockets, (void *)server_socket);

    pthread_t thread;
    pthread_create(&thread, NULL, clients_handler, (void *)server_socket);

    pthread_detach(thread);

    // void *thread_result;
    // pthread_join(thread, &thread_result);
    // printf("Result: %d\n", *(int *)thread_result);
    // free(thread_result);
}

void ipc_unchecked_teardown(void) 
{
    conn_close_sockets(g_sockets);
    queue_destroy_and_destroy_elements(g_sockets, free);
}

START_TEST (test_client_server_connection)
{
    // Check that server socket creates successfully
    int server_socket = conn_create(SERVER, SERVER_IP, PORT_5001);
    ck_assert_int_ne(server_socket, -1);
    
    // Check that server socket binds to correct port
    int status;
    struct sockaddr_in addr;
    socklen_t addr_len = sizeof(addr);
    status = getsockname(server_socket, (struct sockaddr *)&addr, &addr_len);
    ck_assert_int_eq(status, 0);
    ck_assert_int_eq(addr.sin_family, AF_INET);
    ck_assert_str_eq(inet_ntoa(addr.sin_addr), SERVER_IP);
    ck_assert_int_eq(ntohs(addr.sin_port), atoi(PORT_5001));

    // Check that client socket creates successfully
    int client_socket = conn_create(CLIENT, SERVER_IP, PORT_5001);
    ck_assert_int_ne(client_socket, -1);    

    // Check that client socket connects to server socket
    status = getsockname(client_socket, (struct sockaddr *)&addr, &addr_len);
    ck_assert_int_eq(status, 0);
    ck_assert_int_eq(addr.sin_family, AF_INET);
    status = getpeername(client_socket, (struct sockaddr *)&addr, &addr_len);
    ck_assert_int_eq(status, 0);
    ck_assert_str_eq(inet_ntoa(addr.sin_addr), SERVER_IP);
    ck_assert_int_eq(ntohs(addr.sin_port), atoi(PORT_5001));

    // Check that sockets close successfully
    ck_assert_int_eq(close(server_socket), 0);
    ck_assert_int_eq(close(client_socket), 0);
}
END_TEST

START_TEST (test_client_server_echo)
{
    // Check that client socket creates successfully
    int client_socket = conn_create(CLIENT, LOCALHOST, PORT_5000);
    ck_assert_int_ne(client_socket, -1);    

    // Check that client socket sends message to server socket
    int status;
    char buf[] = ECHO_TEXT;
    status = send(client_socket, buf, sizeof(buf), 0);
    ck_assert_int_eq(status, sizeof(buf));

    // Check that server socket receives message from client socket
    status = recv(client_socket, buf, sizeof(buf), 0);
    ck_assert_int_eq(status, sizeof(buf));
    ck_assert_str_eq(buf, ECHO_TEXT);

    // Check that sockets close successfully
    ck_assert_int_eq(close(client_socket), 0);
}
END_TEST

Suite *ipc_test_suite(void)
{
    Suite *s = suite_create(__FILE__);
    TCase *tc = tcase_create(__FILE__);
    tcase_add_test(tc, test_client_server_connection);
    tcase_add_test(tc, test_client_server_echo);
    tcase_add_unchecked_fixture(tc, ipc_unchecked_setup, ipc_unchecked_teardown);
    suite_add_tcase(s, tc);

    return s;
}