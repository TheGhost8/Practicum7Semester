#pragma once

#include <optional>
#include "graph.h"

class TFactory {
    class TImpl;
    std::unique_ptr<const TImpl> Impl;

public:
    TFactory();
    ~TFactory() = default;
    TGraphPtr CreateGraph(const std::string& name, const TOptions& opts) const;
    std::vector<std::string> GetAvailableObjects() const;
};

class TFactory::TImpl {
    class ICreator {
    public:
        virtual ~ICreator() {};
        virtual TGraphPtr Create(const TOptions& opts) const = 0;
    };

    using TCreatorPtr = std::shared_ptr<ICreator>;
    using TRegisteredCreators =	std::map<std::string, TCreatorPtr>;

    TRegisteredCreators RegisteredCreators;

public:
    template <class TCurrentObject>
    class TCreator : public ICreator{
        TGraphPtr Create(const TOptions& opts) const override{
            return std::make_unique<TCurrentObject>(opts);
        }
    };

    TImpl() {
        RegisterAll();
    }

    template <typename T>
    void RegisterCreator(const std::string& name) {
        RegisteredCreators[name] = std::make_shared<TCreator<T>>();
    }

    void RegisterAll() {
        RegisterCreator<TWeightedGraph>("weighted");
        RegisterCreator<TSimpleGraph>("simple");
        RegisterCreator<TBipartiteGraph>("bipartite");
        RegisterCreator<TCompleteGraph>("complete");
    }

    TGraphPtr CreateObject(const std::string& n, const TOptions& opts) const {
        return RegisteredCreators.at(n)->Create(opts);
    }

    std::vector<std::string> GetAvailableObjects () const {
        std::vector<std::string> result;
        for (const auto& creatorPair : RegisteredCreators) {
            result.push_back(creatorPair.first);
        }
        return result;
    }
};
