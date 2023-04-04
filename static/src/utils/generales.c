#include <utils/generales.h>

t_log* iniciar_logger_modulo(int modulo){									

	t_log* logger;

	switch (modulo)
	{
	case KERNEL_LOGGER:
		logger = log_create("../kernel/kernel.log","kernel",1,LOG_LEVEL_INFO); 
		break;
	case CPU_LOGGER:
		logger = log_create("../cpu/cpu.log","cpu",1,LOG_LEVEL_INFO);
		break;
	case MEMORIA_LOGGER:
		logger = log_create("../memoria/memoria.log","memoria",1,LOG_LEVEL_INFO);
		break;
	case CONSOLA_LOGGER:
		logger = log_create("../consola/consola.log","consola",1,LOG_LEVEL_INFO);
		break;
	case FILESYSTEM_LOGGER:
		logger = log_create("../filesystem/filesystem.log","filesystem",1,LOG_LEVEL_INFO);
		break;
	case KERNEL_LOGGER_EXTRA:
		logger = log_create("../kernel/kernelExtra.log","kernel",1,LOG_LEVEL_INFO);
		break;
	case CPU_LOGGER_EXTRA:
		logger = log_create("../cpu/cpuExtra.log","cpu",1,LOG_LEVEL_INFO);
		break;
	case MEMORIA_LOGGER_EXTRA:
		logger = log_create("../memoria/memoriaExtra.log","memoria",1,LOG_LEVEL_INFO);
		break;
	case CONSOLA_LOGGER_EXTRA:
		logger = log_create("../consola/consolaExtra.log","consola",1,LOG_LEVEL_INFO);
		break;
	case FILESYSTEM_LOGGER_EXTRA:
		logger = log_create("../filesystem/filesystemExtra.log","filesystem",1,LOG_LEVEL_INFO);
		break;
	default:
		break;
	}
		if(logger == NULL)																	
	{
		perror("No se pudo crear/encontrar el logger");
		exit(1);
	}
	return logger;
}
