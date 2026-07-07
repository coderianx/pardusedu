#include "mainwindow.h"
#include "ai_client.h"
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <regex>
#include <thread>

using json = nlohmann::json;

namespace {
std::string hafta_basi_str() {
    auto t = std::time(nullptr);
    auto* tm = std::localtime(&t);
    int current_wday = tm->tm_wday;
    if (current_wday == 0) current_wday = 7;
    tm->tm_mday -= current_wday - 1;
    tm->tm_hour = 0; tm->tm_min = 0; tm->tm_sec = 0;
    std::mktime(tm);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}

std::string today_str_koc() {
    auto t = std::time(nullptr);
    auto* tm = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}

std::string md_to_pango(const std::string& md) {
    std::string result;
    result.reserve(md.size());
    bool in_bold = false, in_italic = false;
    for (size_t i = 0; i < md.size(); i++) {
        if (md[i] == '*' && i + 1 < md.size() && md[i+1] == '*') {
            result += in_bold ? "</b>" : "<b>";
            in_bold = !in_bold;
            i++;
        } else if (md[i] == '_' && i + 1 < md.size() && md[i+1] == '_') {
            result += in_bold ? "</b>" : "<b>";
            in_bold = !in_bold;
            i++;
        } else if (md[i] == '*' && !in_bold) {
            result += in_italic ? "</i>" : "<i>";
            in_italic = !in_italic;
        } else if (md[i] == '_' && !in_bold) {
            result += in_italic ? "</i>" : "<i>";
            in_italic = !in_italic;
        } else if (md[i] == '#' && (i == 0 || md[i-1] == '\n')) {
            while (i < md.size() && md[i] == '#') i++;
            if (i < md.size() && md[i] == ' ') i++;
            result += "<b>";
            while (i < md.size() && md[i] != '\n') {
                if (md[i] == '<') result += "&lt;";
                else if (md[i] == '>') result += "&gt;";
                else if (md[i] == '&') result += "&amp;";
                else result += md[i];
                i++;
            }
            result += "</b>";
            if (i < md.size()) result += md[i];
        } else {
            if (md[i] == '<') result += "&lt;";
            else if (md[i] == '>') result += "&gt;";
            else if (md[i] == '&') result += "&amp;";
            else result += md[i];
        }
    }
    if (in_bold) result += "</b>";
    if (in_italic) result += "</i>";
    return result;
}

std::string gun_adi(const std::string& tarih) {
    std::tm tm = {};
    std::istringstream ss(tarih);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) return "";
    std::mktime(&tm);
    static const char* gunler[] = {"Pazar", "Pazartesi", "Sal\u0131", "\u00c7ar\u015famba", "Per\u015fembe", "Cuma", "Cumartesi"};
    return gunler[tm.tm_wday];
}

std::string tarih_formatla(const std::string& tarih) {
    if (tarih.size() < 10) return tarih;
    return tarih.substr(8, 2) + "/" + tarih.substr(5, 2) + "/" + tarih.substr(2, 2);
}
}

void MainWindow::koc_soru_ekle(const std::string& ders, const std::string& konu, int d, int y, int b) {
    GunlukSoru gs;
    gs.tarih = today_str_koc();
    gs.ders = ders;
    gs.konu = konu;
    gs.dogru = d;
    gs.yanlis = y;
    gs.bos = b;
    gunluk_sorular.push_back(gs);
    koc_soru_kaydet();
}

void MainWindow::koc_soru_kaydet() {
    auto dir = get_data_dir();
    std::ofstream f(dir + "/gunluk_soru.dat");
    for (auto& gs : gunluk_sorular) {
        f << gs.tarih << "|" << koc_escape(gs.ders) << "|" << koc_escape(gs.konu) << "|"
          << gs.dogru << "|" << gs.yanlis << "|" << gs.bos << "\n";
    }
}

void MainWindow::koc_hedef_kaydet() {
    auto dir = get_data_dir();
    std::ofstream f(dir + "/ai_koc.dat");
    f << koc_escape(koc_hedef.hedef) << "|" << koc_hedef.olusturma_tarihi << "|"
      << koc_escape(koc_son_rapor) << "|" << koc_escape(koc_hedef.opsiyonel_alan) << "\n";
}

std::string MainWindow::koc_soru_logu_olustur() {
    std::string log;
    std::string hafta_baslangici = hafta_basi_str();
    for (auto& s : gunluk_sorular) {
        if (s.tarih < hafta_baslangici) continue;
        log += "- " + s.tarih + " | " + s.ders + " | " + s.konu +
               " | D:" + std::to_string(s.dogru) +
               " Y:" + std::to_string(s.yanlis) +
               " B:" + std::to_string(s.bos) + "\n";
    }
    return log;
}

