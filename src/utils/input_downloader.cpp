#include <iostream>
#include <fstream>
#include <string>

#include <curl/curl.h>

// Callback function to write data to a file
size_t writeToFile(void* buffer, size_t size, size_t nmemb, void* userp) {
    std::ofstream* outFile = static_cast<std::ofstream*>(userp);
    size_t totalSize = size * nmemb;
    outFile->write(static_cast<char*>(buffer), totalSize);
    return totalSize;
}

bool downloadFile(const std::string& url, const std::string& outputFilename, const std::string& session) {
    CURL* curl;
    CURLcode res;
    std::ofstream outFile(outputFilename, std::ios::binary);

    if (!outFile) {
        std::cerr << "Failed to open file for writing: " << outputFilename << std::endl;
        return false;
    }

    curl = curl_easy_init();
    if (curl) {
        // Set the URL to download
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

        // Set the write callback function
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeToFile);

        // Pass the ofstream to the callback function
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outFile);

        // Load cookies from the specified file
        curl_easy_setopt(curl, CURLOPT_COOKIE, session.c_str());

        // Perform the file download
        res = curl_easy_perform(curl);

        // Cleanup
        curl_easy_cleanup(curl);

        // Check for errors
        if (res != CURLE_OK) {
            std::cerr << "CURL error: " << curl_easy_strerror(res) << std::endl;
            return false;
        }

        std::cout << "\nFile downloaded successfully: " << outputFilename << std::endl;
        return true;
    }

    std::cerr << "\nFailed to initialize CURL." << std::endl;
    return false;
}

int main(int argc, char *argv[]) {
    if(argc != 4) {
        std::cerr << "The downloader requires to specify day, year, and output folder as parameters";
        std::cerr << "\nFor example, ./input_downloader 1 2024 ./data";
        return 1;
    }
    using namespace std::literals::string_literals;
    const std::string url{"https://adventofcode.com/"s + argv[2] + "/day/" + argv[1] + "/input"};
    std::cerr << "\nurl: " << url;
    std::string outputFilename{std::string{argv[3]} + "/day" + argv[1] + ".txt"};
    std::string session{"session="};
    if(auto sessionPtr = getenv("AOC_SESSION")) {
        session += sessionPtr;
    }

    if(session.size() == 8) {
        std::cerr << "\nAOC_SESSION env not specified";
        return 1;
    }

    if (!downloadFile(url, outputFilename, session)) {
        std::cerr << "File download failed." << std::endl;
    }

    return 0;
}
