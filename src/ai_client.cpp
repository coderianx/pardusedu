#include "ai_client.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include <string>
#include <fstream>
#include <algorithm>
#include <cstdlib>
#include <unordered_map>
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

// Wikipedia cevaplarını önbellekte tutuyoruz ki aynı soruyu tekrar sorana kadar beklemesin
static std::unordered_map<std::string,
                          std::string> wiki_cache;

// API'ye erişmek için kullandığımız anahtarlar, kullanıcı kendininkini girebilir
static std::string global_groq_api_key = "";
static std::string global_openrouter_api_key = "";

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

void set_provider(AIProvider provider) {
    global_provider = provider;
}

AIProvider get_provider() {
    return global_provider;
}

std::string get_provider_name() {
    return global_provider == AIProvider::GROQ ? "groq" : "openrouter";
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

// Metni küçük harfe çeviriyoruz ki karşılaştırma yaparken büyük/küçük harf sorunu olmasın
static std::string to_lower(std::string s)
{
    std::transform(
        s.begin(),
        s.end(),
        s.begin(),
        [](unsigned char c)
        {
            return std::tolower(c);
        });

    return s;
}

// Kullanıcının sorusunda "nedir", "kimdir", "araştır" gibi kelimeler varsa Wikipedia'ya bakıyoruz
static bool should_use_wikipedia(
    const std::string& text)
{
    std::string t = to_lower(text);

    return (
        t.find("araştır") != std::string::npos ||
        t.find("arastir") != std::string::npos ||
        t.find("incele") != std::string::npos ||
        t.find("bak") != std::string::npos ||
        t.find("öğren") != std::string::npos ||
        t.find("ogren") != std::string::npos ||
        t.find("nedir") != std::string::npos ||
        t.find("kimdir") != std::string::npos ||
        t.find("açıkla") != std::string::npos ||
        t.find("acikla") != std::string::npos
    );
}

// Kullanıcının sorusundan "nedir", "kimdir" gibi kelimeleri çıkarıp arama yapılacak konuyu buluyoruz
static std::string extract_topic(
    std::string text)
{
    std::vector<std::string> remove_words =
    {
        "nedir",
        "kimdir",
        "açıkla",
        "acikla",
        "araştır",
        "arastir",
        "incele",
        "öğren",
        "ogren",
        "hakkında",
        "hakkinda"
    };

    text = to_lower(text);

    for (const auto& word : remove_words)
    {
        size_t pos;

        while ((pos = text.find(word))
               != std::string::npos)
        {
            text.erase(pos,
                       word.length());
        }
    }

    // Gereksiz boşlukları temizleyip düzgün bir metin elde ediyoruz
    std::stringstream ss(text);

    std::string token;
    std::string cleaned;

    while (ss >> token)
    {
        cleaned += token + " ";
    }

    return cleaned;
}

// Wikipedia'da arama yapıp özet bilgiyi getiriyoruz, Türkçe bulamazsa İngilizce dener
static std::string wikipedia_search(
    const std::string& query,
    const std::string& lang = "tr")
{
    std::string cache_key =
        lang + ":" + query;

    // Daha önce aynı terimi aradık mı? Varsa önbellekten hızlıca veriyoruz
    if (wiki_cache.count(cache_key))
    {
        return wiki_cache[cache_key];
    }

    CURL* curl = curl_easy_init();

    std::string response;

    if (!curl)
        return "";

    char* escaped =
        curl_easy_escape(
            curl,
            query.c_str(),
            query.length());

    std::string search_url =
        "https://" + lang +
        ".wikipedia.org/w/api.php?"
        "action=query"
        "&list=search"
        "&format=json"
        "&srlimit=1"
        "&srsearch=" +
        std::string(escaped);

    curl_free(escaped);

    curl_easy_setopt(
        curl,
        CURLOPT_URL,
        search_url.c_str());

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEFUNCTION,
        WriteCallback);

    curl_easy_setopt(
        curl,
        CURLOPT_WRITEDATA,
        &response);

    curl_easy_setopt(
        curl,
        CURLOPT_SSL_VERIFYPEER,
        1L);

    curl_easy_setopt(
        curl,
        CURLOPT_SSL_VERIFYHOST,
        2L);

    curl_easy_setopt(
        curl,
        CURLOPT_TIMEOUT,
        30L);

    CURLcode res =
        curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        curl_easy_cleanup(curl);
        return "";
    }

    try
    {
        json parsed =
            json::parse(response);

        auto search_results =
            parsed["query"]["search"];

        if (!search_results.is_array() ||
            search_results.empty())
        {
            curl_easy_cleanup(curl);
            return "";
        }

        std::string title =
            search_results[0]["title"]
                .get<std::string>();

        std::string summary_response;

        char* escaped_title =
            curl_easy_escape(
                curl,
                title.c_str(),
                title.length());

        std::string summary_url =
            "https://" + lang +
            ".wikipedia.org/api/rest_v1/page/summary/" +
            std::string(escaped_title);

        curl_free(escaped_title);

        curl_easy_setopt(
            curl,
            CURLOPT_URL,
            summary_url.c_str());

        curl_easy_setopt(
            curl,
            CURLOPT_WRITEDATA,
            &summary_response);

        response.clear();

        res = curl_easy_perform(curl);

        curl_easy_cleanup(curl);

        if (res != CURLE_OK)
            return "";

        wiki_cache[cache_key] =
            summary_response;

        return summary_response;
    }
    catch (...)
    {
        curl_easy_cleanup(curl);
        return "";  
    }
}