std::string MainWindow::koc_rapor_prompt() {
    std::string soru_logu = koc_soru_logu_olustur();
    std::string ek = koc_hedef.opsiyonel_alan.empty()
        ? "" : "\nKullanıcının ek notu: " + koc_hedef.opsiyonel_alan + "\n";
    return
        "Sen bir eğitim koçusun. Kullanıcının hedefi: " + koc_hedef.hedef + "\n\n"
        "Son 7 günün soru çözüm verileri (D: doğru, Y: yanlış, B: boş):\n" + soru_logu + "\n" + ek + "\n"
        "Lütfen şu başlıklarda haftalık rapor ver:\n"
        "1. **Genel Değerlendirme** - Bu hafta genel durum (doğru, yanlış ve boş sayılarını mutlaka belirt)\n"
        "2. **En Çok Zorlanılan Konular** - En yüksek yanlış oranına sahip konular\n"
        "3. **Çalışma İstatistiği** - Toplam soru, doğru/yanlış/boş dağılımı (boş sayısını da ekle)\n"
        "4. **Hedefe Yönelik Öneriler** - Hedefe ulaşmak için neler yapılmalı\n"
        "5. **Önümüzdeki Hafta İçin Tavsiyeler** - Hangi konulara ağırlık verilmeli\n\n"
        "Raporu motive edici, samimi bir dille yaz. Kısa ve öz ol.";
}

void MainWindow::koc_haftalik_rapor() {
    std::string prompt = koc_rapor_prompt();
    koc_rapor_bekliyor = true;
    async_call_ai_json(prompt, [this](std::string response) {
        {
            std::lock_guard<std::mutex> lock(koc_ai_mutex);
            pending_koc_ai_response = response;
        }
        koc_ai_dispatcher.emit();
    });
}

void MainWindow::on_koc_ai_response() {
    std::lock_guard<std::mutex> lock(koc_ai_mutex);
    if (pending_koc_ai_response.empty()) return;
    koc_son_rapor = pending_koc_ai_response;
    koc_rapor_bekliyor = false;
    pending_koc_ai_response.clear();
    koc_hedef_kaydet();
    if (koc_rapor_lbl) {
        koc_rapor_lbl->set_markup(md_to_pango(koc_son_rapor));
    }
    if (pages_built.count("ai_koc")) {
        pages_built.erase("ai_koc");
    }
}

std::string MainWindow::koc_plan_prompt() {
    std::string soru_logu = koc_soru_logu_olustur();
    std::string ek;
    std::string gunluk_ders_talimati;
    if (!koc_hedef.opsiyonel_alan.empty()) {
        ek = "\nKullanıcının ek notu: " + koc_hedef.opsiyonel_alan + "\n";
        gunluk_ders_talimati =
            "Kullanıcının yukarıdaki ek notuna göre günlük ders sayısını belirle. "
            "Örneğin \"günde 3 ders\" yazıyorsa her gün için 3 farklı ders/konu ekle.\n";
    } else {
        gunluk_ders_talimati =
            "Her gün için en az 1-2 ders/konu ekle.\n";
    }
    return
        "Sen bir eğitim koçusun. Kullanıcının hedefi: " + koc_hedef.hedef + "\n\n"
        "Bu haftanın soru çözüm verileri:\n" + soru_logu + "\n" + ek + "\n"
        "Bu verilere göre SADECE JSON çıktısı ver, başka hiçbir şey yazma.\n"
        "JSON formatı:\n"
        "{\n"
        "  \"oncelikli_konular\": [\n"
        "    {\"ders\": \"...\", \"konu\": \"...\", \"yanlis\": N, \"seviye\": \"yüksek/orta/düşük\"}\n"
        "  ],\n"
        "  \"haftalik_program\": [\n"
        "    {\"gun\": \"Pazartesi\", \"ders\": \"Matematik\", \"konu\": \"...\", \"soru_sayisi\": 20},\n"
        "    {\"gun\": \"Pazartesi\", \"ders\": \"Fen\", \"konu\": \"...\", \"soru_sayisi\": 15},\n"
        "    {\"gun\": \"Sal\u0131\", \"ders\": \"T\u00fcrk\u00e7e\", \"konu\": \"...\", \"soru_sayisi\": 30}\n"
        "  ]\n"
        "}\n\n"
        "Gün adları: Pazartesi, Salı, Çarşamba, Perşembe, Cuma, Cumartesi, Pazar\n"
        "oncelikli_konular: en çok yanlış yapılan konuları yanlış sayısına göre sırala (en fazla 5).\n"
        "haftalik_program: Pazartesi'den Pazar'a kadar tüm 7 günü kapsamalı. "
        + gunluk_ders_talimati +
        "Kullanıcının zorlandığı konulara daha çok yer ver.";
}

