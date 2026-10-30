#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>
#include <limits.h>
#include <ctime>
#include <stdlib.h>
#include <sys/wait.h>

// Niels Lohmann's JSON library for C++, github.com/nlohmann/json
#include "json.hpp"
using json = nlohmann::json;

class Logger {
public:
    static void log(const std::string& msg) {
        std::string log_path = getExeDir() + "/run.log";
        std::ofstream ofs(log_path, std::ios_base::app);
        if (!ofs.is_open()) return;
        
        std::time_t now = std::time(nullptr);
        char buf[32];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
        
        ofs << "[" << buf << "] " << msg << std::endl;
    }

private:
    static std::string getExeDir() {
        char result[PATH_MAX];
        ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
        if (count != -1) {
            std::string path(result, count);
            size_t last_slash = path.find_last_of('/');
            if (last_slash != std::string::npos) {
                return path.substr(0, last_slash);
            }
        }
        return ".";
    }
};

// Detect device

class KualConfig {
public:
    static std::unordered_map<std::string, std::string> loadUserConfig(const std::string& path = "/mnt/us/extensions/KUAL/KUAL.cfg") {
        std::unordered_map<std::string, std::string> config;
        std::ifstream file(path);
        std::string line;
        
        while (std::getline(file, line)) {
            size_t delimiterPos = line.find('=');
            if (delimiterPos != std::string::npos) {
                std::string key = line.substr(0, delimiterPos);
                std::string value = line.substr(delimiterPos + 1);
                config[key] = value;
            }
        }
        return config;
    }

    static std::string getModelNickname() {
        std::string line;
        std::ifstream usid_file("/proc/usid");
        if (usid_file.is_open()) {
            std::getline(usid_file, line);
            usid_file.close();
            
            line.erase(line.find_last_not_of(" \n\r\t") + 1);
            line.erase(0, line.find_first_not_of(" \n\r\t"));

            if (line.length() >= 6) {
                char scheme = line[0];
                std::string device;
                
                if (scheme == 'B' || scheme == '9') {
                    device = line.substr(2, 2); 
                    
                    if (device == "02" || device == "03") return "K2";
                    if (device == "04" || device == "05") return "DX";
                    if (device == "09") return "DXG";
                    if (device == "08" || device == "06" || device == "0A") return "K3";
                    if (device == "0E" || device == "23") return "K4";
                    if (device == "0F" || device == "11" || device == "10" || device == "12") return "K5"; 
                    if (device == "24" || device == "1B" || device == "1D" || device == "1F" || device == "1C" || device == "20") return "PW";
                    if (device == "D4" || device == "5A" || device == "D5" || device == "D6" || device == "D7" || device == "D8" || device == "F2" || device == "17" || device == "60" || device == "F4" || device == "F9" || device == "62" || device == "61" || device == "5F") return "PW2";
                    if (device == "C6" || device == "DD") return "KT2"; 
                    if (device == "13" || device == "54" || device == "2A" || device == "4F" || device == "52" || device == "53") return "KV";
                } else {
                    device = line.substr(3, 3); 
                    
                    if (device == "0G1" || device == "0G2" || device == "0G4" || device == "0G5" || device == "0G6" || device == "0G7" || device == "0KB" || device == "0KC" || device == "0KD" || device == "0KE" || device == "0KF" || device == "0KG" || device == "0LK" || device == "0LL") return "PW3";
                    if (device == "0GC" || device == "0GD" || device == "0GR" || device == "0GS" || device == "0GT" || device == "0GU") return "KOA";
                    if (device == "0DU" || device == "0K9" || device == "0KA") return "KT3"; 
                    if (device == "0LM" || device == "0LN" || device == "0LP" || device == "0LQ" || device == "0P1" || device == "0P2" || device == "0P6" || device == "0P7" || device == "0P8" || device == "0S1" || device == "0S2" || device == "0S3" || device == "0S4" || device == "0S7" || device == "0SA") return "KOA2";
                    if (device == "0PP" || device == "0T1" || device == "0T2" || device == "0T3" || device == "0T4" || device == "0T5" || device == "0T6" || device == "0T7" || device == "0TJ" || device == "0TK" || device == "0TL" || device == "0TM" || device == "0TN" || device == "102" || device == "103" || device == "16Q" || device == "16R" || device == "16S" || device == "16T" || device == "16U" || device == "16V") return "PW4";
                    if (device == "10L" || device == "0WF" || device == "0WG" || device == "0WH" || device == "0WJ" || device == "0VB") return "KT4"; 
                    if (device == "11L" || device == "0WQ" || device == "0WP" || device == "0WN" || device == "0WM" || device == "0WL") return "KOA3";
                    if (device == "1LG" || device == "1Q0" || device == "1PX" || device == "1VD" || device == "219" || device == "21A" || device == "2BH" || device == "2BJ" || device == "2DK") return "PW5";
                    if (device == "22D" || device == "25T" || device == "23A" || device == "2AQ" || device == "2AP" || device == "1XH" || device == "22C") return "KT5"; 
                    if (device == "27J" || device == "2BL" || device == "263" || device == "227" || device == "2BM" || device == "23L" || device == "23M" || device == "270") return "KS"; 
                    if (device == "3L5" || device == "3L6" || device == "3L4" || device == "3L3" || device == "A89" || device == "3L2" || device == "3KM") return "KT6"; 
                    if (device == "349" || device == "346" || device == "33X" || device == "33W" || device == "3HA" || device == "3H5" || device == "3H3" || device == "3H8" || device == "3J5" || device == "3JS") return "PW6";
                    if (device == "3V0" || device == "3V1" || device == "3X5" || device == "3UV" || device == "3X4" || device == "3X3" || device == "41E" || device == "41D") return "KS2"; 
                    if (device == "3H9" || device == "3H4" || device == "3HB" || device == "3H6" || device == "3H2" || device == "34X" || device == "3H7" || device == "3JT" || device == "3J6" || device == "456") return "CS"; 
                }
                return "K?? [" + device + "]";
            }
        }
        return "K??";
    }

