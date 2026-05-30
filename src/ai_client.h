#pragma once
#include <string>

std::string call_ai(const std::string& user_text, const std::string& app_context = "");
std::string extract_ai_reply(const std::string& json);
std::string strip_markdown(const std::string& text);
void set_api_key(const std::string& key);
void set_model(const std::string& model);
std::string get_model();