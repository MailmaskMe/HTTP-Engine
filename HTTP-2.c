#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORT 8888
struct upload_data {
    char *data;
    size_t length;
};
static int handle_request(void *cls, struct MHD_Connection *connection,
                          const char *url, const char *method, const char *version,
                          const char *upload_data, size_t *upload_data_size, void **con_cls);

char *assembly_to_c(const char *assembly_code);
int main() {
    struct MHD_Daemon *daemon;
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, PORT, NULL, NULL, 
                              &handle_request, NULL, MHD_OPTION_END);
    if (NULL == daemon) return 1;
    printf("Server is running on port %d\n", PORT);
    getchar();
    MHD_stop_daemon(daemon);
    return 0;
}
static int handle_request(void *cls, struct MHD_Connection *connection,
                          const char *url, const char *method, const char *version,
                          const char *upload_data, size_t *upload_data_size, void **con_cls) {
    static int aptr;
    if (&aptr != *con_cls) {
        *con_cls = &aptr;
        return MHD_YES;
    }
    if (0 == strcmp(method, "POST")) {
        if (*upload_data_size != 0) {
            struct upload_data *data = (struct upload_data *) malloc(sizeof(struct upload_data));
            data->length = *upload_data_size;
            data->data = (char *) malloc(data->length + 1);
            memcpy(data->data, upload_data, data->length);
            data->data[data->length] = '\0';
            *upload_data_size = 0;
            char *c_code = assembly_to_c(data->data);
            struct MHD_Response *response = MHD_create_response_from_buffer(strlen(c_code), (void *)c_code, MHD_RESPMEM_MUST_FREE);
            int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
            MHD_destroy_response(response);

            free(data->data);
            free(data);
            return ret;
        }
    }
    const char *error_page = "<html><body>Invalid Request</body></html>";
    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(error_page), (void *)error_page, MHD_RESPMEM_PERSISTENT);
    return MHD_queue_response(connection, MHD_HTTP_BAD_REQUEST, response);
}
char *assembly_to_c(const char *assembly_code) {
    size_t buffer_size = 1024;
    char *c_code = (char *) malloc(buffer_size);
    strcpy(c_code, "");
    const char *line = strtok((char *)assembly_code, "\n");
    while (line != NULL) {
        if (strstr(line, "MOV") != NULL) {
            char dest[10], src[10];
            if (sscanf(line, "MOV %[^,], %s", dest, src) == 2) {
                sprintf(c_code + strlen(c_code), "%s = %s;\n", dest, src);
            } else {
                strcat(c_code, "// Error parsing MOV instruction\n");
            }
        } else if (strstr(line, "ADD") != NULL) {
            char dest[10], value[10];
            if (sscanf(line, "ADD %[^,], %s", dest, value) == 2) {
                sprintf(c_code + strlen(c_code), "%s += %s;\n", dest, value);
            } else {
                strcat(c_code, "// Error parsing ADD instruction\n");
            }
        } else if (strstr(line, "SUB") != NULL) {
            char dest[10], value[10];
            if (sscanf(line, "SUB %[^,], %s", dest, value) == 2) {
                sprintf(c_code + strlen(c_code), "%s -= %s;\n", dest, value);
            } else {
                strcat(c_code, "// Error parsing SUB instruction\n");
            }
        } else if (strstr(line, "MUL") != NULL) {
            char dest[10], value[10];
            if (sscanf(line, "MUL %[^,], %s", dest, value) == 2) {
                sprintf(c_code + strlen(c_code), "%s *= %s;\n", dest, value);
            } else {
                strcat(c_code, "// Error parsing MUL instruction\n");
            }
        } else if (strstr(line, "DIV") != NULL) {
            char dest[10], value[10];
            if (sscanf(line, "DIV %[^,], %s", dest, value) == 2) {
                sprintf(c_code + strlen(c_code), "%s /= %s;\n", dest, value);
            } else {
                strcat(c_code, "// Error parsing DIV instruction\n");
            }
        } else {
            strcat(c_code, "// Unrecognized instruction\n");
        }
        line = strtok(NULL, "\n");
    }
    return c_code;
}
