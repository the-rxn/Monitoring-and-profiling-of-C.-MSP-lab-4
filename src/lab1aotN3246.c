#define _XOPEN_SOURCE 500 // for nftw()
#define _DEFAULT_SOURCE
#define DF "LAB1DEBUG" // DF - Debug Flag
#define UNUSED(x)(void)(x)

#include <stdlib.h>

#include <unistd.h>

#include <string.h>

#include <stdio.h>

#include <ftw.h>

#include <getopt.h>

#include <dlfcn.h>

#include <errno.h>

#include <alloca.h>

#include <sys/types.h>

#include <dirent.h>

#include <limits.h>

#include "plugin_api.h"

// -------------------------------------------------------------------------------
// App info
// -------------------------------------------------------------------------------

static char * g_program_name = "lab1aotN3246";
static char * g_version = "1.0";
static char * g_author = "Tsydypov A.O.";
static char * g_group = "N3246";
static char * g_lab_variant = "19";

// -------------------------------------------------------------------------------
// Flags for multiple plugins
// -------------------------------------------------------------------------------

static int A = 0;
static int O = 0;
static int N = 0;

// -------------------------------------------------------------------------------
// API vars
// -------------------------------------------------------------------------------

// Found working plugins
static void ** plughandles = NULL;
static int n_plugs = 0;

// Found options from plugins
static struct option * plugs_opts = NULL;
static int n_plugs_opts = 0;

// Supplied ammount of options by a user 
static int n_user_opts = 0;

// Sorted plugin options
static struct option ** sorted_user_opts = NULL;
static int * n_sorted_user_opts = NULL;

// Plugin_process_file array
static char ** longopt_name = NULL;
static int n_longopt_name = 0;

static void ** ppf_func_arr = NULL;
// -------------------------------------------------------------------------------
// Private functions
// -------------------------------------------------------------------------------

void app_info();
void app_help();
int plug_info(const char * ,
    const struct stat * , int, struct FTW * );
int check_func(const char * ,
    const struct stat * , int, struct FTW * );

// -------------------------------------------------------------------------------
// Typedefs
// -------------------------------------------------------------------------------

typedef int( * ppf_func_t)(const char * , struct option * , size_t);
typedef int( * pgi_func_t)(struct plugin_info * );

// -------------------------------------------------------------------------------
//                                  DA FOKIN' APP
// -------------------------------------------------------------------------------

