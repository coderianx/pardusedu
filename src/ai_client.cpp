#include "ai_client.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <string>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <vector>
#include <regex>
#include <sstream>

using json = nlohmann::json;

// Sohbet mesajlarını tutan yapı (kullanıcı veya asistan)
struct Message
{
    std::string role;
    std::string content;
};

// Konuşma geçmişini burada tutuyoruz, böylece AI önceki mesajları hatırlasın
static std::vector<Message> conversation_history;



// API'ye erişmek için kullandığımız anahtarlar, kullanıcı kendininkini girebilir
static std::string global_groq_api_key = "";
static std::string global_openrouter_api_key = "";
static std::string global_gemini_api_key = "";

// Hangi sağlayıcının kullanılacağı (Groq Cloud veya OpenRouter)
static AIProvider global_provider = AIProvider::GROQ;

// Hangi yapay zeka modelini kullanacağımızı belirtiyor, kullanıcı ayarlardan değiştirebilir
static std::string global_model = "llama-3.3-70b-versatile";

void set_groq_api_key(const std::string& key) {
    global_groq_api_key = key;
}

std::string get_groq_api_key() {
    return global_groq_api_key;
}

void set_openrouter_api_key(const std::string& key) {
    global_openrouter_api_key = key;
}

std::string get_openrouter_api_key() {
    return global_openrouter_api_key;
}

void set_gemini_api_key(const std::string& key) {
    global_gemini_api_key = key;
}

std::string get_gemini_api_key() {
    return global_gemini_api_key;
}

void set_provider(AIProvider provider) {
    global_provider = provider;
}

AIProvider get_provider() {
    return global_provider;
}

std::string get_provider_name() {
    if (global_provider == AIProvider::GROQ) return "groq";
    if (global_provider == AIProvider::OPENROUTER) return "openrouter";
    return "gemini";
}

void set_model(const std::string& model) {
    global_model = model;
}

std::string get_model() {
    return global_model;
}

// CURL ile istek atınca gelen cevabı yakalamak için geri çağırma fonksiyonu
static size_t WriteCallback(void* contents,
                            size_t size,
                            size_t nmemb,
                            std::string* s)
{
    s->append((char*)contents,
              size * nmemb);

    return size * nmemb;
}

// API'den gelen JSON cevabından yapay zekanın yazdığı metni çekip alıyoruz
std::string extract_ai_reply(
    const std::string& response)
{
    if (response.empty())
        return "Boş cevap";

    if (response[0] != '{')
        return response;

    try
    {
        json parsed =
            json::parse(response);

        if (parsed.contains("candidates"))
        {
            auto& candidates = parsed["candidates"];
            if (candidates.is_array() && !candidates.empty() &&
                candidates[0].contains("content") &&
                candidates[0]["content"].contains("parts") &&
                candidates[0]["content"]["parts"].is_array() &&
                !candidates[0]["content"]["parts"].empty() &&
                candidates[0]["content"]["parts"][0].contains("text"))
            {
                return candidates[0]["content"]["parts"][0]["text"].get<std::string>();
            }
            return "Gemini yanıtı ayrıştırılamadı";
        }

        if (!parsed.contains("choices"))
            return "choices alanı bulunamadı";

        auto& choices =
            parsed["choices"];

        if (!choices.is_array() ||
            choices.empty())
        {
            return "choices boş geldi";
        }

        return
            choices[0]["message"]["content"]
                .get<std::string>();
    }
    catch (const std::exception& e)
    {
        return
            std::string(
                "JSON parse hatası: "
            ) + e.what();
    }
}

