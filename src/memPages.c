
#include <unistd.h>
#include <stdio.h>


int main () {
    printf("Total system memory pages: %ld\r\n", sysconf(_SC_PHYS_PAGES));
    printf("Memory page size on ARM Mac: %ld\r\n", sysconf(_SC_PAGESIZE));
    long pageSize = sysconf(_SC_PAGESIZE);
    long pageCount = sysconf(_SC_PHYS_PAGES);
    long totalSysMemory = pageSize * pageCount; // (1e9);
    long totalSysMemoryGB = pageSize * pageCount / (1e9);
    printf("Total system memory in Bytes: %ld * %ld = %ld\r\n", pageSize, pageCount, totalSysMemory);
    printf("Total system memory in GB: %ld\r\n", totalSysMemoryGB);

    return 0;
}
