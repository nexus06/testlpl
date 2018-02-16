/bin/rm -rf <%= prefix %>/<%= app_name %> stop || true
/bin/rm /etc/init/<%= app_name %>* || true
/bin/rm /etc/init.d/<%= app_name %>* || true