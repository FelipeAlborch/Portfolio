#include <check.h>
#include <ipc.h>

START_TEST (test_message_create)
{
//     char *expected = "hola";
//     t_message *message = message_create(expected);
//     char *actual = message_create(message);
//     ck_assert_str_eq(expected, actual);
}
END_TEST

Suite *ipc_test_suite(void)
{
    Suite *s = suite_create(__FILE__);
    TCase *tc = tcase_create(__FILE__);
    tcase_add_test(tc, test_message_create);
    suite_add_tcase(s, tc);

    return s;
}

int iniciar_servidor_en(char* ip, char* puerto)
{
	int socket_fd;

	struct addrinfo hints, *servinfo;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &servinfo);
	
	for(struct addrinfo *p = servinfo; p != NULL; p = p->ai_next)
	{
		if ((socket_fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) < 0)
		{
			continue;
		}

    	if (bind(socket_fd, p->ai_addr, p->ai_addrlen) == -1) 
		{
    	    close(socket_fd);
    	    continue;
    	}

		break;
	} 

    if(listen(socket_fd, SOMAXCONN) == -1){					

		perror("Error al escuchar conexiones\n");
	}	

	freeaddrinfo(servinfo);

	return socket_fd;
}