#pragma once
#include <envire_core/TransformGraphTypes.hpp>
#include <envire_core/Transform.hpp>
#include <envire_core/Frame.hpp>
#include "GraphEvent.hpp"

namespace envire { namespace core
{
    class ItemAddedEvent : public GraphEvent
    {
    public:
      ItemAddedEvent(const FrameId& frame, const boost::intrusive_ptr<ItemBase> item) :
        GraphEvent(GraphEvent::ITEM_ADDED_TO_FRAME), frame(frame), item(item){}

      FrameId frame;/**<frame that the item has been added to.*/
      boost::intrusive_ptr<ItemBase> item; /**<The item */
    };
}}