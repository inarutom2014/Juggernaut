// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// Slice is a simple structure containing a pointer into some external
// storage and a size.  The user of a Slice must ensure that the slice
// is not used after the corresponding external storage has been
// deallocated.
//
// Multiple threads can invoke const methods on a Slice without
// external synchronization, but if any of the threads may call a
// non-const method, all threads accessing the same Slice must use
// external synchronization.

#pragma once
#include <assert.h>
#include <string>
//一个简单的char *包装器 自动释放char*

namespace jugg
{
namespace common
{

class DataBlock
{
  public:
    // Create a slice that refers to s[0,strlen(s)-1]
    DataBlock(size_t size) {
        data_ = new char[size];
        size_ = size;
    }
    ~DataBlock()
    {
        if (data_)
        {
            delete data_;
            data_ = nullptr;
        }
    }

   // Return a pointer to the beginning of the referenced data
    const char *data() const { return data_; }

   // Return the length (in bytes) of the referenced data
    size_t size() const { return size_; }

   // Return true iff the length of the referenced data is zero
    bool empty() const { return size_ == 0; }

   // Return the ith byte in the referenced data.
   // REQUIRES: n < size()
    char operator[](size_t n) const
    {
        assert(n < size());
        return data_[n];
    }

private:
    DataBlock(const DataBlock&);

private:
  const char* data_;
  size_t size_;

  // Intentionally copyable
};

}

}
