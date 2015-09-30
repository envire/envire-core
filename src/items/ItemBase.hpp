#ifndef __ENVIRE_CORE_ITEM_BASE__
#define __ENVIRE_CORE_ITEM_BASE__

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/binary_object.hpp>
#include <base/Time.hpp>
#include <string>

namespace envire { namespace core
{
    /**@class ItemBase
    *
    * ItemBase class
    */
    class ItemBase
    {
    public:
        typedef boost::shared_ptr<ItemBase> Ptr;

    protected:

        base::Time time; /** Timestamp */

        boost::uuids::uuid uuid; /** Unique Identifier */

        std::string frame_name; /** Frame name in which the Item is located */

        // TBD: do we want/need this pointer?
        void* user_data_ptr; /** Pointer to the user data */

        static const std::string class_name; /** Name of the inherited class */

    public:

        ItemBase();
        virtual ~ItemBase() {}

        /**@brief setTime
        *
        * Sets the timestamp of the item
        *
        */
        void setTime(const base::Time& time) { this->time = time; }

        /**@brief getTime
        *
        * Returns the timestamp of the item
        *
        */
        const base::Time& getTime() const { return this->time; }

        /**@brief setID
        *
        * Sets the unique identifier of the item
        *
        */
        void setID(const boost::uuids::uuid& id) { this->uuid = id; }

        /**@brief getID
        *
        * Returns the unique identifier of the item
        *
        */
        const boost::uuids::uuid getID() const { return this->uuid; }
        const std::string getIDString() const { return boost::uuids::to_string(this->uuid); }

        /**@brief setFrame
        *
        * Sets the frame name of the item
        *
        */
        void setFrame(const std::string& frame_name) { this->frame_name = frame_name; }

        /**@brief getFrame
        *
        * Returns the frame name of the item
        *
        */
        const std::string& getFrame() const { return this->frame_name; }

        /**@brief getClassName
        *
        * Returns the class name of the item
        *
        */
        virtual const std::string& getClassName() const { return class_name; }

        void* getRawData() const { return user_data_ptr; }

    private:
        friend class boost::serialization::access;

        template <typename Archive>
        void serialize(Archive &ar, const unsigned int version)
        {
            ar & boost::serialization::make_nvp("time", time.microseconds);
            ar & boost::serialization::make_nvp("uuid", boost::serialization::make_binary_object(uuid.data, uuid.size()));
            ar & BOOST_SERIALIZATION_NVP(frame_name);
        }
    };

    BOOST_SERIALIZATION_ASSUME_ABSTRACT(envire::core::ItemBase)

}}

#endif


