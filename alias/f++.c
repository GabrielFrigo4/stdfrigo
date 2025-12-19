#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <process.h>
#else
#include <unistd.h>
#endif

#ifndef CPP
#define CPP "g++"
#endif

int main(int argc, char *argv[]) {
    char *pkg_cmd = "pkg-config --cflags --libs stdfrigo";
    FILE *fp = popen(pkg_cmd, "r");

    if (fp == NULL) {
        fprintf(stderr, "Erro ao executar pkg-config\n");
        return 1;
    }

    char flags[1024];
    if (fgets(flags, sizeof(flags), fp) == NULL) {
        fprintf(stderr, "Erro: Biblioteca 'stdfrigo' não encontrada.\n");
        pclose(fp);
        return 1;
    }
    pclose(fp);

    char **new_argv = malloc((argc + 20) * sizeof(char *));
    new_argv[0] = CPP;

    for (int i = 1; i < argc; i++) {
        new_argv[i] = argv[i];
    }

    int current_idx = argc;
    char *token = strtok(flags, " \n");
    while (token != NULL) {
        new_argv[current_idx++] = strdup(token);
        token = strtok(NULL, " \n");
    }
    new_argv[current_idx] = NULL;

    fflush(stdout);
    fflush(stderr);

#ifdef _WIN32
    int exit_code = _spawnvp(_P_WAIT, CPP, (const char *const *)new_argv);
    if (exit_code == -1) {
        perror("Erro ao executar gcc");
        return 1;
    }
    return exit_code;
#else
    execvp(CPP, new_argv);
    perror("Erro ao executar gcc");
    return 1;
#endif
}
