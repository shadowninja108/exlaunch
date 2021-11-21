#pragma once

#include "fs_types.hpp"

namespace nn::fs {
    /*
        Open a file.
        handleOut:   Output for handle representing opened file.
        path:        File path to open.
        mode:        Mode to open file. See OpenMode.
    */
    Result OpenFile(FileHandle* handleOut, char const* path, int mode);

    /*
        Close a file.
        handle:     Handle to file to be closed.
    */
    Result CloseFile(FileHandle handle);

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
}