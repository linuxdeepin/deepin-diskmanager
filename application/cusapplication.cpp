#include "cusapplication.h"


CusApplication::CusApplication(int &argc, char **argv): DApplication(argc, argv)
{

}

void CusApplication::handleQuitAction()
{
    DApplication::handleQuitAction();
    //emit sighandleQuitAction();
}