    static std::string getMenuModel() {
        std::string nickname = getModelNickname();
        if (nickname == "KS" || nickname == "KS2") return "KindleScribe";
        if (nickname == "KOA" || nickname == "KOA2" || nickname == "KOA3") return "KindleOasis";
        if (nickname == "PW" || nickname == "PW2" || nickname == "PW3" || nickname == "PW4" || nickname == "PW5" || nickname == "PW6") return "KindlePaperWhite";
        if (nickname == "KV") return "KindleVoyage";
        if (nickname == "K5") return "KindleTouch";
        if (nickname == "K4" || nickname == "KT" || nickname == "KT2" || nickname == "KT3" || nickname == "KT4" || nickname == "KT5" || nickname == "KT6") return "KindleBasic";
        return "Unknown";
    }
};

struct KualEntry {
    std::string label;
    std::string action;
    std::string params;
    std::string dir;
    int priority = 0;
    bool exitmenu = true; 
    bool isSubmenu = false;
    std::vector<KualEntry> subItems;
    bool isInternalAction = false;
    int internalAction = -1;
};

// Scan extensions directory

class ExtensionScanner {
public:
    static std::vector<KualEntry> scanExtensions(const std::string& baseDir, bool sortByName, const std::string& menuModel) {
        std::vector<KualEntry> entries;
        DIR* dir = opendir(baseDir.c_str());
        if (!dir) return entries; 

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string extName = entry->d_name;
            if (extName[0] == '.') continue; 

            std::string extPath = baseDir + extName + "/";
            struct stat statbuf;
            if (stat(extPath.c_str(), &statbuf) != 0 || !S_ISDIR(statbuf.st_mode)) continue; 

            std::string jsonPath = extPath + "menu.json";
            if (access(jsonPath.c_str(), F_OK) != -1) {
                try {
                    std::ifstream file(jsonPath);
                    json data = json::parse(file, nullptr, true, true);
                    
                    if (data.is_object()) {
                        if (data.contains("if") && data["if"].is_string()) {
                            if (!evaluateRPN(data["if"], menuModel)) continue;
                        }

                        if (data.contains("name")) {
                            KualEntry kualEntry;
                            kualEntry.label = data.value("name", "Unknown App");
                            kualEntry.action = data.value("action", "");
                            kualEntry.params = data.value("params", "");
                            kualEntry.priority = data.value("priority", 0);
                            kualEntry.exitmenu = data.value("exitmenu", true);
                            kualEntry.dir = extPath;

                            if (data.contains("items") && data["items"].is_array()) {
                                kualEntry.isSubmenu = true;
                                parseJsonItems(data["items"], extPath, kualEntry.subItems, menuModel);
                            }
                            entries.push_back(kualEntry);
                        } else if (data.contains("items") && data["items"].is_array()) {
                            parseJsonItems(data["items"], extPath, entries, menuModel);
                        }
                    } else if (data.is_array()) {
                        parseJsonItems(data, extPath, entries, menuModel);
                    }
                } catch (const json::exception& e) {
                    Logger::log("JSON Error in " + jsonPath + ": " + e.what());
                }
            }
        }
        closedir(dir);
        sortEntries(entries, sortByName);
        return entries;
    }