static std::string remove_markdown(
    std::string text)
{
    // Kod bloklarını (``` ile yazılmış olanları) temizliyoruz, düz metin olarak kalmalı
    text = std::regex_replace(
        text,
        std::regex(R"(```[\s\S]*?```)",
        std::regex::icase),
        ""
    );

    // Satır içi kodları (` ile yazılanları) normal metne çeviriyoruz
    text = std::regex_replace(
        text,
        std::regex(R"(`([^`]*)`)"),
        "$1"
    );

    // Kalın yazılmış metinleri (** ile) düz metin haline getiriyoruz
    text = std::regex_replace(
        text,
        std::regex(R"(\*\*(.*?)\*\*)"),
        "$1"
    );

    // İtalik yazılmış metinleri (* ile) düz metne çeviriyoruz
    text = std::regex_replace(
        text,
        std::regex(R"(\*(.*?)\*)"),
        "$1"
    );

    // Başlıklardaki (#) işaretlerini kaldırıyoruz
    text = std::regex_replace(
        text,
        std::regex(R"(#+\s*)"),
        ""
    );

    // Alıntı satırlarının başındaki (>) işaretini temizliyoruz
    text = std::regex_replace(
        text,
        std::regex(R"(^>\s?)",
        std::regex_constants::multiline),
        ""
    );

    // Liste işaretlerini (-, *, +) düzgün bir görünüme kavuşturuyoruz
    text = std::regex_replace(
        text,
        std::regex(R"(^[-*+]\s)",
        std::regex_constants::multiline),
        "• "
    );

    return text;
}

// Kullanıcı mesajında YouTube linki var mı kontrol ediyoruz
static bool has_youtube_url(
    const std::string& text)
{
    return
        text.find("youtube.com/watch") != std::string::npos ||
        text.find("youtube.com/live") != std::string::npos ||
        text.find("youtu.be/") != std::string::npos ||
        text.find("youtube.com/shorts/") != std::string::npos ||
        text.find("youtube.com/embed/") != std::string::npos;
}

// Metnin içindeki ilk YouTube linkini bulup döndürüyoruz
static std::string extract_youtube_url(
    const std::string& text)
{
    std::regex url_regex(R"((https?://)?(www\.)?(youtube\.com/watch\?v=|youtu\.be/|youtube\.com/shorts/|youtube\.com/embed/)[\w-]+)", std::regex::icase);
    std::smatch match;
    if (std::regex_search(text, match, url_regex))
        return match.str(0);
    return "";
}

// nlohmann/json UTF-8 doğrulamasından geçmeyen byte'ları temizler
static std::string sanitize_utf8(const std::string& input)
{
    std::string result;
    result.reserve(input.size());
    for (size_t i = 0; i < input.size(); i++)
    {
        unsigned char c = input[i];
        if (c < 0x80)
        {
            if (c >= 0x20 || c == 0x09 || c == 0x0A || c == 0x0D)
                result += c;
            continue;
        }
        size_t n = 0;
        if (c >= 0xF0) n = 3;
        else if (c >= 0xE0) n = 2;
        else if (c >= 0xC2) n = 1;
        else continue;
        if (i + n >= input.size()) continue;
        bool ok = true;
        for (size_t j = 1; j <= n; j++)
            if ((input[i + j] & 0xC0) != 0x80) { ok = false; break; }
        if (!ok) continue;
        result += c;
        for (size_t j = 1; j <= n; j++)
            result += input[i + j];
        i += n;
    }
    return result;
}

// yt-dlp ile YouTube videosundan transkript indirip içeriğini döndürüyoruz
static std::string fetch_youtube_transcript(
    const std::string& url)
{
    // Önce yt-dlp kurulu mu kontrol et
    int check = system("which yt-dlp > /dev/null 2>&1");
    if (check != 0)
        return "yt-dlp_bulunamadi";

    // Transkript dosyasını indir (vtt formatında — daha az yer kaplar)
    std::string output = "/tmp/yt_transcript_%(id)s";
    std::string dl_cmd = "yt-dlp --skip-download --write-auto-sub --sub-lang tr --convert-subs vtt --output '" + output + "' " + url + " 2>/dev/null";
    system(dl_cmd.c_str());

    // İndirilen dosyayı oku (önce Türkçe, sonra İngilizce dene)
    // Tüm gereksiz bilgileri temizle: HTML tagları, zaman damgaları, satır numaraları,
    // WEBVTT başlıkları, [Müzik] gibi işaretleyiciler, boş satırlar ve ardışık tekrarlar
    std::string read_cmd = "cat /tmp/yt_transcript_*.tr.vtt /tmp/yt_transcript_*.en.vtt 2>/dev/null | sed -E 's/<[^>]+>//g; /^[[:space:]]*$/d; /^[0-9]+$/d; /^[0-9]{2}:[0-9]{2}:/d; /^WEBVTT/d; /^Kind:/d; /^Language:/d; /^\\[.*\\]/d; /^♪|^♫|^♬/d' | uniq";

    std::string result;
    FILE* pipe = popen(read_cmd.c_str(), "r");
    if (pipe)
    {
        char buffer[4096];
        while (fgets(buffer, sizeof(buffer), pipe))
            result += buffer;
        pclose(pipe);
    }

    // Geçici dosyaları temizle
    system("rm -f /tmp/yt_transcript_* 2>/dev/null");

    // JSON'a girmeden önce geçersiz UTF-8 byte'larını temizle
    return sanitize_utf8(result);
}

