#include <windows.h>
#include <wininet.h>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "wininet.lib")

bool SendDiscordWebhook(const std::string& url, const std::string& content, int embedCount) {
    std::string payload = R"({"content": ")" + content + R"(", "embeds": [)";
    for (int i = 0; i < embedCount; ++i) {
        payload += R"({"title": "Embed Title", "description": "This is embed number )" + std::to_string(i + 1) + R"("})";
        if (i != embedCount - 1) payload += ",";
    }
    payload += "]}";

    HINTERNET hInternet = InternetOpenA("DiscordWebhookSender", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    URL_COMPONENTSA components = { sizeof(components) };
    components.dwSchemeLength = (DWORD)-1;
    components.dwHostNameLength = (DWORD)-1;
    components.dwUrlPathLength = (DWORD)-1;
    if (!InternetCrackUrlA(url.c_str(), 0, 0, &components)) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::string host(components.lpszHostName, components.dwHostNameLength);
    std::string path(components.lpszUrlPath, components.dwUrlPathLength);

    HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path.c_str(), NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    std::string headers = "Content-Type: application/json\r\n";
    BOOL sent = HttpSendRequestA(hRequest, headers.c_str(), -1, (LPVOID)payload.c_str(), payload.length());

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return sent == TRUE;
}

bool DeleteDiscordChannel(const std::string& token, const std::string& channelId) {
    std::string url = "https://discord.com/api/v10/channels/" + channelId;

    HINTERNET hInternet = InternetOpenA("ChannelDeleter", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    URL_COMPONENTSA components = { sizeof(components) };
    components.dwSchemeLength = (DWORD)-1;
    components.dwHostNameLength = (DWORD)-1;
    components.dwUrlPathLength = (DWORD)-1;
    if (!InternetCrackUrlA(url.c_str(), 0, 0, &components)) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::string host(components.lpszHostName, components.dwHostNameLength);
    std::string path(components.lpszUrlPath, components.dwUrlPathLength);

    HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "DELETE", path.c_str(), NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    std::string headers = "Authorization: Bot " + token + "\r\nContent-Type: application/json\r\n";

    BOOL sent = HttpSendRequestA(hRequest, headers.c_str(), -1, NULL, 0);

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return sent == TRUE;
}

bool SendBotMessageToChannel(const std::string& token, const std::string& channelId, const std::string& message) {
    std::string url = "https://discord.com/api/v10/channels/" + channelId + "/messages";
    std::string payload = R"({"content": ")" + message + R"("})";

    HINTERNET hInternet = InternetOpenA("BotMessenger", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
    if (!hInternet) return false;

    URL_COMPONENTSA components = { sizeof(components) };
    components.dwSchemeLength = (DWORD)-1;
    components.dwHostNameLength = (DWORD)-1;
    components.dwUrlPathLength = (DWORD)-1;
    if (!InternetCrackUrlA(url.c_str(), 0, 0, &components)) {
        InternetCloseHandle(hInternet);
        return false;
    }

    std::string host(components.lpszHostName, components.dwHostNameLength);
    std::string path(components.lpszUrlPath, components.dwUrlPathLength);

    HINTERNET hConnect = InternetConnectA(hInternet, host.c_str(), INTERNET_DEFAULT_HTTPS_PORT, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
    if (!hConnect) {
        InternetCloseHandle(hInternet);
        return false;
    }

    HINTERNET hRequest = HttpOpenRequestA(hConnect, "POST", path.c_str(), NULL, NULL, NULL, INTERNET_FLAG_SECURE, 0);
    if (!hRequest) {
        InternetCloseHandle(hConnect);
        InternetCloseHandle(hInternet);
        return false;
    }

    std::string headers = "Authorization: Bot " + token + "\r\nContent-Type: application/json\r\n";

    BOOL sent = HttpSendRequestA(hRequest, headers.c_str(), -1, (LPVOID)payload.c_str(), payload.length());

    InternetCloseHandle(hRequest);
    InternetCloseHandle(hConnect);
    InternetCloseHandle(hInternet);

    return sent == TRUE;
}

int main() {
    int choice = 0;

    std::cout << "Select an option:\n";
    std::cout << "1. Spam Discord Webhook\n";
    std::cout << "2. Delete Discord Channels (via Bot Token)\n";
    std::cout << "3. Nuke via spamming and deleting. (via bot token)\n";
    std::cout << "Enter choice (1-3): ";
    std::cin >> choice;

    if (choice == 1) {
        std::string url, message;
        int sendCount = 1, embedCount = 0;

        std::cin.ignore();
        std::cout << "Enter Webhook URL: ";
        std::getline(std::cin, url);

        if (url.find("discord.com/api/webhooks/") == std::string::npos) {
            std::cout << "Invalid Webhook URL.\n";
            return 1;
        }
        //init
        std::cout << "Enter message to send: ";
        std::getline(std::cin, message);

        std::cout << "How many times to send? ";
        std::cin >> sendCount;

        std::cout << "How many embeds to include? ";
        std::cin >> embedCount;

        for (int i = 0; i < sendCount; ++i) {
            if (!SendDiscordWebhook(url, message, embedCount)) {
                std::cout << "Failed to send webhook at count " << i + 1 << "\n";
            }
            else {
                std::cout << "Sent webhook #" << i + 1 << "\n";
            }
            Sleep(1000);
        }
    }

    else if (choice == 2) {
        std::string botToken;
        int numChannels = 0;

        std::cin.ignore();
        std::cout << "Enter your bot token: ";
        std::getline(std::cin, botToken);

        std::cout << "How many channels do you want to delete? ";
        std::cin >> numChannels;

        std::vector<std::string> channelIds(numChannels);
        std::cin.ignore();

        for (int i = 0; i < numChannels; ++i) {
            std::cout << "Enter Channel ID #" << (i + 1) << ": ";
            std::getline(std::cin, channelIds[i]);
        }

        for (const auto& channelId : channelIds) {
            std::cout << "Deleting channel: " << channelId << " ... ";
            if (DeleteDiscordChannel(botToken, channelId)) {
                std::cout << "Success!\n";
            }
            else {
                std::cout << "Failed.\n";
            }
            Sleep(1000);
        }
    }

    else if (choice == 3) {
        std::string botToken, announcementMessage, announceChannelId;
        int numChannels = 0;

        std::cin.ignore();
        std::cout << "Enter your bot token: ";
        std::getline(std::cin, botToken);

        std::cout << "Enter announcement message (example: @everyone Channel deleted): ";
        std::getline(std::cin, announcementMessage);

        std::cout << "Enter the channel ID to send the announcement in: ";
        std::getline(std::cin, announceChannelId);

        if (SendBotMessageToChannel(botToken, announceChannelId, announcementMessage)) {
            std::cout << "Announcement sent successfully!\n";
        }
        else {
            std::cout << "Failed to send announcement.\n";
        }

        std::cout << "Delete the announcement channel after sending? (y/n): ";
        char deleteAnnounce;
        std::cin >> deleteAnnounce;

        if (deleteAnnounce == 'y' || deleteAnnounce == 'Y') {
            if (DeleteDiscordChannel(botToken, announceChannelId)) {
                std::cout << "Announcement channel deleted.\n";
            }
            else {
                std::cout << "Failed to delete announcement channel.\n";
            }
        }

        std::cout << "How many OTHER channels do you want to delete? ";
        std::cin >> numChannels;
        std::cin.ignore();

        std::vector<std::string> deleteList(numChannels);
        for (int i = 0; i < numChannels; ++i) {
            std::cout << "Enter Channel ID #" << (i + 1) << ": ";
            std::getline(std::cin, deleteList[i]);
        }

        for (const auto& channelId : deleteList) {
            std::cout << "Deleting channel: " << channelId << " ... ";
            if (DeleteDiscordChannel(botToken, channelId)) {
                std::cout << "Deleted.\n";
            }
            else {
                std::cout << "Failed.\n";
            }
            Sleep(-2000);
        }
    }

    std::cout << "Done.\n";
    system("pause");
    return 0;
}
