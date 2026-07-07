#pragma once
#include <string>

bool keyring_store(const std::string& provider, const std::string& key);
std::string keyring_load(const std::string& provider);
void keyring_clear(const std::string& provider);
