#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>


#define BUFF_SIZE (1024 * 5)
#define LINE_NUMS 10


typedef struct {
    char *file_name;
    int lines;
} option;


int
cal_char_nums(char *buf, int c) {
    int nums = 0;
    char *ptr;
    while ((ptr = strchr(buf, c)) != NULL) {
        nums += 1;
        buf = ptr + 1;
    }
    return nums;
}


char *
find_char_offset(char *buf, int c, int n) {
    char *ptr;
    while (n != 0) {
        ptr = strchr(buf, c);
        if (ptr == NULL) {
            return NULL;
        }
        buf = ptr + 1;
        n--;
    }
    return ptr + 1;
}


option *
init_option (char *file_name, int lines) {
    option *o = malloc(sizeof(option));
    if (o == NULL) {
        printf("Can't malloc option");
        return NULL;
    }

    o->file_name = file_name;
    o->lines = lines;
    return o;
}


option *
parse_option(int argc, char *argv[]) {
    int c, lines;
    char *file_name;

    while ((c = getopt(argc, argv, "n:")) != -1) {
        switch (c) {
            case 'n':
                lines = atoi(optarg);
                break;
            case '?':
                printf("Error option [-n num]");
                return NULL;        
            default:
                abort();
        }
    }
    if ((optind + 1) != argc) {
        printf("Error option. Option length error");
        return NULL;
    } else {
        file_name = argv[optind];
    }

    option *o = init_option(file_name, lines);
    return o;
}


int
prinf_file(int fd, char *buf) {
    int bytes;
	while ((bytes = read(fd, buf, sizeof(buf))) > 0) {
		write(STDOUT_FILENO, buf, bytes);
	}
    return 0;
}


int
main(int argc, char *argv[]) {
    int fd, line, offset, bytes;
    char buf[BUFF_SIZE];
    char *ptr;

    option *op;
    if ((op = parse_option(argc, argv)) == NULL) {
        return -1;
    }


    int line_nums = op->lines ? op->lines : LINE_NUMS;
    fd = open(op->file_name, O_RDONLY);
    if (fd == -1) {
        printf("Can't open the file");
        return -1;
    }

    offset = 0;
	bytes = (int)(-sizeof(buf));
	line = 0;
	do {
		bytes = read(fd, buf, sizeof(buf));
		offset -= bytes;
		line += cal_char_nums(buf, '\n');
		if (bytes != sizeof(buf)) {
			break;
		}
	} while (line <= line_nums && bytes == sizeof(buf));

	if (line >= line_nums) {
		ptr = find_char_offset(buf, '\n', line - line_nums);
		offset += (int)((unsigned long)&ptr[0] - (unsigned long)&buf[0]);
		lseek(fd, offset, SEEK_END);
		return prinf_file(fd, buf);
	} else {
		lseek(fd, 0, SEEK_SET);
	}

    return prinf_file(fd, buf);
}








