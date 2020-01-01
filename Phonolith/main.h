#include <workbench/workbench.h>

extern unsigned long main_appID;
extern bool main_hasDockIcon;

extern bool main_quit;
extern int main_meterUpdateSignal;
extern int main_statusUpdateSignal;

extern DiskObject *main_applicationIcon;

void showError(exception &e);

