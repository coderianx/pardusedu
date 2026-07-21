#include "mainwindow.h"
#include <nlohmann/json.hpp>
#include <webkit/webkit.h>
#include <md4c-html.h>
#include <qrencode.h>
#include <curl/curl.h>
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

void MainWindow::setup_notes() {
    auto* paned = Gtk::make_managed<Gtk::Paned>(Gtk::Orientation::HORIZONTAL);
    paned->set_position(200);
    paned->set_resize_start_child(false);

    auto* left_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);
    left_box->set_margin_start(12);
    left_box->set_margin_end(12);
    left_box->set_margin_top(12);
    left_box->set_margin_bottom(12);

    auto* title = Gtk::make_managed<Gtk::Label>("Dersler");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::START);
    left_box->append(*title);

    course_list.set_selection_mode(Gtk::SelectionMode::SINGLE);
    course_list.signal_row_selected().connect(sigc::mem_fun(*this, &MainWindow::on_course_select));

    auto* course_sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    course_sw->set_child(course_list);
    course_sw->set_vexpand(true);
    left_box->append(*course_sw);

    auto* course_btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
    btn_add_course.add_css_class("action-btn");
    btn_add_course.set_hexpand(true);
    btn_add_course.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_add_course));
    course_btn_box->append(btn_add_course);

    btn_add_link_note.add_css_class("action-btn");
    btn_add_link_note.set_tooltip_text("Link notu ekle");
    btn_add_link_note.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_add_link_note));
    course_btn_box->append(btn_add_link_note);

    left_box->append(*course_btn_box);

    paned->set_start_child(*left_box);

    note_editor_box.set_margin_start(16);
    note_editor_box.set_margin_end(16);
    note_editor_box.set_margin_top(16);
    note_editor_box.set_margin_bottom(16);

    note_title.add_css_class("page-title");
    note_title.set_text("Bir ders seçin");
    note_editor_box.append(note_title);

    auto* sep = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::HORIZONTAL);
    sep->set_margin_top(4);
    sep->set_margin_bottom(8);
    note_editor_box.append(*sep);

    auto nbuf = note_view.get_buffer();
    auto ttable = nbuf->get_tag_table();

    tag_bold = Gtk::TextTag::create("bold");
    tag_bold->property_weight() = Pango::Weight::BOLD;
    ttable->add(tag_bold);

    tag_italic = Gtk::TextTag::create("italic");
    tag_italic->property_style() = Pango::Style::ITALIC;
    ttable->add(tag_italic);

    tag_underline = Gtk::TextTag::create("underline");
    tag_underline->property_underline() = Pango::Underline::SINGLE;
    ttable->add(tag_underline);

    tag_strikethrough = Gtk::TextTag::create("strikethrough");
    tag_strikethrough->property_strikethrough() = true;
    ttable->add(tag_strikethrough);

    tag_h1 = Gtk::TextTag::create("h1");
    tag_h1->property_scale() = 2.0;
    tag_h1->property_weight() = Pango::Weight::BOLD;
    ttable->add(tag_h1);

    tag_h2 = Gtk::TextTag::create("h2");
    tag_h2->property_scale() = 1.5;
    tag_h2->property_weight() = Pango::Weight::BOLD;
    ttable->add(tag_h2);

    tag_h3 = Gtk::TextTag::create("h3");
    tag_h3->property_scale() = 1.2;
    tag_h3->property_weight() = Pango::Weight::BOLD;
    ttable->add(tag_h3);

    tag_font_inc = make_font_tag("font_inc", 1.3);
    tag_font_dec = make_font_tag("font_dec", 0.8);

    format_toolbar.set_halign(Gtk::Align::START);
    format_toolbar.add_css_class("note-toolbar-glass");

    auto make_tool_toggle = [this](Gtk::ToggleButton& btn, const std::string& markup, const std::string& tip) {
        auto* lbl = Gtk::make_managed<Gtk::Label>();
        lbl->set_markup(markup);
        btn.set_child(*lbl);
        btn.add_css_class("note-tool-btn");
        btn.set_tooltip_text(tip);
        btn.set_has_frame(false);
        format_toolbar.append(btn);
    };

    auto make_tool_btn = [this](Gtk::Button& btn, const std::string& markup, const std::string& tip) {
        auto* lbl = Gtk::make_managed<Gtk::Label>();
        lbl->set_markup(markup);
        btn.set_child(*lbl);
        btn.add_css_class("note-tool-btn");
        btn.set_tooltip_text(tip);
        btn.set_has_frame(false);
        format_toolbar.append(btn);
    };

    make_tool_toggle(btn_bold, "<b>B</b>", "Kalın (Ctrl+B)");
    make_tool_toggle(btn_italic, "<i>İ</i>", "İtalik (Ctrl+I)");
    make_tool_toggle(btn_underline, "<u>U</u>", "Altı Çizili (Ctrl+U)");
    make_tool_toggle(btn_strikethrough, "<s>S</s>", "Üstü Çizili (Ctrl+S)");

    auto* sep1 = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::VERTICAL);
    sep1->add_css_class("note-tool-sep");
    format_toolbar.append(*sep1);

    make_tool_toggle(btn_h1, "<span size='x-large' weight='bold'>H1</span>", "Başlık 1");
    make_tool_toggle(btn_h2, "<span size='large' weight='bold'>H2</span>", "Başlık 2");
    make_tool_toggle(btn_h3, "<span size='medium' weight='bold'>H3</span>", "Başlık 3");

    auto* sep_math = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::VERTICAL);
    sep_math->add_css_class("note-tool-sep");
    format_toolbar.append(*sep_math);

    make_tool_btn(btn_math, "<span font_family='monospace' weight='bold'>∑</span>", "Matematik Sembolleri");

    auto* math_dialog = Gtk::make_managed<Gtk::Dialog>();
    math_dialog->set_title("Matematik Sembolleri");
    math_dialog->set_transient_for(*this);
    math_dialog->set_modal(true);
    math_dialog->set_default_size(540, 480);
    math_dialog->set_hide_on_close(true);

    auto* content = math_dialog->get_content_area();
    content->set_spacing(0);

    auto* math_sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    math_sw->set_vexpand(true);
    math_sw->set_policy(Gtk::PolicyType::NEVER, Gtk::PolicyType::AUTOMATIC);

    auto* math_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 6);
    math_box->set_margin(12);

    auto add_math_section = [this, math_box, math_dialog](const std::string& title, const std::vector<std::pair<std::string, std::string>>& symbols) {
        auto* lbl = Gtk::make_managed<Gtk::Label>();
        lbl->set_markup("<b>" + title + "</b>");
        lbl->set_halign(Gtk::Align::START);
        lbl->set_margin_top(4);
        lbl->set_margin_bottom(2);
        math_box->append(*lbl);

        auto* flow = Gtk::make_managed<Gtk::FlowBox>();
        flow->set_max_children_per_line(12);
        flow->set_min_children_per_line(1);
        flow->set_row_spacing(4);
        flow->set_column_spacing(4);
        flow->set_homogeneous(true);

        for (auto& [sym, tip] : symbols) {
            auto* b = Gtk::make_managed<Gtk::Button>(sym);
            b->add_css_class("math-sym-btn");
            b->set_tooltip_text(tip);
            b->signal_clicked().connect([this, sym, math_dialog]() {
                note_view.get_buffer()->insert_at_cursor(sym);
                math_dialog->hide();
            });
            flow->append(*b);
        }
        math_box->append(*flow);
    };

    add_math_section("Yunan Harfleri", {
        {"α", "alfa"}, {"β", "beta"}, {"γ", "gama"}, {"δ", "delta"}, {"ε", "epsilon"},
        {"θ", "teta"}, {"λ", "lambda"}, {"μ", "mu"}, {"π", "pi"}, {"ρ", "ro"},
        {"σ", "sigma"}, {"τ", "tau"}, {"φ", "fi"}, {"ψ", "psi"}, {"ω", "omega"},
        {"Δ", "Delta"}, {"Σ", "Sigma"}, {"Ω", "Omega"}
    });

    add_math_section("İşlemciler", {
        {"+", "toplama"}, {"−", "çıkarma"}, {"×", "çarpma"}, {"÷", "bölme"},
        {"±", "artı/eksi"}, {"∓", "eksi/artı"}, {"⋅", "nokta çarpım"}, {"∗", "yıldız çarpım"},
        {"⊕", "direkt toplam"}, {"⊗", "tensör çarpım"}
    });

    add_math_section("İlişki", {
        {"=", "eşit"}, {"≠", "eşit değil"}, {"≈", "yaklaşık"}, {"≡", "denk"},
        {"<", "küçük"}, {">", "büyük"}, {"≤", "küçük eşit"}, {"≥", "büyük eşit"},
        {"≅", "uyumlu"}, {"∼", "orantılı"}, {"∝", "ters"}, {"≃", "asimptotik"}
    });

    add_math_section("Oklar", {
        {"→", "sağ ok"}, {"←", "sol ok"}, {"↑", "yukarı"}, {"↓", "aşağı"},
        {"⇒", "çift sağ"}, {"⇐", "çift sol"}, {"⇔", "çift yön"}, {"↔", "çift ok"},
        {"⟹", "uzun çift"}, {"⟶", "uzun ok"}
    });

    add_math_section("Kalkülüs", {
        {"∫", "integral"}, {"∬", "çift int"}, {"∮", "çember int"},
        {"∂", "kısmi türev"}, {"∇", "nabla"}, {"∞", "sonsuz"},
        {"∑", "toplam"}, {"∏", "çarpım"}
    });

    add_math_section("Üst/Alt Simge", {
        {"²", "karesi"}, {"³", "küpü"}, {"¹", "birinci"},
        {"⁰", "üzeri 0"}, {"ⁿ", "üzeri n"},
        {"₀", "alt 0"}, {"₁", "alt 1"}, {"₂", "alt 2"}, {"₃", "alt 3"},
        {"ᵢ", "alt i"}, {"ⱼ", "alt j"}, {"ₙ", "alt n"}
    });

    add_math_section("Mantık & Küme", {
        {"∀", "her"}, {"∃", "vardır"}, {"∄", "yoktur"},
        {"∧", "ve"}, {"∨", "veya"}, {"¬", "değil"},
        {"∈", "elemanı"}, {"∉", "elemanı değil"},
        {"⊂", "alt küme"}, {"⊆", "alt küme (=)"}, {"⊃", "üst küme"},
        {"∪", "birleşim"}, {"∩", "kesişim"}, {"∅", "boş küme"}
    });

    add_math_section("Kök & Kesir", {
        {"√", "karekök"}, {"∛", "küpkök"}, {"∜", "dördüncü"},
        {"∕", "kesir çizgisi"}, {"▔", "üst çizgi"}, {"⎯", "uzun çizgi"}
    });

    add_math_section("Hazır Kesirler", {
        {"½", "bir bölü iki"}, {"⅓", "bir bölü üç"}, {"⅔", "iki bölü üç"},
        {"¼", "bir bölü dört"}, {"¾", "üç bölü dört"},
        {"⅕", "bir bölü beş"}, {"⅖", "iki bölü beş"}, {"⅗", "üç bölü beş"}, {"⅘", "dört bölü beş"},
        {"⅙", "bir bölü altı"}, {"⅚", "beş bölü altı"},
        {"⅛", "bir bölü sekiz"}, {"⅜", "üç bölü sekiz"}, {"⅝", "beş bölü sekiz"}, {"⅞", "yedi bölü sekiz"}
    });

    auto* frac_lbl = Gtk::make_managed<Gtk::Label>();
    frac_lbl->set_markup("<b>Kesir Oluştur</b>");
    frac_lbl->set_halign(Gtk::Align::START);
    frac_lbl->set_margin_top(8);
    math_box->append(*frac_lbl);

    auto* frac_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);
    frac_box->set_halign(Gtk::Align::START);

    auto* pay_entry = Gtk::make_managed<Gtk::Entry>();
    pay_entry->set_placeholder_text("Pay");
    pay_entry->set_max_length(4);
    pay_entry->set_width_chars(4);
    pay_entry->set_alignment(Gtk::Align::CENTER);

    auto* frac_line = Gtk::make_managed<Gtk::Label>();
    frac_line->set_markup("<span font_family='monospace' size='xx-large'>─</span>");
    frac_line->set_halign(Gtk::Align::CENTER);

    auto* payda_entry = Gtk::make_managed<Gtk::Entry>();
    payda_entry->set_placeholder_text("Payda");
    payda_entry->set_max_length(4);
    payda_entry->set_width_chars(4);
    payda_entry->set_alignment(Gtk::Align::CENTER);

    auto* frac_ekle = Gtk::make_managed<Gtk::Button>("Ekle");
    frac_ekle->add_css_class("math-sym-btn");
    auto* frac_vertical_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 2);
    frac_vertical_box->set_halign(Gtk::Align::CENTER);
    frac_vertical_box->append(*pay_entry);
    frac_vertical_box->append(*frac_line);
    frac_vertical_box->append(*payda_entry);

    frac_box->append(*frac_vertical_box);
    frac_box->append(*frac_ekle);

    auto digit_to_super = [](char c) -> std::string {
        switch (c) {
            case '0': return "\u2070"; case '1': return "\u00B9"; case '2': return "\u00B2";
            case '3': return "\u00B3"; case '4': return "\u2074"; case '5': return "\u2075";
            case '6': return "\u2076"; case '7': return "\u2077"; case '8': return "\u2078";
            case '9': return "\u2079"; default: return std::string(1, c);
        }
    };
    auto digit_to_sub = [](char c) -> std::string {
        switch (c) {
            case '0': return "\u2080"; case '1': return "\u2081"; case '2': return "\u2082";
            case '3': return "\u2083"; case '4': return "\u2084"; case '5': return "\u2085";
            case '6': return "\u2086"; case '7': return "\u2087"; case '8': return "\u2088";
            case '9': return "\u2089"; default: return std::string(1, c);
        }
    };

    frac_ekle->signal_clicked().connect([this, pay_entry, payda_entry, math_dialog, digit_to_super, digit_to_sub]() {
        std::string pay = pay_entry->get_text();
        std::string payda = payda_entry->get_text();
        if (pay.empty() || payda.empty()) return;
        std::string pay_super, payda_sub;
        for (char c : pay) pay_super += digit_to_super(c);
        for (char c : payda) payda_sub += digit_to_sub(c);
        std::string frak = pay_super + "\u2044" + payda_sub;
        note_view.get_buffer()->insert_at_cursor(frak);
        pay_entry->set_text("");
        payda_entry->set_text("");
        math_dialog->hide();
    });

    math_box->append(*frac_box);

    math_sw->set_child(*math_box);
    content->append(*math_sw);
    math_dialog->add_button("Kapat", Gtk::ResponseType::CLOSE);
    math_dialog->signal_response().connect([math_dialog](int) { math_dialog->hide(); });

    btn_math.signal_clicked().connect([math_dialog]() {
        math_dialog->present();
    });

    auto* sep2 = Gtk::make_managed<Gtk::Separator>(Gtk::Orientation::VERTICAL);
    sep2->add_css_class("note-tool-sep");
    format_toolbar.append(*sep2);

    make_tool_btn(btn_font_dec_w, "<span size='small'>A⁻</span>", "Font küçült");
    make_tool_btn(btn_font_inc_w, "<span size='x-large'>A⁺</span>", "Font büyüt");

    note_editor_box.append(format_toolbar);

    btn_bold.signal_toggled().connect([this]() { apply_tag_to_selection(btn_bold, tag_bold); });
    btn_italic.signal_toggled().connect([this]() { apply_tag_to_selection(btn_italic, tag_italic); });
    btn_underline.signal_toggled().connect([this]() { apply_tag_to_selection(btn_underline, tag_underline); });
    btn_strikethrough.signal_toggled().connect([this]() { apply_tag_to_selection(btn_strikethrough, tag_strikethrough); });
    btn_h1.signal_toggled().connect([this]() { apply_tag_to_selection(btn_h1, tag_h1); });
    btn_h2.signal_toggled().connect([this]() { apply_tag_to_selection(btn_h2, tag_h2); });
    btn_h3.signal_toggled().connect([this]() { apply_tag_to_selection(btn_h3, tag_h3); });

    btn_font_inc_w.signal_clicked().connect([this]() {
        auto buf = note_view.get_buffer();
        Gtk::TextIter start, end;
        if (buf->get_selection_bounds(start, end))
            buf->apply_tag(tag_font_inc, start, end);
        else
            buf->apply_tag(tag_font_inc, buf->get_insert()->get_iter(), buf->get_insert()->get_iter());
    });
    btn_font_dec_w.signal_clicked().connect([this]() {
        auto buf = note_view.get_buffer();
        Gtk::TextIter start, end;
        if (buf->get_selection_bounds(start, end))
            buf->apply_tag(tag_font_dec, start, end);
        else
            buf->apply_tag(tag_font_dec, buf->get_insert()->get_iter(), buf->get_insert()->get_iter());
    });

    nbuf->signal_insert().connect([this](const Gtk::TextIter& pos, const Glib::ustring&, int length) {
        if (length <= 0 || note_loading || selected_note_index < 0) return;
        int soff = pos.get_offset() - length;
        if (soff < 0) return;
        auto buf = note_view.get_buffer();
        auto s = buf->get_iter_at_offset(soff);
        auto e = buf->get_iter_at_offset(soff + length);
        if (btn_bold.get_active() && tag_bold) buf->apply_tag(tag_bold, s, e);
        if (btn_italic.get_active() && tag_italic) buf->apply_tag(tag_italic, s, e);
        if (btn_underline.get_active() && tag_underline) buf->apply_tag(tag_underline, s, e);
        if (btn_strikethrough.get_active() && tag_strikethrough) buf->apply_tag(tag_strikethrough, s, e);
        if (btn_h1.get_active() && tag_h1) buf->apply_tag(tag_h1, s, e);
        if (btn_h2.get_active() && tag_h2) buf->apply_tag(tag_h2, s, e);
        if (btn_h3.get_active() && tag_h3) buf->apply_tag(tag_h3, s, e);
    });

    auto* note_sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    note_sw->set_vexpand(true);
    note_sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);

    note_view.set_wrap_mode(Gtk::WrapMode::WORD);
    note_view.set_left_margin(40);
    note_view.set_right_margin(40);
    note_view.set_top_margin(30);
    note_view.set_bottom_margin(30);
    note_view.set_editable(true);
    note_view.set_cursor_visible(true);

    auto key_controller = Gtk::EventControllerKey::create();
    key_controller->signal_key_pressed().connect(
        [this](guint keyval, guint, Gdk::ModifierType state) -> bool {
            if ((state & Gdk::ModifierType::CONTROL_MASK) == Gdk::ModifierType())
                return false;
            switch (gdk_keyval_to_upper(keyval)) {
            case GDK_KEY_B:
                btn_bold.set_active(!btn_bold.get_active());
                return true;
            case GDK_KEY_I:
                btn_italic.set_active(!btn_italic.get_active());
                return true;
            case GDK_KEY_U:
                btn_underline.set_active(!btn_underline.get_active());
                return true;
            case GDK_KEY_S:
                btn_strikethrough.set_active(!btn_strikethrough.get_active());
                return true;
            default:
                return false;
            }
        },
        false
    );
    note_view.add_controller(key_controller);

    auto note_provider = Gtk::CssProvider::create();
    note_provider->load_from_data(
        "textview { background: #ffffff; color: #222222; }"
        "textview text { background: #ffffff; color: #222222; }"
        "textview > border { background: #ffffff; }"
    );
    auto note_display = Gdk::Display::get_default();
    Gtk::StyleContext::add_provider_for_display(note_display, note_provider, GTK_STYLE_PROVIDER_PRIORITY_APPLICATION + 100);

    note_sw->set_child(note_view);
    note_editor_box.append(*note_sw);

    note_stats_label = Gtk::make_managed<Gtk::Label>("");
    note_stats_label->set_halign(Gtk::Align::END);
    note_stats_label->set_margin_top(4);
    note_stats_label->set_margin_bottom(2);
    note_stats_label->set_margin_end(4);
    note_stats_label->add_css_class("note-stats");
    note_editor_box.append(*note_stats_label);

    auto update_stats = [this]() {
        if (!note_stats_label) return;
        auto buf = note_view.get_buffer();
        if (!buf || selected_note_index < 0) {
            note_stats_label->set_text("");
            return;
        }
        auto text = buf->get_text(false);
        int chars = text.size();
        int words = 0;
        bool in_word = false;
        for (char c : text) {
            if (c == ' ' || c == '\n' || c == '\t') { in_word = false; }
            else if (!in_word) { words++; in_word = true; }
        }
        note_stats_label->set_text(
            std::to_string(words) + " kelime · " + std::to_string(chars) + " karakter");
    };
    note_view.get_buffer()->signal_changed().connect(update_stats);

    auto* btn_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
    btn_box->set_margin_top(8);

    btn_save_note.add_css_class("action-btn");
    btn_save_note.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_save_note));

    btn_share_note.add_css_class("action-btn");
    share_icon_ptr = Gtk::make_managed<Gtk::Image>();
    share_icon_ptr->set_from_resource(dark_mode
        ? "/org/ogrenci/merkezi/assets/share.svg"
        : "/org/ogrenci/merkezi/assets/share_dark.svg");
    share_icon_ptr->set_pixel_size(18);
    btn_share_note.set_child(*share_icon_ptr);
    btn_share_note.set_tooltip_text("Notu paylaş");
    btn_share_note.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_share_note));

    btn_delete_course.add_css_class("delete-btn");
    btn_delete_course.signal_clicked().connect(sigc::mem_fun(*this, &MainWindow::on_delete_course));

    btn_box->append(btn_save_note);
    btn_box->append(btn_share_note);
    btn_box->append(btn_delete_course);
    note_editor_box.append(*btn_box);

    auto* right_sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    right_sw->set_child(note_editor_box);
    right_sw->set_vexpand(true);

    paned->set_end_child(*right_sw);

    refresh_course_list();
    stack.add(*paned, "notes");
}

