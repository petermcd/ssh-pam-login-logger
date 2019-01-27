#SSH PAM Login Logger

This PAM module rejects any login attempts and logs the credentials that have been attempted. The module has extremely limited testing and should be used at your own risk.

The intention of this module is to act as a honeypot, capturing login credentials and disallowing the login.

##Requirements

The module relies on the following 2 dev packages to compile:

*libsqlite3-dev
*libpam0g-dev

The module also logs to SQLite, therefore, the following package is required:

*sqlite3

##Compilation

I have not provided a make file for this project however I have included a shell script (compile.sh) which compiles the project. You may need to modify paths to get this to work for you.

##Installation

After compiling, ssh_honeypot.so will be placed into /lib/security/, this will make the module available to PAM.

To configure PAM, first, locate the configuration file for the service in question. This is likely located in /etc/pam.d/. The file will be named after the service such as sshd. Place the following content into the file:

```
auth       required     ssh_honeypot.so
account    required     ssh_honeypot.so
session    required     ssh_honeypot.so
password   required     ssh_honeypot.so
```

##Testing

To ensure this is working try connecting to SSH `ssh 127.0.0.1`, attempt to log in.

Once login fails run the following commands:

```
sqlite3 /var/log/login-attempts.db
select * from logins;
```

You should see the login attempts that you have made. In the event that you cannot see any output check the following file:

```
/var/log/auth.log
```

This should give details on what the problem is. For example:

```
Jan 27 01:56:45 Dev-Laptop sshd[4946]: PAM unable to dlopen(ssh_honeypot.so): /lib/security/ssh_honeypot.so: cannot open shared object file: No such file or directory
Jan 27 01:56:45 Dev-Laptop sshd[4946]: PAM adding faulty module: ssh_honeypot.so
Jan 27 01:56:46 Dev-Laptop sshd[4946]: Failed password for peter from 127.0.0.1 port 50160 ssh2
Jan 27 01:56:49 Dev-Laptop sshd[4946]: message repeated 2 times: [ Failed password for peter from 127.0.0.1 port 50160 ssh2]
Jan 27 01:56:49 Dev-Laptop sshd[4946]: Connection closed by authenticating user peter 127.0.0.1 port 50160 [preauth]

```

If the module is working you will not see the first 2 lines. The first line tries to give an indication of a problem.

###What Next

1. Remove hardcoded link to the location of the database file.
2. Create a make file.
3. Make cross-platform.