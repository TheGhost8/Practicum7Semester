#include "factory.h"


TFactory::TFactory() : Impl(std::make_unique<TFactory::TImpl>()) {};

TGraphPtr TFactory::CreateGraph(const std::string& n, const TOptions& opts) const {
    return Impl->CreateObject(n, opts);
}

std::vector<std::string> TFactory::GetAvailableObjects() const {
    return Impl->GetAvailableObjects();
}