private:
    static bool evaluateRPN(const std::string& expr, const std::string& menuModel) {
        if (expr.empty()) return true;
        std::vector<std::string> stack;
        size_t i = 0;
        
        while (i < expr.length()) {
            if (isspace(expr[i])) { i++; continue; }
            if (expr[i] == '"') {
                size_t end = expr.find('"', i + 1);
                if (end != std::string::npos) {
                    stack.push_back(expr.substr(i + 1, end - i - 1));
                    i = end + 1;
                } else break; 
            } else if (expr.substr(i, 2) == "-m") {
                if (!stack.empty()) {
                    std::string top = stack.back(); stack.pop_back();
                    stack.push_back(top == menuModel ? "1" : "0");
                }
                i += 2;
            } else if (expr[i] == '!') {
                if (!stack.empty()) {
                    std::string top = stack.back(); stack.pop_back();
                    stack.push_back(top == "0" ? "1" : "0");
                }
                i++;
            } else {
                std::string token = "";
                while(i < expr.length() && !isspace(expr[i]) && expr[i] != '!') {
                    token += expr[i];
                    i++;
                }
                if (token == "-e" || token == "-f" || token == "-z!") {
                    if (!stack.empty()) {
                        std::string top = stack.back(); stack.pop_back();
                        stack.push_back(access(top.c_str(), F_OK) != -1 ? "1" : "0");
                    }
                }
            }
        }
        return stack.empty() ? true : (stack.back() != "0");
    }

    static void parseJsonItems(const json& itemsArray, const std::string& extDir, std::vector<KualEntry>& entries, const std::string& menuModel) {
        for (const auto& item : itemsArray) {
            if (item.contains("if") && item["if"].is_string()) {
                if (!evaluateRPN(item["if"], menuModel)) continue;
            }

            KualEntry kualEntry;
            kualEntry.label = item.value("name", "Unknown App");
            kualEntry.action = item.value("action", "");
            kualEntry.params = item.value("params", "");
            kualEntry.priority = item.value("priority", 0);
            kualEntry.exitmenu = item.value("exitmenu", true);
            kualEntry.dir = extDir;

            if (item.contains("items") && item["items"].is_array()) {
                kualEntry.isSubmenu = true;
                parseJsonItems(item["items"], extDir, kualEntry.subItems, menuModel);
            }
            entries.push_back(kualEntry);
        }
    }

    static void sortEntries(std::vector<KualEntry>& entries, bool sortByName) {
        std::sort(entries.begin(), entries.end(), [sortByName](const KualEntry& a, const KualEntry& b) {
            if (sortByName) {
                return a.label < b.label;
            } else {
                if (a.priority != b.priority) return a.priority < b.priority;
                return a.label < b.label;
            }
        });
        for (auto& entry : entries) {
            if (entry.isSubmenu) sortEntries(entry.subItems, sortByName);
        }
    }
};

class KualApp {
private:
    GtkWidget* window;
    GtkWidget* root_vbox;
    GtkWidget* main_hbox;
    GtkWidget* entries_vbox;
    GtkWidget* breadcrumb_label;
    GtkWidget* status_label;
    GtkWidget* prev_button;
    GtkWidget* next_button;
    
    std::unordered_map<std::string, std::string> userConfig;
    std::vector<KualEntry> rootEntries;
    std::vector<KualEntry*> currentMenu;
    std::vector<std::vector<KualEntry*>> menuStack;
    std::vector<std::string> breadcrumbStack;
    std::vector<int> offsetStack;
    
