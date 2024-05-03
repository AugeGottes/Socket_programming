#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <string.h>
#include <bits/stdc++.h>

#define ERROR -1
#define PORT 8080

void receiveMessages(int serverSocket) {
    char buffer[4096];
    while (true) {
        memset(buffer, 0, 4096);
        int bytesReceived = recv(serverSocket, buffer, 4096, 0);
        if (bytesReceived == -1) {
            std::cerr << "Error in recv(). Closing connection." << std::endl;
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "Server disconnected." << std::endl;
            break;
        }

        std::cout << "Received: " << std::string(buffer, 0, bytesReceived) << std::endl;
    }
}


// Function to send messages to the server
void sendMessages(int serverSocket) {
    std::string message;
    while (true) {
        std::getline(std::cin, message);
        send(serverSocket, message.c_str(), message.size() + 1, 0);
    }
}

int start_client(){
int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == ERROR) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Connect to the server
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        std::cerr << "Failed to setup server address." << std::endl;
        return 1;
    }

    if (connect(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == ERROR) {
        std::cerr << "Failed to connect to the server." << std::endl;
        return 1;
    }

    // Start a thread to receive messages from the server
    std::thread receiveThread(receiveMessages, serverSocket);


    // Start a thread to send messages to the server
    std::thread sendThread(sendMessages, serverSocket);


    // Wait for the threads to finish
    receiveThread.join();

    sendThread.join();

    // Close the socket
    close(serverSocket);

    return 0;
}
int main(int argc, const char** argv) {
    // Create a socket
    return start_client();
}