int main(int argc, char * argv[]) {
    char * DEBUG = getenv(DF);
    char * plugin_path_dir = NULL;

    //Look for -P option 
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-P") == 0) {
            if (!(plugin_path_dir = (char * ) malloc(strlen(argv[i + 1] + 1)))) {
                fprintf(stderr, "ERROR: [main()] memory allocation failed: %s\n", strerror(errno));
                goto FREE;
            }
            strcpy(plugin_path_dir, argv[i + 1]);
            break;
        }
    }

    if (!plugin_path_dir) {
        if (!(plugin_path_dir = (char * ) malloc(3))) {
            fprintf(stderr, "ERROR: [main()] memory allocation failed: %s\n", strerror(errno));
            goto FREE;
        }
        strcpy(plugin_path_dir, "./");

    }

    // Look for available plugins
    if (nftw(plugin_path_dir, plug_info, 10, FTW_PHYS) < 0) {
        perror("nftw");
        app_help();
        goto FREE;
    }

    if (!plugs_opts) fprintf(stdout, "WARNING: [main()] no long options supplied.\n");

    if ((n_sorted_user_opts = (int * ) calloc(n_plugs, sizeof(int))) == NULL) {
        fprintf(stderr, "ERROR: calloc() failed %s\n", strerror(errno));
        goto FREE;
    }
    if (DEBUG) fprintf(stdout, "DEBUG:  n_plugs = %d\n", n_plugs);
    if (!(sorted_user_opts = (struct option ** ) calloc((n_plugs), sizeof(struct option * )))) {
        fprintf(stderr, "ERROR: [main()] memory allocation failed: %s\n", strerror(errno));
        goto FREE;
    }

    int c, index;

    while ((c = getopt_long(argc, argv, "P:AONvh", plugs_opts, & index)) != -1) {
        if (DEBUG) fprintf(stdout, "DEBUG: c = %d n_plugs_opts = %d\n", c, n_plugs_opts);
        if ((c > 0) && (c < n_plugs_opts)) {
            if (DEBUG) fprintf(stdout, "We are in!\n");

            if (!(longopt_name = realloc(longopt_name, (n_longopt_name + 1) * sizeof(char * )))) {
                fprintf(stderr, "ERROR: [main()] realloc() failed %s\n", strerror(errno));
                goto FREE;
            }
            n_longopt_name += 1;
            if (!(longopt_name[n_longopt_name - 1] = calloc(strlen(argv[optind - 2]), sizeof(char)))) {
                fprintf(stderr, "ERROR: [main()] calloc() failed %s\n", strerror(errno));
                goto FREE;
            }
            for (int i = 0; i < (int)(strlen(argv[optind - 2]) - 2); i++) {
                if (DEBUG) printf("argv[optind-2] = %s\n", argv[optind - 2]);
                longopt_name[n_longopt_name - 1][i] = argv[optind - 2][i + 2];
            }
            if (!(sorted_user_opts[c - 1] = realloc(sorted_user_opts[c - 1], (n_sorted_user_opts[c - 1] + 1) * sizeof(struct option)))) {
                fprintf(stderr, "ERROR: [main()] realloc() failed %s\n", strerror(errno));
                goto FREE;
            }
            if (DEBUG) fprintf(stdout, "[main()] c = %d", c);
            if (DEBUG) fprintf(stdout, "[main()] optarg = %s\n", optarg);
            if (DEBUG) fprintf(stdout, "[main()] longopt_name[n_longopt_name-1] = %s\n", longopt_name[n_longopt_name - 1]);
            sorted_user_opts[c - 1][n_sorted_user_opts[c - 1]].flag = (int * ) optarg;
            sorted_user_opts[c - 1][n_sorted_user_opts[c - 1]].name = longopt_name[n_longopt_name - 1];
            n_sorted_user_opts[c - 1] += 1;
            n_user_opts++;
            continue;
        }
        if (c == -1)
            break;
        switch (c) {
        case 'P':
            break;
        case 'A':
            A = 1;
            break;
        case 'O':
            O = 1;
            break;
        case 'N':
            N = 1;
            break;
        case 'v':
            app_info();
            goto FREE;
        case ':':
            printf("option needs a value\n");
            goto FREE;
            break;
        case '?':
        case 'h':
            app_help();
            goto FREE;
        default:
            printf("WARNING: [main()] getopt couldn't resolve the option\n");
            break;

        }
    }
    // Check if input arguments were supplied correctly.
    if (argc - optind != 1) {
        app_help();
        goto FREE;
    }

    if (DEBUG) fprintf(stdout, "DEBUG: [main()] Flag states [A = %d] [O = %d] [N = %d]\n", A, O, N);

    // Make sure that flags AND and OR are not used together
    if (A & O) {
        fprintf(stderr, "ERROR: [main()] can't use -A and -O options together\n");
        goto FREE;
    }

    int res = nftw(argv[optind], check_func, 10, FTW_PHYS);
    if (res < 0) {
        perror("nftw");
        goto FREE;
    }

    FREE:
        for (int i = 0; i < n_plugs; i++)
            if (plughandles[i]) dlclose(plughandles[i]);
    if (plughandles) free(plughandles);
    //if (user_opts) free(user_opts);
    if (plugin_path_dir) free(plugin_path_dir);
    if (plugs_opts) free(plugs_opts);
    for (int i = 0; i < n_plugs; i++) {
        if (sorted_user_opts[i]) free(sorted_user_opts[i]);
    }
    if (sorted_user_opts) free(sorted_user_opts);
    if (n_sorted_user_opts) free(n_sorted_user_opts);
    for (int i = 0; i < n_longopt_name; i++) {
        if (longopt_name[i]) free(longopt_name[i]);
    }
    if (ppf_func_arr) free(ppf_func_arr);
    if (longopt_name) free(longopt_name);

    return 0;
}

