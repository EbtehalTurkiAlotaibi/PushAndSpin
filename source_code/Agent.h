#ifndef AGENT_H
#define AGENT_H


/****************************************************************************
Copyright (c) 2013, Ryan Luna, Athanasios Krontiris, Kostas Bekris
All rights reserved.

The Software is available for download and use subject to the terms and
conditions of this License. Access or use of the Software constitutes
acceptance and agreement to the terms and conditions of this License.

Permission to use, copy, modify, and distribute this software and its
documentation for educational, research, and non-profit purposes, without fee,
and without a written agreement is hereby granted.  Permission to incorporate
this software into commercial products may be obtained by contacting the authors.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
  * Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
  * Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.
  * Neither the names of the authors nor the names of its contributors may be
    used to endorse or promote products derived from this software without
    specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
****************************************************************************/

// Author: Ryan Luna


    class Agent
    {
        public:
            Agent (unsigned int id, unsigned int start, unsigned int goal) :
                id_(id), start_(start), goal_(goal), current_(start_) {}

            virtual ~Agent() {}

            unsigned int getID ()    const { return id_; }
            unsigned int getStart () const { return start_; }
            unsigned int getGoal ()  const { return goal_; }

            unsigned int getCurrent () const { return current_; }
            virtual void setCurrent (int c) { current_ = c; }
            virtual void restart() { current_ = start_; }
            void operator= (Agent a) { id_=a.id_ ; start_=a.start_ ; goal_=a.goal_ ; current_=a.current_; }

        protected:
            unsigned int id_, start_, goal_, current_;
    };

#endif // AGENT_H
