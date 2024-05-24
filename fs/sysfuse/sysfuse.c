#include "sysfuse.h"

const char *files[1000];

static const char * _sysfuse_get_filename(unsigned int fd) {
	return files[fd];
}

int sysfuse_fstat(unsigned int fd, struct kstat *stat) {
	printk("stat fd %d", fd);
	return 0;
}

int sysfuse_open(const char *filename, unsigned int fd) {
	files[fd] = filename;
	printk("open %s under fd %d\n", filename, fd);
	return fd;
}

void sysfuse_close(unsigned int fd) {
	files[fd] = NULL;
}

ssize_t sysfuse_read(unsigned int fd, char __user *buf, size_t count) {
	printk("read fd %d with count %zu\n", fd, count);
	const char* filename = _sysfuse_get_filename(fd);
	printk("read file %s\n", filename);
	char dev_response[1024];
	int res_len = request_device("read", filename, NULL, dev_response);
	int status = copy_to_user(buf, dev_response, res_len);
	if (status) {
		printk("Error while copying to user buffer\n");
		return -status;
	}
	return count;
}

bool sysfuse_is_responsible(unsigned int fd) {
	if (fd == -1 || fd >= 1000) {
		return false;
	}
	return _sysfuse_get_filename(fd) != NULL;
}