void MainWindow::refresh_course_list() {
    while (Gtk::Widget* child = course_list.get_first_child()) course_list.remove(*child);

    for (size_t i = 0; i < course_notes.size(); i++) {
        auto* row = Gtk::make_managed<Gtk::ListBoxRow>();
        row->add_css_class("course-row");
        auto* label = Gtk::make_managed<Gtk::Label>(course_notes[i].course);
        label->set_halign(Gtk::Align::START);
        label->set_margin_start(10);
        label->set_margin_top(6);
        label->set_margin_bottom(6);
        row->set_child(*label);
        course_list.append(*row);
    }
}

void MainWindow::on_course_select(Gtk::ListBoxRow* row) {
    if (!row) return;
    int idx = row->get_index();
    if (idx >= 0 && idx < (int)course_notes.size()) {
        selected_note_index = idx;
        note_title.set_text(course_notes[idx].course);
        note_loading = true;
        note_view.get_buffer()->set_text(course_notes[idx].content);
        note_loading = false;
        load_note_format(idx);
        update_format_buttons();
    }
}

void MainWindow::on_add_course() {
    auto dialog = Gtk::make_managed<Gtk::Dialog>();
    dialog->set_title("Yeni Ders");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(300, 100);

    dialog->add_button("İptal", Gtk::ResponseType::CANCEL);
    dialog->add_button("Ekle", Gtk::ResponseType::OK);

    auto* content = dialog->get_content_area();
    content->set_margin_start(16);
    content->set_margin_end(16);
    content->set_margin_top(16);
    content->set_margin_bottom(16);

    auto* entry = Gtk::make_managed<Gtk::Entry>();
    entry->set_placeholder_text("Ders adı (örn: Matematik)");
    entry->set_hexpand(true);
    content->append(*entry);

    dialog->signal_response().connect([this, dialog, entry](int response_id) {
        if (response_id == Gtk::ResponseType::OK) {
            auto name = entry->get_text();
            if (!name.empty()) {
                course_notes.push_back({name, ""});
                refresh_course_list();
                save_data();
            }
        }
        dialog->hide();
    });

    dialog->show();
}

