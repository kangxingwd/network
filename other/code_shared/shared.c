
#define log_debug printf

/* 获取当前进程的进程名称， 
1、获取pid； 
2、从 /proc/pid/status 文件中获取进程名称
*/



// daemon 进程， 单例
int is_myself_instance_running()
{
    int ret = 0;

    static FILE* out = NULL;
    char name[128];
    (void) sprintf(name, sizeof(name), "/var/run/%s.lock", g_proc_name);  // g_proc_name 是当前进程名称

    out = fopen(name, "w");
    if(!out) {
        log_debug("open file %s failed!\n", name);
        exit(1);
    }

    int out_fd = fileno(out);

    ret = file_write_trylock(out_fd);
    if (ret < 0) {
        log_debug("file %s write lock already set!\n", name);
        fclose(out);
    } else {
        fprintg(out, "%d", getpid());
        fflush(out);
        log_debug("file %s write lock set ok!\n", name);
    }

    return ret;
}

// daemon 进程

static int32_t daemon_init()
{
    pid_t pid, sid;
    int no_daemon = 0;
    if (getenv("NO_DAEMON")) {
        no_daemon = 1;
    }

    if (!no_daemon) {
        pid = fork();

        if (pid < 0) {
            log_debug("create sub process failed!, err = %d\n", pid);
            return -1;
        }

        if (pid > 0) {
            exit(0);
        }

        umask(0);

        sid = setsid();
        if (sid < 0) {
            log_debug("create a net session for a process failed!, err = %d\n", sid);
            return -1;
        }
    }

    if ((chdir("/")) < 0 ) {
        log_debug("get working dir failed!\n");
        return -1;
    }

    fclose(stdin);
    return 0;
}

// 
static int32_t set_fd_limits(int32_t fdlimit)
{
    assert(fdlimit > 0);
    struct rlimit limits;

    limits.rlim_cur = fdlimit;
    limits.rlim_max = fdlimit;
    if (setrlimit(RLIMIT_NOFILE, &limits) < 0) {
        log_debug("set system fd limit failed: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}


