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
#include <sys/mman.h>
#elif JUCE_WINDOWS
#include <windows.h>
#pragma comment(lib, "user32.lib")
#endif

using namespace heelp;

struct SharedMemory::Pimpl
{
    Pimpl() : childId_(-1), created_(false), size_(0), shmAddress_(nullptr)
    {
    }
    
    ~Pimpl()
    {
        destruct();
    }

#if JUCE_MAC || JUCE_LINUX
    String shmName()
    {
        return "/tmp/heelp_"+uuid_+"_"+String(childId_);
    }
#endif
    
    void createForChildWithSize(int childId, size_t size)
    {
        Time now = Time::getCurrentTime();
        uuid_ = now.formatted("%y%m%d%H%M%S")+String(now.getMilliseconds());
        childId_ = childId;
        size_ = size;
#if JUCE_MAC || JUCE_LINUX
        String name = shmName();
        int64_t shmFd = shm_open(name.toRawUTF8(), O_CREAT|O_EXCL|O_RDWR, S_IRUSR|S_IWUSR);
        if (shmFd < 0)
        {
            LOG("shm_open " << name << " error " << errno);
            // TODO : clean up more respectfully
            exit(1);
        }
        if (ftruncate(shmFd, size) == -1)
        {
            LOG("ftruncate error " << errno);
            // TODO : clean up more respectfully
            exit(1);
        }
#elif JUCE_WINDOWS
        SECURITY_ATTRIBUTES sec_attr;
        SECURITY_DESCRIPTOR sd;
        InitializeSecurityDescriptor(&sd, SECURITY_DESCRIPTOR_REVISION);
        SetSecurityDescriptorDacl(&sd, TRUE, NULL, FALSE);

        sec_attr.nLength = sizeof(sec_attr);
        sec_attr.bInheritHandle = TRUE;
        sec_attr.lpSecurityDescriptor = &sd;

        HANDLE hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, &sec_attr, PAGE_READWRITE, 0, (DWORD)size, nullptr);
        if (hMapFile == nullptr)
        {
            LOG("Could not create file mapping object (" << (int)GetLastError() << ")");
            // TODO : clean up more respectfully
            exit(1);
        }
        int64_t shmFd = (int64_t)hMapFile;
#endif

        created_ = true;
        shmFd_ = shmFd;
        
        attach();
    }
    
    void attach()
    {
        if (shmAddress_ == nullptr)
        {
#if JUCE_MAC || JUCE_LINUX
            char* sharedMemory = (char*)mmap(nullptr, size_, PROT_READ|PROT_WRITE, MAP_SHARED, shmFd_, 0);
            close(shmFd_);
            if (sharedMemory == MAP_FAILED)
            {
                LOG("mmap error " << errno);
                // TODO : clean up more respectfully
                exit(1);
            }
#elif JUCE_WINDOWS
            LPCTSTR pBuf = (LPTSTR)MapViewOfFile((HANDLE)shmFd_, FILE_MAP_ALL_ACCESS, 0, 0, 0);
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
            String name = shmName();
            if (shm_unlink(name.toRawUTF8()) == -1)
            {
                LOG("shm_unlink error " << errno);
                // TODO : clean up more respectfully
                exit(1);
            }
#elif JUCE_WINDOWS
            CloseHandle((HANDLE)shmFd_);
#endif
            created_ = false;
        }
    }
    
    void detach()
    {
        if (shmAddress_)
        {
#if JUCE_MAC || JUCE_LINUX
            if (munmap(shmAddress_, size_) == -1)
            {
                LOG("munmap error " << errno);
                // TODO : clean up more respectfully
                exit(1);
            }
#elif JUCE_WINDOWS
            UnmapViewOfFile(shmAddress_);
#endif
            shmAddress_ = nullptr;
        }
    }
    
    void attachForChildWithInfo(int childId, String uuid, int64_t info)
    {
        if (shmAddress_ == nullptr && !created_)
        {
            childId_ = childId;
            uuid_ = uuid;
#if JUCE_MAC || JUCE_LINUX
            size_ = info;
            
            String name = shmName();
            int64_t shmFd = shm_open(name.toRawUTF8(), O_RDWR, S_IRUSR|S_IWUSR);
            if (shmFd < 0)
            {
                LOG("shm_open error " << errno);
                // TODO : clean up more respectfully
                exit(1);
            }
            shmFd_ = shmFd;
#elif JUCE_WINDOWS
            shmFd_ = info;
#endif
            attach();
        }
    }
    
    String getShmUUID()
    {
        return uuid_;
    }
    
    int64_t getShmInfo()
    {
#if JUCE_MAC || JUCE_LINUX
        return size_;
#elif JUCE_WINDOWS
        return shmFd_;
#endif
    }
    
    char* getShmAddress()
    {
        return shmAddress_;
    }
    
    int childId_;
    String uuid_;
    bool created_;
    size_t size_;
    int64_t shmFd_;
    char* shmAddress_;
};

SharedMemory::SharedMemory() : pimpl_(new Pimpl())  {}
SharedMemory::~SharedMemory()                       { pimpl_ = nullptr; }

SharedMemory* SharedMemory::createForChildWithSize(int childId, size_t size)
{
    SharedMemory* mem = new SharedMemory();
    mem->pimpl_->createForChildWithSize(childId, size);
    return mem;
}

SharedMemory* SharedMemory::attachForChildWithInfo(int childId, String uuid, int64_t info)
{
    SharedMemory* mem = new SharedMemory();
    mem->pimpl_->attachForChildWithInfo(childId, uuid, info);
    return mem;
}

String SharedMemory::getShmUUID()  { return pimpl_->getShmUUID(); }
int64_t SharedMemory::getShmInfo()  { return pimpl_->getShmInfo(); }
char* SharedMemory::getShmAddress() { return pimpl_->getShmAddress(); }