static std::string json_escape(const std::string& s) {
    std::string out;
    for (char c : s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n"; break;
            case '\t': out += "\\t"; break;
            case '\r': out += "\\r"; break;
            default: out += c;
        }
    }
    return out;
}

static size_t share_WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    s->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void MainWindow::on_add_link_note() {
    auto dialog = Gtk::make_managed<Gtk::Dialog>();
    dialog->set_title("Link Notu Ekle");
    dialog->set_transient_for(*this);
    dialog->set_modal(true);
    dialog->set_default_size(400, 200);

    dialog->add_button("İptal", Gtk::ResponseType::CANCEL);
    dialog->add_button("Ekle", Gtk::ResponseType::OK);

    auto* content = dialog->get_content_area();
    content->set_margin_start(16);
    content->set_margin_end(16);
    content->set_margin_top(16);
    content->set_margin_bottom(16);
    content->set_spacing(12);

    auto* title_entry = Gtk::make_managed<Gtk::Entry>();
    title_entry->set_placeholder_text("Not başlığı");
    title_entry->set_hexpand(true);
    content->append(*title_entry);

    auto* link_entry = Gtk::make_managed<Gtk::Entry>();
    link_entry->set_placeholder_text("Link (URL)");
    link_entry->set_hexpand(true);
    content->append(*link_entry);

    auto* status_label = Gtk::make_managed<Gtk::Label>("");
    status_label->set_halign(Gtk::Align::START);
    status_label->set_visible(false);
    content->append(*status_label);

    dialog->signal_response().connect([this, dialog, title_entry, link_entry, status_label](int response_id) {
        if (response_id == Gtk::ResponseType::OK) {
            auto title = title_entry->get_text();
            auto link = link_entry->get_text();
            if (!title.empty() && !link.empty()) {
                status_label->set_text("Sunucudan getiriliyor...");
                status_label->set_visible(true);

                dialog->set_sensitive(false);

                std::thread([this, dialog, title, link]() {
                    CURL* curl = curl_easy_init();
                    std::string response;

                    if (curl) {
                        curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
                        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
                        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, share_WriteCallback);
                        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
                        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
                        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
                        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

                        CURLcode res = curl_easy_perform(curl);
                        curl_easy_cleanup(curl);

                        if (res != CURLE_OK) {
                            response = "ERROR: " + std::string(curl_easy_strerror(res));
                        }
                    } else {
                        response = "ERROR: Failed to initialize curl";
                    }

                    Glib::signal_idle().connect_once([this, dialog, title, link, response]() {
                        dialog->set_sensitive(true);

                        if (response.rfind("ERROR:", 0) == 0) {
                            auto* err_dialog = Gtk::make_managed<Gtk::MessageDialog>(*this,
                                "Hata", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK);
                            err_dialog->set_secondary_text("Not getirilemedi:\n" + response.substr(6));
                            err_dialog->signal_response().connect([err_dialog](int) { err_dialog->hide(); });
                            err_dialog->show();
                            return;
                        }

                        std::string saved_content;
                        try {
                            auto j = json::parse(response);
                            saved_content = j.value("content", "");
                        } catch (const json::parse_error&) {}

                        if (saved_content.empty()) {
                            auto* err_dialog = Gtk::make_managed<Gtk::MessageDialog>(*this,
                                "Hata", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK);
                            err_dialog->set_secondary_text("Geçersiz yanıt: content bulunamadı.");
                            err_dialog->signal_response().connect([err_dialog](int) { err_dialog->hide(); });
                            err_dialog->show();
                            return;
                        }

                        course_notes.push_back({title, saved_content});
                        refresh_course_list();
                        save_data();

                        int new_idx = (int)course_notes.size() - 1;
                        selected_note_index = new_idx;
                        note_title.set_text(course_notes[new_idx].course);
                        note_view.get_buffer()->set_text(course_notes[new_idx].content);
                        auto* last_row = course_list.get_row_at_index(new_idx);
                        if (last_row) course_list.select_row(*last_row);

                        dialog->hide();
                    });
                }).detach();
                return;
            }
        }
        dialog->hide();
    });

    dialog->show();
}

