#include "device.h"

bool device_active = false;

struct fdt_request {
	char *call;
	const char *filename;
	char *param;
};

struct fdt_request req_queue[1000];
char res_queue[10][1024];

static int fdt_open(struct inode *inode, struct file *file) {
	return 0;
}

static int fdt_close(struct inode *inode, struct file *file) {
	return 0;
}

// Accept the response from the user_buffer and handle it
static ssize_t fdt_write(struct file *file, const char __user *user_buffer, size_t user_len, loff_t *ppos) {
	char buf[1024];
	int status = copy_from_user(buf, user_buffer, user_len);
	buf[user_len] = '\0';
	if (status) {
		printk("error writing to misc device\n");
		return -status;
	}
	strncpy(res_queue[0], buf, strlen(buf));
	return user_len;
}

// Serialize the current request queue and write it to the user_buffer
static ssize_t fdt_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *ppos) {
	char *dummy_res = "Hello Misc";
	size_t len;
	size_t data_len = strlen(dummy_res);
	if (user_len < data_len) {
		len = user_len;
	} else {
		len = data_len;
	}
	int status = copy_to_user(user_buffer, dummy_res, len);
	if (status) {
		printk("Error writing to user buffer\n");
		return -status;
	}
	return data_len;
}

static const struct file_operations fops = {
	.read = fdt_read,
	.write = fdt_write,
	.open = fdt_open,
	.release = fdt_close,
};

static struct miscdevice device = {
	.name = "fdtdev",
	.minor = MISC_DYNAMIC_MINOR,
	.fops = &fops,
};

static void init_misc_device(void) {
	device_active = true;
	printk("Init misc device\n");
	misc_register(&device);
}

static int write_to_queue(char *call, const char *filename, char *param) {
	int i = 0;
	while (req_queue[i].call != NULL) {
		i++;
	}
	struct fdt_request req = {
		call = call,
		filename = filename,
		param = param,
	};
	strncpy(res_queue[i], "", 1);
	req_queue[i] = req;
	printk("Added req to device queue with id %d\n", i);
	return i;
}

static int get_dev_response(int id, char *res_buf) {
	while (res_queue[id][0] == '\0') {
		msleep(1);
	}
	int len = strlen(res_queue[id]);
	strncpy(res_buf, res_queue[id], len);
	req_queue[id].call = NULL;
	req_queue[id].filename = NULL;
	req_queue[id].param = NULL;
	strncpy(res_queue[id], "", 1);
	return len;
}

int request_device(char *call, const char *filename, char *param, char *res_buf) {
	if (!device_active) {
		init_misc_device();
	}
	int id = write_to_queue(call, filename, param);
	return get_dev_response(id, res_buf);
}