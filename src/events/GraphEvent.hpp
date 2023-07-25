//
// Copyright (c) 2015, Deutsches Forschungszentrum für Künstliche Intelligenz GmbH.
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#pragma once
#include <envire_core/events/GraphEventExceptions.hpp>

namespace envire { namespace core
{

    /** TODO comment
     */
    class GraphEvent
    {
    public:
        //all possible events
        enum Type
        {
            EDGE_ADDED,
            EDGE_REMOVED,
            EDGE_MODIFIED,
            ITEM_ADDED_TO_FRAME,
            ITEM_REMOVED_FROM_FRAME,
            FRAME_ADDED,
            FRAME_REMOVED,
            USER_DEFINED
        };

        GraphEvent() = delete;
        virtual ~GraphEvent() {}

        /**
         * This method is used to identify if a new event supersedes and existing event.
         * @returns true if the given event supersedes this one.
         */
        virtual bool mergeable(const GraphEvent& event)
        {
            // not implemented
            return false;
        }

        /**
         * @returns the type
         */
        Type getType() const { return type; }

        std::string getUserDefinedType() const { return user_defined_type; }

        /**
         * This method can be overloaded to allow the event to be cloned.
         */
        virtual GraphEvent* clone() const { throw CloneMethodNotImplementedException(); }

        friend std::ostream& operator<<(std::ostream&, const GraphEvent&);

    protected:
        explicit GraphEvent(const Type type) : type(type) {}
        explicit GraphEvent(const std::string user_defined_type) : type(USER_DEFINED), user_defined_type(user_defined_type) {}

        Type type;

        std::string user_defined_type;
    };

}}
