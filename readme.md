# FSTM (Filesystem Time Machine)
###### Originally named File Time Machine but the abbreviation "FTM" is already a known tem soooooo

## What is this:

FSTM is a lightweight client-only (for now) VCS (Version Control System) written in C++

## Features (a.k.a. what currently works):

- Tracking files
- Blacklisting files, preventing them from being tracked
- Creating snapshots of the repository
- - Snapshots only record the files that changed since the last one was taken! Each one gets a unique ID based on the hash of the affected files
- - You cannot create a snapshot if nothing changes
- All of that via a unified command-line argument pipeline

## Installation: 
### Pre-built (Windows/GNU Linux):
You can grab a build from the [releases page](https://github.com/azazel-261/FSTM/releases)
### Building from source:

###### Please make sure that openssl library is installed!

Firstly, clone the repository

`git clone https://github.com/azazel-261/FSTM && cd FSTM`

Initialize a cmake build

`cmake -S . -B build`

Lastly, switch to your build directory and build!

`cd build`

`cmake --build .`

If the build succeeds, the executable will be in the `build/src` directory 

## Usage:

I recommend adding your executable to PATH for easier access. You can use the `--help` flag to get a small help message from the executable

### Initialization:

To create a repository, run `fstm.exe init` in the desired folder

### Tracking:

When a file is **tracked**, it will be present in the next snapshot (if it has been changed since the last snapshot)

To track a file, run `fstm.exe track path/to/file`

You can run `fstm.exe track *` to track all files in the repository or pass a folder to track all the files in that folder

Similarly, use `fstm.exe untrack path/to/file` (or `*`) to remove files from tracking

### Blacklisting:

When a file is **blacklisted**, it cannot be added to tracking via `fstm.exe track . . . `

To blacklist a file or a folder, simply run `fstm.exe blacklist path/to/file/or/folder`

Similarly, use `fstm.exe unblacklist path/to/file/or/folder` to remove a file or a folder from the blacklist

## Snapshots: 

Once you've made the desired changes and tracked all the affected files, it's time to snapshot!

Run `fstm.exe snapshot` to create a snapshot of your repository. You can attach an optional parameter `-m` to add a message to your snapshot, like this:

`fstm.exe snapshot -m "This is so cool!"`

Though the CLI tool provides no way to view snapshots, you can find those in the `.fstm/snapshots` folder in your repository

Each folder is named after the ID of the snapshot. You can view the message left by the snapshot owner in `example_snapshot_id1/.fstm/message.txt`, or browse through the files in this snapshot

## Credits/Resources used:

- The [openssl library](https://openssl-library.org/) for creating the file hashes. Cmake says it's deprecated but i don't care, i'm not using it for anything cybersecurity-related anyways
- The awesome (!) [CLI11 library](https://github.com/CLIUtils/CLI11) for parsing the command line arguments. I probably would've spent another 15 hours to figure that out on my own
- [cppreference.com](https://cppreference.com/) for their amazing documentation 