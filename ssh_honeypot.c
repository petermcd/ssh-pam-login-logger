#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

#include <sqlite3.h>

#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>

//===========  General  ==========//

int store_login_details(const char *username, const char *password, const char *host){
    sqlite3 *handler = NULL;
    const char *databaseFile = "/var/log/login-attempts.db";
    int error;
    error = sqlite3_open(databaseFile, &handler);
    if(error){
        fprintf(stderr, "\n\rfailed to connect to db\n\r");
        return 1;
    }

    //Create the table
    sqlite3_stmt *stmt = NULL;
    const char *createDatabaseSQL = "CREATE TABLE IF NOT EXISTS \"logins\" (\"id\"	INTEGER PRIMARY KEY AUTOINCREMENT, \"username\"	TEXT DEFAULT \"\", \"password\"	TEXT DEFAULT \"\", \"host\"	TEXT DEFAULT \"\");";
    error = sqlite3_prepare(handler, createDatabaseSQL, -1, &stmt, 0);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    
    //Insert the details
    stmt = NULL;
    const char *sql = "INSERT INTO logins (username, password, host) VALUES (?, ?, ?);";
    error = sqlite3_prepare(handler, sql, -1, &stmt, 0);
    if(error){
        fprintf(stderr, "failed to prepare statement\n");
        fprintf(stderr, "%s", sqlite3_errmsg(handler));
        return 1;
    }

    error = sqlite3_bind_text(stmt, 1, username, strlen(username), SQLITE_STATIC);
    if(error){
        fprintf(stderr, "failecd to assign username\n");
        fprintf(stderr, "%s", sqlite3_errmsg(handler));
        return 1;
    }
    error = sqlite3_bind_text(stmt, 2, password, strlen(password), SQLITE_STATIC);
    if(error){
        fprintf(stderr, "failecd to assign password\n");
        fprintf(stderr, "%s", sqlite3_errmsg(handler));
        return 1;
    }
    error = sqlite3_bind_text(stmt, 3, host, strlen(host), SQLITE_STATIC);
    if(error){
        fprintf(stderr, "failecd to assign host\n");
        fprintf(stderr, "%s", sqlite3_errmsg(handler));
        return 1;
    }

    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
    sqlite3_close(handler);
    return 0;
}

//============  Auth  ============//

PAM_EXTERN int pam_sm_authenticate(pam_handle_t *pamh, int flags, int argc, const char **argv){
    int pam_code;
    const char *username = NULL;
    const char *password = NULL;

    pam_code = pam_get_user(pamh, &username, "USERNAME: ");
    if(pam_code != PAM_SUCCESS){
        fprintf(stderr, "Unable to retrive username");
        return PAM_PERM_DENIED;
    }

    pam_code = pam_get_authtok(pamh, PAM_AUTHTOK, &password, "PGimme ASSWORD: ");
    if(pam_code != PAM_SUCCESS){
        fprintf(stderr, "Unable to retrive PASSWORD");
        return PAM_PERM_DENIED;
    }

    const char *host = NULL;
    pam_get_item(pamh, PAM_RHOST, (void *) &host);

    // write credential out to file or http endpoint
    store_login_details(username, password, host);

    //Reject all requests
    return PAM_PERM_DENIED;
}

PAM_EXTERN int pam_sm_setcred(pam_handle_t *pamh, int flags, int argc, const char **argv){
    return PAM_SERVICE_ERR;
}
PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv){
    return PAM_SERVICE_ERR;
}
PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv){
    return PAM_SERVICE_ERR;
}
PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv){
    return PAM_SERVICE_ERR;
}
PAM_EXTERN int pam_sm_chauthtok(pam_handle_t *pamh, int flags, int argc, const char **argv){
    return PAM_SERVICE_ERR;
}