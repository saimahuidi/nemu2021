#include <stdio.h>
#include <NDL.h>
int main() {
    NDL_Init(0);
    int current = 0;
    int last = NDL_GetTicks();
    while (1) {
        if (last - current > 500000) {
            printf("666\n");
            current = last;
        }
        last = NDL_GetTicks();
    }
    NDL_Quit();
}