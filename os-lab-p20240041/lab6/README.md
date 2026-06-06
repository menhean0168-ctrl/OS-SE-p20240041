Lab 6 — Linux Security, Users, Groups & File Permissions

Student Name: Pong Mengheang
Student ID: p20240041


Task Output Files

 task1_users.txt
 task2_groups.txt
 task3_permissions.txt
 task3_stat_output.txt
 task4_special_bits.txt
 task5_acl.txt
 security_lab/whoami_suid.c


Screenshots
Screenshot 1 — Task 1: User Creation
Show Image
Screenshot 2 — Task 1: User Modification
Show Image
Screenshot 3 — Task 2: Group Setup
Show Image
Screenshot 4 — Task 2: Multiple Group Membership
Show Image
Screenshot 5 — Task 3: Directory Permissions
Show Image
Screenshot 6 — Task 3: Access Denied
Show Image
Screenshot 7 — Task 4: setgid Bit
Show Image
Screenshot 8 — Task 4: Sticky Bit
Show Image
Screenshot 9 — Task 4: setuid Bit
Show Image
Screenshot 10 — Task 5: ACL Directory
Show Image
Screenshot 11 — Task 5: ACL Access Test
Show Image
Screenshot 12 — Task 5: ACL Output File
Show Image

Answers to Lab Questions

What is the difference between userdel and userdel -r?

userdel removes only the user account. userdel -r also deletes the user's home directory and mail spool.


Why is it safer to use visudo instead of directly editing /etc/sudoers?

visudo validates the syntax before saving. A syntax error in /etc/sudoers can lock all users out of sudo, so direct editing is risky.


What happens when a setgid directory contains files created by different users? What benefit does this provide for team collaboration?

New files automatically inherit the directory's group instead of the creator's primary group. This ensures all team members can access each other's files without manual chown.


What limitation of standard Unix permissions does the ACL system solve?

Standard Unix permissions only allow one owner and one group per file. ACLs allow fine-grained access for multiple specific users or groups without changing the file's primary ownership.
