#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
	printf("Usage: %s -n -a <employee name,employee address,employee hours> -f <database file> -l\n", argv[0]);
	printf("\t -n  - create new database file\n");
	printf("\t -a  - add new employee to the database\n");
	printf("\t -f  - (required) path to the database file\n");
	printf("\t -l  - list employees in the database file\n");
	return;
}

int main(int argc, char *argv[]) { 
	char *filepath = NULL;
	char *employee_info = NULL;
	bool newfile = false;
	bool list = false;
	int c;

	int dbfd = -1;
	struct dbheader_t *header = NULL;
	struct employee_t *employees = NULL;

	while ((c = getopt(argc, argv, "nf:a:l")) != -1) {
		switch (c) {
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case 'a':
				employee_info = optarg;
				break;
			case 'l':
				list = true;
				break;				
			case '?':
				// printf("Unknown option -%c\n", optopt);
				print_usage(argv);
			default:
				return -1;

		}
	}

	if (filepath == NULL) {
		printf("Filepath is a required argument\n");
		print_usage(argv);

		return 0;
	}

	if (newfile) {
		dbfd = create_db_file(filepath);

		if (dbfd == STATUS_ERROR) {
			fprintf(stderr, "Unable to create database file\n");
			return -1;
		}

		if (create_db_header(dbfd, &header) == STATUS_ERROR) {
			fprintf(stderr, "Unable to create database header\n");
			return -1;
		}

		if (write_to_db(dbfd, header, employees) == STATUS_ERROR) {
			fprintf(stderr, "Unable to write to database\n");
			return -1;
		}

		printf("Succesfully created db file <%s>\n", filepath);
	}

	else {
		dbfd = open_db_file(filepath);

		if (dbfd == STATUS_ERROR) {
			fprintf(stderr, "Unable to open database file\n");
			return -1;
		}

		if (validate_db_header(dbfd, &header) == STATUS_ERROR) {
			fprintf(stderr, "Failed to validate\n");
			return -1;
		}

		printf("Succesfully opened db file <%s>\n", filepath);
		
		if(read_employees(dbfd, header, &employees) == STATUS_ERROR) {
			fprintf(stderr, "Failed to read employees from the database\n");
			return -1;
		}

		printf("Succesfully read employees from the database\n");
	}


	if(employee_info != NULL) {
		if(add_employee(header, &employees, employee_info) == STATUS_ERROR)
		{
			fprintf(stderr, "Failed to add an employee\n");
			return -1;
		}

		if(write_to_db(dbfd, header, employees) == STATUS_ERROR)
		{
			fprintf(stderr, "Failed to write to the database\n");
			return -1;
		}

		printf("Succesfully added an employee\n");
	}

	if(list) {
		list_employees(header, employees);
	}

	// printf("Newfile: %d\n", newfile);
	// printf("Filepath: %s\n", filepath);
	return 0;
}