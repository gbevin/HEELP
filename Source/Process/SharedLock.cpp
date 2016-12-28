/*
 * This file is part of HEELP.
 * Copyright (c) 2016 Uwyn SPRL.  http://www.uwyn.com
 *
 * HEELP is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * HEELP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "SharedLock.h"

#include "../Utils.h"

#if JUCE_MAC || JUCE_LINUX
#include <semaphore.h>
#include <sys/sem.h>
#elif JUCE_WINDOWS
#endif

using namespace heelp;

struct SharedLock::Pimpl
{
    Pimpl() : childId_(-1)
    {
    }
    
    ~Pimpl()
    {
        destruct();
    }
    
    String semaphoreName()
    {
        return "/heelp/sem/"+String(childId_);
    }
    
    void createForChild(int childId)
    {
        childId_ = childId;
#if JUCE_MAC || JUCE_LINUX
        String name = semaphoreName();
        sem_t* sem = sem_open(name.toRawUTF8(), O_CREAT, 0644, 1);
        if (sem == SEM_FAILED)
        {
            LOG("sem_open error " << errno);
            // TODO : clean up more respectfully
            ::exit(1);
        }
        sem_ = sem;
#elif JUCE_WINDOWS
#endif
    }
    
    void openForChild(int childId)
    {
        childId_ = childId;
#if JUCE_MAC || JUCE_LINUX
        String name = semaphoreName();
        sem_t* sem = sem_open(name.toRawUTF8(), 0, 0644, 0);
        if (sem == SEM_FAILED)
        {
            LOG("sem_open error " << errno);
            // TODO : clean up more respectfully
            ::exit(1);
        }
        sem_ = sem;
#elif JUCE_WINDOWS
#endif
    }

    void destruct()
    {
#if JUCE_MAC || JUCE_LINUX
        sem_close(sem_);
        String name = semaphoreName();
        sem_unlink(name.toRawUTF8());
#elif JUCE_WINDOWS
#endif
    }

    void enter()
    {
#if JUCE_MAC || JUCE_LINUX
        sem_wait(sem_);
#elif JUCE_WINDOWS
#endif
    }
    
    void exit()
    {
#if JUCE_MAC || JUCE_LINUX
        sem_post(sem_);
#elif JUCE_WINDOWS
#endif
    }
    
    int childId_;
#if JUCE_MAC || JUCE_LINUX
    sem_t* sem_;
#elif JUCE_WINDOWS
#endif
};

SharedLock::SharedLock() : pimpl_(new Pimpl())  {}
SharedLock::~SharedLock()                       { pimpl_ = nullptr; }

SharedLock* SharedLock::createForChild(int childId)
{
    SharedLock* lock = new SharedLock();
    lock->pimpl_->createForChild(childId);
    return lock;
}

SharedLock* SharedLock::openForChild(int childId)
{
    SharedLock* lock = new SharedLock();
    lock->pimpl_->openForChild(childId);
    return lock;
}

void SharedLock::enter()    { return pimpl_->enter(); }
void SharedLock::exit()     { return pimpl_->exit(); }
