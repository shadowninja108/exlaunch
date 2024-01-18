#pragma once

#include "fs_types.hpp"

namespace nn::fs {

    /* 
        Create a file.
        path: Path where to create the path.
        size: Size of the file to create.
    */
    Result CreateFile(char const* path, s64 size);

    /*
        Open a file.
        outHandle:   Output for handle representing opened file.
        path:        File path to open.
        mode:        Mode to open file. See OpenMode.
    */
    Result OpenFile(FileHandle* outHandle, char const* path, int mode);

    /*
        Close a file.
        handle:     Handle to file to be closed.
    */
    void CloseFile(FileHandle handle);

    /*
        Read file at a location.
        handle:     Handle representing file to be read.
        position:   Position within the file to be read.
        size:       How many bytes to read from file.
    */
    Result ReadFile(FileHandle handle, long position, void* buffer, ulong size);

    /*
        Read file at a location, with additional options.
        handle:     Handle representing file to be read.
        position:   Position within the file to be read.
        size:       How many bytes to read from file.
        option:     Additional options for reading, see ReadOption.
    */
    Result ReadFile(FileHandle handle, long position, void* buffer, const ReadOption& option);

    /*
        Read file at a location, with an output amount of bytes read.
        bytesRead:  How many bytes were actually read.
        handle:     Handle representing file to be read.
        position:   Position within the file to be read.
        size:       How many bytes to read from file.
    */
    Result ReadFile(ulong* bytesRead, FileHandle handle, long position, void* buffer);

    
    /*
        Read file at a location, with an output amount of bytes read, and additional options.
        bytesRead:  How many bytes were actually read.
        handle:     Handle representing file to be read.
        position:   Position within the file to be read.
        size:       How many bytes to read from file.
        option:     Additional options for reading, see ReadOption.
    */
    Result ReadFile(ulong* bytesRead, FileHandle handle, long position, void* buffer, const ReadOption& option);

    /*
        Gets the size of the file.
        size:   File size.
        handle: Handle representing file to check.
    */
   Result GetFileSize(long* size, nn::fs::FileHandle handle);

    /*
        Writes to a file.
        handle: Handle representing file to write to.
        position: Position within the file to write to.
        buffer: Pointer to the data to be written.
        size: Amount of data to write, from the pointer.
        option: Additional options for writing, like flushing. 
    */
    Result WriteFile(FileHandle handle, s64 position, void const* buffer, u64 size, WriteOption const& option);

    /*
        Flush file.
        handle: Handle representing file to flush. 
    */
   Result FlushFile(FileHandle handle);
}