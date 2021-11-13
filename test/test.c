#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{   
    // Addition, remove to check if all is okay
    system(
        "touch z_1;"
        "touch z_2;"
        "tag add z_2 france red;"
        "tag add z_1 country L3 sy6 cp6;"
        "tag delete z_1 cp6;"
        "tag add z_1 ok super ;"
        "tag delete z_1 ok;"
        "tag get z_1;"
        "tag addgrad _ country;"
        "tag addgrad country france;"
        "tag addgrad france paris;"
        "tag addgrad country china;"
        "tag tree country;"
        "tag addgrad _ color;"
        "tag addgrad color red;"
        "tag tree color;"
        "tag search country;"
        "tag search country et \"pas(L3)\";"
        "tag search country et \"pas(L3)\" et \"pas(red)\";"
    );
    system("rm z_*");

    // Test of multiple repetitions
    system(
        "touch z_3;"
        "tag add z_3 ok super okay smart;"
        "tag delete z_3 super; "
        "tag delete z_3 ok; "
        "tag add z_3 ok323;"
        "tag delete z_3 ok323;"
        "tag add z_3 like_it;"
        "tag get z_3 ;"
        );
    system("rm z_*");

    system("tag deletegrad country");
    system("tag deletegrad color");
}