// Basit token tahmini: ortalama 3.5 karakter ≈ 1 token (Türkçe/İngilizce karışık)
static int estimate_tokens(const std::string& text)
{
    if (text.empty()) return 0;
    return std::max(1, (int)(text.length() / 3.5f));
}

// Modelin context limitini döndürür (çoğu model 128K destekler)
static int get_model_limit()
{
    return 128000;
}

std::string duckduckgo_search(const std::string& query) {
    // Sorguyu temizle: soru ekleri/karakterleri kaldır, gereksiz boşlukları düzelt
    std::string cleaned = query;
    cleaned = std::regex_replace(cleaned, std::regex(R"([?¿?!¡]+)"), " ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\b(kimdir|kim|nedir|neden|nasıl|nasil|nerede|nereye|hangi|kaç|kac|mı|mi|mu|mü|miyim|misin|midir)\b)", std::regex::icase), " ");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s+,)"), ",");
    cleaned = std::regex_replace(cleaned, std::regex(R"(,\s+)"), ",");
    cleaned = std::regex_replace(cleaned, std::regex(R"(\s+)"), " ");
    cleaned.erase(0, cleaned.find_first_not_of(" \t\r\n"));
    cleaned.erase(cleaned.find_last_not_of(" \t\r\n") + 1);
    if (cleaned.empty()) cleaned = query;

    CURL* curl = curl_easy_init();
    if (!curl) return "CURL başlatılamadı";

    std::string encoded_query;
    CURL* curl_enc = curl_easy_init();
    if (curl_enc) {
        char* enc = curl_easy_escape(curl_enc, cleaned.c_str(), (int)cleaned.size());
        if (enc) { encoded_query = enc; curl_free(enc); }
        curl_easy_cleanup(curl_enc);
    }

    std::string url = "https://api.duckduckgo.com/?q=" + encoded_query + "&format=json&no_html=1&skip_disambig=1";
    std::string response;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "PardusEdu/1.0");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK)
        return "DuckDuckGo hatası: " + std::string(curl_easy_strerror(res));

    return response;
}

