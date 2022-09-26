#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
/*
struct passwd {
    char *pw_name;
    char *pw_passwd;
    uid_t pw_uid;
    gid_t pw_gid;
    time_t pw_change;
    char *pw_class;
    char *pw_gecos;
    char *pw_dir;
    char *pw_shell;
    time_t pw_expire;
}; 
struct group {
    char *gr_name;
    char *gr_passwd;
    gid_t gr_gid;
    char **gr_mem;
};
*/
int main() {
    char user[100];
    printf("Enter a username: ");
    scanf("%s", user);
    struct passwd *p_entry1;
    struct group *p_entry2;
    
    p_entry1 = getpwnam(user);
    //Check user
    if(!p_entry1) {
        printf("User %s not found!\n", user);
        exit(EXIT_SUCCESS);
    }
    printf("User ID: %d", p_entry1->pw_uid);
    printf("\nUsername: %s", p_entry1->pw_name);
    printf("\nHome Directory: %s", p_entry1->pw_dir);

    int ngroups = 0; // count the number of groups
    getgrouplist(p_entry1->pw_name, p_entry1->pw_gid, NULL, &ngroups); //Success; the function fills in the group array and sets *ngroups to the number of groups found
    gid_t groups[ngroups]; // Array of GroupID
    getgrouplist(p_entry1->pw_name, p_entry1->pw_gid, groups, &ngroups);
    
    printf("\nGroups: ");
    for (i = 0; i < ngroups; i++) {
        p_entry2 = getgrgid(groups[i]); 
        if (p_entry2 != NULL)
            printf("%s ", p_entry2->gr_name);
    }
    printf("\n");       
    exit(EXIT_SUCCESS);
}