void MainWindow::koc_plan_olustur() {
    std::string prompt = koc_plan_prompt();
    koc_plan_bekliyor = true;
    async_call_ai_json(prompt, [this](std::string response) {
        {
            std::lock_guard<std::mutex> lock(koc_plan_mutex);
            pending_koc_plan_response = response;
        }
        koc_plan_dispatcher.emit();
    });
}

void MainWindow::on_koc_plan_response() {
    std::lock_guard<std::mutex> lock(koc_plan_mutex);
    if (pending_koc_plan_response.empty()) return;
    std::string text = pending_koc_plan_response;

    // Strip markdown code fences if present
    {
        size_t start = text.find("```");
        if (start != std::string::npos) {
            size_t end = text.rfind("```");
            if (end > start) {
                start = text.find('\n', start);
                if (start != std::string::npos) {
                    text = text.substr(start + 1, end - start - 1);
                }
            }
        }
    }
    // Trim whitespace
    {
        auto is_not_space = [](unsigned char c) { return !std::isspace(c); };
        auto left = std::find_if(text.begin(), text.end(), is_not_space);
        auto right = std::find_if(text.rbegin(), text.rend(), is_not_space);
        text = (left < right.base())
                   ? std::string(left, right.base())
                   : std::string();
    }

    koc_plan_json = text;
    koc_plan_bekliyor = false;
    pending_koc_plan_response.clear();
    if (koc_plan_container) {
        while (auto* child = koc_plan_container->get_first_child())
            koc_plan_container->remove(*child);
        auto* new_table = koc_plan_tablosu();
        koc_plan_container->append(*new_table);
    }
    if (koc_plan_btn) {
        koc_plan_btn->set_sensitive(true);
        koc_plan_btn->set_label("Plan Olu\u015ftur");
    }
    if (pages_built.count("ai_koc")) {
        pages_built.erase("ai_koc");
    }
}

