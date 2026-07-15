#include "mainwindow.h"
#include "ai_client.h"
#include <nlohmann/json.hpp>
#include <webkit/webkit.h>
#include <curl/curl.h>
#include <gdk/gdk.h>
#include <gdk/gdkkeysyms.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <regex>
#include <thread>

using json = nlohmann::json;
namespace fs = std::filesystem;

void MainWindow::process_ai_notes(std::string& ai_text) {
    std::regex pattern(R"(\[NOT_ISLEMI:\s*(EKLE|GUNCELLE|SIL)\]([\s\S]*?)\[NOT_ISLEMI:\s*BITTI\])",
                       std::regex::icase);

    std::smatch match;
    std::string remaining = ai_text;
    std::string cleaned;
    bool modified = false;

    while (std::regex_search(remaining, match, pattern)) {
        cleaned += match.prefix().str();

        std::string command = match[1].str();
        std::string body = match[2].str();

        std::regex course_regex(R"(Ders:\s*(.+?)(?:\s*---|\n|$))");
        std::smatch course_match;
        std::string course_name;

        if (std::regex_search(body, course_match, course_regex)) {
            course_name = course_match[1].str();
            course_name.erase(0, course_name.find_first_not_of(" \t\r\n"));
            course_name.erase(course_name.find_last_not_of(" \t\r\n") + 1);
        }

        if (!course_name.empty()) {
            std::string content;
            auto sep_pos = body.find("---");
            if (sep_pos != std::string::npos) {
                content = body.substr(sep_pos + 3);
                content.erase(0, content.find_first_not_of(" \t\r\n"));
                content.erase(content.find_last_not_of(" \t\r\n") + 1);
            }

            if (command == "EKLE" || command == "GUNCELLE") {
                auto it = std::find_if(course_notes.begin(), course_notes.end(),
                    [&](const CourseNote& n) { return n.course == course_name; });

                if (it != course_notes.end()) {
                    if (!content.empty())
                        it->content = content;
                } else {
                    course_notes.push_back({course_name, content});
                }
                modified = true;
            } else if (command == "SIL") {
                auto it = std::find_if(course_notes.begin(), course_notes.end(),
                    [&](const CourseNote& n) { return n.course == course_name; });
                if (it != course_notes.end()) {
                    course_notes.erase(it);
                    modified = true;
                }
            }
        }

        remaining = match.suffix().str();
    }

    cleaned += remaining;
    ai_text = cleaned;

    if (modified) {
        refresh_course_list();
        if (selected_note_index >= 0 && selected_note_index < (int)course_notes.size()) {
            note_title.set_text(course_notes[selected_note_index].course);
            note_view.get_buffer()->set_text(course_notes[selected_note_index].content);
        } else if (course_notes.empty()) {
            selected_note_index = -1;
            note_title.set_text("Bir ders seçin");
            note_view.get_buffer()->set_text("");
        }
        save_data();
    }
}

void MainWindow::on_ai_response() {
    std::string result;
    {
        std::lock_guard<std::mutex> lock(ai_mutex);
        result = pending_ai_response;
        pending_ai_response.clear();
    }

    ai_waiting = false;
    ai_input.set_sensitive(true);
    btn_ai_send.set_sensitive(true);
    btn_ai_ddg.set_sensitive(true);
    btn_ai_image.set_sensitive(true);
    ai_loading_conn.disconnect();

    if (pending_ai_label) {
        std::string display_text;
        if (!result.empty()) {
            display_text = extract_ai_reply(result);
            process_ai_notes(display_text);
        }
        if (display_text.empty())
            display_text = "API cevap veremedi. Daha sonra dene.";
        pending_ai_label->set_text(display_text);
        pending_ai_label->remove_css_class("ai-loading");
        pending_ai_label = nullptr;
    }

    auto adj = ai_scroll.get_vadjustment();
    adj->set_value(adj->get_upper());
}

