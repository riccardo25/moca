/*
    MoCA Montessoro Connector Application
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:   This program uses Qpid-Proton, download from repository: https://github.com/apache/qpid-proton
            To test this program you need ActiveMQ (version 5.15 tested) you can find it at: http://activemq.apache.org/activemq-5150-release.html

*/
#include <stdlib.h>
#include <stdio.h>

#include "libs/fontautil.h"



int main(int argc, char** argv)
{
    char list[MAXSUBFOLDERS][MAXNAMEFILE];
    listfolder(list);
    return 0;
}