// -------------------------------------------------------------------------------
// Get info from a plugin
// -------------------------------------------------------------------------------

int plug_info(const char * fpath,
    const struct stat * sb, int typeflag, struct FTW * ftwbuf) {
    UNUSED(sb);
    UNUSED(ftwbuf);

    if (typeflag == FTW_D || typeflag == FTW_DP || typeflag == FTW_DNR) return 0;

    char * DEBUG = getenv(DF);
    void * handle;
    void * pluginfofunc;

    handle = dlopen(fpath, RTLD_LAZY);
    if (!handle) {
        if (DEBUG) fprintf(stderr, "WARNING: [plug_info()] %s is not a valid plugin.\n",
            fpath);
        return 0;
    } else {
        if (DEBUG) fprintf(stderr, "DEBUG: [plug_info()] %s is a valid plugin.\n",
            fpath);
    }

    pluginfofunc = dlsym(handle, "plugin_get_info");
    if (!pluginfofunc) {
        if (DEBUG) fprintf(stderr, "ERROR: [plug_info()] %s doesn't have plugin_get_info function.\n",
            fpath);
        dlclose(handle);
    }
    // Copy paste from your lab1call.c
    struct plugin_info pi = {
        0
    };

    pgi_func_t pgi_func = (pgi_func_t) pluginfofunc;

    int ret = pgi_func( & pi);
    if (ret < 0) {
        fprintf(stderr, "ERROR: [plug_info()] %s plugin_get_info can't be found.\n",
            fpath);
        dlclose(handle);
        return 0;
    } else {

        // -------------------------------------------------------------------------------
        //              Collect all plugins' handles in plughandles array
        //
        //  1. Reallocate memory for new handles
        //  2. Append the handle to plughandles array
        //  3. If new handle is added increment the number of supported plugins 
        // -------------------------------------------------------------------------------

        plughandles = (void ** ) realloc(plughandles, sizeof(void * ) * (n_plugs + 1));
        if (!plughandles) {
            if (DEBUG) fprintf(stderr, "ERROR: [plug_info()]  memory reallocation failed: %s\n", strerror(errno));
            return -1;
        }

        plughandles[n_plugs] = handle;
        n_plugs++;

        // -------------------------------------------------------------------------------
        //                          Collect all plugins' options
        //
        //  1. Reallocate memory for new options 
        //  2. Append options to plugs_opts array
        //  3. If new options are added increment the number of supported options 
        // -------------------------------------------------------------------------------

        plugs_opts = (struct option * ) realloc(plugs_opts, sizeof(struct option) * (pi.sup_opts_len + n_plugs_opts));
        if (!plugs_opts) {
            if (DEBUG) fprintf(stderr, "ERROR: [plug_info()]  memory reallocation failed: %s\n", strerror(errno));
            return -1;
        }
        for (int i = 0; i < (int) pi.sup_opts_len; i++) {
            plugs_opts[n_plugs + i - 1].name = pi.sup_opts[i].opt.name;
            plugs_opts[n_plugs + i - 1].has_arg = pi.sup_opts[i].opt.has_arg;
            plugs_opts[n_plugs + i - 1].flag = pi.sup_opts[i].opt.flag;
            plugs_opts[n_plugs + i - 1].val = n_plugs;
            n_plugs_opts++;

        }

        // -------------------------------------------------------------------------------
        // Firsly, let's open each library and check again if it's a valid one.
        // Now, let's append ppf functions to ppf_func_arr according to its indecies. 
        // -------------------------------------------------------------------------------

        ppf_func_t func;
        func = dlsym(handle, "plugin_process_file");
        if (!func) {
            if (DEBUG) fprintf(stderr, "ERROR: [plug_info()] %s doesn't have plugin_get_info function.\n",
                fpath);
            dlclose(handle);
        }
        if (!(ppf_func_arr = realloc(ppf_func_arr, n_plugs * sizeof(void * )))) {
            fprintf(stderr, "ERROR: realloc() failed %s\n", strerror(errno));
        }
        ppf_func_arr[n_plugs - 1] = func;
    }

    if (DEBUG) fprintf(stdout, "DEBUG: [plug_info()]  Number of found working plugins: %d\n", n_plugs);

    return 0;
}

