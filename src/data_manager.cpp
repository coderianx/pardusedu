#include "mainwindow.h"
#include "ai_client.h"
#include "calc_letter.h"
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <cctype>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace fs = std::filesystem;
using json = nlohmann::json;

std::string MainWindow::get_data_dir() {
    const char* data = g_get_user_data_dir();
    if (!data) return "/tmp/ogrenci-merkezi";
    return std::string(data) + "/ogrenci-merkezi";
}

void MainWindow::setup_data() {
    auto dir = get_data_dir();
    fs::create_directories(dir);

    auto tf = dir + "/tasks.dat";
    if (fs::exists(tf)) {
        std::ifstream f(tf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p1 = line.find('|');
            auto p2 = line.find('|', p1 + 1);
            auto p3 = line.find('|', p2 + 1);
            auto p4 = line.find('|', p3 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos && p4 != std::string::npos) {
                tasks.push_back({line.substr(0, p1), line.substr(p1+1, p2-p1-1), line.substr(p2+1, p3-p2-1), line[p3+1]=='1', std::stoi(line.substr(p4+1))});
            }
        }
    }

    auto pf = dir + "/pomodoro.dat";
    if (fs::exists(pf)) {
        std::ifstream f(pf);
        std::string line;
        if (std::getline(f, line)) {
            auto p = line.find('|');
            if (p != std::string::npos) {
                pomodoro_completed = std::stoi(line.substr(0, p));
                pomodoro_minutes = std::stoi(line.substr(p+1));
            }
        }
    }

    auto sf = dir + "/streak.dat";
    if (fs::exists(sf)) {
        std::ifstream f(sf);
        std::string line;
        if (std::getline(f, line)) {
            auto p = line.find('|');
            if (p != std::string::npos) {
                study_streak = std::stoi(line.substr(0, p));
                last_streak_date = line.substr(p + 1);
            } else {
                study_streak = std::stoi(line);
            }
        }
    }

    auto nf = dir + "/notes.dat";
    if (fs::exists(nf)) {
        std::ifstream f(nf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p = line.find('|');
            if (p != std::string::npos) {
                course_notes.push_back({line.substr(0, p), ""});
            }
        }
        for (size_t i = 0; i < course_notes.size(); i++) {
            auto cf = dir + "/note_" + std::to_string(i) + ".txt";
            if (fs::exists(cf)) {
                std::ifstream nf2(cf);
                std::string content((std::istreambuf_iterator<char>(nf2)), std::istreambuf_iterator<char>());
                course_notes[i].content = content;
            }
            auto ff = dir + "/note_" + std::to_string(i) + "_fmt.dat";
            if (fs::exists(ff)) {
                std::ifstream ff2(ff);
                std::string fmt((std::istreambuf_iterator<char>(ff2)), std::istreambuf_iterator<char>());
                note_formats.push_back(fmt);
            } else {
                note_formats.push_back("");
            }
        }
    }

    auto gf = dir + "/grades.dat";
    if (fs::exists(gf)) {
        std::ifstream f(gf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p1 = line.find('|');
            auto p2 = line.find('|', p1 + 1);
            auto p3 = line.find('|', p2 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos) {
                GradeEntry g;
                g.course = line.substr(0, p1);
                g.midterm = std::stod(line.substr(p1+1, p2-p1-1));
                g.final = std::stod(line.substr(p2+1, p3-p2-1));
                g.average = g.midterm * 0.4 + g.final * 0.6;
                g.letter = ::calc_letter(g.average);
                grades.push_back(g);
            }
        }
    }

    auto schf = dir + "/schedule.dat";
    if (fs::exists(schf)) {
        std::ifstream f(schf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p1 = line.find('|');
            auto p2 = line.find('|', p1 + 1);
            auto p3 = line.find('|', p2 + 1);
            auto p4 = line.find('|', p3 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos && p4 != std::string::npos) {
                schedule.push_back({line.substr(0, p1), line.substr(p1+1, p2-p1-1), line.substr(p2+1, p3-p2-1), line.substr(p4+1)});
            }
        }
    }

    auto stf = dir + "/schedule_times.dat";
    if (fs::exists(stf)) {
        std::ifstream f(stf);
        std::string line;
        schedule_times.clear();
        while (std::getline(f, line)) {
            if (!line.empty()) schedule_times.push_back(line);
        }
        if (schedule_times.empty()) schedule_times = {"08:30", "09:30", "10:30", "11:30", "13:00", "14:00", "15:00", "16:00"};
    }

    auto df = dir + "/decks.dat";
    if (fs::exists(df)) {
        std::ifstream f(df);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            auto p1 = line.find('|');
            auto p2 = line.find('|', p1 + 1);
            auto p3 = line.find('|', p2 + 1);
            auto p4 = line.find('|', p3 + 1);
            auto p5 = line.find('|', p4 + 1);
            auto p6 = line.find('|', p5 + 1);
            auto p7 = line.find('|', p6 + 1);
            if (p1 != std::string::npos && p2 != std::string::npos && p3 != std::string::npos
                && p4 != std::string::npos && p5 != std::string::npos && p6 != std::string::npos
                && p7 != std::string::npos) {
                FlashDeck d;
                d.id = line.substr(0, p1);
                d.title = line.substr(p1+1, p2-p1-1);
                d.description = line.substr(p2+1, p3-p2-1);
                d.source_note = line.substr(p3+1, p4-p3-1);
                d.source_course = line.substr(p4+1, p5-p4-1);
                d.created_at = std::stol(line.substr(p5+1, p6-p5-1));
                d.last_reviewed_at = std::stol(line.substr(p6+1, p7-p6-1));
                flash_decks.push_back(d);
            }
        }
    }

    auto cf = dir + "/cards.dat";
    if (fs::exists(cf)) {
        std::ifstream f(cf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            try {
                auto j = json::parse(line);
                FlashCard c;
                c.id = j["id"].get<std::string>();
                c.deck_id = j["deck_id"].get<std::string>();
                c.front = j["front"].get<std::string>();
                c.back = j["back"].get<std::string>();
                if (j.contains("hint")) c.hint = j["hint"].get<std::string>();
                if (j.contains("tags")) c.tags = j["tags"].get<std::string>();
                c.interval = j["interval"].get<int>();
                c.repetitions = j["repetitions"].get<int>();
                c.ease_factor = j["ease_factor"].get<double>();
                c.lapses = j["lapses"].get<int>();
                c.next_review_at = j["next_review_at"].get<long>();
                c.last_reviewed_at = j.value("last_reviewed_at", 0L);
                flash_cards.push_back(c);
            } catch (const std::exception&) {}
        }
    }

    auto rf = dir + "/reports.dat";
    if (fs::exists(rf)) {
        std::ifstream f(rf);
        std::string line;
        while (std::getline(f, line)) {
            if (line.empty()) continue;
            std::vector<std::string> parts;
            size_t pos = 0, prev = 0;
            while ((pos = line.find('|', prev)) != std::string::npos) {
                parts.push_back(line.substr(prev, pos - prev));
                prev = pos + 1;
            }
            parts.push_back(line.substr(prev));
            if (parts.size() < 19) continue;

            WeeklyReport r;
            r.week_start = parts[0];
            r.week_end = parts[1];
            r.total_pomo_sessions = std::stoi(parts[2]);
            r.total_focus_minutes = std::stoi(parts[3]);
            r.cards_reviewed = std::stoi(parts[4]);
            r.cards_correct = std::stoi(parts[5]);
            r.cards_wrong = std::stoi(parts[6]);
            r.tasks_completed = std::stoi(parts[7]);
            r.tasks_total = std::stoi(parts[8]);
            r.streak_count = std::stoi(parts[9]);
            r.notes_created = std::stoi(parts[10]);
            r.study_score = std::stoi(parts[11]);
            r.best_deck_id = parts[12];
            r.worst_deck_id = parts[13];
            r.weak_deck_id = parts[14];
            r.created_at = std::stol(parts[15]);
            r.ai_summary = parts[16];
            r.ai_advice = parts[17];
            r.ai_motivation = parts[18];
            weekly_reports.push_back(r);
        }
    }

    auto pf_ = dir + "/provider.dat";
    if (fs::exists(pf_)) {
        std::ifstream f(pf_);
        std::string line;
        if (std::getline(f, line) && !line.empty()) {
            if (line == "gemini") ai_provider = AIProvider::GEMINI;
            else ai_provider = (line == "openrouter") ? AIProvider::OPENROUTER : AIProvider::GROQ;
            set_provider(ai_provider);
        }
    }

    auto akf_g = dir + "/apikey_groq.dat";
    if (fs::exists(akf_g)) {
        std::ifstream f(akf_g);
        std::string line;
        if (std::getline(f, line) && !line.empty()) {
            ai_api_key_groq = line;
            set_groq_api_key(line);
        }
    }

    auto akf_o = dir + "/apikey_openrouter.dat";
    if (fs::exists(akf_o)) {
        std::ifstream f(akf_o);
        std::string line;
        if (std::getline(f, line) && !line.empty()) {
            ai_api_key_openrouter = line;
            set_openrouter_api_key(line);
        }
    }

    auto akf_gm = dir + "/apikey_gemini.dat";
    if (fs::exists(akf_gm)) {
        std::ifstream f(akf_gm);
        std::string line;
        if (std::getline(f, line) && !line.empty()) {
            ai_api_key_gemini = line;
            set_gemini_api_key(line);
        }
    }

    auto mf_g = dir + "/model_groq.dat";
    if (fs::exists(mf_g)) {
        std::ifstream f(mf_g);
        std::string line;
        if (std::getline(f, line) && !line.empty()) {
            ai_model_groq = line;
        }
    }

    auto mf_o = dir + "/model_openrouter.dat";
    if (fs::exists(mf_o)) {
        std::ifstream f(mf_o);
        std::string line;
        if (std::getline(f, line) && !line.empty()) {
            ai_model_openrouter = line;
        }
    }

    auto mf_gm = dir + "/model_gemini.dat";
    if (fs::exists(mf_gm)) {
        std::ifstream f(mf_gm);
        std::string line;
        if (std::getline(f, line) && !line.empty()) {
            ai_model_gemini = line;
        }
    }

    // Apply the active provider's model
    if (ai_provider == AIProvider::GROQ)
        set_model(ai_model_groq);
    else if (ai_provider == AIProvider::OPENROUTER)
        set_model(ai_model_openrouter);
    else
        set_model(ai_model_gemini);
}