static std::string extract_wiki_summary(
    const std::string& response)
{
    try
    {
        json parsed =
            json::parse(response);

        if (parsed.contains("extract"))
        {
            return parsed["extract"]
                .get<std::string>();
        }

        return "";
    }
    catch (...)
    {
        return "";
    }
}


static std::string build_wiki_context(
    const std::string& query)
{
    std::string raw =
        wikipedia_search(query, "tr");

    // Türkçe bulunamazsa İngilizce dene
    if (raw.empty())
    {
        raw =
            wikipedia_search(query, "en");
    }

    if (raw.empty())
        return "";

    std::string summary =
        extract_wiki_summary(raw);

    if (summary.empty())
        return "";

    return
        "Aşağıdaki bilgiler yalnızca "
        "referans amaçlıdır:\n\n"
        "Wikipedia bilgisi:\n" +
        summary +
        "\n\n";
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

std::string call_ai(
    const std::string& user_text,
    const std::string& app_context)
{
    CURL* curl =
        curl_easy_init();

    std::string response;

    if (!curl)
        return "CURL başlatılamadı";


    std::string api_key = global_provider == AIProvider::GROQ ? global_groq_api_key : global_openrouter_api_key;

    std::string system_prompt =
        "Sen Pardus Linux üzerinde çalışan "
        "PardusEdu uygulamasının teknik eğitim "
        "asistanısın. "

        "Adın PardusEdu Yapay Zeka Asistanı. "

        "Taban Modelin " + global_model + ". "

        "Ali Eymen İçli (Coderian) tarafından "
        "geliştirildin. (Taban Modelin Hariç)"

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

        "Groq mu OpenRouter mı? "

        "Groq: "
        "- Çok hızlı cevap verir. "
        "- Gerçek zamanlı kullanım için uygundur. "
        "- Düşük gecikme (latency) sunar. "
        "- Daha sınırlı model seçenekleri vardır. "
        "- Kimler içindir: "
            "- Anlık sohbet isteyenler. "
            "- Canlı asistan kullananlar. "
            "- Hızın önemli olduğu uygulamalar. "

        "OpenRouter: "
        "- Daha yavaş olabilir (Aşırı fark hissedilmez). "
        "- Çok fazla model seçeneği sunar. "
        "- GPT, Claude, Gemini, Qwen, DeepSeek gibi modelleri içerir. "
        "- En iyi modeli seçme imkanı sağlar. "
        "- Hız modelden modele değişir. "
        "- Kimler içindir: "
            "- Kaliteli cevap isteyenler. "
            "- Kodlama ve analiz yapanlar. "
            "- Farklı modelleri denemek isteyenler. "

        "Özet: "
        "- Groq = En hızlı sistem. "
        "- OpenRouter = En akıllı ve en geniş model sistemi. "

        "Model Açıklamaları(Kullanıcılar için): "

        "Groq Cloud Servisinde: "

        "- Llama 3.1 8B; "
            "- Çok hızlı çalışır. "
            "- Basit sorular için uygun. "
            "- Daha az token harcar. "
            "- 8 Milyar Parametreye sahiptir. "
            "- Dezavantaj: "
                "- 8B olduğu için bilgi kapasitesi düşüktür. "
                "- Zor sorularda hızlı hata yapar. "
            "- Kimler içindir: "
                "- Hızlı ama günlük sohbet isteyenler. "
        
        "- Llama 3.3 70B; "
            "- Dengelidir. "
            "- Akıllıdır. "
            "- Çoğu işte kaliteli sonuç verir. "
            "~70 Milyar Parametreye sahiptir. "
            "- Dezavantaj: "
                "- Küçük Modellere göre daha pahalı çalışır. "
            "- Kimler içindir: "
                "- Kaliteli cevap isteyen öğrenciler. "
                "- Detaylı bilgi isteyenler. "

                "- Llama 4 Scout 17B; "
            "- Akıllı planlama yapabilir. "
            "- Adım adım düşünmede başarılıdır. "
            "- Proje ve araştırma işlerinde güçlüdür. "
            "- 17 Milyar aktif parametreye sahiptir. "
            "- Dezavantaj: "
                "- Bazen gereğinden fazla detay verebilir. "
                "- Yeni olduğu için bazı konularda tutarsız olabilir. "
            "- Kimler içindir: "
                "- Proje yapan öğrenciler. "
                "- Araştırma yapan öğrenciler. "
                "- Planlı çalışmak isteyenler. "

        "- GPT OSS 120B; "
            "- Çok güçlü düşünme yeteneğine sahiptir. "
            "- Karmaşık soruları çözmede başarılıdır. "
            "- Kodlama ve analiz konusunda çok güçlüdür. "
            "- 120 Milyar Parametreye sahiptir. "
            "- Dezavantaj: "
                "- Daha yavaş çalışabilir. "
                "- Fazla kaynak tüketebilir. "
            "- Kimler içindir: "
                "- En kaliteli cevapları isteyen öğrenciler. "
                "- Zor derslerle çalışanlar. "
                "- İleri seviye öğrenciler. "

        "- GPT OSS 20B; "
            "- Dengeli ve hızlıdır. "
            "- Günlük kullanım için uygundur. "
            "- Orta seviye sorularda başarılıdır. "
            "- 20 Milyar Parametreye sahiptir. "
            "- Dezavantaj: "
                "- Çok karmaşık sorularda zorlanabilir. "
            "- Kimler içindir: "
                "- Günlük ders çalışan öğrenciler. "
                "- Hız ve kalite dengesi isteyenler. "
        
        "- GPT OSS Safeguard 20B; "
            "- Daha güvenli ve kontrollü sohbet deneyimi sunar. "
            "- Küfürlü ve uygunsuz içerikleri azaltmaya yardımcı olur. "
            "- Öğrenci dostu kullanım için geliştirilmiştir. "
            "- Dezavantaj: "
                "- Tüm uygunsuz mesajları tamamen engelleyemeyebilir. "
            "- Kimler içindir: "
                "- Güvenli sohbet isteyen öğrenciler. "
                "- Kontrollü kullanım tercih eden kullanıcılar. "

        "- Qwen 3 32B; "
            "- Kodlama ve matematikte güçlüdür. "
            "- Türkçe performansı oldukça iyidir. "
            "- Teknik konularda başarılıdır. "
            "- 32 Milyar Parametreye sahiptir. "
            "- Dezavantaj: "
                "- Bazen uzun cevaplar verebilir. "
            "- Kimler içindir: "
                "- Yazılım öğrenen öğrenciler. "
                "- Matematik ve mühendislik öğrencileri. "
                "- Teknik ders çalışanlar. "

        "- Groq Compound; "
            "- Çok hızlı cevap verir. "
            "- Gerçek zamanlı kullanım için uygundur. "
            "- Akıcı sohbet deneyimi sağlar. "
            "- Birden fazla sistemi birlikte kullanabilir. "
            "- Dezavantaj: "
                "- Bazı cevaplarda tutarsızlık olabilir. "
            "- Kimler içindir: "
                "- Hızlı cevap isteyen öğrenciler. "
                "- Canlı ders sırasında yardım alanlar. "
                "- Beklemek istemeyen kullanıcılar. "
            "- UYARI: "
                "- Bu Model Sorun Çıkarabilir!!! "
        
        
        
        "OpenRouter Servisinde: "

        "- DeepSeek R1; "
            "- Çok güçlü akıl yürütme modelidir. "
            "- Zor matematik, mantık ve analiz sorularında başarılıdır. "
            "- Adım adım düşünme kabiliyeti yüksektir. "
            "- Dezavantaj: "
                "- Bazen yavaş cevap verebilir. "
                "- Günlük sohbet için fazla detaylı olabilir. "
            "- Kimler içindir: "
                "- Matematik ve problem çözmek isteyenler. "
                "- Analiz ve mantık soruları çözenler. "
                "- Derin düşünme gerektiren işler yapanlar. "

        "- DeepSeek V4 Flash; "
            "- Çok hızlı ve verimli çalışan yeni nesil bir modeldir. "
            "- Uzun metinleri (çok büyük bağlamları) işleyebilir. "
            "- Kodlama ve yapay zeka ajan görevlerinde güçlüdür. "
            "- Dezavantaj: "
                "- Çok yeni olduğu için bazı durumlarda tutarsız olabilir. "
                "- R1 kadar saf düşünme odaklı değildir. "
            "- Kimler içindir: "
                "- Hızlı ama akıllı cevap isteyenler. "
                "- Kod yazan ve proje geliştirenler. "
                "- Uzun dokümanlarla çalışanlar. "

        "- Kimi K2 Thinking; "
            "- Uzun metinleri çok iyi analiz edebilir. "
            "- Mantık yürütme ve planlama konusunda başarılıdır. "
            "- Sohbetlerde bağlamı iyi korur. "
            "- Dezavantaj: "
                "- Bazen gereğinden uzun cevaplar verebilir. "
                "- Hızlı kullanımda biraz ağır kalabilir. "
            "- Kimler içindir: "
                "- Uzun proje ve araştırma yapanlar. "
                "- Detaylı analiz isteyen kullanıcılar. "
                "- Adım adım plan çıkaranlar. "

        "- Qwen 3 Coder; "
            "- Yazılım geliştirme için özel olarak güçlüdür. "
            "- Kod yazma, düzeltme ve açıklamada başarılıdır. "
            "- Birçok programlama dilini destekler. "
            "- Dezavantaj: "
                "- Genel sohbetlerde diğer modellere göre daha teknik kalabilir. "
            "- Kimler içindir: "
                "- Yazılım öğrenenler. "
                "- Kod yazan geliştiriciler. "
                "- Proje geliştiren kullanıcılar. "

        "- Qwen 3.6 Flash; "
            "- Çok hızlı ve düşük maliyetli bir modeldir. "
            "- Günlük kullanım için idealdir. "
            "- Basit sorulara hızlı cevap verir. "
            "- Dezavantaj: "
                "- Karmaşık sorularda yüzeysel kalabilir. "
            "- Kimler içindir: "
                "- Hızlı cevap isteyen kullanıcılar. "
                "- Günlük sohbet yapanlar. "
                "- Basit işler için model arayanlar. "

        "- Claude Haiku 4.5; "
            "- Çok hızlı ve dengeli bir modeldir. "
            "- Doğal ve akıcı metin üretir. "
            "- Kodlama ve yazı yazmada başarılıdır. "
            "- Dezavantaj: "
                "- Sonnet modellerine göre daha zayıftır. "
                "- Çok karmaşık görevlerde sınırlı kalabilir. "
            "- Kimler içindir: "
                "- Hızlı ve kaliteli cevap isteyenler. "
                "- Yazı yazma ve sohbet amaçlı kullanıcılar. "

        "- Claude Sonnet 4.6; "
            "- Çok güçlü ve profesyonel bir modeldir. "
            "- Analiz, yazı yazma ve kodlamada üst seviye performans verir. "
            "- Karmaşık görevleri iyi çözebilir. "
            "- Dezavantaj: "
                "- Daha pahalı ve daha yavaştır. "
            "- Kimler içindir: "
                "- Profesyonel kullanım isteyenler. "
                "- En kaliteli cevapları arayanlar. "
                "- İleri seviye analiz yapanlar. "

        "- GPT 5 Mini; "
            "- Dengeli, modern ve güçlü bir modeldir. "
            "- Çoğu görevde yüksek kalite verir. "
            "- Kodlama ve sohbet performansı iyidir. "
            "- Dezavantaj: "
                "- En büyük modeller kadar derin analiz yapamaz. "
            "- Kimler içindir: "
                "- Genel kullanım isteyenler. "
                "- Öğrenciler ve günlük kullanıcılar. "
                "- Hız + kalite dengesi isteyenler. "

        "- GPT 4o Mini; "
            "- Çok hızlı ve ekonomik bir modeldir. "
            "- Günlük sohbet ve basit işler için uygundur. "
            "- Dezavantaj: "
                "- Karmaşık sorularda zayıf kalabilir. "
            "- Kimler içindir: "
                "- Hızlı cevap isteyen kullanıcılar. "
                "- Basit kullanım için model arayanlar. "

        "- Gemini 2.5 Flash; "
            "- Çok hızlı çalışan Google modelidir. "
            "- Günlük kullanım için idealdir. "
            "- Akıcı ve stabil cevaplar verir. "
            "- Dezavantaj: "
                "- Derin analizlerde sınırlı kalabilir. "
            "- Kimler içindir: "
                "- Hızlı cevap isteyenler. "
                "- Günlük sohbet kullanıcıları. "

        "- Gemini 2.5 Pro; "
            "- Çok güçlü analiz ve reasoning modelidir. "
            "- Karmaşık problemleri çözmede başarılıdır. "
            "- Kodlama ve araştırmada güçlüdür. "
            "- Dezavantaj: "
                "- Flash modellere göre daha yavaştır. "
            "- Kimler içindir: "
                "- Araştırma yapanlar. "
                "- Zor problemleri çözenler. "
                "- Profesyonel kullanıcılar. "

        "- Gemma 4 31B; "
            "- Açık kaynaklı güçlü bir modeldir. "
            "- Dengeli ve genel kullanım için uygundur. "
            "- Türkçe dahil birçok dilde iyi performans verir. "
            "- Dezavantaj: "
                "- En üst seviye modeller kadar güçlü değildir. "
            "- Kimler içindir: "
                "- Açık kaynak model tercih edenler. "
                "- Dengeli kullanım isteyenler. "

        "- Mistral Large; "
            "- Çok güçlü genel amaçlı bir modeldir. "
            "- Yazı yazma, analiz ve eğitim içeriklerinde başarılıdır. "
            "- Uzun ve detaylı açıklamalar üretebilir. "
            "- Dezavantaj: "
                "- Daha yavaş ve maliyetli olabilir. "
            "- Kimler içindir: "
                "- Öğretici ve detaylı cevap isteyenler. "
                "- Analiz ve araştırma yapanlar. "

        "- Mistral Small 4; "
            "- Hızlı ve çok yönlü bir modeldir. "
            "- Kodlama, analiz ve agent görevlerinde dengelidir. "
            "- Dezavantaj: "
                "- Çok karmaşık görevlerde sınırlı kalabilir. "
            "- Kimler içindir: "
                "- Günlük kullanım isteyenler. "
                "- Hızlı ama akıllı cevap isteyenler. "

        "- Gemini 1.5 Flash; "
            "- Çok hızlı ve ekonomik bir Google modelidir. "
            "- Özetleme ve basit görevlerde başarılıdır. "
            "- Dezavantaj: "
                "- Derin analizlerde sınırlı kalabilir. "
            "- Kimler içindir: "
                "- Hızlı cevap isteyen kullanıcılar. "
                "- Ders ve günlük kullanım. "

        "- GPT 4.1 Mini; "
            "- Stabil ve dengeli bir OpenAI modelidir. "
            "- Eğitim ve açıklama görevlerinde başarılıdır. "
            "- Dezavantaj: "
                "- Büyük modeller kadar güçlü değildir. "
            "- Kimler içindir: "
                "- Güvenilir günlük kullanım isteyenler. "

        "- Qwen 3.7 Max; "
            "- Uzun görevleri planlayabilen güçlü bir modeldir. "
            "- Agent ve proje yönetiminde başarılıdır. "
            "- Dezavantaj: "
                "- Basit sorular için ağır olabilir. "
            "- Kimler içindir: "
                "- Proje yapan ve uzun işlerle uğraşanlar. "
            
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

    "Kullanıcının Uygulama Verileri (notlar, görevler, ders programı):\n"
    "Bu veriler kullanıcının PardusEdu uygulamasına girdiği bilgilerdir. "
    "Kullanıcı 'notlarımı oku', 'ders notlarım', 'Tarih notlarım', "
    "'görevlerim', 'programım' gibi bir şey söylerse "
    "aşağıdaki verilere bakıp ona göre cevap ver. "
    "Eğer kullanıcı belirli bir dersin notlarını soruyorsa "
    "sadece o derse ait notları kullan.\n\n"
    + app_context;

    // Kullanıcının ne sormak istediğini anlayıp konuyu çıkarıyoruz
    std::string topic =
        extract_topic(user_text);

    // Eğer kullanıcı bir şey araştırmamızı istiyorsa Wikipedia'dan bilgi getiriyoruz
    std::string wiki_context;

    if (should_use_wikipedia(user_text))
    {
        wiki_context =
            build_wiki_context(topic);
    }

    std::string final_user_text =
        wiki_context + user_text;

    // Kullanıcının mesajını konuşma geçmişine ekliyoruz ki AI bağlamı anlasın
    conversation_history.push_back({
        "user",
        final_user_text
    });

    // API'ye göndereceğimiz mesajları sırayla diziyoruz (sistem + geçmiş)
    json messages = json::array();

    messages.push_back({
        {"role", "system"},
        {"content", system_prompt}
    });

    int start =
        std::max(
            0,
            (int)conversation_history.size() - 6);

    for (int i = start;
         i < conversation_history.size();
         i++)
    {
        messages.push_back({
            {
                "role",
                conversation_history[i].role
            },

            {
                "content",
                conversation_history[i].content
            }
        });
    }

    // API'ye göndereceğimiz isteğin gövdesini hazırlıyoruz (model, sıcaklık, mesajlar)
    json body = {
        {"model", global_model},
        {"temperature", 0.4},
        {"top_p", 0.9},
        {"max_tokens", 2048},
        {"messages", messages}
    };

    std::string json_body = body.dump();

    // API isteğinin başlık bilgilerini ayarlıyoruz: yetkilendirme anahtarı ve içerik türü
    struct curl_slist* headers = nullptr;

    headers = curl_slist_append(
        headers,
        ("Authorization: Bearer " + api_key).c_str());

    headers = curl_slist_append(
        headers,
        "Content-Type: application/json");

    // Sağlayıcıya göre doğru API adresini belirliyoruz
    std::string api_url;
    if (global_provider == AIProvider::GROQ) {
        api_url = "https://api.groq.com/openai/v1/chat/completions";
    } else {
        api_url = "https://openrouter.ai/api/v1/chat/completions";
    }

    curl_easy_setopt(curl, CURLOPT_URL, api_url.c_str());

    // OpenRouter ekstra başlıklar ister
    if (global_provider == AIProvider::OPENROUTER) {
        headers = curl_slist_append(headers, "HTTP-Referer: https://pardusedu.app");
        headers = curl_slist_append(headers, "X-Title: PardusEdu");
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_body.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    // Güvenli bağlantı için SSL sertifikası doğrulamasını açıyoruz
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // İstek çok uzun sürerse iptal etmesi için zaman aşımı süreleri
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 10L);

    // API'ye isteği gönderip cevabı bekliyoruz
    CURLcode res = curl_easy_perform(curl);

    if (res != CURLE_OK)
    {
        std::string error = curl_easy_strerror(res);
        curl_slist_free_all(headers);
        curl_easy_cleanup(curl);
        return "API hatası: " + error;
    }

    // HTTP cevap kodunu kontrol ediyoruz, 200 değilse bir sorun var demektir
    long http_code = 0;

    curl_easy_getinfo(
        curl,
        CURLINFO_RESPONSE_CODE,
        &http_code);

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