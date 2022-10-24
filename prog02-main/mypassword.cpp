#define _XOPEN_SOURCE
#include <unistd.h>
#include <iostream>
#include <crypt.h>
#include <ctype.h>
#include <errno.h>
#include <pwd.h>
#include <shadow.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctime>
#include <cstdlib>
#include <random>
#include <fstream>
using namespace std;

string generate_salt() {
    unsigned long seed[2];
    char salt[] = "$6$................";
    const char *const seedchars = "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    char *password;
    int i;
    seed[0] = time(NULL);
    seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);
    for (int i = 0; i < 16; i++) {
        salt[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];    
    }
    return string(salt);
}

void remove_user(string user) {
    string line;
    ifstream ifs;
    ifs.open("/etc/shadow");
    ofstream ofs;
    ofs.open("/tmp/shadow.temp");
    while (getline(ifs, line)) {
        if (line.find(user + ":$6$") == string::npos) {
            ofs << line << endl;
        }
    }
    ofs.close();
    ifs.close();
    remove("/etc/shadow");
    rename("/tmp/shadow.temp", "/etc/shadow");
}

bool check_password(string password, string user) {
    struct passwd* entry = getpwnam(user.c_str());
    if (strcmp(entry->pw_passwd, "x") != 0) {
        return strcmp(entry->pw_passwd, crypt(password.c_str(), entry->pw_passwd));
    } else {
        struct spwd* shadow = getspnam(user.c_str());
        if (!shadow) {
            perror("fail to check");
        }
        return strcmp(shadow->sp_pwdp, crypt(password.c_str(), shadow->sp_pwdp));
    }
}

int main() {
    string salt, old_password, new_password, user = "A";
    struct spwd *sw;
    cout << "Input old password: ";
    cin >> old_password;
    if (!check_password(old_password, user)) {
        while ((sw = getspent()) != NULL) {
            string valid = sw->sp_namp;
            if(valid == user) {
                break;
            }
        };
        cout << "Set new password: ";
        cin >> new_password;
        salt = generate_salt();
        char* hash_char = crypt(new_password.c_str(), salt.c_str());
        remove_user(user);
        sw->sp_pwdp = hash_char;
        FILE* shadow_file = fopen("/etc/shadow", "a");
        if (!shadow_file) {
            perror("failed to open /etc/shadow");
            return 0;
        }
        if (putspent(sw, shadow_file) < 0) {
            perror("error");
            return 0;
        }
        cout << "Done!" << endl;
        return 0;
    }
    cout << "Wrong password!" << endl;
    return 0;
}