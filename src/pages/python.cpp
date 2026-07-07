#include "mainwindow.h"
#include <webkit/webkit.h>
#include <md4c-html.h>
#include <sstream>

void MainWindow::setup_python() {

    // Python dersleri için kaydırılabilir bir alan oluşturuyoruz
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_hexpand(true);
    sw->set_vexpand(true);

    // İçerikleri düzenli bir şekilde yerleştirmek için dikey bir kutu kullanıyoruz
    auto* box = Gtk::make_managed<Gtk::Box>(
        Gtk::Orientation::VERTICAL,
        0
    );
    box->set_hexpand(true);
    box->set_vexpand(true);

    // Web tabanlı ders içeriğini göstermek için WebKit tarayıcı bileşenini kullanıyoruz
    python_webview = WEBKIT_WEB_VIEW(webkit_web_view_new());

    auto* web_widget =
        Gtk::manage(Glib::wrap(GTK_WIDGET(python_webview)));

    web_widget->set_hexpand(true);
    web_widget->set_vexpand(true);

    // Python derslerini HTML'e çevirip gösteriyoruz, tema değişince de yeniden yüklüyoruz
    reload_python();

    // WebKit bileşenini kutuya ekleyip kaydırılabilir alanla ana sayfaya yerleştiriyoruz
    box->append(*web_widget);
    sw->set_child(*box);

    stack.add(*sw, "python");

    // Sayfanın tüm alanı kaplaması için genişlik ve yükseklik ayarlarını yapıyoruz
    stack.set_hexpand(true);
    stack.set_vexpand(true);
}

void MainWindow::reload_python() {
   if (!python_webview) return;

    GError* error = nullptr;
    GBytes* bytes = g_resources_lookup_data(
        "/org/ogrenci/merkezi/docs/python/python.md",
        G_RESOURCE_LOOKUP_FLAGS_NONE,
        &error
    );

    std::string markdown;
    if (bytes) {
        gsize size;
        const char* data = (const char*)g_bytes_get_data(bytes, &size);
        markdown.assign(data, size);
        g_bytes_unref(bytes);
    } else {
        markdown = "# HATA\nMarkdown yüklenemedi";
    }

    std::string html;
    auto callback = [](
        const MD_CHAR* text,
        MD_SIZE size,
        void* userdata
    ) {
        auto* out = static_cast<std::string*>(userdata);
        out->append(text, size);
    };

    md_html(
        markdown.c_str(),
        markdown.size(),
        callback,
        &html,
        0,
        0
    );

    // Aydınlık ve karanlık mod için ayrı CSS stilleri hazırlıyoruz
    std::string css_light = R"(
html, body { margin:0; padding:0; height:100%; }
body{ background:transparent; color:#111111 !important; font-family: Inter, sans-serif; }
.md-container { background:#ffffff !important; padding:40px; line-height:1.8; max-width:1000px; margin:40px auto; border-radius:16px; box-shadow: 0 6px 18px rgba(16,24,40,0.06); }
h1 { color:#1f2937; } h2 { color:#374151; }
a { color:#1d4ed8 !important; text-decoration:none; }
a:hover { color:#2563eb !important; text-decoration:underline; }
pre{ background:#f3f4f6 !important; padding:16px; border-radius:12px; overflow:auto; border:1px solid #d1d5db !important; }
code{ color:#2563eb !important; font-family: monospace; }
hr{ border:none; height:1px; background:#e5e7eb; margin:40px 0; }
)";

    std::string css_dark = R"(
html, body { margin:0; padding:0; height:100%; }
body{ background:transparent; color:#e6eef6 !important; font-family: Inter, sans-serif; }
.md-container { background:#0b0c0d !important; padding:40px; line-height:1.8; max-width:1000px; margin:40px auto; border-radius:16px; box-shadow: 0 6px 18px rgba(2,6,23,0.6); }
h1 { color:#e6eef6 !important; } h2 { color:#cbd5e1 !important; }
a { color:#7dd3fc !important; text-decoration:none; }
a:hover { color:#38bdf8 !important; text-decoration:underline; }
pre{ background:#0f1720 !important; padding:16px; border-radius:12px; overflow:auto; border:1px solid #1f2933 !important; }
code{ color:#93c5fd !important; font-family: monospace; }
hr{ border:none; height:1px; background:#1f2933; margin:40px 0; }
)";

    std::string final_html = std::string("\n\n<!DOCTYPE html>\n<html>\n<head>\n<meta charset=\"utf-8\">\n<style>\n")
        + (dark_mode ? css_dark : css_light)
        + std::string("\n</style>\n</head>\n<body>\n\n<div class=\"md-container\">\n")
        + html
        + std::string("\n</div>\n</body>\n</html>\n");

    // WebView arka plan rengini temaya göre ayarlıyoruz, bazı platformlarda CSS geçmeyebiliyor
    GdkRGBA bg;
    gdk_rgba_parse(&bg, (dark_mode ? "#111213" : "#ffffff"));
    webkit_web_view_set_background_color(python_webview, &bg);

    webkit_web_view_load_html(
        python_webview,
        final_html.c_str(),
        nullptr
    );
}