Gtk::Widget* MainWindow::koc_plan_tablosu() {
    auto* vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);

    if (koc_plan_json.empty()) {
        auto* lbl = Gtk::make_managed<Gtk::Label>("Hen\u00fcz plan olu\u015fturulmad\u0131.");
        lbl->add_css_class("koc-muted");
        lbl->set_halign(Gtk::Align::START);
        vbox->append(*lbl);
        return vbox;
    }

    json j;
    try {
        j = json::parse(koc_plan_json);
    } catch (const json::parse_error&) {
        auto* lbl = Gtk::make_managed<Gtk::Label>("");
        lbl->set_markup("<span foreground='#e74c3c'>Plan ayr\u0131\u015ft\u0131r\u0131lamad\u0131. AI yan\u0131t\u0131 ge\u00e7ersiz JSON i\u00e7eriyor.</span>");
        lbl->set_halign(Gtk::Align::START);
        vbox->append(*lbl);
        return vbox;
    }

    try {
    // ---- Öncelikli Konular ----
    if (j.contains("oncelikli_konular") && j["oncelikli_konular"].is_array()) {
        auto* baslik = Gtk::make_managed<Gtk::Label>("");
        baslik->set_markup("<b>\u00d6ncelikli Konular</b>");
        baslik->set_halign(Gtk::Align::START);
        vbox->append(*baslik);

        for (auto& item : j["oncelikli_konular"]) {
            std::string ders = item.value("ders", "");
            std::string konu = item.value("konu", "");
            int yanlis = item.value("yanlis", 0);
            std::string seviye = item.value("seviye", "");

            std::string renk = seviye == "yüksek" ? "#e74c3c" :
                               seviye == "orta" ? "#f39c12" : "#2ecc71";

            auto* row = Gtk::make_managed<Gtk::Label>();
            row->set_markup(
                "<b>" + Glib::Markup::escape_text(ders) + "</b>  \u00b7  " +
                Glib::Markup::escape_text(konu) + "  \u00b7  " +
                "<span foreground='" + renk + "'>" +
                Glib::Markup::escape_text(seviye) + "</span>  \u00b7  " +
                std::to_string(yanlis) + " yanl\u0131\u015f"
            );
            row->set_halign(Gtk::Align::START);
            row->set_ellipsize(Pango::EllipsizeMode::END);
            vbox->append(*row);
        }
    }

    // ---- Haftalık Program ----
    if (j.contains("haftalik_program") && j["haftalik_program"].is_array()) {
            auto* prog_baslik = Gtk::make_managed<Gtk::Label>("");
            prog_baslik->set_markup("<b>Haftal\u0131k \u00c7al\u0131\u015fma Program\u0131</b>");
            prog_baslik->set_halign(Gtk::Align::START);
            prog_baslik->set_margin_top(8);
            vbox->append(*prog_baslik);

            auto* grid = Gtk::make_managed<Gtk::Grid>();
            grid->set_column_spacing(16);
            grid->set_row_spacing(6);
            grid->set_margin_start(4);

            // Başlıklar
            auto* h_gun = Gtk::make_managed<Gtk::Label>("");
            h_gun->set_markup("<b>G\u00fcn</b>");
            h_gun->set_halign(Gtk::Align::START);
            auto* h_ders = Gtk::make_managed<Gtk::Label>("");
            h_ders->set_markup("<b>Ders</b>");
            h_ders->set_halign(Gtk::Align::START);
            auto* h_konu = Gtk::make_managed<Gtk::Label>("");
            h_konu->set_markup("<b>Konu</b>");
            h_konu->set_halign(Gtk::Align::START);
            auto* h_soru = Gtk::make_managed<Gtk::Label>("");
            h_soru->set_markup("<b>Soru</b>");
            h_soru->set_halign(Gtk::Align::START);

            grid->attach(*h_gun, 0, 0, 1, 1);
            grid->attach(*h_ders, 1, 0, 1, 1);
            grid->attach(*h_konu, 2, 0, 1, 1);
            grid->attach(*h_soru, 3, 0, 1, 1);

            int row_idx = 1;
            for (auto& item : j["haftalik_program"]) {
                std::string gun = item.value("gun", "");
                std::string ders = item.value("ders", "");
                std::string konu = item.value("konu", "");
                int soru = item.value("soru_sayisi", 0);

            auto* l_gun = Gtk::make_managed<Gtk::Label>(gun);
            l_gun->set_halign(Gtk::Align::START);
            auto* l_ders = Gtk::make_managed<Gtk::Label>(ders);
            l_ders->set_halign(Gtk::Align::START);
            l_ders->set_ellipsize(Pango::EllipsizeMode::END);
            l_ders->set_max_width_chars(16);
            auto* l_konu = Gtk::make_managed<Gtk::Label>(konu);
            l_konu->set_halign(Gtk::Align::START);
            l_konu->set_ellipsize(Pango::EllipsizeMode::END);
            l_konu->set_max_width_chars(24);
            auto* l_soru = Gtk::make_managed<Gtk::Label>(std::to_string(soru));
                l_soru->set_halign(Gtk::Align::START);

                grid->attach(*l_gun, 0, row_idx, 1, 1);
                grid->attach(*l_ders, 1, row_idx, 1, 1);
                grid->attach(*l_konu, 2, row_idx, 1, 1);
                grid->attach(*l_soru, 3, row_idx, 1, 1);
                row_idx++;
            }

            vbox->append(*grid);
        }
    } catch (const std::exception& e) {
        auto* err = Gtk::make_managed<Gtk::Label>("Plan ayr\u0131\u015ft\u0131r\u0131lamad\u0131: " +
            Glib::Markup::escape_text(e.what()));
        err->add_css_class("koc-muted");
        err->set_halign(Gtk::Align::START);
        err->set_wrap(true);
        err->set_max_width_chars(80);
        vbox->append(*err);
    }

    return vbox;
}

void MainWindow::koc_bugunku_listeyi_guncelle(Gtk::Box* liste) {
    while (auto* child = liste->get_first_child())
        liste->remove(*child);

    std::string yedi_once = hafta_basi_str();
    int idx = 0;
    for (auto& s : gunluk_sorular) {
        if (s.tarih < yedi_once) { idx++; continue; }
        auto* row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
        row->add_css_class("koc-liste-row");

        auto* tarih_lbl = Gtk::make_managed<Gtk::Label>();
        tarih_lbl->set_markup(
            "<b>" + Glib::Markup::escape_text(tarih_formatla(s.tarih)) + " - " +
            Glib::Markup::escape_text(gun_adi(s.tarih)) + "</b>"
        );
        tarih_lbl->set_halign(Gtk::Align::START);
        tarih_lbl->set_hexpand(true);

        auto* icerik_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);

        auto* lbl = Gtk::make_managed<Gtk::Label>();
        lbl->set_markup(
            Glib::Markup::escape_text(s.ders) + "  \u00b7  " +
            Glib::Markup::escape_text(s.konu) + "  \u00b7  " +
            "<span foreground='#2ecc71'>+" + std::to_string(s.dogru) + "</span>  " +
            "<span foreground='#e74c3c'>-" + std::to_string(s.yanlis) + "</span>  " +
            "<span foreground='#888888'>" + std::to_string(s.bos) + " bo\u015f</span>"
        );
        lbl->set_halign(Gtk::Align::START);
        lbl->set_hexpand(true);
        lbl->set_ellipsize(Pango::EllipsizeMode::END);

        auto* btn_sil = Gtk::make_managed<Gtk::Button>("\u00d7");
        btn_sil->add_css_class("koc-sil-btn");
        int idx_copy = idx;
        btn_sil->signal_clicked().connect([this, idx_copy, liste]() {
            if (idx_copy < (int)gunluk_sorular.size()) {
                gunluk_sorular.erase(gunluk_sorular.begin() + idx_copy);
                koc_soru_kaydet();
                koc_bugunku_listeyi_guncelle(liste);
            }
        });

        icerik_box->append(*lbl);
        icerik_box->append(*btn_sil);

        row->append(*tarih_lbl);
        row->append(*icerik_box);
        liste->append(*row);
        idx++;
    }
}

