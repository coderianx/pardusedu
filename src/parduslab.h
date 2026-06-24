#pragma once
#include <string>
#include <vector>
#include <functional>
#include <regex>
#include <ctime>

struct LabContainer {
    std::string container_id;
    std::string image;
    bool running = false;
    std::string shell;
};

class PardusLab {
public:
    PardusLab();
    ~PardusLab();

    std::string start_container(const std::string& image = "ubuntu:latest");
    bool stop_container(const std::string& id);
    bool remove_container(const std::string& id);
    bool restart_container(const std::string& id);

    bool is_running(const std::string& id);
    std::string get_status(const std::string& id);

    std::string exec_command(const std::string& container_id,
                             const std::string& command);

    std::string snapshot(const std::string& container_id,
                         const std::string& tag);

    struct Challenge {
        std::string id;
        std::string title;
        std::string description;
        std::string hint;
        std::string expected_output;
        std::string command_to_test;
        std::string badge_id;
        int xp = 0;
    };

    void load_challenges(const std::string& json_path);
    Challenge get_challenge(const std::string& id);
    bool check_challenge(const std::string& container_id,
                         const std::string& challenge_id);

    bool copy_to_container(const std::string& container_id,
                           const std::string& host_path,
                           const std::string& container_path);
    std::string read_file(const std::string& container_id,
                          const std::string& path);

    bool podman_available() const { return podman_found; }
    std::string podman_version() const { return podman_ver; }

    void cleanup_all();

    std::string exec_podman(const std::string& args);

private:
    std::vector<LabContainer> containers;
    std::vector<Challenge> challenges;
    bool podman_found = false;
    std::string podman_ver;

    std::string generate_id();
};
