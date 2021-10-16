//#include <curlpp/Easy.hpp>
//#include <curlpp/Multi.hpp>
//#include <curlpp/Options.hpp>
//#include <curlpp/cURLpp.hpp>
//#include <string>
//#include <sstream>
//#include <iostream>
//#include <fstream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <regex>
//#include <boost/log/trivial.hpp>
//#include <mutex>
//#include <vector>
//#include <thread>
//
//std::wstring utf82uni(const std::string& utf8_string)
//{
//	auto wide_size = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, NULL, 0);
//	if (wide_size == ERROR_NO_UNICODE_TRANSLATION)
//		throw std::exception("Invalid UTF-8 sequence.");
//	if (wide_size == 0)
//		throw std::exception("Error in conversion.");
//	std::wstring uni_string;
//	uni_string.resize(wide_size - 1ull);
//	MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, &uni_string[0], wide_size - 1);
//
//	return uni_string;
//}
//
//std::string uni2utf8(const std::wstring& uni_string)
//{
//	auto multi_size = WideCharToMultiByte(CP_UTF8, 0, uni_string.c_str(), -1, NULL, 0, NULL, NULL);
//	if (multi_size == ERROR_NO_UNICODE_TRANSLATION)
//		throw std::exception("Invalid UTF-8 sequence.");
//	if (multi_size == 0)
//		throw std::exception("Error in conversion.");
//	std::string utf8_string;
//	utf8_string.resize(multi_size - 1ull);
//	WideCharToMultiByte(CP_UTF8, 0, uni_string.c_str(), -1, &utf8_string[0], multi_size - 1, NULL, NULL);
//
//	return utf8_string;
//}
//
//std::mutex g_locker;
//size_t g_count = 1;
//
//bool getSinglePicture(std::string utf8_url)
//{
//	try
//	{
//		curlpp::Cleanup cleanup;
//
//		std::ostringstream utf8_response_html;
//		curlpp::Easy request_html;
//		request_html.setOpt(cURLpp::Options::Proxy("127.0.0.1"));
//		request_html.setOpt(cURLpp::Options::ProxyPort(10809));
//		request_html.setOpt(cURLpp::Options::HttpProxyTunnel(true));
//		request_html.setOpt(cURLpp::Options::Url(utf8_url));
//		request_html.setOpt(cURLpp::Options::WriteStream(&utf8_response_html));
//		request_html.setOpt(cURLpp::Options::UserAgent("Mozilla/5.0"));
//		request_html.setOpt(cURLpp::Options::Referer("https://www.pixiv.net/ranking.php?mode=daily&content=illust"));
//
//		request_html.perform();
//
//		std::string utf8_response_html_string = utf8_response_html.str();
//
//		auto utf8_search_file_name_result = strstr(utf8_response_html_string.c_str(), "\"illustTitle\":\"");
//		auto utf8_search_original_url_result = strstr(utf8_response_html_string.c_str(), "\"original\":\"");
//		if (utf8_search_file_name_result && utf8_search_original_url_result)
//		{
//			std::string utf8_file_name;
//			std::string utf8_original_url;
//			g_locker.lock();
//			utf8_file_name += std::to_string(g_count);
//			BOOST_LOG_TRIVIAL(debug) << "current artwork url:" << utf8_url << std::endl;
//			BOOST_LOG_TRIVIAL(info) << "downloading " << g_count << " picture" << std::endl;
//			++g_count;
//			g_locker.unlock();
//			utf8_file_name += "_";
//
//			char* utf8_file_name_end = (char*)utf8_search_file_name_result + sizeof("\"illustTitle\":\"") - 1;
//			for (; *utf8_file_name_end != 0; utf8_file_name_end++)
//			{
//				if (*utf8_file_name_end == '"')
//					break;
//
//				/*if (*utf8_file_name_end == '\\' ||
//					*utf8_file_name_end == '/' ||
//					*utf8_file_name_end == ':' ||
//					*utf8_file_name_end == '*' ||
//					*utf8_file_name_end == '?' ||
//					*utf8_file_name_end == '>' ||
//					*utf8_file_name_end == '<' ||
//					*utf8_file_name_end == '|')
//					continue;*/
//
//				utf8_file_name += *utf8_file_name_end;
//			}
//			char* utf8_original_url_end = (char*)utf8_search_original_url_result + sizeof("\"original\":\"") - 1;
//			for (; *utf8_original_url_end != 0; utf8_original_url_end++)
//			{
//				if (*utf8_original_url_end == '"')
//					break;
//
//				utf8_original_url += *utf8_original_url_end;
//			}
//
//			utf8_file_name += ".jpg";
//			auto uni_file_name = utf82uni(utf8_file_name);
//			for (auto& ch : uni_file_name)
//			{
//				if (ch == L'\\' ||
//					ch == L'/' ||
//					ch == L':' ||
//					ch == L'*' ||
//					ch == L'?' ||
//					ch == L'>' ||
//					ch == L'<' ||
//					ch == L'|')
//					ch = L'-';
//			}
//
//			FILE* pfile = nullptr;
//			if (!_wfopen_s(&pfile, uni_file_name.c_str(), L"wb") && pfile)
//			{
//				curlpp::Easy request_picture;
//				request_picture.setOpt(cURLpp::Options::Proxy("127.0.0.1"));
//				request_picture.setOpt(cURLpp::Options::ProxyPort(10809));
//				request_picture.setOpt(cURLpp::Options::HttpProxyTunnel(true));
//				request_picture.setOpt(cURLpp::Options::Url(utf8_original_url));
//				request_picture.setOpt(cURLpp::Options::WriteFile(pfile));
//				request_picture.setOpt(cURLpp::Options::UserAgent("Mozilla/5.0"));
//				request_picture.setOpt(cURLpp::Options::Referer("https://www.pixiv.net/ranking.php?mode=daily&content=illust"));
//
//				request_picture.perform();
//
//				fclose(pfile);
//
//				return true;
//			}
//			else
//			{
//				BOOST_LOG_TRIVIAL(error) << "fopen_s error" << std::endl;
//			}
//		}
//		else
//		{
//			BOOST_LOG_TRIVIAL(error) << "regex_search error" << std::endl;
//		}
//	}
//	catch (curlpp::RuntimeError& e)
//	{
//		BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
//	}
//	catch (curlpp::LogicError& e)
//	{
//		BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
//	}
//
//	return false;
//}
//
//void work_thread(int64_t begin, int64_t end)
//{
//	bool is_exit = false;
//
//	while (!is_exit)
//	{
//		try
//		{
//			for (int64_t i = begin; i < end; i++)
//			{
//				curlpp::Cleanup cleanup;
//
//				std::string url = "https://www.pixiv.net/ranking.php?mode=daily&content=illust&p=";
//				url += std::to_string(i);
//				url += "&format=json";
//				std::ostringstream utf8_response_html;
//				curlpp::Easy request_html;
//				request_html.setOpt(cURLpp::Options::Proxy("127.0.0.1"));
//				request_html.setOpt(cURLpp::Options::ProxyPort(10809));
//				request_html.setOpt(cURLpp::Options::HttpProxyTunnel(true));
//				request_html.setOpt(cURLpp::Options::Url(url));
//				request_html.setOpt(cURLpp::Options::WriteStream(&utf8_response_html));
//				request_html.setOpt(cURLpp::Options::UserAgent("Mozilla/5.0"));
//
//				BOOST_LOG_TRIVIAL(debug) << "search url:" << url << std::endl;
//
//				request_html.perform();
//
//				std::string utf8_response_html_string = utf8_response_html.str();
//				const char* utf8_search_id_result = utf8_response_html_string.c_str();
//
//				while (utf8_search_id_result)
//				{
//					utf8_search_id_result = (char*)strstr(utf8_search_id_result, "\"illust_id\":") + sizeof("\"illust_id\":") - 1;
//					if (!(utf8_search_id_result - (sizeof("\"illust_id\":") - 1)))
//						break;
//
//					std::string utf8_artwork_url = "https://www.pixiv.net/artworks/";
//
//					for (auto utf8_id_end = utf8_search_id_result; *utf8_id_end != 0; utf8_id_end++)
//					{
//						if (*utf8_id_end == ',')
//							break;
//
//						utf8_artwork_url += *utf8_id_end;
//					}
//
//					auto ret = getSinglePicture(utf8_artwork_url);
//
//					if (!ret)
//						BOOST_LOG_TRIVIAL(error) << "download error" << std::endl;
//				}
//
//				is_exit = true;
//			}
//		}
//		catch (curlpp::RuntimeError& e)
//		{
//			BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
//		}
//		catch (curlpp::LogicError& e)
//		{
//			BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
//		}
//	}
//}
//
//#define THREADS_COUNT std::thread::hardware_concurrency()
//#define MAX_PART 12
//
//int main()
//{
//	SetConsoleOutputCP(65001);
//	CONSOLE_FONT_INFOEX Info = { 0 };
//	Info.cbSize = sizeof(Info);
//	Info.dwFontSize.Y = 16;
//	Info.FontWeight = FW_NORMAL;
//	wcscpy_s(Info.FaceName, L"Consolas");
//	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &Info);
//
//	BOOST_LOG_TRIVIAL(info) << "strating " << THREADS_COUNT << " thread(s)" << std::endl;
//
//	uint64_t last_start = 1;
//	uint64_t thread_count = 1;
//
//	std::vector<std::thread*> thread_pool;
//	for (size_t i = 0; i < THREADS_COUNT; i++)
//	{
//		BOOST_LOG_TRIVIAL(info) << "strat " << thread_count << " thread,begin:" << last_start << ",end:" << last_start + MAX_PART / THREADS_COUNT << std::endl;
//
//		thread_pool.push_back(new std::thread(work_thread, last_start, last_start + MAX_PART / THREADS_COUNT));
//
//		using namespace std::chrono_literals;
//		std::this_thread::sleep_for(5s);
//
//		last_start += MAX_PART / THREADS_COUNT;
//		++thread_count;
//	}
//
//	for (auto& thread_item : thread_pool)
//		thread_item->join();
//
//	for (auto& thread_item : thread_pool)
//		delete thread_item;
//
//	return 0;
//}