    int currentOffset = 0;
    int pageSize = 10;
    bool sortByName = false;
    
    std::string privilegeHint = "%";
    std::string deviceNickname = "K??";
    std::string menuModel = "Unknown";
    
    std::string commandToRunOnExit = "";
    std::string paramsToRunOnExit = "";
    std::string dirToRunOnExit = "";

    KualEntry upEntry;
    KualEntry quitEntry;

    const std::string BULLET = "▪";

public:
    KualApp() {
        upEntry.label = "/";
        upEntry.isInternalAction = true;
        upEntry.internalAction = 1;

        quitEntry.label = "× Quit";
        quitEntry.isInternalAction = true;
        quitEntry.internalAction = 2;
    }

    void run(int argc, char *argv[]) {
        setenv("DISPLAY", ":0", 1);
        gtk_init(&argc, &argv);
        
        initializeState();
        refreshData();
        initializeUI();
        
        gtk_widget_show_all(window);
        gtk_window_present(GTK_WINDOW(window));
        forceEinkRefresh();
        
        gtk_main();

        if (!commandToRunOnExit.empty()) {
            execute(commandToRunOnExit, paramsToRunOnExit, dirToRunOnExit, true);
            usleep(175000);
        }
    }

private:
    void forceEinkRefresh() {
        while (gtk_events_pending()) gtk_main_iteration();
        system("/usr/sbin/eips -f >/dev/null 2>&1");
    }

    void initializeState() {
        userConfig = KualConfig::loadUserConfig();
        if (userConfig.count("page_size")) pageSize = std::stoi(userConfig["page_size"]);

        std::string user = getenv("USER") ? getenv("USER") : "";
        if (user == "root") privilegeHint = "#";
        else if (access("/var/local/mkk/gandalf", F_OK) != -1) privilegeHint = "$";

        deviceNickname = KualConfig::getModelNickname();
        menuModel = KualConfig::getMenuModel();
    }

    void refreshData() {
        rootEntries = ExtensionScanner::scanExtensions("/mnt/us/extensions/", sortByName, menuModel);

        KualEntry kualSelfMenu;
        kualSelfMenu.label = "KUAL"; 
        kualSelfMenu.isSubmenu = true;
        kualSelfMenu.priority = -1000; 
        
        KualEntry kualSort;
        kualSort.label = sortByName ? "Sort menu abc" : "Sort menu 123";
        kualSort.isInternalAction = true;
        kualSort.internalAction = 3;
        
        KualEntry kualLog;
        kualLog.label = "Save and reset KUAL log";
        kualLog.isInternalAction = true;
        kualLog.internalAction = 4;

        kualSelfMenu.subItems.push_back(kualSort);
        kualSelfMenu.subItems.push_back(kualLog);
        kualSelfMenu.subItems.push_back(quitEntry);
        kualSelfMenu.subItems.push_back(upEntry); 
        
        rootEntries.insert(rootEntries.begin(), kualSelfMenu);

        buildPointerList(rootEntries, currentMenu);
    }

    void buildPointerList(std::vector<KualEntry>& source, std::vector<KualEntry*>& dest) {
        dest.clear();
        for (auto& entry : source) dest.push_back(&entry);
    }

    void initializeUI() {
        gtk_rc_parse_string(
            "style \"kual_white_buttons\" {\n"
            "  bg[NORMAL] = \"#FFFFFF\"\n"
            "  bg[PRELIGHT] = \"#F5F5F5\"\n"
            "  bg[ACTIVE] = \"#EBEBEB\"\n"
            "}\n"
            "widget \"*GtkButton*\" style \"kual_white_buttons\"\n"
        );

        window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "L:A_N:application_PC:T_ID:com.mobileread.kual");
        gtk_window_fullscreen(GTK_WINDOW(window)); 
        gtk_container_set_border_width(GTK_CONTAINER(window), 10);
        
        GdkColor whiteBg;
        gdk_color_parse("#FFFFFF", &whiteBg);
        gtk_widget_modify_bg(window, GTK_STATE_NORMAL, &whiteBg);
        
        g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

        root_vbox = gtk_vbox_new(FALSE, 10);
        gtk_container_add(GTK_CONTAINER(window), root_vbox);

