#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144

struct dbheader_t {
    unsigned int magic;
    unsigned short version;
    unsigned short count;
    unsigned int filesize;
};

struct employee_t {
    char name[64];
    char address[64];
    unsigned int hours;
};


int create_db_header(int fd, struct dbheader_t **headerOut);
int validate_db_header(int fd, struct dbheader_t **headerOut);
int write_to_db(int fd, struct dbheader_t *headerIn, struct employee_t *employeesIn);
int add_employee(struct dbheader_t *headerIn, struct employee_t **employeesOut, char *employee); 
int read_employees(int fd, struct dbheader_t *headerIn, struct employee_t **employeesOut);
void list_employees(struct dbheader_t *headerIn, struct employee_t *employeesIn);

#endif