void MainWindow::on_save_note() {
    if (selected_note_index >= 0 && selected_note_index < (int)course_notes.size()) {
        auto buf = note_view.get_buffer();
        Gtk::TextIter start, end;
        buf->get_bounds(start, end);
        course_notes[selected_note_index].content = buf->get_text(start, end, false);
        save_note_format(selected_note_index);
        save_data();
    }
}

void MainWindow::on_delete_course() {
    if (selected_note_index >= 0 && selected_note_index < (int)course_notes.size()) {
        if (selected_note_index < (int)note_formats.size())
            note_formats.erase(note_formats.begin() + selected_note_index);
        course_notes.erase(course_notes.begin() + selected_note_index);
        selected_note_index = -1;
        note_title.set_text("Bir ders seçin");
        note_view.get_buffer()->set_text("");
        refresh_course_list();
        save_data();
    }
}

Glib::RefPtr<Gtk::TextTag> MainWindow::make_font_tag(const std::string& name, double scale) {
    auto tag = Gtk::TextTag::create(name);
    tag->property_scale_set() = true;
    tag->property_scale() = scale;
    note_view.get_buffer()->get_tag_table()->add(tag);
    return tag;
}

void MainWindow::apply_tag_to_selection(Gtk::ToggleButton& btn, const Glib::RefPtr<Gtk::TextTag>& tag) {
    auto buf = note_view.get_buffer();
    Gtk::TextIter start, end;
    if (buf->get_selection_bounds(start, end)) {
        if (btn.get_active())
            buf->apply_tag(tag, start, end);
        else
            buf->remove_tag(tag, start, end);
    } else {
        auto iter = buf->get_insert()->get_iter();
        if (btn.get_active())
            buf->apply_tag(tag, iter, iter);
        else
            buf->remove_tag(tag, iter, iter);
    }
}