void MainWindow::save_data() {
    auto dir = get_data_dir();
    fs::create_directories(dir);

    { std::ofstream f(dir + "/tasks.dat"); for (auto& t : tasks) f << t.title << "|" << t.category << "|" << t.due_date << "|" << (t.done?"1":"0") << "|" << t.priority << "\n"; }
    { std::ofstream f(dir + "/pomodoro.dat"); f << pomodoro_completed << "|" << pomodoro_minutes << "\n"; }
    { std::ofstream f(dir + "/streak.dat"); f << study_streak << "|" << last_streak_date << "\n"; }
    { std::ofstream f(dir + "/notes.dat"); for (auto& n : course_notes) f << n.course << "|\n"; }
    for (size_t i = 0; i < course_notes.size(); i++) {
        std::ofstream f(dir + "/note_" + std::to_string(i) + ".txt");
        f << course_notes[i].content;
    }
    for (size_t i = 0; i < note_formats.size(); i++) {
        std::ofstream f(dir + "/note_" + std::to_string(i) + "_fmt.dat");
        f << note_formats[i];
    }
    { std::ofstream f(dir + "/grades.dat"); for (auto& g : grades) f << g.course << "|" << g.midterm << "|" << g.final << "|\n"; }
    { std::ofstream f(dir + "/schedule.dat"); for (auto& s : schedule) f << s.day << "|" << s.time << "|" << s.course << "|" << s.location << "\n"; }
    { std::ofstream f(dir + "/schedule_times.dat"); for (auto& t : schedule_times) f << t << "\n"; }
    { std::ofstream f(dir + "/decks.dat");
        for (auto& d : flash_decks)
            f << d.id << "|" << d.title << "|" << d.description << "|"
              << d.source_note << "|" << d.source_course << "|"
              << d.created_at << "|" << d.last_reviewed_at << "\n"; }
    { std::ofstream f(dir + "/cards.dat");
        for (auto& c : flash_cards) {
            json j;
            j["id"] = c.id;
            j["deck_id"] = c.deck_id;
            j["front"] = c.front;
            j["back"] = c.back;
            j["hint"] = c.hint;
            j["tags"] = c.tags;
            j["interval"] = c.interval;
            j["repetitions"] = c.repetitions;
            j["ease_factor"] = c.ease_factor;
            j["lapses"] = c.lapses;
            j["next_review_at"] = c.next_review_at;
            j["last_reviewed_at"] = c.last_reviewed_at;
            f << j.dump() << "\n";
        } }
    { std::ofstream f(dir + "/reports.dat");
        for (auto& r : weekly_reports) {
            f << r.week_start << "|"
              << r.week_end << "|"
              << r.total_pomo_sessions << "|"
              << r.total_focus_minutes << "|"
              << r.cards_reviewed << "|"
              << r.cards_correct << "|"
              << r.cards_wrong << "|"
              << r.tasks_completed << "|"
              << r.tasks_total << "|"
              << r.streak_count << "|"
              << r.notes_created << "|"
              << r.study_score << "|"
              << r.best_deck_id << "|"
              << r.worst_deck_id << "|"
              << r.weak_deck_id << "|"
              << r.created_at << "|"
              << r.ai_summary << "|"
              << r.ai_advice << "|"
              << r.ai_motivation << "\n";
        } }
    { std::ofstream f(dir + "/provider.dat"); f << (ai_provider == AIProvider::GROQ ? "groq" : ai_provider == AIProvider::OPENROUTER ? "openrouter" : "gemini") << "\n"; }
    { std::ofstream f(dir + "/apikey_groq.dat"); f << ai_api_key_groq << "\n"; }
    { std::ofstream f(dir + "/apikey_openrouter.dat"); f << ai_api_key_openrouter << "\n"; }
    { std::ofstream f(dir + "/apikey_gemini.dat"); f << ai_api_key_gemini << "\n"; }
    { std::ofstream f(dir + "/model_groq.dat"); f << ai_model_groq << "\n"; }
    { std::ofstream f(dir + "/model_openrouter.dat"); f << ai_model_openrouter << "\n"; }
    { std::ofstream f(dir + "/model_gemini.dat"); f << ai_model_gemini << "\n"; }

    // Sync anki_export TSV files with current data
    std::string export_dir = dir + "/anki_export";
    fs::create_directories(export_dir);
    for (auto& entry : fs::directory_iterator(export_dir)) {
        if (entry.path().extension() == ".tsv") fs::remove(entry.path());
    }
    for (auto& d : flash_decks) {
        std::string slug;
        for (char c : d.title) {
            if (isalnum(c) || c == '-' || c == '_') slug += c;
            else if (c == ' ') slug += '_';
        }
        if (slug.empty()) slug = "deck";
        std::string path = export_dir + "/" + slug + ".tsv";
        std::ofstream f(path);
        f << "front\tback\ttags\n";
        for (auto& c : flash_cards) {
            if (c.deck_id != d.id) continue;
            std::string ef = c.front, eb = c.back, et = c.tags;
            size_t p;
            while ((p = ef.find('\t')) != std::string::npos) ef.replace(p, 1, " ");
            while ((p = ef.find('\n')) != std::string::npos) ef.replace(p, 1, " ");
            while ((p = eb.find('\t')) != std::string::npos) eb.replace(p, 1, " ");
            while ((p = eb.find('\n')) != std::string::npos) eb.replace(p, 1, " ");
            while ((p = et.find('\t')) != std::string::npos) et.replace(p, 1, " ");
            f << ef << "\t" << eb << "\t" << et << "\n";
        }
    }
}

static std::string today_str() {
    auto t = std::time(nullptr);
    auto* tm = std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(tm, "%Y-%m-%d");
    return oss.str();
}

static bool is_next_day(const std::string& prev, const std::string& today) {
    std::tm tm = {};
    std::istringstream ss(prev);
    ss >> std::get_time(&tm, "%Y-%m-%d");
    if (ss.fail()) return false;
    tm.tm_mday += 1;
    std::mktime(&tm);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d");
    return oss.str() == today;
}

void MainWindow::update_streak() {
    auto today = today_str();
    if (last_streak_date == today) return;
    if (last_streak_date.empty() || !is_next_day(last_streak_date, today)) {
        study_streak = 1;
    } else {
        study_streak++;
    }
    for (auto& d : flash_decks) {
        flash_refresh_deck_stats(d.id);
    }
    last_streak_date = today;
    save_data();
}