//----------------------------------------

//#include <curlpp/Easy.hpp>
//#include <curlpp/Multi.hpp>
//#include <curlpp/Options.hpp>
//#include <curlpp/cURLpp.hpp>
//#include <string>
//#include <sstream>
//#include <iostream>
//#include <fstream>
//#include <stdio.h>
//#include <stdlib.h>
//#include <regex>
//#include <boost/log/trivial.hpp>
//#include <mutex>
//#include <vector>
//#include <thread>
//#include <chrono>
//
//std::wstring utf82uni(const std::string& utf8_string)
//{
//	auto wide_size = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, NULL, 0);
//	if (wide_size == ERROR_NO_UNICODE_TRANSLATION)
//		throw std::exception("Invalid UTF-8 sequence.");
//	if (wide_size == 0)
//		throw std::exception("Error in conversion.");
//	std::wstring uni_string;
//	uni_string.resize(wide_size - 1ull);
//	MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, &uni_string[0], wide_size - 1);
//
//	return uni_string;
//}
//
//std::string uni2utf8(const std::wstring& uni_string)
//{
//	auto multi_size = WideCharToMultiByte(CP_UTF8, 0, uni_string.c_str(), -1, NULL, 0, NULL, NULL);
//	if (multi_size == ERROR_NO_UNICODE_TRANSLATION)
//		throw std::exception("Invalid UTF-8 sequence.");
//	if (multi_size == 0)
//		throw std::exception("Error in conversion.");
//	std::string utf8_string;
//	utf8_string.resize(multi_size - 1ull);
//	WideCharToMultiByte(CP_UTF8, 0, uni_string.c_str(), -1, &utf8_string[0], multi_size - 1, NULL, NULL);
//
//	return utf8_string;
//}
//
//std::mutex g_locker;
//size_t g_count = 1;
//
//bool perform_multi(curlpp::Multi& multi_requests)
//{
//	int still_running_handles = 0;
//	bool ret = true;
//
//	while (!multi_requests.perform(&still_running_handles)) {};
//
//	while (still_running_handles)
//	{
//		struct timeval timeout;
//		int rc; /* select() return code */
//
//		fd_set fdread;
//		fd_set fdwrite;
//		fd_set fdexcep;
//		int maxfd;
//
//		FD_ZERO(&fdread);
//		FD_ZERO(&fdwrite);
//		FD_ZERO(&fdexcep);
//
//		/* set a suitable timeout to play around with */
//		timeout.tv_sec = 10;
//		timeout.tv_usec = 0;
//
//		/* get file descriptors from the transfers */
//		multi_requests.fdset(&fdread, &fdwrite, &fdexcep, &maxfd);
//
//		rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
//		switch (rc) {
//		case -1:
//			/* select error */
//			still_running_handles = 0;
//			ret = false;
//			break;
//		case 0:
//			/* timeout, do something else */
//			break;
//		default:
//			/* one or more of curl's file descriptors say there's data to read
//			   or write */
//			while (!multi_requests.perform(&still_running_handles)) {};
//			break;
//		}
//	}
//
//	return ret;
//}
//
//void work_thread(int64_t begin, int64_t end)
//{
//	try
//	{
//		curlpp::Cleanup cleanup;
//		curlpp::Multi ranking_multi_requests;
//		curlpp::Easy** ranking_requests_handles = new curlpp::Easy * [end - begin]{};
//		std::ostringstream** ranking_responses_handles = new std::ostringstream * [end - begin]{};
//
//		for (int64_t i = begin; i < end; i++)
//		{
//			std::string utf8_ranking_url = "https://www.pixiv.net/ranking.php?mode=daily&content=illust&p=";
//			utf8_ranking_url += std::to_string(i + 1);
//			utf8_ranking_url += "&format=json";
//			ranking_responses_handles[i - begin] = new std::ostringstream;
//			ranking_requests_handles[i - begin] = new curlpp::Easy;
//			ranking_requests_handles[i - begin]->setOpt(cURLpp::Options::Proxy("127.0.0.1"));
//			ranking_requests_handles[i - begin]->setOpt(cURLpp::Options::ProxyPort(10809));
//			ranking_requests_handles[i - begin]->setOpt(cURLpp::Options::HttpProxyTunnel(true));
//			ranking_requests_handles[i - begin]->setOpt(cURLpp::Options::Url(utf8_ranking_url));
//			ranking_requests_handles[i - begin]->setOpt(cURLpp::Options::WriteStream(ranking_responses_handles[i - begin]));
//			ranking_requests_handles[i - begin]->setOpt(cURLpp::Options::UserAgent("Mozilla/5.0"));
//			if (ranking_responses_handles[i - begin] && ranking_requests_handles[i - begin])
//				ranking_multi_requests.add(ranking_requests_handles[i - begin]);
//
//			BOOST_LOG_TRIVIAL(debug) << "search url:" << utf8_ranking_url << std::endl;
//		}
//
//		if (perform_multi(ranking_multi_requests))
//		{
//			size_t count = 1;
//			curlpp::Multi artwork_multi_requests;
//			curlpp::Easy** artwork_requests_handles = new curlpp::Easy * [(end - begin) * 50]{};
//			std::ostringstream** artwork_responses_handles = new std::ostringstream * [(end - begin) * 50]{};
//
//			for (size_t i = 0; i < end - begin; i++)
//			{
//				std::string utf8_response_html_string = ranking_responses_handles[i]->str();
//				auto utf8_search_id_result = utf8_response_html_string.c_str();
//
//				while (utf8_search_id_result)
//				{
//					utf8_search_id_result = (char*)strstr(utf8_search_id_result, "\"illust_id\":") + sizeof("\"illust_id\":") - 1;
//					if (!(utf8_search_id_result - (sizeof("\"illust_id\":") - 1)))
//						break;
//
//					std::string utf8_artwork_url = "https://www.pixiv.net/artworks/";
//
//					for (auto utf8_id_end = utf8_search_id_result; *utf8_id_end != 0; utf8_id_end++)
//					{
//						if (*utf8_id_end == ',')
//							break;
//
//						utf8_artwork_url += *utf8_id_end;
//					}
//
//					artwork_responses_handles[count - 1] = new std::ostringstream;
//					artwork_requests_handles[count - 1] = new curlpp::Easy;
//					artwork_requests_handles[count - 1]->setOpt(cURLpp::Options::Proxy("127.0.0.1"));
//					artwork_requests_handles[count - 1]->setOpt(cURLpp::Options::ProxyPort(10809));
//					artwork_requests_handles[count - 1]->setOpt(cURLpp::Options::HttpProxyTunnel(true));
//					artwork_requests_handles[count - 1]->setOpt(cURLpp::Options::Url(utf8_artwork_url));
//					artwork_requests_handles[count - 1]->setOpt(cURLpp::Options::WriteStream(artwork_responses_handles[count - 1]));
//					artwork_requests_handles[count - 1]->setOpt(cURLpp::Options::UserAgent("Mozilla/5.0"));
//					if (artwork_responses_handles[count - 1] && artwork_requests_handles[count - 1])
//						artwork_multi_requests.add(artwork_requests_handles[count - 1]);
//
//					BOOST_LOG_TRIVIAL(debug) << count++ << ":" << utf8_artwork_url << std::endl;
//				}
//			}
//
//			--count;
//
//			if (perform_multi(artwork_multi_requests))
//			{
//				curlpp::Multi picture_multi_requests;
//				curlpp::Easy** picture_requests_handles = new curlpp::Easy * [count] {};
//				FILE*** picture_files_handles = new FILE * *[count] {};
//
//				for (size_t i = 0; i < count; i++)
//				{
//					std::string utf8_response_html_string = artwork_responses_handles[i]->str();
//
//					auto utf8_search_file_name_result = strstr(utf8_response_html_string.c_str(), "\"illustTitle\":\"");
//					auto utf8_search_original_url_result = strstr(utf8_response_html_string.c_str(), "\"original\":\"");
//					if (utf8_search_file_name_result && utf8_search_original_url_result)
//					{
//						std::string utf8_file_name;
//						std::string utf8_original_url;
//						utf8_file_name += std::to_string(i + 1);
//						utf8_file_name += "_";
//
//						char* utf8_file_name_end = (char*)utf8_search_file_name_result + sizeof("\"illustTitle\":\"") - 1;
//						for (; *utf8_file_name_end != 0; utf8_file_name_end++)
//						{
//							if (*utf8_file_name_end == '"')
//								break;
//
//							utf8_file_name += *utf8_file_name_end;
//						}
//						char* utf8_original_url_end = (char*)utf8_search_original_url_result + sizeof("\"original\":\"") - 1;
//						for (; *utf8_original_url_end != 0; utf8_original_url_end++)
//						{
//							if (*utf8_original_url_end == '"')
//								break;
//
//							utf8_original_url += *utf8_original_url_end;
//						}
//
//						utf8_file_name += ".jpg";
//						auto uni_file_name = utf82uni(utf8_file_name);
//						for (auto& ch : uni_file_name)
//						{
//							if (ch == L'\\' ||
//								ch == L'/' ||
//								ch == L':' ||
//								ch == L'*' ||
//								ch == L'?' ||
//								ch == L'>' ||
//								ch == L'<' ||
//								ch == L'|')
//								ch = L'-';
//						}
//
//						BOOST_LOG_TRIVIAL(debug) << i + 1 << ":" << utf8_file_name << std::endl;
//
//						picture_files_handles[i] = new FILE*;
//						if (_wfopen_s(picture_files_handles[i], uni_file_name.c_str(), L"wb") || !*picture_files_handles[i])
//							BOOST_LOG_TRIVIAL(error) << i + 1 << ":" << "_wfopen_s error" << std::endl;
//						picture_requests_handles[i] = new curlpp::Easy;
//						picture_requests_handles[i]->setOpt(cURLpp::Options::Proxy("127.0.0.1"));
//						picture_requests_handles[i]->setOpt(cURLpp::Options::ProxyPort(10809));
//						picture_requests_handles[i]->setOpt(cURLpp::Options::HttpProxyTunnel(true));
//						picture_requests_handles[i]->setOpt(cURLpp::Options::Url(utf8_original_url));
//						picture_requests_handles[i]->setOpt(cURLpp::Options::WriteFile(*picture_files_handles[i]));
//						picture_requests_handles[i]->setOpt(cURLpp::Options::UserAgent("Mozilla/5.0"));
//						picture_requests_handles[i]->setOpt(cURLpp::Options::Referer("https://www.pixiv.net/ranking.php?mode=daily&content=illust"));
//						if (picture_files_handles[i] && picture_requests_handles[i])
//							if (*picture_files_handles[i])
//								picture_multi_requests.add(picture_requests_handles[i]);
//					}
//				}
//
//				if (!perform_multi(picture_multi_requests))
//					BOOST_LOG_TRIVIAL(error) << "perform_multi error" << std::endl;
//
//				for (size_t i = 0; i < count; i++)
//				{
//					if (picture_requests_handles[i])
//						picture_multi_requests.remove(picture_requests_handles[i]);
//					if (picture_requests_handles[i])
//					{
//						if (*picture_files_handles[i])
//							fclose(*picture_files_handles[i]);
//						delete picture_requests_handles[i];
//					}
//					if (picture_files_handles[i])
//						delete picture_files_handles[i];
//				}
//
//				if (picture_requests_handles)
//					delete[] picture_requests_handles;
//				if (picture_files_handles)
//					delete[] picture_files_handles;
//			}
//			else
//				BOOST_LOG_TRIVIAL(error) << "perform_multi error" << std::endl;
//
//			for (size_t i = 0; i < (end - begin) * 50; i++)
//			{
//				if (artwork_requests_handles[i])
//					artwork_multi_requests.remove(artwork_requests_handles[i]);
//				if (artwork_requests_handles[i])
//					delete artwork_requests_handles[i];
//				if (artwork_responses_handles[i])
//					delete artwork_responses_handles[i];
//			}
//
//			if (artwork_requests_handles)
//				delete[] artwork_requests_handles;
//			if (artwork_responses_handles)
//				delete[] artwork_responses_handles;
//		}
//		else
//			BOOST_LOG_TRIVIAL(error) << "perform_multi error" << std::endl;
//
//		for (size_t i = 0; i < end - begin; i++)
//		{
//			if (ranking_requests_handles[i])
//				ranking_multi_requests.remove(ranking_requests_handles[i]);
//			if (ranking_requests_handles[i])
//				delete ranking_requests_handles[i];
//			if (ranking_responses_handles[i])
//				delete ranking_responses_handles[i];
//		}
//
//		if (ranking_requests_handles)
//			delete[] ranking_requests_handles;
//		if (ranking_responses_handles)
//			delete[] ranking_responses_handles;
//	}
//	catch (curlpp::RuntimeError& e)
//	{
//		BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
//	}
//	catch (curlpp::LogicError& e)
//	{
//		BOOST_LOG_TRIVIAL(error) << e.what() << std::endl;
//	}
//}
//
//#define THREADS_COUNT std::thread::hardware_concurrency()
//#define MAX_PART 12
//
//int main()
//{
//	SetConsoleOutputCP(65001);
//	CONSOLE_FONT_INFOEX Info = { 0 };
//	Info.cbSize = sizeof(Info);
//	Info.dwFontSize.Y = 16;
//	Info.FontWeight = FW_NORMAL;
//	wcscpy_s(Info.FaceName, L"Consolas");
//	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &Info);
//
//	auto start = std::chrono::system_clock::now();
//
//	BOOST_LOG_TRIVIAL(info) << "strating " << THREADS_COUNT << " thread(s)" << std::endl;
//
//	uint64_t last_start = 1;
//	uint64_t thread_count = 1;
//
//	std::vector<std::thread*> thread_pool;
//	for (size_t i = 0; i < THREADS_COUNT; i++)
//	{
//		BOOST_LOG_TRIVIAL(info) << "strat " << thread_count << " thread,begin:" << last_start << ",end:" << last_start + MAX_PART / THREADS_COUNT << std::endl;
//
//		thread_pool.push_back(new std::thread(work_thread, last_start, last_start + MAX_PART / THREADS_COUNT));
//
//		using namespace std::chrono_literals;
//		std::this_thread::sleep_for(5s);
//
//		last_start += MAX_PART / THREADS_COUNT;
//		++thread_count;
//	}
//
//	for (auto& thread_item : thread_pool)
//		thread_item->join();
//
//	for (auto& thread_item : thread_pool)
//		delete thread_item;
//
//	auto end = std::chrono::system_clock::now();
//	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
//
//	BOOST_LOG_TRIVIAL(info) << "the program ran for " << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << " seconds" << std::endl;
//
//	//work_thread(1, 4);
//
//	return 0;
//}