void MainWindow::update_format_buttons() {
    auto buf = note_view.get_buffer();
    if (!buf) return;
    auto iter = buf->get_insert()->get_iter();

    auto check = [&](const Glib::RefPtr<Gtk::TextTag>& tag) -> bool {
        if (!tag) return false;
        Gtk::TextIter s, e;
        if (buf->get_selection_bounds(s, e))
            return s.has_tag(tag) || e.has_tag(tag);
        return iter.has_tag(tag);
    };

    btn_bold.set_active(check(tag_bold));
    btn_italic.set_active(check(tag_italic));
    btn_underline.set_active(check(tag_underline));
    btn_strikethrough.set_active(check(tag_strikethrough));
    btn_h1.set_active(check(tag_h1));
    btn_h2.set_active(check(tag_h2));
    btn_h3.set_active(check(tag_h3));
}

void MainWindow::save_note_format(size_t index) {
    if (index >= note_formats.size())
        note_formats.resize(index + 1, "");

    auto buf = note_view.get_buffer();
    Gtk::TextIter start, end;
    buf->get_bounds(start, end);
    int len = end.get_offset();

    json j = json::array();
    auto record_tag = [&](const Glib::RefPtr<Gtk::TextTag>& tag, const std::string& tname) {
        int rs = -1;
        for (int i = 0; i <= len; i++) {
            auto it = (i < len) ? buf->get_iter_at_offset(i) : end;
            bool has = it.has_tag(tag);
            if (has && rs < 0) rs = i;
            else if (!has && rs >= 0) {
                json r;
                r["s"] = rs; r["e"] = i; r["t"] = tname;
                j.push_back(r);
                rs = -1;
            }
        }
    };

    record_tag(tag_bold, "b");
    record_tag(tag_italic, "i");
    record_tag(tag_underline, "u");
    record_tag(tag_strikethrough, "s");
    record_tag(tag_h1, "h1");
    record_tag(tag_h2, "h2");
    record_tag(tag_h3, "h3");
    record_tag(tag_font_inc, "fi");
    record_tag(tag_font_dec, "fd");

    note_formats[index] = j.dump();
}

