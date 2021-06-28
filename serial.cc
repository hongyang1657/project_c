#include <stdio.h>
#include <stdlib.h>

int main(){
    system("echo -e \xfa\xfd\x0b\x01\x02\x88\x0\x0\x0\x96\xfb > /dev/ttyUSB0");
    return 0;
}
