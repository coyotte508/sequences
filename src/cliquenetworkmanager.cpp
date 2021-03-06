#include "cliquenetworkmanager.h"

CliqueNetworkManager::CliqueNetworkManager()
{

}

void CliqueNetworkManager::addNetwork(CliqueNetwork &nw)
{
    nws.insert(&nw);
}

void CliqueNetworkManager::iterate()
{
    for (int i = 0; i < 8; i++) {
        for (CliqueNetwork *nw: nws) {
            nw->propagate();
        }
        for (CliqueNetwork *nw: nws) {
            nw->stabilize();
        }
    }
}

void CliqueNetworkManager::shutdown()
{
    for (CliqueNetwork *nw: nws) {
        nw->shutdown();
    }
}