#include "curl/curl.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <regex>
#include <boost/log/trivial.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <mutex>
#include <vector>
#include <thread>
#include <chrono>

#define THREADS_COUNT std::thread::hardware_concurrency()
#define MAX_PART 4

#define PROXY "127.0.0.1"
#define PROXYPORT 10809l

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
// allocations to be of _CLIENT_BLOCK type
#else
#define DBG_NEW new
#endif

#define MAX_PARALLEL 10 /* number of simultaneous transfers */

typedef struct _WRITE_CALLBACK_PARAM
{
	std::string* body;
	long num_transfer;
}WRITE_CALLBACK_PARAM, * PWRITE_CALLBACK_PARAM;

std::wstring utf82uni(const std::string& utf8_string)
{
	auto wide_size = MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, NULL, 0);
	if (wide_size == ERROR_NO_UNICODE_TRANSLATION)
		throw std::exception("Invalid UTF-8 sequence.");
	if (wide_size == 0)
		throw std::exception("Error in conversion.");
	std::wstring uni_string;
	uni_string.resize(wide_size - 1ull);
	MultiByteToWideChar(CP_UTF8, 0, utf8_string.c_str(), -1, &uni_string[0], wide_size - 1);

	return uni_string;
}

std::wstring* utf82uni(std::string const* utf8_string)
{
	auto wide_size = MultiByteToWideChar(CP_UTF8, 0, utf8_string->c_str(), -1, NULL, 0);
	if (wide_size == ERROR_NO_UNICODE_TRANSLATION)
		throw std::exception("Invalid UTF-8 sequence.");
	if (wide_size == 0)
		throw std::exception("Error in conversion.");
	std::wstring* uni_string = new std::wstring{};
	uni_string->resize(wide_size - 1ull);
	MultiByteToWideChar(CP_UTF8, 0, utf8_string->c_str(), -1, &(*uni_string)[0], wide_size - 1);

	return uni_string;
}