        breadcrumb_label = gtk_label_new("");
        gtk_misc_set_alignment(GTK_MISC(breadcrumb_label), 0.0, 0.5);
        gtk_box_pack_start(GTK_BOX(root_vbox), breadcrumb_label, FALSE, FALSE, 0);

        main_hbox = gtk_hbox_new(FALSE, 10);
        gtk_box_pack_start(GTK_BOX(root_vbox), main_hbox, TRUE, TRUE, 0);

        prev_button = gtk_button_new_with_label(" ◀ ");
        gtk_widget_set_size_request(prev_button, 70, -1);
        g_signal_connect(prev_button, "clicked", G_CALLBACK(on_prev_clicked), this);
        gtk_box_pack_start(GTK_BOX(main_hbox), prev_button, FALSE, TRUE, 0);

        entries_vbox = gtk_vbox_new(FALSE, 15); 
        gtk_box_pack_start(GTK_BOX(main_hbox), entries_vbox, TRUE, TRUE, 20);

        next_button = gtk_button_new_with_label(" ▶ ");
        gtk_widget_set_size_request(next_button, 70, -1);
        g_signal_connect(next_button, "clicked", G_CALLBACK(on_next_clicked), this);
        gtk_box_pack_end(GTK_BOX(main_hbox), next_button, FALSE, TRUE, 0);

        status_label = gtk_label_new("Status");
        gtk_box_pack_start(GTK_BOX(root_vbox), status_label, FALSE, FALSE, 0);

