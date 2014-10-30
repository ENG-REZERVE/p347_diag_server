#include <sys/stat.h>
#include <sys/syscall.h> 
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int module_insert(const char* module_path) {
    unsigned long len = 0;
    int fd;
    int ret = 0;
    void *buff = NULL;
    struct stat module_stat;

    ret = stat(module_path,&module_stat);
    if (ret != 0) {
	return -errno;
    }

    buff = malloc(module_stat.st_size);
    if (buff == NULL) {
	return -ENOMEM;
    }

    fd = open(module_path,O_RDONLY);
    if (fd < 0) {
	return -errno;
    }
    
    if (module_stat.st_size != read(fd,buff,module_stat.st_size)) {
	ret = -errno;
    } else {
	ret = syscall(__NR_init_module, buff, module_stat.st_size, NULL);
    }

    free(buff);
    close(fd);

    return ret;
}

int module_remove(const char* module_name) {
    int ret = syscall(__NR_delete_module,module_name,O_TRUNC);
    return ret;
}
