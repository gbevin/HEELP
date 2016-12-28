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
#include "SharedMemory.h"

#include "../Utils.h"

#if JUCE_MAC || JUCE_LINUX
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#elif JUCE_WINDOWS
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#pragma comment(lib, "user32.lib")
#endif

using namespace heelp;

struct SharedMemory::Pimpl
{
    Pimpl() : created_(false), shmAddress_(nullptr)
    {
    }
    
    ~Pimpl()
    {
        destruct();
    }
    
    void createWithSize(size_t size)
    {
#if JUCE_MAC || JUCE_LINUX
        int64_t shmId = shmget(IPC_PRIVATE, size, IPC_CREAT|IPC_EXCL|0666);
        if (shmId < 0)
        {
            LOG("shmget error " << errno);
            // TODO : clean up more respectfully
            exit(1);
        }
#elif JUCE_WINDOWS
        SECURITY_ATTRIBUTES sa;
        sa.bInheritHandle = true;
        HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, &sa, PAGE_READWRITE, 0, size, nullptr);
        if (hMapFile == nullptr)
        {
            LOG("Could not create file mapping object (" << (int)GetLastError() << ")");
            // TODO : clean up more respectfully
            exit(1);
        }
        int64_t shmId = (int64_t)hMapFile;
#endif

        created_ = true;
        shmId_ = shmId;
        
        attach();
    }
    
    void attach()
    {
        if (shmAddress_ == nullptr)
        {
#if JUCE_MAC || JUCE_LINUX
            char* sharedMemory = (char*)shmat(shmId_, 0, SHM_RND);
            if (sharedMemory == nullptr)
            {
                LOG("shmat error " << errno);
                // TODO : clean up more respectfully
                exit(1);
            }
#elif JUCE_WINDOWS
            LPCTSTR pBuf = (LPTSTR)MapViewOfFile((HANDLE)shmId_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
            if (pBuf == nullptr)
            {
                LOG("Could not map view of file (" << (int)GetLastError() << ")");
                // TODO : clean up more respectfully
                exit(1);
            }
            char* sharedMemory = (char*)pBuf;
#endif

            shmAddress_ = sharedMemory;
        }
    }
    
    void destruct()
    {
        detach();
        
        if (created_)
        {
#if JUCE_MAC || JUCE_LINUX
            shmctl(shmId_, IPC_RMID, 0);
#elif JUCE_WINDOWS
            CloseHandle((HANDLE)shmId_);
#endif
            created_ = false;
        }
    }
    
    void detach()
    {
        if (shmAddress_)
        {
#if JUCE_MAC || JUCE_LINUX
            shmdt(shmAddress_);
#elif JUCE_WINDOWS
            UnmapViewOfFile(shmAddress_);
#endif
            shmAddress_ = nullptr;
        }
    }
    
    void attachWithId(int64_t shmId)
    {
        if (shmAddress_ == nullptr && !created_)
        {
            shmId_ = shmId;
            attach();
        }
    }
    
    int64_t getShmId()
    {
        return shmId_;
    }
    
    char* getShmAddress()
    {
        return shmAddress_;
    }
    
    bool created_;
    int64_t shmId_;
    char* shmAddress_;
};

SharedMemory::SharedMemory() : pimpl_(new Pimpl())  {}
SharedMemory::~SharedMemory()                       { pimpl_ = nullptr; }

SharedMemory* SharedMemory::createWithSize(size_t size)
{
    SharedMemory* mem = new SharedMemory();
    mem->pimpl_->createWithSize(size);
    return mem;
}

SharedMemory* SharedMemory::attachWithId(int64_t shmId)
{
    SharedMemory* mem = new SharedMemory();
    mem->pimpl_->attachWithId(shmId);
    return mem;
}

int64_t SharedMemory::getShmId()    { return pimpl_->getShmId(); }
char* SharedMemory::getShmAddress() { return pimpl_->getShmAddress(); }
