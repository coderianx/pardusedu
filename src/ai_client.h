#pragma once
#include <string>

enum class AIProvider { GROQ, OPENROUTER, GEMINI, OLLAMA };

std::string call_ai(const std::string& user_text, const std::string& app_context = "");
std::string call_ai_json(const std::string& prompt);
std::string extract_ai_reply(const std::string& json);
std::string strip_markdown(const std::string& text);

void set_groq_api_key(const std::string& key);
std::string get_groq_api_key();
void set_openrouter_api_key(const std::string& key);
std::string get_openrouter_api_key();
void set_gemini_api_key(const std::string& key);
std::string get_gemini_api_key();
void set_provider(AIProvider provider);
AIProvider get_provider();
std::string get_provider_name();
void set_model(const std::string& model);
std::string get_model();

void set_ollama_model(const std::string& model);
std::string get_ollama_model();
void set_ollama_url(const std::string& url);
std::string get_ollama_url();

std::string duckduckgo_search(const std::string& query);