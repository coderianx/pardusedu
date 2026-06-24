#include "parduslab.h"
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cstring>
#include <thread>
#include <chrono>
#include <nlohmann/json.hpp>

PardusLab::PardusLab() {
    std::string ver = exec_podman("--version");
    podman_found = !ver.empty();
    if (podman_found) podman_ver = ver;
}

PardusLab::~PardusLab() {
    cleanup_all();
}

std::string PardusLab::exec_podman(const std::string& args) {
    std::string cmd = "podman " + args + " 2>/dev/null";
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), pipe))
        result += buffer;
    pclose(pipe);
    if (!result.empty() && result.back() == '\n')
        result.pop_back();
    return result;
}

std::string PardusLab::generate_id() {
    auto now = std::time(nullptr);
    auto* tm = std::localtime(&now);
    char buf[64];
    strftime(buf, sizeof(buf), "%Y%m%d_%H%M%S", tm);
    return std::string(buf);
}

std::string PardusLab::start_container(const std::string& image) {
    if (!containers.empty() && containers.back().running)
        return containers.back().container_id;

    std::string id = exec_podman(
        "run -d -t --name parduslab_" + generate_id() +
        " " + image + " /bin/bash -c 'while true; do sleep 3600; done'"
    );
    if (id.empty()) return "";

    for (int i = 0; i < 15; i++) {
        std::string cmd = "podman exec " + id + " true >/dev/null 2>&1";
        if (system(cmd.c_str()) == 0) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    std::string script_path = "/tmp/pardus_setup_" + generate_id() + ".sh";
    {
        std::ofstream f(script_path);
        f << "#!/bin/bash\n";
        f << "DEBIAN_FRONTEND=noninteractive apt-get update -qq 2>/dev/null\n";
        f << "DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends sudo nano 2>/dev/null\n";
        f << "useradd -m parduslab 2>/dev/null\n";
        f << "printf '%s:%s' parduslab parduslab123 | chpasswd\n";
        f << "usermod -aG sudo parduslab 2>/dev/null\n";
        f << "cat > /home/parduslab/.bashrc <<'EOF'\n";
        f << "if ! command -v sudo >/dev/null 2>&1; then\n";
        f << "    echo \"PardusLab kuruluyor... lutfen bekleyin.\"\n";
        f << "fi\n";
        f << "EOF\n";
        f << "chown parduslab:parduslab /home/parduslab/.bashrc\n";
        f << "touch /tmp/setup_done\n";
        f.close();
    }

    exec_podman("cp " + script_path + " " + id + ":/setup.sh");
    exec_podman("exec -d " + id + " bash /setup.sh");
    remove(script_path.c_str());

    LabContainer c;
    c.container_id = id;
    c.image = image;
    c.running = true;
    c.shell = "/bin/bash";
    containers.push_back(c);
    return id;
}

bool PardusLab::stop_container(const std::string& id) {
    std::string r = exec_podman("stop " + id);
    for (auto& c : containers)
        if (c.container_id == id) c.running = false;
    return !r.empty();
}

bool PardusLab::remove_container(const std::string& id) {
    exec_podman("rm -f " + id);
    for (size_t i = 0; i < containers.size(); ++i)
        if (containers[i].container_id == id) {
            containers.erase(containers.begin() + i);
            break;
        }
    return true;
}

bool PardusLab::restart_container(const std::string& id) {
    stop_container(id);
    std::string r = exec_podman("start " + id);
    for (auto& c : containers)
        if (c.container_id == id) c.running = !r.empty();
    return !r.empty();
}

bool PardusLab::is_running(const std::string& id) {
    std::string r = exec_podman("inspect -f '{{.State.Running}}' " + id);
    return r == "true";
}

std::string PardusLab::get_status(const std::string& id) {
    return exec_podman("inspect -f '{{.State.Status}}' " + id);
}

std::string PardusLab::exec_command(const std::string& container_id,
                                     const std::string& command) {
    std::string escaped;
    for (char ch : command) {
        if (ch == '\'') escaped += "'\\''";
        else if (ch == '\\') escaped += "\\\\";
        else if (ch == '"') escaped += "\\\"";
        else if (ch == '`') escaped += "\\`";
        else if (ch == '$') escaped += "\\$";
        else if (ch == '\n') escaped += " ";
        else escaped += ch;
    }
    return exec_podman(
        "exec -u parduslab " + container_id + " /bin/bash -c '" + escaped + "'"
    );
}

std::string PardusLab::snapshot(const std::string& container_id,
                                 const std::string& tag) {
    return exec_podman("commit " + container_id + " " + tag);
}

bool PardusLab::copy_to_container(const std::string& container_id,
                                   const std::string& host_path,
                                   const std::string& container_path) {
    std::string result = exec_podman(
        "cp " + host_path + " " + container_id + ":" + container_path
    );
    return result.empty();
}

std::string PardusLab::read_file(const std::string& container_id,
                                  const std::string& path) {
    return exec_command(container_id, "cat " + path);
}

void PardusLab::load_challenges(const std::string& json_path) {
    std::ifstream f(json_path);
    if (!f.is_open()) return;
    nlohmann::json j;
    try { f >> j; } catch (...) { return; }
    challenges.clear();
    for (auto& item : j["challenges"]) {
        Challenge c;
        c.id = item["id"];
        c.title = item["title"];
        c.description = item["description"];
        c.hint = item["hint"];
        c.expected_output = item["expected_output"];
        c.command_to_test = item["command_to_test"];
        c.badge_id = item.value("badge_id", "");
        c.xp = item.value("xp", 0);
        challenges.push_back(c);
    }
}

PardusLab::Challenge PardusLab::get_challenge(const std::string& id) {
    for (auto& c : challenges)
        if (c.id == id) return c;
    return {};
}

bool PardusLab::check_challenge(const std::string& container_id,
                                 const std::string& challenge_id) {
    Challenge c = get_challenge(challenge_id);
    if (c.id.empty()) return false;
    std::string output = exec_command(container_id, c.command_to_test);
    std::regex pattern(c.expected_output);
    return std::regex_search(output, pattern);
}

void PardusLab::cleanup_all() {
    for (auto& c : containers) {
        exec_podman("rm -f " + c.container_id);
    }
    containers.clear();

    std::string leftover = exec_podman("ps -aq --filter name=parduslab_");
    if (!leftover.empty())
        exec_podman("rm -f $(podman ps -aq --filter name=parduslab_)");
}