void MainWindow::setup_ai_chat() {
    ai_dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_ai_response));

    // Ana sayfa (dış scroll sadece sayfa taşarsa diye)
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* page = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    page->set_margin_start(20);
    page->set_margin_end(20);
    page->set_margin_top(20);
    page->set_margin_bottom(20);

    // ─── Header: Başlık + Model rozeti + Ayarlar ───────────────────────
    auto* header_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    header_box->add_css_class("ai-header-box");

    auto* title = Gtk::make_managed<Gtk::Label>("Yapay Zeka Asistanı");
    title->add_css_class("ai-page-title");
    title->set_halign(Gtk::Align::START);
    title->set_hexpand(false);

    // Model rozeti - model ID'yi okunabilir hale getir
    auto format_model_name = [](const std::string& id) -> std::string {
        std::string name = id;
        auto slash = name.find('/');
        if (slash != std::string::npos) name = name.substr(slash + 1);
        for (auto& c : name) {
            if (c == '-' || c == '_') c = ' ';
        }
        if (!name.empty()) name[0] = std::toupper(name[0]);
        return name;
    };

    ai_model_badge = Gtk::make_managed<Gtk::Label>(format_model_name(get_model()));
    ai_model_badge->add_css_class("ai-model-badge");
    ai_model_badge->set_halign(Gtk::Align::START);

    auto* spacer = Gtk::make_managed<Gtk::Label>();
    spacer->set_hexpand(true);

    auto* set_icon = Gtk::make_managed<Gtk::Image>();
    set_icon->set_from_resource(dark_mode
        ? "/org/ogrenci/merkezi/assets/settings.svg"
        : "/org/ogrenci/merkezi/assets/settings_dark.svg");
    set_icon->set_pixel_size(20);
    btn_ai_key.set_child(*set_icon);
    btn_ai_key.set_size_request(38, 38);
    btn_ai_key.add_css_class("ai-key-btn");
    btn_ai_key.set_halign(Gtk::Align::END);
    btn_ai_key.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::show_ai_key_dialog));

    header_box->append(*title);
    header_box->append(*ai_model_badge);
    header_box->append(*spacer);
    header_box->append(btn_ai_key);

    // ─── Chat mesaj alanı ──────────────────────────────────────────
    ai_chat_box.set_orientation(Gtk::Orientation::VERTICAL);
    ai_chat_box.set_spacing(6);
    ai_chat_box.set_vexpand(true);
    ai_chat_box.set_hexpand(true);

    ai_scroll.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    ai_scroll.set_vexpand(true);
    ai_scroll.set_child(ai_chat_box);

    // ─── Input alanı (ChatGPT tarzı bileşik) ──────────────────────
    ai_input.set_placeholder_text("PardusEdu Asistana soru sor...");

    auto* send_icon = Gtk::make_managed<Gtk::Image>();
    send_icon->set_from_resource("/org/ogrenci/merkezi/assets/send.svg");
    send_icon->set_pixel_size(20);

    btn_ai_send.set_child(*send_icon);
    btn_ai_send.set_size_request(44, 44);
    btn_ai_send.add_css_class("ai-send-btn");

    auto* ddg_icon = Gtk::make_managed<Gtk::Image>();
    ddg_icon->set_from_resource("/org/ogrenci/merkezi/assets/duckduckgo.svg");
    ddg_icon->set_pixel_size(22);
    btn_ai_ddg.set_child(*ddg_icon);
    btn_ai_ddg.set_size_request(44, 44);
    btn_ai_ddg.set_tooltip_text("Aktifse mesaj gönderiminde DuckDuckGo araştırması yapılıp AI analiz eder");
    btn_ai_ddg.add_css_class("ai-ddg-btn");

    // ─── Fotoğraf ekleme butonu ────────────────────────────────────
    auto* image_icon = Gtk::make_managed<Gtk::Image>();
    image_icon->set_from_resource(dark_mode
        ? "/org/ogrenci/merkezi/assets/image_dark.svg"
        : "/org/ogrenci/merkezi/assets/image.svg");
    image_icon->set_pixel_size(20);
    btn_ai_image.set_child(*image_icon);
    btn_ai_image.set_size_request(38, 38);
    btn_ai_image.set_tooltip_text("Soru fotoğrafı ekle");
    btn_ai_image.add_css_class("ai-image-btn");

    // Görsel seçme dialogu
    btn_ai_image.signal_clicked().connect([this]() {
        auto file_dialog = Gtk::FileDialog::create();
        file_dialog->set_title("Soru fotoğrafı seç");
        auto filter = Gtk::FileFilter::create();
        filter->set_name("Resim Dosyaları");
        filter->add_mime_type("image/png");
        filter->add_mime_type("image/jpeg");
        filter->add_mime_type("image/webp");
        auto filters = Gio::ListStore<Gtk::FileFilter>::create();
        filters->append(filter);
        file_dialog->set_filters(filters);
        file_dialog->set_default_filter(filter);
        file_dialog->open(*this, [this, file_dialog](const Glib::RefPtr<Gio::AsyncResult>& result) {
            auto file = file_dialog->open_finish(result);
            if (file) {
                ai_image_path = file->get_path();
                ai_image_mime = "image/png";
                auto path_lower = ai_image_path;
                std::transform(path_lower.begin(), path_lower.end(),
                    path_lower.begin(), ::tolower);
                auto has_suffix = [](const std::string& s, const std::string& suf) {
                    if (s.size() < suf.size()) return false;
                    return s.compare(s.size() - suf.size(), suf.size(), suf) == 0;
                };
                if (has_suffix(path_lower, ".jpg") || has_suffix(path_lower, ".jpeg"))
                    ai_image_mime = "image/jpeg";
                else if (has_suffix(path_lower, ".webp"))
                    ai_image_mime = "image/webp";
                std::ifstream ifs(ai_image_path, std::ios::binary);
                if (ifs) {
                    std::ostringstream buf;
                    buf << ifs.rdbuf();
                    ai_image_base64 = base64_encode(buf.str());
                }
                try {
                    auto pixbuf = Gdk::Pixbuf::create_from_file(ai_image_path, 80, 60, true);
                    if (pixbuf) {
                        auto texture = Gdk::Texture::create_for_pixbuf(pixbuf);
                        ai_image_thumbnail.set(texture);
                        ai_image_thumbnail.set_pixel_size(80);
                    }
                } catch (const Glib::Error& e) {
                    g_warning("Thumbnail hatası: %s", e.what());
                }
                ai_image_preview_box.set_visible(true);
            }
        });
    });

    // Görsel kaldırma (Label + gesture, çerçevesiz yuvarlak)
    auto* remove_lbl = Gtk::make_managed<Gtk::Label>("✕");
    remove_lbl->add_css_class("ai-image-remove-btn");
    remove_lbl->set_size_request(24, 24);
    remove_lbl->set_valign(Gtk::Align::CENTER);
    auto click = Gtk::GestureClick::create();
    click->signal_pressed().connect([this](int, double, double) {
        ai_image_clear();
    });
    remove_lbl->add_controller(click);

    ai_image_preview_box.set_spacing(6);
    ai_image_preview_box.add_css_class("ai-image-preview");
    ai_image_preview_box.append(ai_image_thumbnail);
    ai_image_preview_box.append(*remove_lbl);
    ai_image_preview_box.set_visible(false);

    auto* input_container = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    input_container->add_css_class("ai-input-container");

    ai_input.set_hexpand(true);
    input_container->append(btn_ai_image);
    input_container->append(ai_input);
    input_container->append(btn_ai_ddg);
    input_container->append(btn_ai_send);

    // Focus - container'ın border'ı tüm genişliği kaplasın
    auto fc = Gtk::EventControllerFocus::create();
    fc->signal_enter().connect([input_container]() {
        input_container->add_css_class("focused");
    });
    fc->signal_leave().connect([input_container]() {
        input_container->remove_css_class("focused");
    });
    ai_input.add_controller(fc);

    // ─── Send mesaj (DDG toggle aktifse araştırma + AI) ────────────
    auto send_message = [this]() {
        if (ai_waiting) return;
        std::string text = ai_input.get_text();
        bool has_image = !ai_image_base64.empty();
        if (text.empty() && !has_image) return;

        // Görsel yalnızca Gemini veya Groq vision modellerinde çalışır
        if (has_image && ai_provider != AIProvider::GEMINI) {
            std::string model = get_model();
            bool is_groq_vision = (ai_provider == AIProvider::GROQ && (
                model.find("llama-4-scout") != std::string::npos ||
                model.find("qwen3.6") != std::string::npos
            ));
            if (!is_groq_vision) {
                auto* warn = Gtk::make_managed<Gtk::MessageDialog>(
                    *this,
                    "Görsel analizi yaln\u0131zca Google Gemini veya "
                    "Groq vision modellerinde (\n"
                    "  \u2022 meta-llama/llama-4-scout-17b-16e-instruct\n"
                    "  \u2022 qwen/qwen3.6-27b\n"
                    ") kullan\u0131labilir.",
                    false, Gtk::MessageType::WARNING, Gtk::ButtonsType::OK, true);
                warn->signal_response().connect([warn](int) { warn->close(); });
                warn->show();
                return;
            }
        }

        bool ddg_active = btn_ai_ddg.get_active();

        std::string display_text = text;
        if (has_image) display_text += " [📷]";
        auto* user_lbl = Gtk::make_managed<Gtk::Label>(ddg_active ? "Araştır: " + display_text : display_text);
        user_lbl->add_css_class("user-msg");
        user_lbl->set_wrap(true);
        user_lbl->set_selectable(true);
        user_lbl->set_halign(Gtk::Align::END);
        ai_chat_box.append(*user_lbl);

        pending_ai_label = Gtk::make_managed<Gtk::Label>(
            ddg_active ? "DuckDuckGo Üzerinden ara\u015ft\u0131r\u0131l\u0131yor.." : "Cevap al\u0131n\u0131yor");
        pending_ai_label->add_css_class("ai-msg");
        pending_ai_label->add_css_class("ai-loading");
        pending_ai_label->set_wrap(true);
        pending_ai_label->set_selectable(true);
        pending_ai_label->set_halign(Gtk::Align::START);

        auto gesture = Gtk::GestureClick::create();
        gesture->set_button(3);
        gesture->signal_pressed().connect([this](int, double, double) {
            if (pending_ai_label) {
                auto clipboard = Gdk::Display::get_default()->get_clipboard();
                clipboard->set_text(pending_ai_label->get_text());
            }
        });
        pending_ai_label->add_controller(gesture);

        ai_chat_box.append(*pending_ai_label);

        ai_dot_count = 0;
        ai_loading_conn = Glib::signal_timeout().connect([this, ddg_active]() {
            if (!pending_ai_label) return false;
            ai_dot_count = (ai_dot_count + 1) % 4;
            std::string dots(ai_dot_count, '.');
            std::string msg = ddg_active
                ? "DuckDuckGo ara\u015ft\u0131r\u0131l\u0131yor"
                : "Cevap al\u0131n\u0131yor";
            pending_ai_label->set_text(msg + dots);
            return true;
        }, 400);

        auto adj = ai_scroll.get_vadjustment();
        adj->set_value(adj->get_upper());

        ai_input.set_text("");

        ai_waiting = true;
        ai_input.set_sensitive(false);
        btn_ai_send.set_sensitive(false);
        btn_ai_ddg.set_sensitive(false);
        btn_ai_image.set_sensitive(false);

        std::string text_copy = text;
        bool do_ddg = ddg_active;

        std::string img_b64 = ai_image_base64;
        std::string img_mime = ai_image_mime;
        ai_image_clear();

        std::thread([this, text_copy, do_ddg, img_b64, img_mime]() {
            std::string final_user_text = text_copy;
            std::string app_context;

            if (!img_b64.empty()) {
                final_user_text = text_copy.empty()
                    ? "Kullanıcı bir görsel gönderdi. Görseldeki soruyu çöz."
                    : "Kullanıcı bir görsel gönderdi: " + text_copy;
            }

            if (do_ddg && img_b64.empty()) {
                std::string ddg_raw = duckduckgo_search(text_copy);
                if (ddg_raw.empty() || ddg_raw.rfind("DuckDuckGo hatas\u0131", 0) == 0) {
                    final_user_text = "Kullanıcının sorusu: " + text_copy +
                        "\n\nNOT: DuckDuckGo araması başarısız oldu. "
                        "Kendi bilginle cevap ver.";
                } else {
                    std::string ddg_summary;
                    try {
                        auto j = json::parse(ddg_raw);
                        std::string abstract = j.value("Abstract", "");
                        std::string heading = j.value("Heading", "");
                        auto results = j["Results"];
                        auto topics = j["RelatedTopics"];

                        if (!heading.empty())
                            ddg_summary += "Başlık: " + heading + "\n";
                        if (!abstract.empty())
                            ddg_summary += "Özet: " + abstract + "\n";

                        if (results.is_array()) {
                            for (auto& r : results) {
                                if (r.contains("Text") && !r["Text"].empty())
                                    ddg_summary += "- " + r["Text"].get<std::string>() + "\n";
                            }
                        }
                        if (topics.is_array()) {
                            int added = 0;
                            for (auto& t : topics) {
                                if (added >= 10) break;
                                if (t.is_object() && t.contains("Text") && !t["Text"].empty()) {
                                    ddg_summary += "- " + t["Text"].get<std::string>() + "\n";
                                    added++;
                                } else if (t.is_object() && t.contains("Topics") && t["Topics"].is_array()) {
                                    for (auto& st : t["Topics"]) {
                                        if (added >= 10) break;
                                        if (st.contains("Text") && !st["Text"].empty()) {
                                            ddg_summary += "- " + st["Text"].get<std::string>() + "\n";
                                            added++;
                                        }
                                    }
                                }
                            }
                        }
                    } catch (const json::parse_error&) {
                        ddg_summary = "DuckDuckGo yanıtı ayrıştırılamadı.";
                    }
                    final_user_text =
                        "Kullanıcı şunu sordu: " + text_copy +
                        "\n\nDuckDuckGo arama sonuçları:\n" + ddg_summary +
                        "\n\nYukarıdaki arama sonuçlarına göre kullanıcının sorusunu "
                        "detaylıca yanıtla. Kaynak olarak arama sonuçlarını kullan, "
                        "kendi bilgini de ekleyerek zenginleştir. "
                        "Yanıtın en sonuna • DuckDuckGo Üzerinden Araştırıldı yaz. "
                        "Yanıtında maddeler halinde ve açıklayıcı ol.";
                }
            } else {
                std::string lower = text_copy;
                std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
                static const std::vector<std::string> keywords = {
                    "not", "ders", "görev", "gorev", "ödev", "odev",
                    "oku", "analiz", "program", "takvim", "çalışma", "calisma"
                };
                bool needs = false;
                for (auto& kw : keywords)
                    if (lower.find(kw) != std::string::npos) { needs = true; break; }
                if (needs) {
                    for (auto& n : course_notes) {
                        app_context += "- Ders: " + n.course + "\n";
                        if (!n.content.empty())
                            app_context += "  İçerik:\n" + n.content + "\n\n";
                    }
                }
            }

            std::string raw = call_ai(final_user_text, app_context, img_b64, img_mime);
            {
                std::lock_guard<std::mutex> lock(ai_mutex);
                pending_ai_response = raw;
            }
            ai_dispatcher.emit();
        }).detach();
    };

    ai_input.signal_activate().connect(send_message);
    btn_ai_send.signal_clicked().connect(send_message);

    // Sayfayı birleştir 
    page->append(*header_box);
    page->append(ai_scroll);
    page->append(ai_image_preview_box);
    page->append(*input_container);

    sw->set_child(*page);
    stack.add(*sw, "ai");
}

