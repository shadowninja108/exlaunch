#pragma once

#include "fs_types.hpp"

namespace nn::fs {
    /*
        Open a directory given the path and open mode.
        outHandle: Pointer to write the handle into.
        path: Path to the directory to open.
        openMode: Mode to open the directory with, see nn::fs::OpenDirectoryMode.
    */
    Result OpenDirectory(DirectoryHandle* outHandle, char const* path, s32 openMode);

    /*
        Closes directory.
        handle: Handle of directory to close. 
    */
    void CloseDirectory(DirectoryHandle handle);

    /* 
        Read entries for a given opened directory into a provided buffer.
        outEntryCount: Pointer to write actual amount of entries read.
        outEntries: Pointer to buffer containing (entryBufferLength) amount of DirectoryEntry
        handle: Handle of directory to be opened.
        entryBufferLength: How many entries provided in the outEntries argument.
    */
    Result ReadDirectory(s64* outEntryCount, DirectoryEntry* outEntries, DirectoryHandle handle, s64 entryBufferLength);

    /* 
        Creates a directory at  given path.
        path: Path to path to create.
    */
    Result CreateDirectory(char const* path);

    /*
        Get the amount of entries in a given opened directory.
        outEntryCount: Pointer to write the entry count.
        handle: Handle of the directory to count entries.
    */
    Result GetDirectoryEntryCount(s64* outEntryCount, DirectoryHandle handle);

    /*
        Delete a given directory path and all of it's subdirectories/folders.
        path: Path to the directory to delete.
    */
    Result DeleteDirectoryRecursively(char const* path);

    /*
        TODO: ?
        path: Path to the directory to be cleaned. 
    */
    Result CleanDirectoryRecursively(char const* path);

    /* 
        TODO: ?
    */
    Result RenameDirectory(char const*, char const*);
}