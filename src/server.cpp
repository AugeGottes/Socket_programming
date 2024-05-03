#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <map>
#include <mutex>
#include<bits/stdc++.h>

#define PORT 8080
#define ERROR -1
#define backlog 5

struct Client {
    int socket;
    std::string ip;
    int port;
    int id;
};
// push all this in header file later also make Makefikle
std::unordered_map<int, Client> clients;
std::mutex clients_mutex;
int nextId = 0;

void sendMessageToClient(const std::string& message) {
    // Format: <client_id>_<message> (e.g. 0_ssdsdsd ) will be send to client 0

    //lock the client map
    std::lock_guard<std::mutex> lock(clients_mutex);
    
    // this can be better modify in the future
    size_t underscorePos = message.find('_');
    if (underscorePos == std::string::npos) {
        std::cerr << "Invalid message format." << std::endl;
        return;
    }

    int clientId = std::stoi(message.substr(0, underscorePos));
    std::string actualMessage = message.substr(underscorePos + 1);

    if (clients.count(clientId) > 0) {
        send(clients[clientId].socket, actualMessage.c_str(), actualMessage.size() + 1, 0);
    } else {
        std::cerr << "Client " << clientId << " not found." << std::endl;
    }
}

void handleClient(Client client) {
    try {
        char buffer[4096];
        while (true) {
            memset(buffer, 0, 4096);
            int bytesReceived = recv(client.socket, buffer, 4096, 0);
            if (bytesReceived == -1) {
                std::cerr << "Error in recieving info check recv(). Closing connection." << std::endl;
                break;
            }

            if (bytesReceived == 0) {
                std::cout << "Client disconnected." << std::endl;
                break;
            }

            std::cout << "Received from client " << client.id << ": " << std::string(buffer, 0, bytesReceived) << std::endl;
        }
    } catch (const std::exception& e) {
        std::cerr << "Exception handling client: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Some bug has occured" << std::endl;
    }

    close(client.socket);

    std::lock_guard<std::mutex> lock(clients_mutex);
    clients.erase(client.id);
}

int start_server(){
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Socket creation failed" << std::endl;
        // std::cout<<"hello server1"<<std::endl;
        return EXIT_FAILURE;
    }

    int opt = 1;
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == ERROR) {
        std::cerr << "Error setting socket options!" << std::endl;
        return EXIT_FAILURE;
    }
    // std::cout<<"hello server2"<<std::endl;
    // Bind the socket to our specified IP and port 
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == ERROR) {
        std::cerr << "Bind to IP/Port failed!" << std::endl;
        return EXIT_FAILURE;
    }
    //check man if you forget
    if (listen(serverSocket, backlog) == ERROR) {
        std::cerr << "Listen failed!" << std::endl;
        return EXIT_FAILURE;
    }

    std::thread acceptThread([&]() {
        while (true) {
            sockaddr_in clientAddress{};
            socklen_t clientSize = sizeof(clientAddress);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddress, &clientSize);

            if (clientSocket == ERROR) {
                std::cerr << "Failed to accept the client connection." << std::endl;
                continue;
            }

            Client client;
            client.socket = clientSocket;
            client.ip = inet_ntoa(clientAddress.sin_addr);
            client.port = ntohs(clientAddress.sin_port);
            client.id = nextId++;

            std::lock_guard<std::mutex> lock(clients_mutex);
            clients[client.id] = client;

            std::thread(handleClient, client).detach();
        }
    });

    std::string message;
    while (std::getline(std::cin, message)) {
        sendMessageToClient(message);
    }

    acceptThread.join();

    close(serverSocket);

    return 0;
}
int main() {
    
   return start_server();

    
}