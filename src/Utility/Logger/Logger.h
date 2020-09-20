#pragma once


// just log messages with severity >= SEVERITY_THRESHOLD are written
#define SEVERITY_THRESHOLD logging::trivial::warning

// register a global logger
BOOST_LOG_GLOBAL_LOGGER(logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>)

// just a helper macro used by the macros below - don't use it in your code
#define LOG(severity) BOOST_LOG_SEV(logger::get(),boost::log::trivial::severity)


// ===== log macros =====
#define LOG_TRACE   LOG(trace)
#define LOG_DEBUG	LOG(debug)
#define LOG_INFO    LOG(info)
#define LOG_WARNING LOG(warning)
#define LOG_ERROR   LOG(error)
#define LOG_FATAL   LOG(fatal)

static std::wstring  GetLastErrorToString(DWORD err = -1)
{
	if (err == -1)
		err = GetLastError();


	TCHAR* errMessage = nullptr;
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		nullptr,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // default language
		reinterpret_cast<LPTSTR>(& errMessage),
		0,
		nullptr))
		return L"E_FAILED";

	return errMessage;
}