void MainWindow::koc_hata_analizini_doldur(Gtk::Box* liste) {
    std::string hafta_baslangici = hafta_basi_str();

    struct KonuVeri {
        int dogru = 0, yanlis = 0, bos = 0;
    };
    std::map<std::string, KonuVeri> konular;

    for (auto& s : gunluk_sorular) {
        if (s.tarih < hafta_baslangici) continue;
        std::string anahtar = s.ders + " / " + s.konu;
        konular[anahtar].dogru += s.dogru;
        konular[anahtar].yanlis += s.yanlis;
        konular[anahtar].bos += s.bos;
    }

    int toplam_d = 0, toplam_y = 0, toplam_b = 0;
    for (auto& [k, v] : konular) {
        toplam_d += v.dogru; toplam_y += v.yanlis; toplam_b += v.bos;
    }
    if (toplam_d + toplam_y + toplam_b > 0) {
        auto* ozet = Gtk::make_managed<Gtk::Label>();
        ozet->set_markup(
            "<b>Bu Hafta:</b>  "
            "<span foreground='#2ecc71'>+" + std::to_string(toplam_d) + "</span>  "
            "<span foreground='#e74c3c'>-" + std::to_string(toplam_y) + "</span>  "
            "<span foreground='#888888'>" + std::to_string(toplam_b) + " bo\u015f</span>  "
            "\u00b7  Toplam " + std::to_string(toplam_d + toplam_y + toplam_b) + " soru"
        );
        ozet->set_halign(Gtk::Align::START);
        ozet->set_margin_bottom(6);
        liste->append(*ozet);
    }

    std::vector<std::pair<std::string, KonuVeri>> sirali(konular.begin(), konular.end());
    std::sort(sirali.begin(), sirali.end(), [](auto& a, auto& b) {
        int a_top = a.second.dogru + a.second.yanlis + a.second.bos;
        int b_top = b.second.dogru + b.second.yanlis + b.second.bos;
        return a_top > b_top;
    });

    for (auto& [konu, v] : sirali) {
        auto* row = Gtk::make_managed<Gtk::Label>();
        std::string satir =
            "<b>" + Glib::Markup::escape_text(konu) + "</b>  \u00b7  " +
            "<span foreground='#2ecc71'>+" + std::to_string(v.dogru) + "</span>  ";
        if (v.yanlis > 0)
            satir += "<span foreground='#e74c3c'>-" + std::to_string(v.yanlis) + "</span>  ";
        if (v.bos > 0)
            satir += "<span foreground='#888888'>" + std::to_string(v.bos) + " bo\u015f</span>";
        row->set_markup(satir);
        row->set_halign(Gtk::Align::START);
        row->set_ellipsize(Pango::EllipsizeMode::END);
        liste->append(*row);
    }

    if (sirali.empty()) {
        auto* empty = Gtk::make_managed<Gtk::Label>("Bu hafta hen\u00fcz veri yok");
        empty->add_css_class("koc-muted");
        empty->set_halign(Gtk::Align::START);
        liste->append(*empty);
    }
}

void MainWindow::koc_hata_listesini_guncelle(Gtk::Box* liste) {
    while (auto* child = liste->get_first_child())
        liste->remove(*child);
    koc_hata_analizini_doldur(liste);
}