std::string uni2utf8(const std::wstring& uni_string)
{
	auto multi_size = WideCharToMultiByte(CP_UTF8, 0, uni_string.c_str(), -1, NULL, 0, NULL, NULL);
	if (multi_size == ERROR_NO_UNICODE_TRANSLATION)
		throw std::exception("Invalid UTF-8 sequence.");
	if (multi_size == 0)
		throw std::exception("Error in conversion.");
	std::string utf8_string;
	utf8_string.resize(multi_size - 1ull);
	WideCharToMultiByte(CP_UTF8, 0, uni_string.c_str(), -1, &utf8_string[0], multi_size - 1, NULL, NULL);

	return utf8_string;
}

static size_t write_cb(char* data, size_t n, size_t l, void* userp)
{
	PWRITE_CALLBACK_PARAM response = (PWRITE_CALLBACK_PARAM)userp;
	response->body->append(data, n * l);
	return n * l;
}

static void add_transfer(CURLM* multi_handle, CURL** request, std::string const* url, PWRITE_CALLBACK_PARAM responses)
{
	*request = curl_easy_init();
	curl_easy_setopt(*request, CURLOPT_URL, url->c_str());
	curl_easy_setopt(*request, CURLOPT_USERAGENT, "Mozilla/5.0");
	curl_easy_setopt(*request, CURLOPT_WRITEFUNCTION, write_cb);
	curl_easy_setopt(*request, CURLOPT_WRITEDATA, responses);
	curl_easy_setopt(*request, CURLOPT_PROXY, PROXY);
	curl_easy_setopt(*request, CURLOPT_PROXYPORT, PROXYPORT);
	curl_easy_setopt(*request, CURLOPT_HTTPPROXYTUNNEL, 1l);
	curl_easy_setopt(*request, CURLOPT_PRIVATE, responses);
	curl_easy_setopt(*request, CURLOPT_REFERER, "https://www.pixiv.net/");
	curl_multi_add_handle(multi_handle, *request);
}

