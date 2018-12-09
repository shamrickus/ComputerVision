#ifndef _FILE_H_
#define _FILE_H_

#include <vector>

#ifdef _WIN32
#define PATH_MAX 2048
#include <processenv.h>
#include <consoleapi.h>
#include <fileapi.h>
#include <handleapi.h>
#else
#include <dirent.h>
#include <uninstd.h>
#include <limits.h>
#endif

class File
{
public:
	static std::vector<std::string> ReadFile(const char* pFileName)
	{
		char line[255];
		auto file = std::vector<std::string>();

		FILE * fp = fopen(pFileName, "r");
		if(fp != nullptr)
		{
			while(fgets(line, sizeof(line), fp) != nullptr)
			{
				file.emplace_back(line);
			}
			fclose(fp);
		}

		return file;
	}
	static std::string ReadFileToStr(const char* pFileName)
	{
		char line[255];
		auto file = std::string();

		FILE * fp = fopen(pFileName, "r");
		if(fp != nullptr)
		{
			while(fgets(line, sizeof(line), fp) != nullptr)
			{
				file += std::string(line);
			}
			fclose(fp);
		}

		return file;
	}
	static std::vector<const char*>* ReadFileC(const char* pFileName)
	{
		auto readFile = ReadFile(pFileName);
		auto cfile = new std::vector<const char*>();
		for(auto& str: readFile)
		{
			cfile->push_back(str.c_str());
		}
		return cfile;
	}

	static std::vector<std::string> ListFolder(const char* pDir)
	{
#ifdef _WIN32
		return ListWindows(pDir);
#else
		return ListLinux(pDir);
#endif
	}
	static std::string GetCWD()
	{
#ifdef _WIN32
		return CWDWindows();
#else
		return CWDLinux();
#endif
	}
private:
#ifdef _WIN32
	static std::string CWDWindows()
	{
		char cwd[PATH_MAX];
		GetCurrentDirectory(PATH_MAX, cwd);
		return cwd;
	}
	static std::vector<std::string> ListWindows(const char* pDir)
	{
		auto strs = std::vector<std::string>();
		WIN32_FIND_DATA fdFile;
		HANDLE hFind = NULL;
		char line[PATH_MAX];

		sprintf(line, "%s\\*.*", pDir);

		if((hFind = FindFirstFile(line, &fdFile)) != INVALID_HANDLE_VALUE)
		{
			do
			{
				if(strcmp(fdFile.cFileName, ".") != 0 && strcmp(fdFile.cFileName, "..") != 0)
				{
					sprintf(line, "%s\\%s", pDir, fdFile.cFileName);
					strs.push_back(line);
				}
			} while (FindNextFile(hFind, &fdFile));
		}

		FindClose(hFind);
		return strs;
	}
#else
	static std::string CWDLinux()
	{
		char cwd[PATH_MAX];
		if(getcwd(cwd, sizeof(cwd)) != NULL)
			return cwd;
	}
	static std::vector<std::string> ListLinux(const char* pDir)
	{
		auto strs = std::vector<std::string>();

		struct dirent *dir;
		DIR *d = opendir(pDir);
		if(d != nullptr)
		{
			while ((de = readdir(d)) != NULL)
				strs.push_back(de->d_name);
			closedir(d);
		}

		return strs;
	}
#endif
};


#endif