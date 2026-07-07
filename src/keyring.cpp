#include "keyring.h"

#include <cstdio>
#include <array>
#include <memory>

bool keyring_store(const std::string& provider, const std::string& key) {
    if (key.empty()) return true;
    std::string cmd = "secret-tool store --label='PardusEdu API Key' provider " + provider;
    FILE* pipe = popen(cmd.c_str(), "w");
    if (!pipe) return false;
    fwrite(key.data(), 1, key.size(), pipe);
    int ret = pclose(pipe);
    return ret == 0;
}

std::string keyring_load(const std::string& provider) {
    std::string cmd = "secret-tool lookup provider " + provider;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return {};
    std::array<char, 4096> buf{};
    std::string result;
    size_t n;
    while ((n = fread(buf.data(), 1, buf.size() - 1, pipe)) > 0) {
        buf[n] = '\0';
        result += buf.data();
    }
    int ret = pclose(pipe);
    if (ret != 0) return {};
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
}

void keyring_clear(const std::string& provider) {
    std::string cmd = "secret-tool clear provider " + provider;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (pipe) pclose(pipe);
}