PWRITE_CALLBACK_PARAM perform_multi(const std::vector<std::string*>& urls)
{
	auto num_of_urls = urls.size();
	long transfers = 0;
	long max_parallel = min(num_of_urls, MAX_PARALLEL);

	CURL** requests = new CURL * [num_of_urls] { nullptr };
	PWRITE_CALLBACK_PARAM responses = new WRITE_CALLBACK_PARAM[num_of_urls]{};
	CURLM* multi_handle = curl_multi_init();
	curl_multi_setopt(multi_handle, CURLMOPT_MAXCONNECTS, MAX_PARALLEL);

	for (transfers = 0; transfers < max_parallel; transfers++)
	{
		responses[transfers].num_transfer = transfers;
		responses[transfers].body = new std::string{};

		add_transfer(multi_handle, &requests[transfers], urls[transfers], &responses[transfers]);

		BOOST_LOG_TRIVIAL(debug) << transfers + 1 << ":url:" << *urls[transfers] << ",using requests object:" << requests[transfers] << ",using responses object:" << responses[transfers].body << std::endl;
	}

	int msgs_left = -1;
	int still_alive = 1;
	CURLMsg* msg = nullptr;

	do
	{
		curl_multi_perform(multi_handle, &still_alive);

		while ((msg = curl_multi_info_read(multi_handle, &msgs_left)))
		{
			if (msg->msg == CURLMSG_DONE)
			{
				PWRITE_CALLBACK_PARAM param = nullptr;
				curl_off_t dl = {};
				curl_off_t speed = {};
				curl_off_t total = {};
				CURL* e = msg->easy_handle;
				curl_easy_getinfo(msg->easy_handle, CURLINFO_PRIVATE, &param);
				curl_easy_getinfo(msg->easy_handle, CURLINFO_SPEED_DOWNLOAD_T, &speed);
				curl_easy_getinfo(msg->easy_handle, CURLINFO_SIZE_DOWNLOAD_T, &dl);
				curl_easy_getinfo(msg->easy_handle, CURLINFO_TOTAL_TIME_T, &total);
				BOOST_LOG_TRIVIAL(info) << param->num_transfer << ":perform_multi success" << ",speed:" << (double)speed / 1024.0 / 1024.0 << " MB/s,total size:" << (double)dl / 1024.0 / 1024.0 << "MB,total time:" << (double)total / 1000000.0 << "s,return code:" << msg->data.result << "-" << curl_easy_strerror(msg->data.result) << std::endl;
				curl_multi_remove_handle(multi_handle, e);
				curl_easy_cleanup(e);
			}
			else
				BOOST_LOG_TRIVIAL(error) << "perform_multi error,error code:" << msg->msg << std::endl;

			if (transfers < num_of_urls)
			{
				responses[transfers].num_transfer = transfers;
				responses[transfers].body = new std::string{};

				add_transfer(multi_handle, &requests[transfers], urls[transfers], &responses[transfers]);

				BOOST_LOG_TRIVIAL(debug) << transfers + 1 << ":url:" << *urls[transfers] << ",using requests object:" << requests[transfers] << ",using responses object:" << responses[transfers].body << std::endl;

				++transfers;
			}
		}

		if (still_alive)
			curl_multi_wait(multi_handle, NULL, 0, 10000, NULL);
	} while (still_alive || (transfers < num_of_urls));

	delete[] requests;

	return responses;
}