// -------------------------------------------------------------------------------
// Check files with plugin_process_file()
// -------------------------------------------------------------------------------

int check_func(const char * fpath,
    const struct stat * sb, int typeflag, struct FTW * ftwbuf) {
    UNUSED(sb);
    UNUSED(ftwbuf);

    if (typeflag == FTW_D || typeflag == FTW_DP || typeflag == FTW_DNR) return 0;

    char * DEBUG = getenv(DF);
    errno = 0;

    int ret = 1;

    int * plug_res = alloca(n_plugs * sizeof(int));

    for (int i = 0; i < n_plugs; ++i) {
        ppf_func_t ppf_func = ppf_func_arr[i];
        if ((plug_res[i] = ppf_func(fpath, sorted_user_opts[i], n_sorted_user_opts[i])) < 0) {
            if (DEBUG) fprintf(stderr, "DEBUG: [check_func()] plugin_process_file() failed %d\n", plug_res[i]);
        }
        if (DEBUG) fprintf(stderr, "DEBUG: [check_func()] plugin_process_file() returned %d\n", plug_res[i]);
    }
    if (DEBUG) printf("n_plugs_opts = %d\n", n_plugs_opts);
    if (A) {
        ret = 1;
        for (int i = 0; i < n_plugs; ++i) {
            ret = ret && !(plug_res[i]);
        }
    } else {
        ret = 0;
        for (int i = 0; i < n_plugs; ++i) {
            ret = ret || (!plug_res[i]);
        }
    }

    if (N) ret = !ret;

    if (ret) fprintf(stdout, "%s\n", fpath);

    return 0;
}

// -------------------------------------------------------------------------------
// Print help info
// -------------------------------------------------------------------------------
void app_help() {
    printf("Usage: %s <OPTIONS>... <PATH>\n", g_program_name);
    printf("Recursive search of files starting from a given path\n");
    printf("Available arguments:\n");
    printf("   -P <DIRECTORY>          specifies directory with plugins (current directory is set by default)\n");
    printf("   -A                      prints files that match all specified options\n");
    printf("   -O                      prints files that match at lesat one specified option\n");
    printf("   -N                      inverts the result after combining options\n");
    printf("   -h                      displays this help and exits\n");
    printf("   -v                      optputs version and information about creator and exits\n");
    if (n_plugs > 0) {
        printf("\nAvailable long options:\n");
        for (int i = 0; i < n_plugs; ++i) {
            void * func = dlsym(plughandles[i], "plugin_get_info");
            struct plugin_info pi = {
                0
            };
            pgi_func_t pgi_func = (pgi_func_t) func;
            int ret = pgi_func( & pi);
            if (ret < 0) fprintf(stderr, "ERROR: [app_help()] plugin_get_info failed\n");

            printf("Long options from plugin №%d:\n", i + 1);
            for (size_t j = 0; j < pi.sup_opts_len; ++j) {
                printf("%s\r", pi.sup_opts[j].opt.name);
                printf("\t\t\t%s\n", pi.sup_opts[j].opt_descr);
            }
        }
    }
}

// -------------------------------------------------------------------------------
// Print additional information about the app
// -------------------------------------------------------------------------------
void app_info() {
    printf("Application %s. Version: %s\n", g_program_name, g_version);
    printf("Creator: %s, university group %s\n", g_author, g_group);
    printf("Task number: %s\n", g_lab_variant);
}