std::string call_ai(
    const std::string& user_text,
    const std::string& app_context)
{
    CURL* curl =
        curl_easy_init();

    std::string response;

    if (!curl)
        return "CURL başlatılamadı";


    std::string api_key;
    if (global_provider == AIProvider::GROQ) api_key = global_groq_api_key;
    else if (global_provider == AIProvider::OPENROUTER) api_key = global_openrouter_api_key;
    else api_key = global_gemini_api_key;

    std::string system_prompt =
        "Sen Pardus Linux üzerinde çalışan "
        "PardusEdu uygulamasının teknik eğitim "
        "asistanısın. "

        "Adın PardusEdu Yapay Zeka Asistanı. "

        "Taban Modelin " + global_model + ". "

        "PardusEdu Uygulamasının Geliştiricisi: Ali Eymen İçli. "

        "Uygulama C++ ve GTK Kullanılarak geliştirilmiştir. "

        "Geliştiric Uygulamayı 13 Yaşında Geliştirmeye başladı. "
        
        "PardusEdu Repo: github.com/coderianx/pardusedu "

        "Emoji Kullan ama az sayıda kullan."
        "Tüm sohbeti doldurma emojiyle her mesajda 1 yeter yada 2. "

        "Yanıtların doğru, açıklayıcı ve "
        "gerektiğinde örnekli olmalıdır. "

        "Linux, eğitim ve okul derslerinde uzmansın. "

        "Kullanıcıya sadece chat sunma ona Ders Koçuymuş gibi davran. "
        "Bugün sana ders planı yapalım. "

        "PardusEdu Uygulamasında: "
        "- Pomdoro Sayacı. "
        "- Görevlerim : Kullanıcıların Ödevlerini, Görevlerini Not Alması için Yapılmıştır. "
        "- Notlarım : Kullanıcıların Ders Notları Tutabilmesi için Yapılmıştır Her derse özel Yeni sayfa açılabilir. "
        "- Ders Programı : Kullancının Okul/Üniversitedeki Ders Programlarını Yazabilmeleri İçindir. "
        "- Dikkat Modu : Kullanıcın Dikkatinin dağılmaması için yapılmıştır, Sosyal Medya sitelerini engeller. "
        "- Linux : Kullanıcıların Linux Öğrenme Komutlarını Öğrenmesi için Komut Listesi ve açıklamaları olan bir yer. "
        "- PardusEdu Yapay Zeka Asistanı : Senin bulunduğun yer. "

        "Pomodoro Sayacı: "
        "- Başlat Tuşuyla 25 Dakikalık Sayaç Başlar. "
        "- Sıfırla Tuşuyla zamanı sıfırlar. "
        "- Atla tuşuyla mola vaktine geçilir. "
        "- Tuşların altındaki +/- tuşları ile Sayaç süreleri ayarlanabilir. "

        "Görevler: "
        "- Önünüzdeki İnput'a Görev adı yazılır (Ödev sayfaları ve benzeri şeyler olabilir). "
        "- İnput yanındaki Tarih yerinden Tarih ayarlanabilir GG.AA.YYYY formatında. "
        "- Altındaki Seçeneklerden Genel kımından Görevin Türü (Ödev, Sınav, Ders Çalışma, Proje Ödevi). "
        "- Sondaki seçenekten Görevin önemlilik seviyesini ayarlayabiliriz (Kolay, Orta, Zor). "
        "- Görev Ekle tuşuyla Görev eklenir. "

        "Notlarım: "
        "- Dersler Kısmında Dersleriniz bulunur. "
        "- 'Ders Ekle' tuşuyla Yeni ders eklenebilir. "
        "- Eklenen Dersin üzerine tıklanıp sağdaki sayfaya notlar alınmaya başlanabilir. "
        "- Sağdaki sayfanın altındaki 'Kaydet' butonu ile Notunuz kaydedilir. "
        "- Sağdaki Sayfanın altındaki 'Dersi Sil' butonu ile Dersiniz silinir. "

        "NOT DÜZENLEME YETENEĞİN (ÖNEMLİ): "
        "Kullanıcı senden bir not eklemeni, güncellemeni veya silmeni isterse "
        "aşağıdaki özel formatı KESİNLİKLE kullan. Bu format olmadan notlar değişmez. "
        "Her satırı ayrı ayrı yaz, aynen gösterildiği gibi:\n"

        "Not eklemek için:\n"
        "[NOT_ISLEMI: EKLE]\n"
        "Ders: Ders Adı\n"
        "---\n"
        "Not içeriği buraya yazılır...\n"
        "[NOT_ISLEMI: BITTI]\n\n"

        "Not güncellemek için:\n"
        "[NOT_ISLEMI: GUNCELLE]\n"
        "Ders: Ders Adı\n"
        "---\n"
        "Güncellenmiş not içeriği...\n"
        "[NOT_ISLEMI: BITTI]\n\n"

        "Not silmek için:\n"
        "[NOT_ISLEMI: SIL]\n"
        "Ders: Ders Adı\n"
        "[NOT_ISLEMI: BITTI]\n\n"

        "Bu komutları normal konuşmanın içinde kullan. "
        "Kullanıcıya işlemi yaptığını bildir. "
        "Her NOT_ISLEMI mutlaka NOT_ISLEMI: BITTI ile kapatılmalıdır. "
        "Komut satırları arasında boş satır olmamalıdır. "
        "Ders Adı yalnızca dersin adıdır, yanına --- veya içerik ekleme. "

        "Ders Programı: "
        "- Saat yazılarının üzerine tıklanarak ders saati değiştirebilir. "
        "- '+' Tuşlarının üzerine tıklanıp Ders Adı ve yeri eklenebilir. "
        "- 'Ders Ekle' tuşuyla ders eklenir. "
        "- 'Programı Temizle' tuşuyla tüm program temizlenir. "

        "Dikkat Modu: "
        "- 'Dikkat Modunu Başlat' butonu ile dikkat modu başlar. "
        "- Dikkat modu açıkken engellenen sitelere erişim kısıtlanır. "
        
        "Linux: "
        "- Öğrenmek istediğiniz komutun adını 'Komut ara...' ile arayabilirsiniz. "
        "- Öğrenmek istediğimiz Komutun üzerine tıkladığınızda alta doğru uzar ve açıklamalar açılır. "
    
        "Yapay Zeka: "
        "- Burada kullanıcılar seninle konuşabilir. "

        "Python Öğren: "
        "- Burada Kullanıcılar en temelden Python Öğrenebilir. "
        "- Dokümantasyon en temelden 'Orta-İleri' seviyeye kadar çıkartılır. "
        "- Dokümantasyonu Okuyan kişi şunlara sahip olur: "
            "- OOP. "
            "- Asenkron Programlama. "
            "- Test Yazma. "
            "- Ve çok daha fazla şey"

        "Kullanıcı model seçimi konusunda yardım isterse (hangi modeli kullanmalıyım, "
        "Groq, OpenRouter, Gemini arasındaki fark nedir gibi sorular sorarsa) "
        "ona Groq, OpenRouter ve Gemini sağlayıcılarını ve modelleri detaylıca açıkla. "
        "Ama kullanıcı sormadıkça modellerden bahsetme. "
        "Hangi modeli seçmesi gerektiğini pardusedu.netlify.app/ sitesinden bakabilir. "

        "Kurallar: "
        "- Öğrencilerin seviyesine göre anlatımS yap. "
        "- Karmaşık konuları seviyesine göre güzel anlat. "
        "- Gerekirse Adım Adım açıkla. "
        "- Bilgi eksikse "
        "- Markdown kullanmak kesinlikle yasak. "
        "- Asla Markdown kullanma"
        "'Bundan emin değilim' de. "
        "- Tahmin yapma. "
        "- Gerektiğinde adım adım açıkla. " 
        "- Eğer kullanıcı “öğret” derse, konuyu ders formatında anlat. "
        "- Eğer kullanıcı “özetle” derse kısa özet ver. "
        "- Eğer kullanıcı “örnek ver” derse sadece örnek odaklı cevap ver"
        "- Türkçe cevap ver."

        "   YouTube Transkriptinden Ders Notu Çıkarma\n"
        "Kullanıcı bir YouTube linki gönderdiğinde transkript otomatik alınır "
        "ve sana 'YouTube videosunun transkriptidir. Bu transkripti ders notlarına çevir:' "
        "şeklinde iletilir. Aşağıdaki kurallara uy:\n"
        "- Sadece verilen transkriptteki bilgileri kullan.\n"
        "- Timestamps, tekrarlar ve gereksiz konuşmaları yok say.\n"
        "- Uydurma bilgi ekleme.\n"
        "- Bilgi net değilse 'videoda net belirtilmemiş' yaz.\n"
        "- Basit, anlaşılır Türkçe kullan.\n"
        "- Öğrenci odaklı öğretici anlatım yap.\n"
        "ÇIKTI FORMATI:\n"
        "1. 📌 Kısa Özet (3-5 cümle)\n"
        "2. 📌 Önemli Noktalar (madde madde)\n"
        "3. 📌 Ders Notları (detaylı açıklama)\n"
        "EK KURAL:\n"
        "- Eğer kullanıcı sınav soruları isterse:\n"
        "  Soruları ayrı bir başlık açmadan, ders notlarının içine doğal şekilde serpiştir.\n"
        "  Örneğin: açıklamanın sonunda veya önemli bir konudan sonra soru şeklinde ekle.\n"
        "  Format: 'Soru: ... ? Cevap: ...' hatasız\n"
        "Eğer sınav soruaları istemezse sınav sorusu sorma sadece kullanıcı isterse sor!!!.\n"

    "Kullanıcının Uygulama Verileri (notlar, görevler, ders programı):\n"
    "Bu veriler kullanıcının PardusEdu uygulamasına girdiği bilgilerdir. "
    "Kullanıcı 'notlarımı oku', 'ders notlarım', 'Tarih notlarım', "
    "'görevlerim', 'programım' gibi bir şey söylerse "
    "aşağıdaki verilere bakıp ona göre cevap ver. "
    "Eğer kullanıcı belirli bir dersin notlarını soruyorsa "
    "sadece o derse ait notları kullan.\n\n"
    + app_context;

    // Kullanıcı YouTube linki attıysa yt-dlp ile transkript indir
    std::string transcript_context;

    if (has_youtube_url(user_text))
    {
        std::string youtube_url =
            extract_youtube_url(user_text);

        std::string transcript =
            fetch_youtube_transcript(youtube_url);

        if (transcript.empty())
        {
            transcript_context =
                "⚠️ Bu video için transkript "
                "bulunamadı. Video Türkçe veya "
                "İngilizce altyazı içermiyor "
                "olabilir.\n\n";
        }
        else if (transcript == "yt-dlp_bulunamadi")
        {
            transcript_context =
                "⚠️ yt-dlp kurulu değil. "
                "Transkript almak için: "
                "sudo apt install yt-dlp\n\n";
        }
        else
        {
            // Token limitini aşmamak için transkripti kısalt
            const size_t MAX_TRANSCRIPT_CHARS = 18000;
            std::string truncated = transcript;

            if (truncated.size() > MAX_TRANSCRIPT_CHARS)
            {
                // İlk yarı ve son yarıyı al ki giriş+sonuç kaybolmasın
                size_t half = MAX_TRANSCRIPT_CHARS / 2;
                truncated = truncated.substr(0, half)
                    + "\n\n[...ortadaki kısım atlandı, video çok uzun...]\n\n"
                    + truncated.substr(truncated.size() - half);
            }

            transcript_context =
                "Aşağıdaki metin, kullanıcının "
                "gönderdiği YouTube videosunun "
                "transkriptidir. Bu transkripti "
                "ders notlarına çevir:\n\n" +
                truncated + "\n\n";
        }
    }

    std::string final_user_text =
        transcript_context + user_text;

    // Kullanıcının mesajını konuşma geçmişine ekliyoruz ki AI bağlamı anlasın
    conversation_history.push_back({
        "user",
        final_user_text
    });

    // ─── Mesajları hazırla ───────────────────────────────────────────
    json messages = json::array();

    messages.push_back({
        {"role", "system"},
        {"content", system_prompt}
    });

    int start = std::max(0, (int)conversation_history.size() - 3);

    for (int i = start; i < (int)conversation_history.size(); i++)
    {
        messages.push_back({
            {"role", conversation_history[i].role},
            {"content", conversation_history[i].content}
        });
    }

    // ─── Token tahmini ve akıllı kırpma ───────────────────────────────
    int max_tokens = 3050;
    const int MIN_TOKENS = 128;
    const int MODEL_LIMIT = get_model_limit();
    const int SAFETY_MARGIN = 1024; // model limitine göre emniyet payı

    // Toplam prompt token'larını tahmin et
    int estimated_prompt = 0;
    for (const auto& msg : messages)
    {
        if (msg.contains("content") && msg["content"].is_string())
            estimated_prompt += estimate_tokens(msg["content"].get<std::string>());
        estimated_prompt += 4; // her mesajın format overhead'i
    }

    // Eğer prompt + max_tokens limiti aşıyorsa, önce history'yi kırp
    int total_target = estimated_prompt + max_tokens;
    int limit = MODEL_LIMIT - SAFETY_MARGIN;

    while (total_target > limit && (int)messages.size() > 1)
    {
        // System mesajını (index 0) koru, en eski sohbet mesajını sil
        auto removed = messages[1]["content"];
        estimated_prompt -= estimate_tokens(removed.get<std::string>());
        estimated_prompt -= 4;
        messages.erase(messages.begin() + 1);
        total_target = estimated_prompt + max_tokens;
    }

    // Hala aşıyorsa max_tokens'ı kırp
    if (total_target > limit)
        max_tokens = std::max(MIN_TOKENS, max_tokens - (total_target - limit));

    // ─── cURL hazırlıkları ───────────────────────────────────────────
    struct curl_slist* headers = nullptr;

    bool is_gemini = (global_provider == AIProvider::GEMINI);

    if (!is_gemini) {
        headers = curl_slist_append(
            headers,
            ("Authorization: Bearer " + api_key).c_str());
    }
    
    headers = curl_slist_append(
        headers,
        "Content-Type: application/json");

    std::string api_url;
    if (global_provider == AIProvider::GROQ) {
        api_url = "https://api.groq.com/openai/v1/chat/completions";
    } else if (is_gemini) {
        api_url = "https://generativelanguage.googleapis.com/v1beta/models/" + global_model + ":generateContent?key=" + api_key;
    } else {
        api_url = "https://openrouter.ai/api/v1/chat/completions";
    }

    curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());

    if (global_provider == AIProvider::OPENROUTER) {
        headers = curl_slist_append(headers, "HTTP-Referer: https://pardusedu.app");
        headers = curl_slist_append(headers, "X-Title: PardusEdu");
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    // ─── İsteği gönder, 413'te akıllıca yeniden dene ─────────────────
    long http_code = 0;

    for (int attempt = 0; attempt < 5; attempt++)
    {
        json body;
        if (is_gemini)
        {
            json gemini_contents = json::array();

            for (size_t mi = 1; mi < messages.size(); mi++)
            {
                std::string role = messages[mi]["role"];
                if (role == "assistant") role = "model";
                std::string content = messages[mi]["content"];
                gemini_contents.push_back({
                    {"role", role},
                    {"parts", json::array({json{{"text", content}}})}
                });
            }

            std::string system_text = messages[0]["content"];
            body = {
                {"system_instruction", {{"parts", json::array({json{{"text", system_text}}})}}},
                {"contents", gemini_contents},
                {"generationConfig", {
                    {"temperature", 0.4},
                    {"topP", 0.9},
                    {"maxOutputTokens", max_tokens}
                }}
            };
        }
        else
        {
            body = {
                {"model", global_model},
                {"temperature", 0.4},
                {"top_p", 0.9},
                {"max_tokens", max_tokens},
                {"messages", messages}
            };
        }

        std::string json_body = body.dump(-1, ' ', false, json::error_handler_t::ignore);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
        response.clear();

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK)
        {
            std::string error = curl_easy_strerror(res);
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return "API hatası: " + error;
        }

        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (http_code == 413)
        {
            // Önce history'den en eski mesajı at
            if ((int)messages.size() > 1)
            {
                messages.erase(messages.begin() + 1);
                continue;
            }
            // History bitmişse max_tokens'ı yarıya indir
            if (max_tokens > MIN_TOKENS)
            {
                max_tokens = std::max(MIN_TOKENS, max_tokens / 2);
                continue;
            }
        }

        break;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (http_code != 200)
    {
        return
            "HTTP hata kodu: " +
            std::to_string(http_code) +
            "\n\n" +
            response;
    }

    std::ofstream log(
        "/tmp/ai_response.log",
        std::ios::app);

    if (log.is_open())
    {
        log << "=== RESPONSE ===\n";
        log << response << "\n\n";

        log.close();
    }

    std::string ai_reply =
        extract_ai_reply(response);

    ai_reply = std::regex_replace(
        ai_reply,
        std::regex(R"(<think>[\s\S]*?</think>)"),
        ""
    );

    ai_reply = remove_markdown(ai_reply);

    // Cevabın başında ve sonunda gereksiz boşluklar varsa temizliyoruz
    ai_reply.erase(0, ai_reply.find_first_not_of(" \n\r\t"));
    ai_reply.erase(ai_reply.find_last_not_of(" \n\r\t") + 1);

    conversation_history.push_back({
        "assistant",
        ai_reply
    });

    return ai_reply;
}