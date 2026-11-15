#include <iostream>

#include "include/CLI11.hpp"

#include "include/diff.h"
#include "include/fs.h"
#include "include/snapshot.h"
#include "include/init.h"
#include "include/tracker.h"

using namespace FSTM;

std::filesystem::path findRepositoryPath(const std::filesystem::path& _path)
{
    std::filesystem::path absPath = std::filesystem::absolute(_path);
    while (!std::filesystem::exists(absPath / ".fstm"))
    {
        if (std::filesystem::equivalent(absPath, absPath.parent_path()))
        {
            throw std::runtime_error("Repository not found");
        }
        absPath = absPath.parent_path();
    }
    return absPath;
}

int main(int argc, char *argv[])
{
    CLI::App app {"FSTM - a small c++ VCS"};

    std::string repoPathArg = ".";
    app.add_option("-r,--repository", repoPathArg, "(OPTIONAL) Path to the repository, defaults to the current directory");

    CLI::App *init_command = app.add_subcommand("init", "Initialize a new repository");

    std::string message;
    CLI::App *snapshot_command = app.add_subcommand("snapshot", "Create a snapshot");
    snapshot_command -> add_option("-m,--message", message, "(OPTIONAL) A description to attach to your snapshot");

    std::string pathArg;

    CLI::App *track_command = app.add_subcommand("track", "Track a file or directory, * for all files in the repository");
    track_command -> add_option("path", pathArg, "Path to the file or directory to be tracked") -> required();

    CLI::App *untrack_command = app.add_subcommand("untrack", "Untrack a file or directory, * for all files in the repository");
    untrack_command -> add_option("path", pathArg, "Path to the file or directory to be untracked") -> required();

    CLI::App *ignore_command = app.add_subcommand("blacklist", "Add a file or a folder to the blacklist, to prevent it from being tracked");
    ignore_command -> add_option("path", pathArg, "Path to the file or directory to be blacklisted") -> required();

    CLI::App *unignore_command = app.add_subcommand("unblacklist", "Remove a file or a folder from the blacklist");
    unignore_command -> add_option("path", pathArg, "Path to the file or directory to be removed from the blacklist") -> required();

    CLI11_PARSE(app, argc, argv);

    if (*init_command)
    {
        initRepository(repoPathArg);
        std::cout << "Repository created successfully" << std::endl;
        return 0;
    }

    if (*track_command)
    {
        std::string repoPath;
        try
        {
            repoPath = findRepositoryPath(repoPathArg).string();
        } catch (const std::runtime_error &e)
        {
            std::cout << e.what() << std::endl;
            return -1;
        }
        if (pathArg == "*")
        {
            const auto trackableFiles = Tracker::getTrackableFiles(repoPath);
            for (const auto &file : trackableFiles)
            {
                Tracker::trackFile(repoPath, file);
            }
            std::cout << "Tracking all files" << std::endl;
            return 0;
        }
        if (!std::filesystem::exists(pathArg) || !Tracker::is_subpath(pathArg, repoPath))
        {
            std::cout << "File doesn't exist or is outside the repository scope" << std::endl;
            return -1;
        }
        Tracker::trackFile(repoPath, pathArg);
        std::cout << "Started tracking " << pathArg << std::endl;
        return 0;
    }
    if (*untrack_command)
    {
        std::string repoPath;
        try
        {
            repoPath = findRepositoryPath(repoPathArg).string();
        } catch (const std::runtime_error &e)
        {
            std::cout << e.what() << std::endl;
            return -1;
        }
        if (pathArg == "*")
        {
            Tracker::untrackAll(repoPath);
            std::cout << "Removed all files from tracking" << std::endl;
            return 0;
        }
        if (!std::filesystem::exists(pathArg) || !Tracker::is_subpath(pathArg, repoPath))
        {
            std::cout << "File doesn't exist or is outside the repository scope" << std::endl;
            return -1;
        }
        Tracker::untrackFile(repoPath, pathArg);
        std::cout << "Stopped tracking " << pathArg << std::endl;
        return 0;
    }
    if (*ignore_command)
    {
        std::string repoPath;
        try
        {
            repoPath = findRepositoryPath(repoPathArg).string();
        } catch (const std::runtime_error &e)
        {
            std::cout << e.what() << std::endl;
            return -1;
        }
        if (!std::filesystem::exists(pathArg) || !Tracker::is_subpath(pathArg, repoPath))
        {
            std::cout << "File doesn't exist or is outside the repository scope" << std::endl;
            return -1;
        }
        Tracker::blacklistFile(repoPath, pathArg);
        std::cout << "Blacklisted " << pathArg << std::endl;
        return 0;
    }
    if (*unignore_command)
    {
        std::string repoPath;
        try
        {
            repoPath = findRepositoryPath(repoPathArg).string();
        } catch (const std::runtime_error &e)
        {
            std::cout << e.what() << std::endl;
            return -1;
        }
        if (!std::filesystem::exists(pathArg) || !Tracker::is_subpath(pathArg, repoPath))
        {
            std::cout << "File doesn't exist or is outside the repository scope" << std::endl;
            return -1;
        }
        Tracker::unblacklistFile(repoPath, pathArg);
        std::cout << "Removed from blacklist " << pathArg << std::endl;
        return 0;
    }
    if (*snapshot_command)
    {
        std::string repoPath;
        try
        {
            repoPath = findRepositoryPath(repoPathArg).string();
        } catch (const std::runtime_error &e)
        {
            std::cout << e.what() << std::endl;
            return -1;
        }
        auto changedFiles = Diff::findChangedFiles(repoPath, Tracker::getTrackedFiles(repoPath));
        if (changedFiles.empty())
        {
            std::cout << "No changes in the tracked files since last snapshot. Try using fstm track * to track all your files" << std::endl;
            return 0;
        }
        std::string currentId = Filesystem::getCurrentId(repoPath);
        Snapshots::Snapshot s(repoPath, currentId, changedFiles);
        if (std::filesystem::exists(repoPath + "/.fstm/snapshots/" + s.id))
        {
            std::cout << "A snapshot with the same hash already exists: " << s.id << std::endl;
            return -1;
        }
        Filesystem::Filesystem fs = Filesystem::Filesystem::fromSnapshot(repoPath, s);
        s.write(repoPath, message);
        fs.write(repoPath + "/.fstm/snapshots/" + s.id);
        Filesystem::setCurrent(repoPath, s.id);
        std::cout << "Successfully created snapshot " << s.id << std::endl;
        return 0;
    }
    std::cout << "No command provided. Try using fstm --help for help" << std::endl;

    return 0;
}