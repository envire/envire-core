#pragma once

#include <envire_core/items/ItemBase.hpp>

#include <boost/archive/polymorphic_iarchive.hpp>
#include <boost/archive/polymorphic_oarchive.hpp>

namespace envire { namespace core
{

typedef boost::archive::polymorphic_iarchive ArchiveInType;
typedef boost::archive::polymorphic_oarchive ArchiveOutType;

class SerializationHandle
{
public:
    virtual ~SerializationHandle() {}

    virtual bool save(ArchiveOutType& ar, const ItemBase::Ptr& item)
    {
        throw std::runtime_error("Method save is not implemented!");
    };

    virtual bool load(ArchiveInType& ar, ItemBase::Ptr& item)
    {
        throw std::runtime_error("Method load is not implemented!");
    };
};

}}