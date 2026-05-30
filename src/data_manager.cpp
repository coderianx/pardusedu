#include "mainwindow.h"
#include "ai_client.h"
#include "calc_letter.h"
#include <fstream>
#include <filesystem>
#include <cstdlib>

namespace fs = std::filesystem;

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
        if (std::getline(f, line)) study_streak = std::stoi(line);
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

    auto akf = dir + "/apikey.dat";
    if (fs::exists(akf)) {
        std::ifstream f(akf);
        std::string line;
        if (std::getline(f, line) && !line.empty()) {
            ai_api_key = line;
            set_api_key(line);
        }
    }

    auto mf = dir + "/model.dat";
    if (fs::exists(mf)) {
        std::ifstream f(mf);
        std::string line;
        if (std::getline(f, line) && !line.empty()) {
            ai_model = line;
            set_model(line);
        }
    }
}

void MainWindow::save_data() {
    auto dir = get_data_dir();
    fs::create_directories(dir);

    { std::ofstream f(dir + "/tasks.dat"); for (auto& t : tasks) f << t.title << "|" << t.category << "|" << t.due_date << "|" << (t.done?"1":"0") << "|" << t.priority << "\n"; }
    { std::ofstream f(dir + "/pomodoro.dat"); f << pomodoro_completed << "|" << pomodoro_minutes << "\n"; }
    { std::ofstream f(dir + "/streak.dat"); f << study_streak << "\n"; }
    { std::ofstream f(dir + "/notes.dat"); for (auto& n : course_notes) f << n.course << "|\n"; }
    for (size_t i = 0; i < course_notes.size(); i++) {
        std::ofstream f(dir + "/note_" + std::to_string(i) + ".txt");
        f << course_notes[i].content;
    }
    { std::ofstream f(dir + "/grades.dat"); for (auto& g : grades) f << g.course << "|" << g.midterm << "|" << g.final << "|\n"; }
    { std::ofstream f(dir + "/schedule.dat"); for (auto& s : schedule) f << s.day << "|" << s.time << "|" << s.course << "|" << s.location << "\n"; }
    { std::ofstream f(dir + "/schedule_times.dat"); for (auto& t : schedule_times) f << t << "\n"; }
    { std::ofstream f(dir + "/apikey.dat"); f << ai_api_key << "\n"; }
    { std::ofstream f(dir + "/model.dat"); f << ai_model << "\n"; }
}

