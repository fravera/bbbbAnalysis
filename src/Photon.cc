#include "Photon.h"

#include "BuildP4.h"

void Photon::buildP4()
{
    p4_.BUILDP4(Photon, nat_);
}