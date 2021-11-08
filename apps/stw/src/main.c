#include <kernel/os.h>
#include <kernel/hostio.h>
extern int stw(void);

int main(void)
{
    stw();
    while (true) {
        os_msleep(1000);
    }
    return 0;
}
