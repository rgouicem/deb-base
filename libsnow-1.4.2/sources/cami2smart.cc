#include "PNetIO.h"

int
main (int argc, const char** argv)
{
  PNet* net = parseCAMI("model.cami");
  net->ExportToSmart("model.sm");
}
