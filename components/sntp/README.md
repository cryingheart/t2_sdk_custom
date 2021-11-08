how to use:
================

headerfile:
    sntp.h

function to kick start sntp:
    void sntp_init();

function to read the time:
    unsigned int sntp_time();
    by default, value will be zero, once sntp is completed, it will return the time

example:
    sntp_init();
    while(1)
    {
        os_printf("val = %d\n", sntp_time());
        os_msleep(100);
    }

