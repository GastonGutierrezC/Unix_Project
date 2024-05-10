#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 30

void handle_request(int client_socket) {
    char buffer[BUFFER_SIZE];
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("Error al recibir datos del cliente");
        return;
    }
    printf("Solicitud HTTP recibida:\n%s\n", buffer);

    char method[10];
    char url[256];
    sscanf(buffer, "%s %s", method, url);

    if (strcmp(method, "GET") == 0) {
        if (strcmp(url, "/") == 0) {
            const char* response = "HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\n\r\nServidor HTTP versión 1.0\r\n";
            send(client_socket, response, strlen(response), 0);
        } else {
            int file_fd = open(url + 1, O_RDONLY);
            if (file_fd < 0) {
                const char* response = "HTTP/1.0 404 Not Found\r\nContent-Type: text/plain\r\n\r\nRecurso no encontrado\r\n";
                send(client_socket, response, strlen(response), 0);
            } else {
                char file_buffer[BUFFER_SIZE];
                ssize_t bytes_read;
                while ((bytes_read = read(file_fd, file_buffer, BUFFER_SIZE)) > 0) {
                    send(client_socket, file_buffer, bytes_read, 0);
                }
                close(file_fd);
            }
        }
    } else {
        const char* response = "HTTP/1.0 405 Method Not Allowed\r\nContent-Type: text/plain\r\n\r\nMétodo no permitido\r\n";
        send(client_socket, response, strlen(response), 0);
    }

    close(client_socket);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf(stderr, "Uso: %s <puerto>\n", argv[0]);
        return 1;
    }

    int port = atoi(argv[1]);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Error al crear el socket del servidor");
        return 1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Error al enlazar el socket del servidor");
        return 1;
    }

    if (listen(server_socket, MAX_CONNECTIONS) < 0) {
        perror("Error al poner el socket en modo de escucha");
        return 1;
    }

    printf("Servidor HTTP escuchando en el puerto %d...\n", port);

    while (1) {
        struct sockaddr_in client_address;
        socklen_t client_address_size = sizeof(client_address);
        int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_size);
        if (client_socket < 0) {
            perror("Error al aceptar la conexión del cliente");
            continue;
        }
        printf("Conexión aceptada desde %s:%d\n", inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

        pid_t pid = fork();
        if (pid == 0) {
            handle_request(client_socket);
            exit(0);
        } else if (pid < 0) {
            perror("Error al crear el proceso hijo");
        }

        close(client_socket);
    }

    close(server_socket);
    return 0;
}
