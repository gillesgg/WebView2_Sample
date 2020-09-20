#include <pch.h>
#include "Logger.h"
//#include "Settings/Settings.h"



BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", logging::trivial::severity_level)
BOOST_LOG_ATTRIBUTE_KEYWORD(processid, "ProcessID", logging::attributes::current_process_id::value_type)

HRESULT GetLog(fs::path& pFileName)
{
	TCHAR				buffer[MAX_PATH * sizeof(TCHAR)];
	LPWSTR				wszPath = nullptr;
	HRESULT				hr = S_OK;

	hr = SHGetKnownFolderPath(FOLDERID_ProgramData, KF_FLAG_CREATE, nullptr, &wszPath);
	if (SUCCEEDED(hr))
	{
		GetModuleFileName(nullptr, buffer, MAX_PATH);
		const auto pos = std::wstring(buffer).find_last_of(L"\\/");
		const auto pos1 = std::wstring(buffer).find_last_of(L".");

		if (pos != 0 && pos1 != 0 && pos1 > pos)
		{
			const auto str_FileNameNoEx = std::wstring(buffer).substr(pos + 1, pos1 - pos - 1);

			const auto root = fs::path(wszPath).append(str_FileNameNoEx);

			const auto str_FileNamePath = fs::path(wszPath).append(str_FileNameNoEx).append(L"logs");

			pFileName = str_FileNamePath;
			pFileName.append(str_FileNameNoEx);

			if (!is_directory(root))
			{
				if (!create_directory(root))
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
				}
			}
			if (!is_directory(str_FileNamePath))
			{
				if (!create_directory(str_FileNamePath))
				{
					hr = HRESULT_FROM_WIN32(GetLastError());
				}
			}
		}
		else
		{
			hr = HRESULT_FROM_WIN32(ERROR_PATH_NOT_FOUND);
		}
	}
	return (hr);
}

BOOST_LOG_GLOBAL_LOGGER_INIT(logger, src::severity_logger_mt) {
	src::severity_logger_mt<boost::log::trivial::severity_level> logger;
	fs::path				pFileName;

	// add attributes
	logger.add_attribute("LineID", attrs::counter<unsigned int>(1));     // lines are sequentially numbered
	logger.add_attribute("TimeStamp", attrs::local_clock());             // each log line gets a timestamp

																		 
	typedef sinks::synchronous_sink<sinks::text_file_backend> TextSink; // add a text sink
	
//#if _DEBUG
	typedef sinks::synchronous_sink<sinks::debug_output_backend> outputdebugstring_sink;
	boost::shared_ptr<outputdebugstring_sink> output_sink(new outputdebugstring_sink());
//#endif // _DEBUG
	if (GetLog(pFileName) == S_OK)
	{
		auto backend1 = boost::make_shared<sinks::text_file_backend>(
			keywords::file_name = pFileName.generic_string() + "sign_%Y-%m-%d_%H-%M-%S.%N.log",
			keywords::rotation_size = 10 * 1024 * 1024,
			keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
			keywords::min_free_space = 30 * 1024 * 1024);
			
		backend1->auto_flush(true);

		

		boost::shared_ptr<TextSink> sink(new TextSink(backend1));

		logging::formatter formatter = expr::stream
			<< std::setw(7) << std::setfill('0') << line_id << std::setfill(' ') << " | "
			<< format_date_time(timestamp, "%Y-%m-%d, %H:%M:%S.%f") << " | "
			<< "[" << logging::trivial::severity << "]" << " | "
			<< expr::smessage;
		sink->set_formatter(formatter);

		auto currentlog = logging::trivial::info;

		auto iLogLevel = 1;
		currentlog = iLogLevel == 0 ? logging::trivial::trace : iLogLevel == 1 ? logging::trivial::debug : logging::trivial::info;

		logging::core::get()->set_filter(logging::trivial::severity >= currentlog);
		

//#if _DEBUG
		logging::formatter formatter1 = expr::stream
			<< std::setw(7) << std::setfill('0') << line_id << std::setfill(' ') << " | "
			<< "[" << format_date_time(
				timestamp, "%Y-%m-%d %H:%M:%S") << "]"
			<< "[" << severity << "] "
			<< expr::smessage
			<< std::endl;

		output_sink->set_formatter(formatter1);
		logging::core::get()->add_sink(output_sink);
//#endif // _DEBUG
		// "register" our sink
		logging::core::get()->add_sink(sink);		
	}
	return logger;
}

