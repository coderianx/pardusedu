#pragma once
#include <string>

enum class AIProvider { GROQ, OPENROUTER };

std::string call_ai(const std::string& user_text, const std::string& app_context = "");
std::string extract_ai_reply(const std::string& json);
std::string strip_markdown(const std::string& text);

void set_groq_api_key(const std::string& key);
std::string get_groq_api_key();
void set_openrouter_api_key(const std::string& key);
std::string get_openrouter_api_key();
void set_provider(AIProvider provider);
AIProvider get_provider();
std::string get_provider_name();
void set_model(const std::string& model);
std::string get_model();