void MainWindow::ai_image_clear() {
    ai_image_path.clear();
    ai_image_base64.clear();
    ai_image_mime.clear();
    ai_image_preview_box.set_visible(false);
}

void MainWindow::show_ai_key_dialog() {
    auto* dialog = new Gtk::Dialog();
    dialog->set_title("Yapay Zeka Ayarları");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(540, 520);

    auto* content = dialog->get_content_area();
    content->set_margin(16);
    content->set_spacing(12);

    // Sağlayıcı seçimi
    auto* provider_lbl = Gtk::make_managed<Gtk::Label>("Sağlayıcı:");
    provider_lbl->set_halign(Gtk::Align::START);

    auto* provider_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    provider_combo->append("groq", "Groq Cloud");
    provider_combo->append("openrouter", "OpenRouter");
    provider_combo->append("gemini", "Google Gemini");
    provider_combo->append("ollama", "Ollama (Yerel)");
    std::string active_provider = ai_provider == AIProvider::GEMINI ? "gemini" : ai_provider == AIProvider::OPENROUTER ? "openrouter" : ai_provider == AIProvider::OLLAMA ? "ollama" : "groq";
    provider_combo->set_active_id(active_provider);

    content->append(*provider_lbl);
    content->append(*provider_combo);

    auto* sep = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
    sep->set_margin_top(8);
    sep->set_margin_bottom(8);
    content->append(*sep);

    // Groq Cloud bölümü
    auto* groq_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);

    auto* groq_header = Gtk::make_managed<Gtk::Label>("");
    groq_header->set_markup("<b>Groq Cloud</b>");
    groq_header->set_halign(Gtk::Align::START);

    auto* groq_key_lbl = Gtk::make_managed<Gtk::Label>(
        "API Anahtarı:\n(https://console.groq.com/keys adresinden alabilirsiniz)");
    groq_key_lbl->set_wrap(true);
    groq_key_lbl->set_halign(Gtk::Align::START);

    auto* groq_key_entry = Gtk::make_managed<Gtk::Entry>();
    groq_key_entry->set_placeholder_text("gsk_...");
    groq_key_entry->set_text(ai_api_key_groq);
    groq_key_entry->set_hexpand(true);
    groq_key_entry->set_visibility(false);

    auto* groq_key_toggle = Gtk::make_managed<Gtk::ToggleButton>();
    groq_key_toggle->set_icon_name("view-reveal-symbolic");
    groq_key_toggle->set_tooltip_text("Anahtarı göster/gizle");
    groq_key_toggle->add_css_class("password-toggle");
    groq_key_toggle->signal_toggled().connect([groq_key_entry, groq_key_toggle]() {
        groq_key_entry->set_visibility(groq_key_toggle->get_active());
        groq_key_toggle->set_icon_name(groq_key_toggle->get_active() ? "view-conceal-symbolic" : "view-reveal-symbolic");
    });

    auto* groq_key_hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    groq_key_hbox->append(*groq_key_entry);
    groq_key_hbox->append(*groq_key_toggle);

    auto* groq_model_lbl = Gtk::make_managed<Gtk::Label>("Model:");
    groq_model_lbl->set_halign(Gtk::Align::START);

    auto* groq_model_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    groq_model_combo->append("llama-3.1-8b-instant", "Llama 3.1 8B");
    groq_model_combo->append("llama-3.3-70b-versatile", "Llama 3.3 70B");
    groq_model_combo->append("meta-llama/llama-4-scout-17b-16e-instruct", "Llama 4 Scout 17B-16E");
    groq_model_combo->append("openai/gpt-oss-120b", "OpenAi GPT OSS 120B");
    groq_model_combo->append("openai/gpt-oss-20b", "OpenAi GPT OSS 20B");
    groq_model_combo->append("openai/gpt-oss-safeguard-20b", "OpenAİ GPT OSS SafeGuard 20B");
    groq_model_combo->append("qwen/qwen3-32b", "Qwen 3 32B");
    groq_model_combo->append("qwen/qwen3.6-27b", "Qwen 3.6 27B");
    groq_model_combo->append("allam-2-7b", "ALLaM 2 7B");
    groq_model_combo->append("groq-compound", "Groq Compound");
    groq_model_combo->append("groq/compound-mini", "Groq Compound Mini");

    
    groq_model_combo->set_active_id(ai_model_groq);
    groq_box->append(*groq_header);
    groq_box->append(*groq_key_lbl);
    groq_box->append(*groq_key_hbox);
    groq_box->append(*groq_model_lbl);
    groq_box->append(*groq_model_combo);

    // OpenRouter bölümü
    auto* or_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);

    auto* or_header = Gtk::make_managed<Gtk::Label>("");
    or_header->set_markup("<b>OpenRouter</b>");
    or_header->set_halign(Gtk::Align::START);

    auto* or_key_lbl = Gtk::make_managed<Gtk::Label>(
        "API Anahtarı:\n(https://openrouter.ai/keys adresinden alabilirsiniz)");
    or_key_lbl->set_wrap(true);
    or_key_lbl->set_halign(Gtk::Align::START);

    auto* or_key_entry = Gtk::make_managed<Gtk::Entry>();
    or_key_entry->set_placeholder_text("sk-or-...");
    or_key_entry->set_text(ai_api_key_openrouter);
    or_key_entry->set_hexpand(true);
    or_key_entry->set_visibility(false);

    auto* or_key_toggle = Gtk::make_managed<Gtk::ToggleButton>();
    or_key_toggle->set_icon_name("view-reveal-symbolic");
    or_key_toggle->set_tooltip_text("Anahtarı göster/gizle");
    or_key_toggle->add_css_class("password-toggle");
    or_key_toggle->signal_toggled().connect([or_key_entry, or_key_toggle]() {
        or_key_entry->set_visibility(or_key_toggle->get_active());
        or_key_toggle->set_icon_name(or_key_toggle->get_active() ? "view-conceal-symbolic" : "view-reveal-symbolic");
    });

    auto* or_key_hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    or_key_hbox->append(*or_key_entry);
    or_key_hbox->append(*or_key_toggle);

    auto* or_model_lbl = Gtk::make_managed<Gtk::Label>("Model:");
    or_model_lbl->set_halign(Gtk::Align::START);

    auto* or_model_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    // Google
    or_model_combo->append("google/gemma-4-31b-it", "Google Gemma 4 31B");
    or_model_combo->append("google/gemini-3.5-flash", "Gemini 3.5 Flash");
    or_model_combo->append("google/gemini-2.5-flash", "Gemini 2.5 Flash");
    or_model_combo->append("google/gemini-3.1-flash-lite", "Gemini 3.1 Flash Lite");
    or_model_combo->append("google/gemini-2.5-pro", "Gemini 2.5 Pro");
    or_model_combo->append("google/gemini-3.1-pro-preview", "Gemini 3.1 Pro");
    // Qwen
    or_model_combo->append("qwen/qwen3.6-flash", "Qwen 3.6 Flash");
    or_model_combo->append("qwen/qwen3-coder", "Qwen 3 Coder");
    or_model_combo->append("qwen/qwen3.7-max", "Qwen 3.7 Max");
    or_model_combo->append("qwen/qwen3-next-80b-a3b-instruct:free", "Qwen3 Next Free");
    or_model_combo->append("qwen/qwen3.6-flash", "Qwen 3.6 Flash");
    // Deepseek
    or_model_combo->append("deepseek/deepseek-r1", "Deepseek R1");
    or_model_combo->append("deepseek/deepseek-v4-pro", "Deepseek V4 Pro");
    or_model_combo->append("deepseek/deepseek-v4-flash:nitro", "Deepseek V4 Flash Nitro");
    // Mistral
    or_model_combo->append("mistralai/mistral-small-2603", "Mistral Small 4");
    // Kimi
    or_model_combo->append("moonshotai/kimi-k2-thinking", "Kimi K2");
    or_model_combo->append("moonshotai/kimi-k2.6", "Kimi K2.6");
    // Claude
    or_model_combo->append("anthropic/claude-haiku-4.5", "Claude Haiku 4.5");
    or_model_combo->append("anthropic/claude-sonnet-4.6", "Claude Sonnet 4.6");
    // GPT
    or_model_combo->append("openai/gpt-5-mini", "ChatGPT 5 Mini");
    or_model_combo->append("openai/gpt-4o-mini", "ChatGPT 4o Mini");
    or_model_combo->append("openai/gpt-4.1-mini", "ChatGPT 4.1 Mini");

    

    or_model_combo->set_active_id(ai_model_openrouter);

    or_box->append(*or_header);
    or_box->append(*or_key_lbl);
    or_box->append(*or_key_hbox);
    or_box->append(*or_model_lbl);
    or_box->append(*or_model_combo);

    // Google Gemini bölümü
    auto* gemini_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);

    auto* gemini_header = Gtk::make_managed<Gtk::Label>("");
    gemini_header->set_markup("<b>Google Gemini</b>");
    gemini_header->set_halign(Gtk::Align::START);

    auto* gemini_key_lbl = Gtk::make_managed<Gtk::Label>(
        "API Anahtarı:\n(https://aistudio.google.com/apikey adresinden alabilirsiniz)");
    gemini_key_lbl->set_wrap(true);
    gemini_key_lbl->set_halign(Gtk::Align::START);

    auto* gemini_key_entry = Gtk::make_managed<Gtk::Entry>();
    gemini_key_entry->set_placeholder_text("AI...");
    gemini_key_entry->set_text(ai_api_key_gemini);
    gemini_key_entry->set_hexpand(true);
    gemini_key_entry->set_visibility(false);

    auto* gemini_key_toggle = Gtk::make_managed<Gtk::ToggleButton>();
    gemini_key_toggle->set_icon_name("view-reveal-symbolic");
    gemini_key_toggle->set_tooltip_text("Anahtarı göster/gizle");
    gemini_key_toggle->add_css_class("password-toggle");
    gemini_key_toggle->signal_toggled().connect([gemini_key_entry, gemini_key_toggle]() {
        gemini_key_entry->set_visibility(gemini_key_toggle->get_active());
        gemini_key_toggle->set_icon_name(gemini_key_toggle->get_active() ? "view-conceal-symbolic" : "view-reveal-symbolic");
    });

    auto* gemini_key_hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
    gemini_key_hbox->append(*gemini_key_entry);
    gemini_key_hbox->append(*gemini_key_toggle);

    auto* gemini_model_lbl = Gtk::make_managed<Gtk::Label>("Model:");
    gemini_model_lbl->set_halign(Gtk::Align::START);

    auto* gemini_model_combo = Gtk::make_managed<Gtk::ComboBoxText>();  
        
    // Gemini Flash Lite Models
    gemini_model_combo->append("gemini-2.5-flash-lite", "Gemini 2.5 Flash Lite");
    gemini_model_combo->append("gemini-3.1-flash-lite", "Gemini 3.1 Flash Lite");
    // Gemini Flash Models
    gemini_model_combo->append("gemini-2.5-flash", "Gemini 2.5 Flash");
    gemini_model_combo->append("gemini-3.5-flash", "Gemini 3.5 Flash");
    // Gemini Pro Models
    gemini_model_combo->append("gemini-2.5-pro", "Gemini 2.5 Pro");
    gemini_model_combo->append("gemini-3-pro-preview", "Gemini 3 Pro");
    gemini_model_combo->append("gemini-3.1-pro-preview", "Gemini 3.1 Pro");
    

    gemini_model_combo->set_active_id(ai_model_gemini);

    gemini_box->append(*gemini_header);
    gemini_box->append(*gemini_key_lbl);
    gemini_box->append(*gemini_key_hbox);
    gemini_box->append(*gemini_model_lbl);
    gemini_box->append(*gemini_model_combo);

    // Ollama bölümü
    auto* ollama_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);

    auto* ollama_header = Gtk::make_managed<Gtk::Label>("");
    ollama_header->set_markup("<b>Ollama (Yerel)</b>");
    ollama_header->set_halign(Gtk::Align::START);

    auto* ollama_url_lbl = Gtk::make_managed<Gtk::Label>(
        "Ollama Sunucu Adresi:\n(http://localhost:11434 varsayılan)");
    ollama_url_lbl->set_wrap(true);
    ollama_url_lbl->set_halign(Gtk::Align::START);

    auto* ollama_url_entry = Gtk::make_managed<Gtk::Entry>();
    ollama_url_entry->set_placeholder_text("http://localhost:11434");
    ollama_url_entry->set_text(ai_ollama_url);
    ollama_url_entry->set_hexpand(true);

    auto* ollama_model_lbl = Gtk::make_managed<Gtk::Label>("Model:");
    ollama_model_lbl->set_halign(Gtk::Align::START);

    auto* ollama_model_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    ollama_model_combo->append("llama3.1:8b", "Ollama 3.1 8B");
    ollama_model_combo->append("medgemma1.5:4b", "Medgemma 1.5 4B");
    ollama_model_combo->append("granite4:3b", "Granite4 3B");
    ollama_model_combo->append("gemma3:latest", "Gemma-3 4B");
    ollama_model_combo->append("gemma3:27b-cloud", "Gemma-3 27B Cloud");
    ollama_model_combo->append("gemma4:31b-cloud", "Gemma-4 31B Cloud");
    ollama_model_combo->append("deepseek-v3.1:671b-cloud", "Deepseek v3.1 Cloud");
    ollama_model_combo->append("deepseek-v4-flash:cloud", "Deepseek v4 flash Cloud");
    ollama_model_combo->append("qwen3.5:397b-cloud", "Qwen3.5 Cloud");
    ollama_model_combo->append("minimax-m3:cloud", "Minimax-m3 Cloud");
    ollama_model_combo->append("kimi-k2.6:cloud", "Kimi k2.6 Cloud");


    ollama_model_combo->set_active_id(ai_model_ollama);

    ollama_box->append(*ollama_header);
    ollama_box->append(*ollama_url_lbl);
    ollama_box->append(*ollama_url_entry);
    ollama_box->append(*ollama_model_lbl);
    ollama_box->append(*ollama_model_combo);

    content->append(*groq_box);
    content->append(*or_box);
    content->append(*gemini_box);
    content->append(*ollama_box);

    // Sağlayıcı değişince görünürlüğü ayarla
    auto update_visibility = [groq_box, or_box, gemini_box, ollama_box, provider_combo]() {
        std::string id = provider_combo->get_active_id();
        groq_box->set_visible(id == "groq");
        or_box->set_visible(id == "openrouter");
        gemini_box->set_visible(id == "gemini");
        ollama_box->set_visible(id == "ollama");
    };
    provider_combo->signal_changed().connect(update_visibility);
    update_visibility();

    // Kullanıcının ayarları kaydetmesi veya iptal etmesi için butonlar
    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->set_halign(Gtk::Align::END);
    btn_box->set_margin_top(12);

    auto* btn_cancel = Gtk::make_managed<Gtk::Button>("İptal");
    auto* btn_save = Gtk::make_managed<Gtk::Button>("Kaydet");
    btn_save->add_css_class("suggested-action");

    btn_box->append(*btn_cancel);
    btn_box->append(*btn_save);

    content->append(*btn_box);

    btn_cancel->signal_clicked().connect([dialog]() {
        dialog->close();
    });

    btn_save->signal_clicked().connect([this, dialog, provider_combo, groq_key_entry, groq_model_combo, or_key_entry, or_model_combo, gemini_key_entry, gemini_model_combo, ollama_url_entry, ollama_model_combo]() {
        std::string prov = provider_combo->get_active_id();
        if (prov == "gemini") ai_provider = AIProvider::GEMINI;
        else if (prov == "openrouter") ai_provider = AIProvider::OPENROUTER;
        else if (prov == "ollama") ai_provider = AIProvider::OLLAMA;
        else ai_provider = AIProvider::GROQ;
        set_provider(ai_provider);

        // Groq ayarlarını kaydet
        std::string groq_key = groq_key_entry->get_text();
        if (!groq_key.empty()) {
            ai_api_key_groq = groq_key;
            set_groq_api_key(groq_key);
        }
        ai_model_groq = groq_model_combo->get_active_id();

        // OpenRouter ayarlarını kaydet
        std::string or_key = or_key_entry->get_text();
        if (!or_key.empty()) {
            ai_api_key_openrouter = or_key;
            set_openrouter_api_key(or_key);
        }
        ai_model_openrouter = or_model_combo->get_active_id();

        // Gemini ayarlarını kaydet
        std::string gemini_key = gemini_key_entry->get_text();
        if (!gemini_key.empty()) {
            ai_api_key_gemini = gemini_key;
            set_gemini_api_key(gemini_key);
        }
        ai_model_gemini = gemini_model_combo->get_active_id();

        // Ollama ayarlarını kaydet
        std::string ollama_url = ollama_url_entry->get_text();
        if (!ollama_url.empty()) {
            ai_ollama_url = ollama_url;
            set_ollama_url(ollama_url);
        }
        ai_model_ollama = ollama_model_combo->get_active_id();
        set_ollama_model(ai_model_ollama);

        // Aktif sağlayıcının modelini uygula
        if (ai_provider == AIProvider::GROQ) {
            set_model(ai_model_groq);
        } else if (ai_provider == AIProvider::OPENROUTER) {
            set_model(ai_model_openrouter);
        } else if (ai_provider == AIProvider::GEMINI) {
            set_model(ai_model_gemini);
        } else {
            set_model(ai_model_ollama);
        }

        save_data();

        // Model rozetini güncelle
        if (ai_model_badge) {
            std::string model_id = get_model();
            std::string display = model_id;
            auto slash = display.find('/');
            if (slash != std::string::npos) display = display.substr(slash + 1);
            for (auto& c : display) {
                if (c == '-' || c == '_') c = ' ';
            }
            if (!display.empty()) display[0] = std::toupper(display[0]);
            ai_model_badge->set_text(display);
        }

        dialog->close();
    });

    dialog->signal_close_request().connect([dialog]() -> bool {
        delete dialog;
        return true;
    }, false);

    dialog->present();
}
