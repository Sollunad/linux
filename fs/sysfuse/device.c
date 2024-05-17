#include "device.h"

bool device_active = false;

struct sfdev_request {
	char *call;
	int len_call;
	const char *filename;
	int len_filename;
	char *param;
	int len_param;
};

struct sfdev_request req_queue[1000];
char res_queue[10][1024];

static int sfdev_open(struct inode *inode, struct file *file) {
	return 0;
}

static int sfdev_close(struct inode *inode, struct file *file) {
	return 0;
}

// Accept the response from the user_buffer and handle it
static ssize_t sfdev_write(struct file *file, const char __user *user_buffer, size_t user_len, loff_t *ppos) {
	char req_id[3];
	char buf[1024];
	long parsed_id;

	int status1 = copy_from_user(req_id, user_buffer, 3);
	int status2 = copy_from_user(buf, user_buffer + 3, user_len - 3);
	int status3 = kstrtol(req_id, 10, &parsed_id);
	buf[user_len - 3] = '\0';
	if (status1) {
		printk("error copying request id from misc device\n");
		return -status1;
	}
	if (status2) {
		printk("error copying response buffer from misc device\n");
		return -status2;
	}
	if (status3) {
		printk("error parsing request id\n");
		return -status3;
	}
	printk("got response %s for req id %ld\n", buf, parsed_id);
	strncpy(res_queue[parsed_id], buf, strlen(buf));
	return user_len;
}

// Serialize the current request queue and write it to the user_buffer
static ssize_t sfdev_read(struct file *file, char __user *user_buffer, size_t user_len, loff_t *ppos) {
	char response[1000];
	for (int i = 0; i < 1000; i++) {
		struct sfdev_request req = req_queue[i];
		if (req.len_call != 0) {
			char id_string[4];
			sprintf(id_string, "%03d", i);
			strcat(response, id_string);
			strcat(response, req.call);
			strcat(response, req.filename);
			if (req.len_param != 0) {
				strcat(response, req.param);
			}
		}
	}
	size_t len;
	size_t data_len = strlen(response);
	if (user_len < data_len) {
		len = user_len;
	} else {
		len = data_len;
	}
	int status = copy_to_user(user_buffer, response, len);
	if (status) {
		printk("Error writing to user buffer\n");
		return -status;
	}
	return data_len;
}

static const struct file_operations fops = {
	.read = sfdev_read,
	.write = sfdev_write,
	.open = sfdev_open,
	.release = sfdev_close,
};

static struct miscdevice device = {
	.name = "sysfuse",
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
	while (req_queue[i].len_call != 0) {
		i++;
	}
	int len_param;
	if (param == NULL) {
		len_param = 0;
	} else {
		len_param = strlen(param);
	}
	struct sfdev_request req = {
		.call = call,
		.len_call = strlen(call),
		.filename = filename,
		.len_filename = strlen(filename),
		.param = param,
		.len_param = len_param,
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
	req_queue[id].len_call = 0;
	req_queue[id].filename = NULL;
	req_queue[id].len_filename = 0;
	req_queue[id].param = NULL;
	req_queue[id].len_param = 0;
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