void MainWindow::load_note_format(size_t index) {
    auto buf = note_view.get_buffer();
    if (index >= note_formats.size() || note_formats[index].empty())
        return;

    try {
        auto j = json::parse(note_formats[index]);
        auto tag_for_name = [this](const std::string& t) -> Glib::RefPtr<Gtk::TextTag> {
            if (t == "b") return tag_bold;
            if (t == "i") return tag_italic;
            if (t == "u") return tag_underline;
            if (t == "s") return tag_strikethrough;
            if (t == "h1") return tag_h1;
            if (t == "h2") return tag_h2;
            if (t == "h3") return tag_h3;
            if (t == "fi") return tag_font_inc;
            if (t == "fd") return tag_font_dec;
            return {};
        };
        for (auto& r : j) {
            int s = r["s"].get<int>();
            int e = r["e"].get<int>();
            auto tag = tag_for_name(r["t"].get<std::string>());
            if (tag) {
                auto sit = buf->get_iter_at_offset(s);
                auto eit = buf->get_iter_at_offset(e);
                buf->apply_tag(tag, sit, eit);
            }
        }
    } catch (const json::parse_error&) {}
}

void MainWindow::on_share_note() {
    if (selected_note_index < 0 || selected_note_index >= (int)course_notes.size()) {
        auto msg = Gtk::make_managed<Gtk::MessageDialog>(*this, "Uyarı", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK);
        msg->set_secondary_text("Lütfen önce bir not seçin.");
        msg->signal_response().connect([msg](int) { msg->hide(); });
        msg->show();
        return;
    }

    auto buf = note_view.get_buffer();
    Gtk::TextIter start, end;
    buf->get_bounds(start, end);
    std::string content = buf->get_text(start, end, false);
    if (content.empty()) {
        auto msg = Gtk::make_managed<Gtk::MessageDialog>(*this, "Uyarı", false, Gtk::MessageType::INFO, Gtk::ButtonsType::OK);
        msg->set_secondary_text("Not içeriği boş, paylaşılacak bir şey yok.");
        msg->signal_response().connect([msg](int) { msg->hide(); });
        msg->show();
        return;
    }

    auto* loading = Gtk::make_managed<Gtk::MessageDialog>(*this, "Paylaşılıyor...", false, Gtk::MessageType::INFO, Gtk::ButtonsType::NONE);
    loading->show();

    std::thread([this, content, loading]() {
        CURL* curl = curl_easy_init();
        std::string response;

        if (curl) {
            std::string note_title_text = course_notes[selected_note_index].course;
            std::string json = "{\"title\":\"" + json_escape(note_title_text) + "\",\"content\":\"" + json_escape(content) + "\"}";

            struct curl_slist* headers = nullptr;
            headers = curl_slist_append(headers, "Content-Type: application/json");

            curl_easy_setopt(curl, CURLOPT_URL, "http://192.168.1.214:8000/notes");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, share_WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);
            curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);

            CURLcode res = curl_easy_perform(curl);

            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);

            if (res != CURLE_OK) {
                response = "ERROR: " + std::string(curl_easy_strerror(res));
            }
        } else {
            response = "ERROR: Failed to initialize curl";
        }

        Glib::signal_idle().connect_once([this, response, loading]() {
            loading->hide();

            if (response.rfind("ERROR:", 0) == 0) {
                auto err_msg = Gtk::make_managed<Gtk::MessageDialog>(*this, "Hata", false, Gtk::MessageType::ERROR, Gtk::ButtonsType::OK);
                err_msg->set_secondary_text(response.substr(6));
                err_msg->signal_response().connect([err_msg](int) { err_msg->hide(); });
                err_msg->show();
                return;
            }

            std::string api_link, web_link;
            try {
                auto j = json::parse(response);
                api_link = j["data"].value("link", "");
                web_link = j["data"].value("web_viewer_link", "");
            } catch (const json::parse_error&) {}

            if (api_link.empty()) api_link = "http://192.168.1.214:8000";
            if (web_link.empty()) web_link = api_link;

            auto* result_dialog = Gtk::make_managed<Gtk::Dialog>();
            result_dialog->set_title("Not Paylaşıldı");
            result_dialog->set_transient_for(*this);
            result_dialog->set_modal(true);
            result_dialog->set_default_size(480, 260);

            result_dialog->add_button("Kapat", Gtk::ResponseType::CLOSE);

            auto* vbox = result_dialog->get_content_area();
            vbox->set_margin_start(16);
            vbox->set_margin_end(16);
            vbox->set_margin_top(16);
            vbox->set_margin_bottom(16);
            vbox->set_spacing(12);

            auto* lbl = Gtk::make_managed<Gtk::Label>();
            lbl->set_markup("<b>Notunuz paylaşıldı! Aşağıdaki linklerden kopyalayabilirsiniz:</b>");
            lbl->set_wrap(true);
            vbox->append(*lbl);

            auto make_link_row = [vbox](const std::string& label, const std::string& url) {
                auto* row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 6);

                auto* lbl = Gtk::make_managed<Gtk::Label>();
                lbl->set_markup("<b>" + label + ":</b>");
                lbl->set_size_request(20, -1);
                row->append(*lbl);

                auto* entry = Gtk::make_managed<Gtk::Entry>();
                entry->set_text(url);
                entry->set_hexpand(true);
                entry->set_editable(false);
                row->append(*entry);

                auto* btn = Gtk::make_managed<Gtk::Button>("Kopyala");
                btn->add_css_class("action-btn");
                btn->signal_clicked().connect([entry]() {
                    auto display = Gdk::Display::get_default();
                    if (display) {
                        auto clipboard = display->get_clipboard();
                        clipboard->set_text(entry->get_text());
                    }
                });
                row->append(*btn);

                vbox->append(*row);
            };

            make_link_row("API", api_link);
            make_link_row("Web", web_link);

            {
                auto* qr = QRcode_encodeString(web_link.c_str(), 0, QR_ECLEVEL_M, QR_MODE_8, 1);
                if (qr) {
                    int border = 2;
                    int dim = qr->width + 2 * border;
                    int pixel_size = 4;
                    int img_size = dim * pixel_size;

                    auto* pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, false, 8, img_size, img_size);
                    gdk_pixbuf_fill(pixbuf, 0xffffffff);

                    int stride = gdk_pixbuf_get_rowstride(pixbuf);
                    auto* pixels = gdk_pixbuf_get_pixels(pixbuf);

                    for (int y = 0; y < qr->width; y++) {
                        for (int x = 0; x < qr->width; x++) {
                            if (qr->data[y * qr->width + x] & 0x01) {
                                for (int py = 0; py < pixel_size; py++) {
                                    for (int px = 0; px < pixel_size; px++) {
                                        int ix = (x + border) * pixel_size + px;
                                        int iy = (y + border) * pixel_size + py;
                                        auto* p = pixels + iy * stride + ix * 3;
                                        p[0] = 0; p[1] = 0; p[2] = 0;
                                    }
                                }
                            }
                        }
                    }

                    auto ref = Glib::wrap(pixbuf, false);
                    QRcode_free(qr);

                    auto* qr_pic = Gtk::make_managed<Gtk::Picture>();
                    qr_pic->set_pixbuf(ref);
                    qr_pic->set_size_request(img_size, img_size);
                    auto* qr_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
                    qr_box->set_halign(Gtk::Align::CENTER);
                    qr_box->set_margin_top(8);
                    qr_box->append(*qr_pic);
                    vbox->append(*qr_box);
                }
            }

            result_dialog->signal_response().connect([result_dialog](int) {
                result_dialog->hide();
            });

            result_dialog->show();
        });
    }).detach();
}
