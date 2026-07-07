#include "mainwindow.h"
#include <sstream>

void MainWindow::setup_badges() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 12);
    box->set_margin_start(16);
    box->set_margin_end(16);
    box->set_margin_top(16);
    box->set_margin_bottom(16);

    auto* title = Gtk::make_managed<Gtk::Label>("Rozetlerim");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::START);
    box->append(*title);

    auto* sub = Gtk::make_managed<Gtk::Label>("Çalışma serini koruyarak rozetler kazan!");
    sub->set_halign(Gtk::Align::START);
    sub->add_css_class("dash-sub");
    sub->set_margin_bottom(8);
    box->append(*sub);

    auto* streak_lbl = Gtk::make_managed<Gtk::Label>("");
    streak_lbl->set_markup("Mevcut Seri: <b>" + std::to_string(study_streak) + " gün</b>");
    streak_lbl->set_halign(Gtk::Align::START);
    streak_lbl->set_margin_bottom(16);
    box->append(*streak_lbl);

    struct BadgeDef { std::string name; std::string resource; int required; std::string desc; };
    std::vector<BadgeDef> badges = {
        {
            "5 Gün Serisi",  
            "/org/ogrenci/merkezi/assets/rozet/5days.png", 
             5,  
             "5 gün ara vermeden çalış"
        },
        {
            "7 Gün Serisi",  
            "/org/ogrenci/merkezi/assets/rozet/7days.png",  
            7,  
            "7 gün ara vermeden çalış"
        },
        {
            "14 Gün Serisi", 
            "/org/ogrenci/merkezi/assets/rozet/14days.png", 
            14, 
            "14 gün ara vermeden çalış"
        },
        {
            "21 Gün",
            "/org/ogrenci/merkezi/assets/rozet/21days.png",
            21,
            "Ha gayret, 1 Ay olacak"
        },
        {
            "30 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/30days.png",
            30,
            "1 Ay, iyi İlerliyorsun!"
        },
        {
            "60 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/60days.png",
            60,
            "2 Ay, Sadık Öğrenci..."
        },
        {
            "90 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/90days.png",
            90,
            "Ders Ustası"
        },
        {
            "180 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/180days.png",
            180,
            "Ders Canavarı"
        },
        {
            "365 Gün Serisi",
            "/org/ogrenci/merkezi/assets/rozet/365days.png",
            365,
            "Akademik Canavar..."
        }
    };

    auto* flow = Gtk::make_managed<Gtk::FlowBox>();
    flow->set_max_children_per_line(3);
    flow->set_selection_mode(Gtk::SelectionMode::NONE);
    flow->set_column_spacing(16);
    flow->set_row_spacing(16);
    flow->set_homogeneous(true);
    flow->set_margin_top(4);

    for (auto& b : badges) {
        bool earned = study_streak >= b.required;

        auto* card = Gtk::make_managed<Gtk::Frame>();
        card->add_css_class("card");

        auto* vb = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 8);
        vb->set_halign(Gtk::Align::CENTER);
        vb->set_valign(Gtk::Align::CENTER);
        vb->set_margin_start(24);
        vb->set_margin_end(24);
        vb->set_margin_top(24);
        vb->set_margin_bottom(24);

        auto* img = Gtk::make_managed<Gtk::Image>();
        img->set_from_resource(b.resource);
        img->set_pixel_size(96);

        if (!earned) img->set_opacity(0.2);
        vb->append(*img);

        auto* nl = Gtk::make_managed<Gtk::Label>("");
        nl->set_markup("<b>" + b.name + "</b>");
        nl->set_halign(Gtk::Align::CENTER);
        nl->set_margin_top(4);
        if (!earned) nl->set_opacity(0.5);
        vb->append(*nl);

        auto* dl = Gtk::make_managed<Gtk::Label>(b.desc);
        dl->set_halign(Gtk::Align::CENTER);
        dl->set_wrap(true);
        dl->set_opacity(0.55);
        vb->append(*dl);

        if (earned) {
            auto* el = Gtk::make_managed<Gtk::Label>("Kazanıldı!");
            el->set_halign(Gtk::Align::CENTER);
            el->add_css_class("badge-earned");
            vb->append(*el);
        } else {
            auto* ll = Gtk::make_managed<Gtk::Label>("Kilitli");
            ll->set_halign(Gtk::Align::CENTER);
            ll->set_opacity(0.4);
            vb->append(*ll);
        }

        card->set_child(*vb);
        flow->append(*card);
    }

    box->append(*flow);
    sw->set_child(*box);
    stack.add(*sw, "badges");
}