        breadcrumbStack.push_back(""); 
        updateDisplayedLaunchers();
    }

    void updateDisplayedLaunchers() {
        GList *children = gtk_container_get_children(GTK_CONTAINER(entries_vbox));
        for (GList *iter = children; iter != NULL; iter = g_list_next(iter)) {
            gtk_widget_destroy(GTK_WIDGET(iter->data));
        }
        g_list_free(children);

        int end = currentOffset;
        int totalItems = currentMenu.size() + 1; 

        for (int i = 0; i < pageSize; ++i) {
            KualEntry* ke = nullptr;
            if (currentOffset + i < currentMenu.size()) {
                ke = currentMenu[currentOffset + i];
            } else if (currentOffset + i == currentMenu.size()) {
                ke = menuStack.empty() ? &quitEntry : &upEntry; 
            } else {
                break; 
            }
            
            std::string btnText = ke->label;
            if (ke->isSubmenu) btnText += " \u25BD";
            
            GtkWidget* btn = gtk_button_new_with_label(btnText.c_str());
            gtk_widget_set_size_request(btn, -1, 120);
            g_object_set_data(G_OBJECT(btn), "entry", ke);
            g_signal_connect(btn, "clicked", G_CALLBACK(on_launcher_clicked), this);
            gtk_box_pack_start(GTK_BOX(entries_vbox), btn, FALSE, FALSE, 0);
            end++;
        }
        gtk_widget_show_all(entries_vbox);

        std::string currentPath = privilegeHint + " " + BULLET + " ";
        bool first = true;
        for (const auto& p : breadcrumbStack) {
            if (p.empty()) {
                currentPath += "/";
            } else {
                if (!first && currentPath.back() != '/') currentPath += "/";
                currentPath += p;
            }
            first = false;
        }
        gtk_label_set_text(GTK_LABEL(breadcrumb_label), currentPath.c_str());

        if (userConfig["no_show_status"] != "true") {
            setStatus("Entries " + std::to_string(currentOffset + 1) + " - " + 
                      std::to_string(end) + " of " + std::to_string(totalItems) + 
                      " " + BULLET + " v4.0 C++ " + BULLET + " " + deviceNickname);
        }

        gtk_widget_set_sensitive(prev_button, currentOffset > 0);
        gtk_widget_set_sensitive(next_button, end < totalItems);
    }

    void handlePaging(int direction) {
        int newOffset = currentOffset + pageSize * direction;
        int totalItems = currentMenu.size() + 1;
        if (newOffset < 0) newOffset = 0;
        if (newOffset >= totalItems) return; 
        
        if (newOffset != currentOffset) {
            currentOffset = newOffset;
            updateDisplayedLaunchers();
            forceEinkRefresh(); 
        }
    }

    void handleLauncherButton(KualEntry* ke) {
        if (!ke) return;

        if (ke->isSubmenu) {
            menuStack.push_back(currentMenu);
            offsetStack.push_back(currentOffset);
            breadcrumbStack.push_back(ke->label);
            buildPointerList(ke->subItems, currentMenu);
            currentOffset = 0;
            updateDisplayedLaunchers();
            forceEinkRefresh(); 
            return;
        }

        if (ke->isInternalAction) {
            if (ke->internalAction == 1) { 
                if (!menuStack.empty()) {
                    currentMenu = menuStack.back();
                    currentOffset = offsetStack.back();
                    menuStack.pop_back();
                    offsetStack.pop_back();
                    breadcrumbStack.pop_back();
                    updateDisplayedLaunchers();
                    forceEinkRefresh(); 
                }
            } else if (ke->internalAction == 2) { 
                gtk_main_quit(); 
            } else if (ke->internalAction == 3) {
                sortByName = !sortByName;
                menuStack.clear();
                breadcrumbStack.clear();
                offsetStack.clear();
                currentOffset = 0;
                refreshData();
                updateDisplayedLaunchers();
                setStatus(sortByName ? "Sorted alphabetically." : "Sorted by priority.");
                forceEinkRefresh();
            } else if (ke->internalAction == 4) {
                system("mv /var/tmp/KUAL.log /var/tmp/KUAL.log.old 2>/dev/null; echo 'KUAL Log Cleared' > /var/tmp/KUAL.log");
                setStatus("Log saved and reset.");
                forceEinkRefresh();
            }
            return;
        }
        
        if (!ke->action.empty()) {
            killKnownOffenders();
            setStatus("Launching: " + ke->label);
            
            if (ke->exitmenu) {
                commandToRunOnExit = ke->action;
                paramsToRunOnExit = ke->params;
                dirToRunOnExit = ke->dir;
                gtk_main_quit(); 
            } else {
                execute(ke->action, ke->params, ke->dir, true);
            }
        }
    }

    void execute(const std::string& cmd, const std::string& params, const std::string& dir, bool background) {
        if (!dir.empty() && access(dir.c_str(), F_OK) == -1) {
            setStatus("Directory not found: " + dir);
            return;
        }

        std::string full_target = cmd;
        if (!params.empty()) {
            full_target += " " + params;
        }

        char script_path[] = "/tmp/klauncher_background-XXXXXX.sh";
        int fd = mkstemps(script_path, 3);
        if (fd == -1) {
            Logger::log("Failed to create temporary script file");
            return;
        }

        std::string script_content = "#!/bin/ash\n{ " + full_target + " ; } 2>>/var/tmp/KUAL.log" + (background ? " &\n" : "\n");
        write(fd, script_content.c_str(), script_content.length());
        close(fd);

        chmod(script_path, 0755);

        std::string exec_cmd;
        if (privilegeHint == "$") {
            exec_cmd = "/var/local/mkk/su -s /bin/ash -c '" + std::string(script_path) + "'";
        } else {
            exec_cmd = "/bin/sh '" + std::string(script_path) + "'";
        }

        Logger::log("Executing scriptlet: " + exec_cmd + " in dir: " + dir);

        std::string currentDir = get_current_dir_name();
        if (!dir.empty()) chdir(dir.c_str());
        system(exec_cmd.c_str());
        if (!dir.empty()) chdir(currentDir.c_str());
    }

    void killKnownOffenders() {
        system("/usr/bin/killall matchbox-keyboard kterm skipstone cr3 2>/dev/null");
    }

    void setStatus(const std::string& text) {
        gtk_label_set_text(GTK_LABEL(status_label), text.c_str());
    }

    static void on_prev_clicked(GtkWidget* widget, gpointer data) {
        static_cast<KualApp*>(data)->handlePaging(-1);
    }
    static void on_next_clicked(GtkWidget* widget, gpointer data) {
        static_cast<KualApp*>(data)->handlePaging(1);
    }
    static void on_launcher_clicked(GtkWidget* widget, gpointer data) {
        KualApp* app = static_cast<KualApp*>(data);
        KualEntry* ke = static_cast<KualEntry*>(g_object_get_data(G_OBJECT(widget), "entry"));
        app->handleLauncherButton(ke);
    }
};

int main(int argc, char *argv[]) {
    KualApp app;
    app.run(argc, argv);
    return 0;
}