void work_thread(long begin, long end)
{
	std::vector<std::string*> ranking_urls{};
	for (long i = 0; i < end - begin; i++)
	{
		std::string* ranking_url = new std::string{};
		*ranking_url = "https://www.pixiv.net/ranking.php?mode=daily&content=illust&p=";
		*ranking_url += std::to_string(begin + i);
		*ranking_url += "&format=json";
		ranking_urls.push_back(ranking_url);

		BOOST_LOG_TRIVIAL(debug) << i + 1 << ":" << "ranking url:" << *ranking_url << std::endl;
	}

	size_t count = 0;
	std::vector<std::string*> artwork_urls{};
	BOOST_LOG_TRIVIAL(debug) << "------------perform_multi start------------" << std::endl;
	auto ranking_result = perform_multi(ranking_urls);
	BOOST_LOG_TRIVIAL(debug) << "-------------perform_multi end-------------" << std::endl;
	for (long i = 0; i < end - begin; i++)
	{
		if (!ranking_result[i].body)
			continue;

		auto search_id_result = ranking_result[i].body->c_str();
		while (true)
		{
			search_id_result = strstr(search_id_result, "\"illust_id\":") + sizeof("\"illust_id\":") - 1;
			if (!(search_id_result - (sizeof("\"illust_id\":") - 1)))
				break;

			std::string* artwork_url = new std::string{};
			*artwork_url = "https://www.pixiv.net/artworks/";

			for (auto utf8_id_end = search_id_result; *utf8_id_end != 0; utf8_id_end++)
			{
				if (*utf8_id_end == ',')
					break;

				*artwork_url += *utf8_id_end;
			}

			artwork_urls.push_back(artwork_url);

			BOOST_LOG_TRIVIAL(debug) << ++count << ":" << "artwork url:" << *artwork_url << std::endl;
		}

		delete ranking_result[i].body;
	}

	delete[] ranking_result;

	for (auto& item : ranking_urls)
		delete item;

	count = 0;
	std::vector<std::wstring*> uni_filenames{};
	std::vector<std::string*> original_urls{};
	BOOST_LOG_TRIVIAL(debug) << "------------perform_multi start------------" << std::endl;
	auto artwork_result = perform_multi(artwork_urls);
	BOOST_LOG_TRIVIAL(debug) << "-------------perform_multi end-------------" << std::endl;
	for (size_t i = 0; i < artwork_urls.size(); i++)
	{
		if (!artwork_result[i].body)
			continue;

		auto search_filename_result = strstr(artwork_result[i].body->c_str(), "\"illustTitle\":\"");
		auto search_original_url_result = strstr(artwork_result[i].body->c_str(), "\"original\":\"");
		if (search_filename_result && search_original_url_result)
		{
			std::string utf8_filename{};
			std::string* original_url = new std::string{};
			utf8_filename += std::to_string(i + 1);
			utf8_filename += "_";

			char* utf8_filename_end = (char*)search_filename_result + sizeof("\"illustTitle\":\"") - 1;
			for (; *utf8_filename_end != 0; utf8_filename_end++)
			{
				if (*utf8_filename_end == '"')
					break;

				utf8_filename += *utf8_filename_end;
			}
			char* original_url_end = (char*)search_original_url_result + sizeof("\"original\":\"") - 1;
			for (; *original_url_end != 0; original_url_end++)
			{
				if (*original_url_end == '"')
					break;

				*original_url += *original_url_end;
			}

			utf8_filename += ".jpg";
			auto uni_filename = utf82uni(&utf8_filename);
			for (auto& ch : *uni_filename)
			{
				if (ch == L'\\' ||
					ch == L'/' ||
					ch == L':' ||
					ch == L'*' ||
					ch == L'?' ||
					ch == L'>' ||
					ch == L'<' ||
					ch == L'|')
					ch = L'-';
			}

			uni_filenames.push_back(uni_filename);
			original_urls.push_back(original_url);

			BOOST_LOG_TRIVIAL(debug) << ++count << ":" << *original_url << "->" << utf8_filename << std::endl;
		}

		delete artwork_result[i].body;
	}

	delete[] artwork_result;

	for (auto& item : artwork_urls)
		delete item;

	count = 0;
	BOOST_LOG_TRIVIAL(debug) << "------------perform_multi start------------" << std::endl;
	auto picture_result = perform_multi(original_urls);
	BOOST_LOG_TRIVIAL(debug) << "-------------perform_multi end-------------" << std::endl;
	for (size_t i = 0; i < original_urls.size(); i++)
	{
		std::ofstream output_file(*uni_filenames[i], std::ios::trunc | std::ios::binary);
		if (output_file.is_open())
		{
			output_file << *picture_result[i].body << std::flush;
			output_file.close();
		}
		else
			BOOST_LOG_TRIVIAL(error) << "ofstream error" << std::endl;

		delete picture_result[i].body;
	}

	delete[] picture_result;

	for (auto& item : original_urls)
		delete item;

	for (auto& item : uni_filenames)
		delete item;
}

