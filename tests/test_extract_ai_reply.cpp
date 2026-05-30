#include <gtest/gtest.h>
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

static std::string extract_ai_reply(const std::string& response) {
    if (response.empty()) return "Boş cevap";
    if (response[0] != '{') return response;
    try {
        json parsed = json::parse(response);
        if (!parsed.contains("choices")) return "choices alanı bulunamadı";
        auto& choices = parsed["choices"];
        if (!choices.is_array() || choices.empty()) return "choices boş geldi";
        return choices[0]["message"]["content"].get<std::string>();
    } catch (const std::exception& e) {
        return std::string("JSON parse hatası: ") + e.what();
    }
}

TEST(ExtractAiReplyTest, EmptyResponse) {
    EXPECT_EQ("Boş cevap", extract_ai_reply(""));
}

TEST(ExtractAiReplyTest, NonJsonResponse) {
    EXPECT_EQ("Merhaba", extract_ai_reply("Merhaba"));
}

TEST(ExtractAiReplyTest, ValidJsonResponse) {
    json j = {
        {"choices", json::array({
            json::object({
                {"message", json::object({
                    {"content", "Python bir programlama dilidir."}
                })}
            })
        })}
    };
    EXPECT_EQ("Python bir programlama dilidir.", extract_ai_reply(j.dump()));
}

TEST(ExtractAiReplyTest, MultipleChoicesUsesFirst) {
    json j = {
        {"choices", json::array({
            json::object({{"message", json::object({{"content", "Birinci"}})}}),
            json::object({{"message", json::object({{"content", "İkinci"}})}})
        })}
    };
    EXPECT_EQ("Birinci", extract_ai_reply(j.dump()));
}

TEST(ExtractAiReplyTest, TurkishCharacters) {
    json j = {
        {"choices", json::array({
            json::object({{"message", json::object({{"content", "şğıüçö ŞĞİÜÇÖ"}})}})
        })}
    };
    EXPECT_EQ("şğıüçö ŞĞİÜÇÖ", extract_ai_reply(j.dump()));
}

TEST(ExtractAiReplyTest, MissingChoices) {
    json j = {{"error", "hata"}};
    EXPECT_EQ("choices alanı bulunamadı", extract_ai_reply(j.dump()));
}

TEST(ExtractAiReplyTest, EmptyChoicesArray) {
    json j = {{"choices", json::array()}};
    EXPECT_EQ("choices boş geldi", extract_ai_reply(j.dump()));
}

TEST(ExtractAiReplyTest, InvalidJson) {
    std::string r = extract_ai_reply("{bozuk");
    EXPECT_TRUE(r.find("JSON parse hatası") != std::string::npos);
}
