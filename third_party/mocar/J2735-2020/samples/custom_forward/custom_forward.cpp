#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define LOG_FILE "/huali/log/cv2x.log"

void send_udp_message(const std::string& message) {
    int sockfd;
    struct sockaddr_in servaddr, sourceaddr;

    // Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation failed" << std::endl;
        return;
    }

    // Bind the socket to the source IP (192.168.253.20)
    memset(&sourceaddr, 0, sizeof(sourceaddr));
    sourceaddr.sin_family = AF_INET;
    sourceaddr.sin_port = htons(0);  // Use any available port
    sourceaddr.sin_addr.s_addr = inet_addr("192.168.3.110");

    if (bind(sockfd, (const struct sockaddr*)&sourceaddr, sizeof(sourceaddr)) < 0) {
        std::cerr << "Failed to bind to source IP 192.168.3.110" << std::endl;
        close(sockfd);
        return;
    }

    // Configure server information (destination IP)
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);  // Target port (8080)
    servaddr.sin_addr.s_addr = inet_addr("192.168.253.11");  // Destination IP

    // Send message
    std::cout << "Sending UDP message: " << message << " to 192.168.253.11" << std::endl;
    sendto(sockfd, message.c_str(), message.length(), MSG_CONFIRM, (const struct sockaddr*)&servaddr, sizeof(servaddr));

    // Close socket
    close(sockfd);
}

void monitor_log_file(const std::string& file_name) {
    std::ifstream file(file_name);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open log file: " << file_name << std::endl;
        return;
    }

    // Move the read pointer to the end of the file
    file.seekg(0, std::ios::end);
    std::cout << "Monitoring log file: " << file_name << std::endl;

    while (true) {
        // Check for new lines
        while (std::getline(file, line)) {
            
            if (line.find("rxmsg_TIM") != std::string::npos) {
            	std::cout << "New log entry: " << line << std::endl;
                std::cout << "'TIM' received found in log entry. Sending UDP message..." << std::endl;
                send_udp_message(line);
            }
        }

        // Sleep to avoid busy-waiting
        std::cout << "Waiting for new log entries..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        // Clear error flags in case the file grows after EOF
        if (file.eof()) {
            file.clear();
        }
    }
}

int main() {
    std::cout << "Starting log file monitoring..." << std::endl;
    // Monitor the log file for changes
    monitor_log_file(LOG_FILE);

    return 0;
}

