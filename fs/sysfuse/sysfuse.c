#include "sysfuse.h"

const char *files[1000];

const int FD_OFFSET = 10000;

static const char * _sysfuse_get_filename(unsigned int fd) {
	return files[fd - FD_OFFSET];
}

int sysfuse_fstat(unsigned int fd, struct kstat *stat) {
	printk("SYSFUSE stat fd %d", fd);
	return 0;
}

int sysfuse_open(const char *filename) {
	int i = 0;
	while (files[i] != NULL) {
		i++;
	}
	files[i] = filename;
	printk("SYSFUSE open %s under fd %d\n", filename, i + FD_OFFSET);
	return i + FD_OFFSET;
}

int sysfuse_close(unsigned int fd) {
	printk("SYSFUSE close fd %d\n", fd);
	files[fd] = NULL;
	return 0;
}

ssize_t sysfuse_read(unsigned int fd, char __user *buf, size_t count) {
	printk("SYSFUSE read fd %d with count %zu\n", fd, count);
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
	if (fd < FD_OFFSET) {
		return false;
	}
	return _sysfuse_get_filename(fd) != NULL;
}

