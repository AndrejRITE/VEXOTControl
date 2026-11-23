#pragma once
#ifndef APPLOG_H
#define APPLOG_H

// Application log: one plain-text file per day in a log folder (by default
// %LOCALAPPDATA%\VEXOTControl\logs). Thread-safe and independent of wx, so
// the hardware layer (Motor, Ketek) can use it too. Logging never throws
// and never fails the caller; if the file can't be written the message is
// dropped.

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <vector>

namespace AppLog
{
	enum class Level { Info, Warning, Error };

	namespace Detail
	{
		struct State
		{
			std::mutex mutex{};
			std::filesystem::path directory{};
		};

		inline auto GetState() -> State&
		{
			static State state{};
			return state;
		}

		inline auto LocalTime(const std::time_t time) -> std::tm
		{
			std::tm result{};
			localtime_s(&result, &time);
			return result;
		}

		inline auto LevelName(const Level level) -> const char*
		{
			switch (level)
			{
			case Level::Warning: return "WARN ";
			case Level::Error:   return "ERROR";
			default:             return "INFO ";
			}
		}
	}

	// Sets the log folder (created if needed) and deletes daily log files
	// older than keepDays. Call once at startup.
	inline void Initialize(const std::filesystem::path& directory, const int keepDays = 30)
	{
		auto& state = Detail::GetState();
		std::lock_guard<std::mutex> lock(state.mutex);

		std::error_code ec{};
		std::filesystem::create_directories(directory, ec);
		state.directory = directory;

		// Prune old files: VEXOTControl_YYYY-MM-DD.log
		const auto cutoff = std::filesystem::file_time_type::clock::now() - std::chrono::hours(24 * keepDays);
		for (std::filesystem::directory_iterator it(directory, ec), end; !ec && it != end; it.increment(ec))
		{
			std::error_code fileEc{};
			if (!it->is_regular_file(fileEc) || it->path().extension() != ".log")
				continue;

			const auto written = it->last_write_time(fileEc);
			if (!fileEc && written < cutoff)
				std::filesystem::remove(it->path(), fileEc);
		}
	}

	inline auto Directory() -> std::filesystem::path
	{
		auto& state = Detail::GetState();
		std::lock_guard<std::mutex> lock(state.mutex);
		return state.directory;
	}

	inline void Write(const Level level, const std::string_view component, const std::string_view message)
	{
		try
		{
			auto& state = Detail::GetState();
			std::lock_guard<std::mutex> lock(state.mutex);

			if (state.directory.empty())
				return;

			const auto now = std::chrono::system_clock::now();
			const auto time = std::chrono::system_clock::to_time_t(now);
			const auto local = Detail::LocalTime(time);
			const auto milliseconds =
				std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

			std::ostringstream fileName{};
			fileName << "VEXOTControl_" << std::put_time(&local, "%Y-%m-%d") << ".log";

			std::ofstream out(state.directory / fileName.str(), std::ios::out | std::ios::app);
			if (!out)
				return;

			out << std::put_time(&local, "%Y-%m-%d %H:%M:%S") << '.'
				<< std::setw(3) << std::setfill('0') << milliseconds << std::setfill(' ')
				<< "  " << Detail::LevelName(level)
				<< "  [" << component << "]"
				<< "  (thread " << GetCurrentThreadId() << ")  "
				<< message << '\n';
		}
		catch (...)
		{
			// Logging must never take the application down.
		}
	}

	inline void Info(const std::string_view component, const std::string_view message) { Write(Level::Info, component, message); }
	inline void Warning(const std::string_view component, const std::string_view message) { Write(Level::Warning, component, message); }
	inline void Error(const std::string_view component, const std::string_view message) { Write(Level::Error, component, message); }
}

namespace FileUtils
{
	// UTF-8 text of a path for messages. path::string() throws on MSVC for
	// characters outside the ANSI code page (e.g. a Cyrillic user name).
	inline auto PathToUtf8(const std::filesystem::path& path) -> std::string
	{
		const auto utf8 = path.u8string();
		return std::string(utf8.begin(), utf8.end());
	}

	// Writes `content` to `path` so that the file is either fully replaced
	// or left untouched: the data goes to a temporary file next to it, which
	// is flushed to disk and then renamed over the target. A crash, power
	// loss or full disk in the middle can't leave a truncated or empty file.
	inline auto WriteFileAtomically
	(
		const std::filesystem::path& path,
		const std::string& content,
		std::string* error = nullptr
	) -> bool
	{
		const auto fail = [&](const std::string& message)
			{
				if (error)
					*error = message;
				return false;
			};

		std::error_code ec{};
		if (path.has_parent_path())
			std::filesystem::create_directories(path.parent_path(), ec);

		auto tempPath = path;
		tempPath += L".tmp";

		{
			const HANDLE file = CreateFileW
			(
				tempPath.c_str(), GENERIC_WRITE, 0, nullptr,
				CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr
			);

			if (file == INVALID_HANDLE_VALUE)
			{
				const DWORD code = GetLastError();
				return fail("cannot create " + PathToUtf8(tempPath) + " (Windows error " + std::to_string(code) + ")");
			}

			bool written = true;
			DWORD code = 0;
			std::size_t offset = 0;

			while (written && offset < content.size())
			{
				const DWORD chunk = static_cast<DWORD>(std::min<std::size_t>(content.size() - offset, 1u << 20));
				DWORD done = 0;
				written = WriteFile(file, content.data() + offset, chunk, &done, nullptr) && done == chunk;
				offset += done;
			}

			// Make sure the data is on disk before the rename makes it the
			// real file; otherwise a power cut could leave it empty.
			if (written)
				written = FlushFileBuffers(file) != FALSE;

			if (!written)
				code = GetLastError();

			CloseHandle(file);

			if (!written)
			{
				std::filesystem::remove(tempPath, ec);
				return fail("writing " + PathToUtf8(tempPath) + " failed (Windows error " + std::to_string(code) + ", disk full?)");
			}
		}

		if (!MoveFileExW(tempPath.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH))
		{
			const DWORD code = GetLastError();
			std::filesystem::remove(tempPath, ec);
			return fail("replacing " + PathToUtf8(path) + " failed (Windows error " + std::to_string(code) + ")");
		}

		return true;
	}
}

#endif // !APPLOG_H