void MainWindow::koc_hedef_ekle() {
    auto* dialog = new Gtk::Dialog();
    dialog->set_title("Hedef Belirle");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(420, 220);
    dialog->signal_hide().connect([dialog]() { delete dialog; });

    auto* content = dialog->get_content_area();
    content->set_margin(20);
    content->set_spacing(14);

    auto* baslik = Gtk::make_managed<Gtk::Label>("");
    baslik->set_markup("<b>Hedefini Belirle</b>");
    baslik->set_halign(Gtk::Align::START);
    content->append(*baslik);

    auto* aciklama = Gtk::make_managed<Gtk::Label>(
        "AI ko\u00e7un bu hedefe g\u00f6re sana \u00f6zel \u00e7al\u0131\u015fma plan\u0131 \u00e7\u0131karacak.\n"
        "\u00d6rnek: \"LGS'den 460+ puan almak\", \"YKS'de ilk 10 bin\"");
    aciklama->set_wrap(true);
    aciklama->set_halign(Gtk::Align::START);
    aciklama->add_css_class("koc-muted");
    content->append(*aciklama);

    auto* entry = Gtk::make_managed<Gtk::Entry>();
    entry->set_placeholder_text("Hedefini yaz...");
    entry->set_text(koc_hedef.hedef);
    entry->set_hexpand(true);
    entry->add_css_class("koc-entry");
    content->append(*entry);

    auto* ops_entry = Gtk::make_managed<Gtk::Entry>();
    ops_entry->set_placeholder_text("Opsiyonel: \u00f6rn: Haftal\u0131k 500 soru matematik fen a\u011f\u0131rl\u0131kl\u0131");
    ops_entry->set_text(koc_hedef.opsiyonel_alan);
    ops_entry->set_hexpand(true);
    ops_entry->add_css_class("koc-entry");
    content->append(*ops_entry);

    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->set_halign(Gtk::Align::END);
    btn_box->set_margin_top(8);

    auto* btn_iptal = Gtk::make_managed<Gtk::Button>("İptal");
    auto* btn_kaydet = Gtk::make_managed<Gtk::Button>("Kaydet");
    btn_kaydet->add_css_class("suggested-action");

    btn_box->append(*btn_iptal);
    btn_box->append(*btn_kaydet);
    content->append(*btn_box);

    btn_iptal->signal_clicked().connect([dialog]() { dialog->close(); });
    btn_kaydet->signal_clicked().connect([this, dialog, entry, ops_entry]() {
        std::string h = entry->get_text();
        auto is_space = [](unsigned char c) { return std::isspace(c); };
        auto left = std::find_if_not(h.begin(), h.end(), is_space);
        auto right = std::find_if_not(h.rbegin(), h.rend(), is_space);
        h = (left < right.base()) ? std::string(left, right.base()) : std::string();
        if (!h.empty()) {
            koc_hedef.hedef = h;
            koc_hedef.olusturma_tarihi = today_str_koc();
            koc_hedef.opsiyonel_alan = ops_entry->get_text();
            koc_hedef_kaydet();
            if (pages_built.count("ai_koc")) {
                pages_built.erase("ai_koc");
            }
        }
        dialog->close();
    });

    dialog->show();
}

