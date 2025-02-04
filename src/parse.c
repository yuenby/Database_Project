#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *headerIn, struct employee_t *employeesIn) {
    printf("---------------------------------------------------------------\n");
    int count = headerIn->count;
    for (int i = 0; i < count; i++) {
        printf("Employee: %d\n", i);
        printf("\tName: %s\n", employeesIn[i].name);
        printf("\tAddress: %s\n", employeesIn[i].address);
        printf("\tHours: %d\n", employeesIn[i].hours);
    }
}

int create_db_header(int fd, struct dbheader_t **headerOut){
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));

    if (header == NULL) {
        fprintf(stderr, "Couldn't allocate memory for dbheader in function 'create_db_header'\n");
        return STATUS_ERROR;
    }

    header->version = VERSION_NUMBER;
    header->count = 0;
    header->magic = HEADER_MAGIC;
    header->filesize = sizeof(struct dbheader_t);

    *headerOut = header;
    return STATUS_SUCCESS;
}


int validate_db_header(int fd, struct dbheader_t **headerOut) {
    
    if (fd < 0) {
        fprintf(stderr, "Bad fd given to function 'validate_db_header'\n");
        return STATUS_ERROR;
    }
    
    struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));

    if (header == NULL) {
        fprintf(stderr, "Couldn't allocate memory for dbheader in function 'validate_db_header'\n");
        return STATUS_ERROR;
    }
    
    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek failed in function 'validate_db_header'");
        return STATUS_ERROR;
    }

    if(read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
        perror("read failed in function 'create_db_header'\n");
        free(header);
        return STATUS_ERROR;
    }

    header->version = ntohs(header->version);
    header->count = ntohs(header->count);
    header->magic = ntohl(header->magic);
    header->filesize = ntohl(header->filesize);

    if(header->magic != HEADER_MAGIC) {
        printf("Wrong file type\n");
        free(header);
        return STATUS_ERROR;
    }

    if(header->version != VERSION_NUMBER) {
        printf("Version mismatch\n");
        free(header);
        return STATUS_ERROR;
    }

    struct stat dbstat = {0};

    fstat(fd, &dbstat);

    if(dbstat.st_size != header->filesize) {
        printf("Header-file size mismatch\n");
        free(header);
        return STATUS_ERROR;
    }

    *headerOut = header;

    return STATUS_SUCCESS;
}

int write_to_db(int fd, struct dbheader_t *headerIn, struct employee_t *employeesIn) {

    if (fd < 0) {
        fprintf(stderr, "Bad fd given to function 'write_to_db'\n");
        return STATUS_ERROR;
    }

    if (lseek(fd, 0, SEEK_SET) == -1) {
        perror("lseek failed in function 'write_to_db'");
        return STATUS_ERROR;
    }

    headerIn->filesize = sizeof(struct dbheader_t) + sizeof(struct employee_t) * headerIn->count;

    headerIn->version = htons(headerIn->version);
    headerIn->count = htons(headerIn->count);
    headerIn->magic = htonl(headerIn->magic);
    headerIn->filesize = htonl(headerIn->filesize);

    if (write(fd, headerIn, sizeof(struct dbheader_t)) == -1) {
        perror("write of header failed in function 'write_to_db'");
        return STATUS_ERROR;
    }

    headerIn->version = ntohs(headerIn->version);
    headerIn->count = ntohs(headerIn->count);
    headerIn->magic = ntohl(headerIn->magic);
    headerIn->filesize = ntohl(headerIn->filesize);

    for(int i=0; i<headerIn->count; i++) {
        employeesIn[i].hours = htonl(employeesIn[i].hours);
    }
    
    if (write(fd, employeesIn, sizeof(struct employee_t) * headerIn->count) == -1) {
        perror("write of employees failed in function 'write_to_db'");
        return STATUS_ERROR;
    }
    
    for(int i=0; i<headerIn->count; i++) {
        employeesIn[i].hours = ntohl(employeesIn[i].hours);
    }

    return STATUS_SUCCESS;
}

int add_employee(struct dbheader_t *headerIn, struct employee_t **employeesOut, char *employee){

    headerIn->count++;

    struct employee_t *employees = *employeesOut;
    employees = realloc(employees, headerIn->count);


    if(employees == NULL)
    {
        perror("realloc failed in function 'add_employee'");

        headerIn->count--;

        return STATUS_ERROR;
    }

    char *name = strtok(employee, ",");
    char *address = strtok(NULL, ",");
    char *tmphours = strtok(NULL, ",");

    if(name == NULL || address == NULL || tmphours == NULL){

        fprintf(stderr, "Bad input string given to function 'add_employee'\n");

        headerIn->count--;
        employees = realloc(employees, headerIn->count);

        employeesOut = &employees;

        return STATUS_ERROR;
    }

    int hours = atoi(tmphours);

    strncpy(employees[headerIn->count-1].name, name, sizeof(employees[headerIn->count-1].name));
    strncpy(employees[headerIn->count-1].address, address, sizeof(employees[headerIn->count-1].address));
    employees[headerIn->count-1].hours = hours;
    
    employeesOut = &employees;

    return STATUS_SUCCESS;
}

int read_employees(int fd, struct dbheader_t *headerIn, struct employee_t **employeesOut){

    if (fd < 0) {
        fprintf(stderr, "Bad fd given to function 'read_employees'\n");
        return STATUS_ERROR;
    }

    struct employee_t *employees = calloc(headerIn->count, sizeof(struct employee_t));

    if(employees == NULL)
    {
        perror("calloc failed in function 'read_employees'");
        return STATUS_ERROR;
    }

    if (lseek(fd, sizeof(struct dbheader_t), SEEK_SET) == -1) {
        perror("lseek failed in function 'read_employees'");
        return STATUS_ERROR;
    }

    if(read(fd, employees, sizeof(struct employee_t) * headerIn->count) == -1)
    {
        perror("read failed in function 'read_employees'");
        return STATUS_ERROR;
    }

    for(int i=0; i<headerIn->count; i++)
    {
        employees[i].hours = ntohl(employees[i].hours);
    }

    *employeesOut = employees;

    return STATUS_SUCCESS;
}