int main()
{
#ifndef _DEBUG
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
#endif

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);

	SetConsoleOutputCP(65001);
	CONSOLE_FONT_INFOEX Info = { 0 };
	Info.cbSize = sizeof(Info);
	Info.dwFontSize.Y = 16;
	Info.FontWeight = FW_NORMAL;
	wcscpy_s(Info.FaceName, L"Consolas");
	SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &Info);

	curl_global_init(CURL_GLOBAL_DEFAULT);

	auto start = std::chrono::system_clock::now();

	BOOST_LOG_TRIVIAL(info) << "strating " << THREADS_COUNT << " thread(s)" << std::endl;

	long last_start = 1;
	long thread_count = 1;

	std::vector<std::thread*> thread_pool;
	for (long i = 0; i < THREADS_COUNT; i++)
	{
		BOOST_LOG_TRIVIAL(info) << "strat " << thread_count << " thread,begin:" << last_start << ",end:" << last_start + MAX_PART / THREADS_COUNT << std::endl;

		thread_pool.push_back(new std::thread(work_thread, last_start, last_start + MAX_PART / THREADS_COUNT));

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(5s);

		last_start += MAX_PART / THREADS_COUNT;
		++thread_count;
	}

	for (auto& thread_item : thread_pool)
		thread_item->join();

	for (auto& thread_item : thread_pool)
		delete thread_item;

	auto end = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

	curl_global_cleanup();

	BOOST_LOG_TRIVIAL(info) << "the program ran for " << double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den << " seconds" << std::endl;

	_CrtDumpMemoryLeaks();

	auto unused = getchar();

	return EXIT_SUCCESS;
}