void MainWindow::setup_ai_koc() {
    if (!koc_dispatchers_connected) {
        koc_ai_dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_koc_ai_response));
        koc_plan_dispatcher.connect(sigc::mem_fun(*this, &MainWindow::on_koc_plan_response));
        koc_dispatchers_connected = true;
    }

    koc_rapor_lbl = nullptr;
    koc_plan_container = nullptr;
    koc_plan_btn = nullptr;

    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);
    sw->set_hexpand(true);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
    box->set_halign(Gtk::Align::FILL);
    box->set_hexpand(true);
    box->set_margin_start(24);
    box->set_margin_end(24);
    box->set_margin_top(20);
    box->set_margin_bottom(20);
    box->set_spacing(20);

    // ---- HEDEF BANNER ----
    auto* hedef_card = Gtk::make_managed<Gtk::Frame>();
    hedef_card->add_css_class("koc-hedef-card");
    hedef_card->set_hexpand(true);

    auto* hedef_ic = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 16);
    hedef_ic->set_margin_start(20);
    hedef_ic->set_margin_end(20);
    hedef_ic->set_margin_top(16);
    hedef_ic->set_margin_bottom(16);

    auto* hedef_emoji = Gtk::make_managed<Gtk::Label>("\U0001F3AF");
    hedef_emoji->add_css_class("koc-hedef-emoji");

    auto* hedef_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    hedef_vbox->set_hexpand(true);

    auto* hedef_lbl = Gtk::make_managed<Gtk::Label>();
    if (koc_hedef.hedef.empty()) {
        hedef_lbl->set_markup("<span size='large' weight='bold'>Hen\u00fcz bir hedef belirlemedin</span>");
    } else {
        hedef_lbl->set_markup("<span size='large' weight='bold'>\U0001F3AF " +
            Glib::Markup::escape_text(koc_hedef.hedef) + "</span>");
    }
    hedef_lbl->set_halign(Gtk::Align::START);
    hedef_lbl->set_ellipsize(Pango::EllipsizeMode::END);
    hedef_lbl->set_max_width_chars(50);
    hedef_vbox->append(*hedef_lbl);

    auto* hedef_alt = Gtk::make_managed<Gtk::Label>("AI Ko\u00e7un sana \u00f6zel \u00e7al\u0131\u015fma plan\u0131 haz\u0131rlayacak");
    hedef_alt->add_css_class("koc-hedef-alt");
    hedef_alt->set_halign(Gtk::Align::START);
    hedef_vbox->append(*hedef_alt);


    auto* btn_hedef = Gtk::make_managed<Gtk::Button>("Hedef Belirle");
    btn_hedef->add_css_class("koc-btn");
    btn_hedef->signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::koc_hedef_ekle));

    hedef_ic->append(*hedef_emoji);
    hedef_ic->append(*hedef_vbox);
    hedef_ic->append(*btn_hedef);
    hedef_card->set_child(*hedef_ic);
    box->append(*hedef_card);

    auto* soru_card = Gtk::make_managed<Gtk::Frame>();
    soru_card->add_css_class("card");

    auto* soru_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 14);
    soru_vbox->set_margin_start(20);
    soru_vbox->set_margin_end(20);
    soru_vbox->set_margin_top(20);
    soru_vbox->set_margin_bottom(20);

    auto* soru_baslik = Gtk::make_managed<Gtk::Label>("");
    soru_baslik->set_markup("<b>Bug\u00fcn Ka\u00e7 Soru \u00c7\u00f6zd\u00fcn?</b>");
    soru_baslik->set_halign(Gtk::Align::START);
    soru_baslik->add_css_class("koc-section-title");
    soru_vbox->append(*soru_baslik);

    auto* ust_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 12);

    auto* ders_combo = Gtk::make_managed<Gtk::ComboBoxText>();
    ders_combo->append("Matematik");
    ders_combo->append("Türkçe");
    ders_combo->append("Paragraf");
    ders_combo->append("Fen Bilimleri");
    ders_combo->append("Sosyal Bilgiler");
    ders_combo->append("İngilizce");

    ders_combo->append("Fizik");
    ders_combo->append("Kimya");
    ders_combo->append("Biyoloji");
    ders_combo->append("Tarih");
    ders_combo->append("Coğrafya");
    ders_combo->append("Felsefe");
    ders_combo->append("Din Kültürü ve Ahlak Bilgisi");

    ders_combo->append("Geometri");
    ders_combo->append("Edebiyat");
    ders_combo->append("Dil ve Anlatım");

    ders_combo->append("T.C. İnkılap Tarihi ve Atatürkçülük");

    ders_combo->append("Almanca");
    ders_combo->append("Fransızca");
    ders_combo->append("Arapça");

    ders_combo->append("Mantık");
    
    ders_combo->set_active(0);
    ders_combo->set_hexpand(true);
    ders_combo->add_css_class("koc-combo");

    auto* konu_entry = Gtk::make_managed<Gtk::Entry>();
    konu_entry->set_placeholder_text("Konu (\u00f6rn: \u00dcsl\u00fc \u0130fadeler)");
    konu_entry->set_hexpand(true);
    konu_entry->add_css_class("koc-entry");

    ust_row->append(*ders_combo);
    ust_row->append(*konu_entry);
    soru_vbox->append(*ust_row);

    auto* alt_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    alt_row->set_halign(Gtk::Align::START);

    auto make_spin = [](int val) {
        auto* s = Gtk::make_managed<Gtk::SpinButton>();
        s->set_range(0, 999);
        s->set_value(val);
        s->set_increments(1, 5);
        s->set_numeric(true);
        s->set_max_width_chars(4);
        s->add_css_class("koc-spin");
        return s;
    };

    auto* spin_dogru = make_spin(0);
    auto* spin_yanlis = make_spin(0);
    auto* spin_bos = make_spin(0);

    auto make_label = [](const std::string& text, const std::string& css) {
        auto* lbl = Gtk::make_managed<Gtk::Label>(text);
        lbl->add_css_class(css);
        return lbl;
    };

    alt_row->append(*make_label("Do\u011fru", "koc-dogru"));
    alt_row->append(*spin_dogru);
    alt_row->append(*make_label("Yanl\u0131\u015f", "koc-yanlis"));
    alt_row->append(*spin_yanlis);
    alt_row->append(*make_label("Bo\u015f", "koc-bos"));
    alt_row->append(*spin_bos);

    auto* btn_soru_ekle = Gtk::make_managed<Gtk::Button>("+ Ekle");
    btn_soru_ekle->add_css_class("koc-btn-ekle");

    alt_row->append(*btn_soru_ekle);
    soru_vbox->append(*alt_row);

    auto* bugun_liste = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);
    bugun_liste->set_margin_top(8);

    auto* liste_baslik = Gtk::make_managed<Gtk::Label>("");
    liste_baslik->set_markup("<small>\u00c7\u00f6z\u00fclen/Eklenen Sorular:</small>");
    liste_baslik->set_halign(Gtk::Align::START);
    liste_baslik->add_css_class("koc-muted");
    soru_vbox->append(*liste_baslik);
    soru_vbox->append(*bugun_liste);

    soru_card->set_child(*soru_vbox);
    box->append(*soru_card);

    // Populate today's list
    koc_bugunku_listeyi_guncelle(bugun_liste);

    // ---- HAFTALIK RAPOR KARTI ----
    auto* rapor_card = Gtk::make_managed<Gtk::Frame>();
    rapor_card->add_css_class("card");

    auto* rapor_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    rapor_vbox->set_margin_start(20);
    rapor_vbox->set_margin_end(20);
    rapor_vbox->set_margin_top(20);
    rapor_vbox->set_margin_bottom(20);

    auto* rapor_baslik = Gtk::make_managed<Gtk::Label>("");
    rapor_baslik->set_markup("<b>Haftal\u0131k Rapor</b>");
    rapor_baslik->set_halign(Gtk::Align::START);
    rapor_vbox->append(*rapor_baslik);

    auto* hata_liste = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);
    koc_hata_analizini_doldur(hata_liste);
    rapor_vbox->append(*hata_liste);

    btn_soru_ekle->signal_clicked().connect([this, ders_combo, konu_entry, spin_dogru, spin_yanlis, spin_bos, bugun_liste, hata_liste]() {
        std::string ders = ders_combo->get_active_text();
        std::string konu = konu_entry->get_text();
        int d = (int)spin_dogru->get_value();
        int y = (int)spin_yanlis->get_value();
        int b = (int)spin_bos->get_value();
        if (konu.empty() || (d == 0 && y == 0 && b == 0)) return;

        koc_soru_ekle(ders, konu, d, y, b);
        konu_entry->set_text("");
        spin_dogru->set_value(0);
        spin_yanlis->set_value(0);
        spin_bos->set_value(0);
        koc_bugunku_listeyi_guncelle(bugun_liste);
        koc_hata_listesini_guncelle(hata_liste);
    });

    auto* ai_rapor_lbl = Gtk::make_managed<Gtk::Label>();
    ai_rapor_lbl->set_wrap(true);
    ai_rapor_lbl->set_wrap_mode(Pango::WrapMode::WORD);
    ai_rapor_lbl->set_max_width_chars(80);
    ai_rapor_lbl->set_halign(Gtk::Align::START);
    ai_rapor_lbl->set_selectable(true);

    if (!koc_son_rapor.empty()) {
        ai_rapor_lbl->set_markup(md_to_pango(koc_son_rapor));
    } else {
        ai_rapor_lbl->set_text("Hen\u00fcz haftal\u0131k rapor olu\u015fturulmad\u0131.\n"
            "Pazar g\u00fcn\u00fc veya yeterli veri birikince \"Rapor Olu\u015ftur\" butonuna bas.");
    }
    rapor_vbox->append(*ai_rapor_lbl);

    auto* btn_rapor = Gtk::make_managed<Gtk::Button>("Rapor Olu\u015ftur");
    btn_rapor->add_css_class("koc-btn-rapor");
    btn_rapor->signal_clicked().connect([this, ai_rapor_lbl]() {
        if (koc_rapor_bekliyor) return;
        ai_rapor_lbl->set_text("Rapor olu\u015fturuluyor...");
        koc_rapor_lbl = ai_rapor_lbl;
        koc_haftalik_rapor();
    });
    rapor_vbox->append(*btn_rapor);

    rapor_card->set_child(*rapor_vbox);
    box->append(*rapor_card);

    // ---- HAFTALIK PLAN KARTI ----
    auto* plan_kart = Gtk::make_managed<Gtk::Frame>();
    plan_kart->add_css_class("card");

    auto* plan_vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    plan_vbox->set_margin_start(20);
    plan_vbox->set_margin_end(20);
    plan_vbox->set_margin_top(20);
    plan_vbox->set_margin_bottom(20);

    auto* plan_baslik = Gtk::make_managed<Gtk::Label>("");
    plan_baslik->set_markup("<b>Haftal\u0131k \u00c7al\u0131\u015fma Plan\u0131</b>");
    plan_baslik->set_halign(Gtk::Align::START);
    plan_vbox->append(*plan_baslik);

    auto* plan_ic = koc_plan_tablosu();
    plan_vbox->append(*plan_ic);

    auto* btn_plan = Gtk::make_managed<Gtk::Button>("Plan Olu\u015ftur");
    btn_plan->add_css_class("koc-btn-rapor");
    koc_plan_btn = btn_plan;
    btn_plan->signal_clicked().connect([this, plan_ic, btn_plan]() {
        if (koc_plan_bekliyor) return;
        btn_plan->set_sensitive(false);
        btn_plan->set_label("Olu\u015fturuluyor...");
        koc_plan_container = static_cast<Gtk::Box*>(plan_ic);
        koc_plan_olustur();
    });
    plan_vbox->append(*btn_plan);

    plan_kart->set_child(*plan_vbox);
    box->append(*plan_kart);

    sw->set_child(*box);
    stack.add(*sw, "ai_koc");
}
