struct file *files[1000];

int FD_OFFSET = 10000;

int add_file(struct file *file) {
	int i = 0;
	while (files[i] != NULL) {
		i++;
	}
	files[i] = file;
	return i + FD_OFFSET;
}