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
#include <windows.h>
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
#if JUCE_MAC || JUCE_LINUX
        return "/com.uwyn.heelp_sem_"+String(childId_);
#elif JUCE_WINDOWS
        return "Global\\com.uwyn.heelp_sem_" + String(childId_);
#endif
    }
    
    void createForChild(int childId)
    {
        childId_ = childId;
        String name = semaphoreName();
#if JUCE_MAC || JUCE_LINUX
        sem_t* sem = sem_open(name.toRawUTF8(), O_CREAT, 0644, 1);
        if (sem == SEM_FAILED)
        {
            LOG("sem_open error " << errno);
            // TODO : clean up more respectfully
            ::exit(1);
        }
        sem_ = sem;
#elif JUCE_WINDOWS
        HANDLE hSem = CreateSemaphoreW(NULL, 1, 1, name.toWideCharPointer());
        if (hSem == nullptr)
        {
            LOG("Could not create semaphore object (" << (int)GetLastError() << ")");
            // TODO : clean up more respectfully
            ::exit(1);
        }
        sem_ = hSem;

#endif
    }
    
    void openForChild(int childId)
    {
        childId_ = childId;
        String name = semaphoreName();
#if JUCE_MAC || JUCE_LINUX
        sem_t* sem = sem_open(name.toRawUTF8(), 0, 0644, 0);
        if (sem == SEM_FAILED)
        {
            LOG("sem_open error " << errno);
            // TODO : clean up more respectfully
            ::exit(1);
        }
        sem_ = sem;
#elif JUCE_WINDOWS
        HANDLE hSem = OpenSemaphoreW(SEMAPHORE_MODIFY_STATE, false, name.toWideCharPointer());
        if (hSem == nullptr)
        {
            LOG("Could not open semaphore object (" << (int)GetLastError() << ")");
            // TODO : clean up more respectfully
            ::exit(1);
        }
        sem_ = hSem;
#endif
    }

    void destruct()
    {
#if JUCE_MAC || JUCE_LINUX
        sem_close(sem_);
        String name = semaphoreName();
        sem_unlink(name.toRawUTF8());
#elif JUCE_WINDOWS
        CloseHandle(sem_);
#endif
    }

    void enter()
    {
#if JUCE_MAC || JUCE_LINUX
        sem_wait(sem_);
#elif JUCE_WINDOWS
        WaitForSingleObject(sem_, INFINITE);
#endif
    }
    
    void exit()
    {
#if JUCE_MAC || JUCE_LINUX
        sem_post(sem_);
#elif JUCE_WINDOWS
        ReleaseSemaphore(sem_, 1, NULL);
#endif
    }
    
    int childId_;
#if JUCE_MAC || JUCE_LINUX
    sem_t* sem_;
#elif JUCE_WINDOWS
    HANDLE sem_;
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
